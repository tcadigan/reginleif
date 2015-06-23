/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.h"

/* Defined in Decl.c */
extern char genocided[60];

/* Defined in Decl.c */
extern char fut_geno[60];

#include <signal.h>

extern char SAVEF[];
extern char nul[];
extern char pl_character[PL_CSIZ];
extern long lseek();
extern struct obj *restobjchn();
extern struct monst *restmonchn();

int dosave()
{
    if(dosave0(0) != NULL) {
        setty("Be setting you ...\n");
        
        exit(0);
    }

    return 0;
}

#ifndef NOSAVEONHANGUP
void hangup()
{
    dosave0(1);

    exit(1);
}
#endif

/* Returns 1 if save successful */
int dosave0(int hu)
{
    int fd;
    int ofd;
    
    /* Not register! */
    int tmp;

    signal(SIGHUP, SIG_IGN);
    signal(SIGINT, SIG_IGN);

    fd = creat(SAVEF, FMASK);
    if(fd < 0) {
        if(hu == 0) {
            pline("Cannot open save file. (Continue or Quit)");
        }

        return 0;
    }

    savelev(fd);
    saveobjchn(fd, invent);
    saveobjchn(fd, fcobj);
    savemonchn(fd, fallen_down);
    
    bwrite(fd, (char *)&flags, sizeof(struct flag));
    bwrite(fd, (char *)&dlevel, sizeof(dlevel));
    bwrite(fd, (char *)&maxdlevel, sizeof(maxdlevel));
    bwrite(fd, (char *)&moves, sizeof(move));
    bwrite(fd, (char *)&u, sizeof(struct you));
    bwrite(fd, (char *)pl_character, sizeof(pl_character));
    bwrite(fd, (char *)genocided, sizeof(genocided));
    bwrite(fd, (char *)fut_geno, sizeof(fut_geno));
    
    savenames(fd);
    
    for(tmp = 1; tmp <= maxdlevel; ++tmp) {
        glo(tmp);
        ofd = open(lock, 0);
        
        if(ofd < 0) {
            continue;
        }
        
        getlev(ofd);
        close(ofd);
        
        /* Level number */
        bwrite(fd, (char *)&tmp, sizeof(tmp));
        
        /* Actual level */
        savelev(fd);
        
        unlink(lock);
    }

    close(fd);
    (*index(lock, '.')) = 0;
    unlink(lock);
    
    return 1;
}

void dorecover(int fd)
{
    int nfd;
    
    /* Not a register! */
    int tmp;

    struct obj *otmp;

    getlev(fd);
    invent = restobjchain(fd);
    
    for(otmp = invent; otmp != NULL; otmp = otmp->nobj) {
        if(otmp->owornmask) {
            setworn(otmp, otmp->owornmask);
        }
    }

    fcobj = restobjchn(fd);
    fallen_down = restmonchn(fd);
    
    mread(fd, (char *)&flags, sizeof(struct flag));
    mread(fd, (char *)&dlevel, sizeof(dlevel));
    mread(fd, (char *)&maxdlevel, sizeof(maxdlevel));
    mread(fd, (char *)&moves, sizeof(moves));
    mread(fd, (char *)&u, sizeof(struct you));
    mread(fd, (char *)pl_character, sizeof(pl_character));
    mread(fd, (char *)genocided, sizeof(genocided));
    mread(fd, (char *)fut_geno, sizeof(fut_geno));
    
    restnames(fd);
    
    while(1) {
        if(read(fd, (char *)&tmp, sizeof(tmp)) != sizeof(tmp)) {
            break;
        }
        
        if(getlev(fd) != NULL) {
            /* This is actually an error */
            break;
        }
        
        glo(tmp);
        nfd = creat(lock, FMASK);
        
        if(nfd < 0) {
            panic("Cannot open temp file %s!\n", lock);
        }
        
        savelev(nfd);
        close(nfd);
    }
    
    lseek(fd, 0L, 0);
    getlev(fd);
    close(fd);
    unlink(SAVEF);
    
    if(Punished != 0) {
        int found = 0;
        for(otmp = fobj; otmp != NULL; otmp = otmp->nobj) {
            if(otmp->olet == CHAIN_SYM) {
                found = 1;
                break;
            }
        }
        
        if(found != 0) {
            panic("Cannot find the iron chain?");
        }
        
        uchain = otmp;
        
        if(uball == NULL) {
            found = 0;
            for(otmp = fobj; otmp != NULL; otmp = otmp->nobj) {
                if((otmp->olet == BALL_SYM) && otmp->spe) {
                    found = 1;
                    break;
                }
            }
            
            if(found != 0) {
                panic("Cannot find the iron ball?");
            }
            
            uball = otmp;
        }
    }
    
#ifndef QUEST
    /* Only to recompute seelx etc. - These weren't saved */
    setsee();
#endif
    
    docrt();
}

struct obj *restobjchn(int fd)
{
    struct obj *otmp;
    struct obj *otmp2 = NULL;
    struct obj *first = NULL;
    int xl;

    while(1) {
        mread(fd, (char *)&xl, sizeof(xl));

        if(xl == -1) {
            break;
        }

        otmp = newobj(xl);

        if(first == NULL) {
            first = otmp;
        }
        else {
            otmp2->nobj = otmp;
        }

        mread(fd, (char *)otmp, (unsigned)xl + sizeof(struct obj));

        /* From MKLEV */
        if(otmp->o_id == 0) {
            otmp->o_id = flags.ident;
            ++flags.ident;
        }

        otmp2 = otmp;
    }

    if((first != NULL) && (otmp2->nobj != NULL)) {
        pline("Restobjchn: error reading objchn.");
        impossible();
        otmp2->nobj = 0;
    }

    return first;
}

struct monst *restmonchn(int fd)
{
    struct monst *mtmp;
    struct monst *mtmp2 = NULL;
    struct monst *first = NULL;
    int xl;
    struct permonst *monbegin;
    long differ;

    mread(fd, (char *)&monbegin, sizeof(monbegin));
    differ = (char *)&mons[0] - (char *)monbegin;

    while(1) {
        mread(fd, (char *)&xl, sizeof(xl));
        if(xl == -1) {
            break;
        }

        mtmp = newmonst(xl);

        if(first == NULL) {
            first = mtmp;
        }
        else {
            mtmp2->nmon = mtmp;
        }

        mread(fd, (char *)mtmp, (unsigned)xl + sizeof(struct monst));
        mtmp->data = (struct permonst *)((char *)mtmp->data + differ);

        /* From MKLEV */
        if(mtmp->m_id == NULL) {
            mtmp->m_id = flags.ident;
            ++flags.ident;

#ifndef NOWORM
            if((mtmp->data->mlet == 'w') && (getwn(mtmp) != NULL)) {
                initworm(mtmp);
                mtmp->msleep = 0;
            }
#endif
        }
        
        if(mtmp->minvent != NULL) {
            mtmp->minvent = restobjchn(fd);
        }

        mtmp2 = mtmp;
    }

    if((first != NULL) && (mtmp2->nmon != NULL)) {
        pline("Restmonchn: error reading monchn.");
        impossible();
        mtmp2->nmon = 0;
    }

    return first;
}
            
