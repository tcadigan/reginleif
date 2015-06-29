/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.rumors.h"

#include <stdio.h>
#include "config.h"

/* Number of bits in a char */
#define CHARSZ 8
#define RUMORFILE "rumors"

int n_rumors = 0;
int n_used_rumors = -1;
char *usedbits;

void init_rumors(FILE *rumf)
{
    int i;
    n_used_rumors = 0;
    
    while(skipline(rumf) != NULL) {
        ++n_rumors;
    }

    rewind(rumf);
    i = n_rumors / CHARSZ;
    usedbits = (char *)alloc((unsigned)(i + 1));

    while(i >= 0) {
        usedbits[i] = 0;
        --i;
    }
}

int skipline(FILE *rumf)
{
    char line[COLNO];

    while(1) {
        if(fgets(line, sizeof(line), rumf) == 0) {
            return 0;
        }

        if(index(line, '\n') != 0) {
            return 1;
        }
    }
}

void outline(FILE *rumf)
{
    char line[COLNO];
    char *ep;

    if(fgets(line, sizeof(line), rumf) == 0) {
        return;
    }

    ep = index(line, '\n');
    if(ep != 0) {
        *ep = 0;
    }

    pline("This cookie has a scrap of paper inside! It reads: ");
    pline(line);
}

void outrumor()
{
    int rn;
    int i;
    FILE *rumf;

    if(n_rumors <= n_used_rumors) {
        return;
    }
    else {
        rumf = fopen(RUMORFILE, "r");

        if(rumf == NULL) {
            return;
        }
    }

    if(n_used_rumors < 0) {
        init_rumors(rumf);
    }

    if(n_rumors == 0) {
        fclose(rumf);
        
        return;
    }

    rn = rn2(n_rumors - n_used_rumors);
    i = 0;

    while((rn != 0) || (used(i) != 0)) {
        skipline(rumf);
        
        if(used(i) == 0) {
            --rn;
        }

        ++i;
    }

    usedbits[i / CHARSZ] |= (1 << (i % CHARSZ));
    ++n_used_rumors;
    outline(rumf);
    fclose(rumf);
}

int used(int i)
{
    return (usedbits[i / CHARSZ] & (1 << (i % CHARSZ)));
}
