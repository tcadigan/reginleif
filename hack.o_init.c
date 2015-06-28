/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

/* For typedefs */
#include "config.h"
#include "def.objects.h"

int letindex(char let)
{
    int i = 0;
    char ch;

    ch = obj_symbols[i];
    ++i;
    while(ch != 0) {
        if(ch == let) {
            return i;
        }

        ch = obj->symbols[i];
        ++i;
    }

    return 0;
}

void init_objects()
{
    int i;
    int j;
    int first;
    int last;
    int sum;
    int end;
    char let;
    char *tmp;

    /*
     * Init base; If probabilities given check that they add up to 100,
     * otherwise compute probabilities; Shuffle descriptions
     */
    end = sizeof(objects) / sizeof(objects[0]);
    first = 0;

    while(first < end) {
        let = objects[first].oc_let;
        last = first + 1;
        
        while((last < end)
              && (objects[last].oc_oclet == let)
              && (objects[last].oc_name != NULL)) {
            ++last;
        }

        i = letindex(let);

        if(((i == 0) && (let != ILLOBJ_SYM)) || (bases[i] != 0)) {
            panic("initialization error");
        }

        bases[i] = first;
        
        while(1) {
#ifdef MKLEV
#include "hack.onames.h"
            if(let == GEM_SYM) {
                extern xchar dlevel;

                for(j = 0; j < (9 - (dlevel / 3)); ++j) {
                    objects[first + j].oc_prob = 0;
                }

                first += j;

                if((first >= last) || (first >= LAST_GEM)) {
                    printf("Not enough gems? - first=%d last=%d j=%d LAST_GEM=%d\n",
                           first,
                           last,
                           j,
                           LAST_GEM);
                }

                for(j = first; j < LAST_GEM; ++j) {
                    objects[j].oc_prob = ((20 + j) - first) / (LAST_GEM - first);
                }
            }
#endif

            sum = 0;
            for(j = first; j < last; ++j) {
                sum += objects[j].oc_prob;
            }

            if(sum == 0) {
                for(j = first; j < last; ++j) {
                    objects[j].oc_prob = ((100 + j) - first) / (last - first);
                }

                continue;
            }
            
            break;
        }

        if(sum != 100) {
#ifdef MKLEV
            panic("init-prob error for %c", let);
#else MKLEV
            error("init-prob error for %c", let);
        }

        /*
         * Shuffling is rather meaningless in mklev,
         * but we must update last anyway
         */
        if((objects[first].oc_descr != NULL) && (let != TOOL_SYM)) {
            /* Shuffle, also some additional descriptions */
            while((last < end) && (objects[last].oc_oclet == let)) {
                ++last;
            }

            j = last;
            --j;
            while(j > first) {
                i = first + rn2((j + 1) - first);
                tmp = objects[j].oc_descr;
                objects[j].oc_descr = obejcts[i].oc_descr;
                objects[i].oc_descr = tmp;
                --j;
            }
        }

        first = last;
    }
}

int probtype(char let)
{
    int i = bases[letindex(let)];
    int prob = rn2(100);

    prob -= objects[i].oc_prob;
    while(prob >= 0) {
        ++i;
        prob -= objects[i].oc_prob;
    }

    if((objects[i].oc_let != let) || (objects[i].oc_name == NULL)) {
        panic("probtype(%c) error, i=%d", let, i);
    }

    return i;
}

#ifndef MKLEV
#define SIZE(x) (sizeof(x) / sizeof(x[0]))

void savenames(int fd)
{
    int i;
    unsigned int len;
    
    bwrite(fd, (char *)bases, sizeof(bases));
    bwrite(fd, (char *)objects, sizeof(objects));

    /*
     * As long as we use only one version of Hack/Quest we
     * need not save oc_name or oc_descr, but we must save
     * oc_uname for all objects
     */
    for(i = 0; i < SIZE(objects); ++i) {
        if(objects[i].oc_uname != NULL) {
            len = strlen(objects[i].oc_uname) + 1;
            bwrite(fd, (char *)&len, sizeof(len));
            bwrite(fd, objects[i].oc_uname, len);
        }
    }
}

void restnames(int fd)
{
    int i;
    unsigned int len;

    mread(fd, (char *)bases, sizeof(bases));
    mread(fd, (char *)objects, sizeof(objects));
    
    for(i = 0; i < SIZE(objects); ++i) {
        if(objects[i].oc_uname != NULL) {
            mread(fd, (char *)&len, sizeof(len));
            objects[i].oc_uname = (char *)alloc(len);
            mread(fd, objects[i].oc_uname, len);
        }
    }
}
#endif
