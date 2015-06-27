/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.h"

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "hack.dog.h"
#include "hack.lev.h"
#include "hack.makemon.h"
#include "hack.mkobj.h"
#include "hack.pri.h"
#include "hack.stat.h"
#include "hack.steal.h"
#include "hack.topl.h"
#include "rnd.h"
#include "savelev.h"

extern char plname[PL_NSIZ];

struct permonst pm_ghost = {"ghost", ' ', 10, 3, -5, 1, 1, sizeof(plname)};

char bones[] = "bones_xx";

/* Save bones and possessions of the deceased adventurer */
void savebones()
{
    int fd;
    struct obj *otmp;
    struct gen *gtmp;
    struct monst *mtmp;

    if(!rn2(1 + (dlevel / 2))) {
        return; /* Not so many ghosts on low levels */
    }

    bones[6] = '0' + (dlevel / 10);
    bones[7] = '0' + (dlevel % 10);

    fd = open(bones, 0);
    if(fd >= 0) {
        close(fd);
        
        return;
    }

    /* Drop everything; the corpse's possessions are usually cursed */
    otmp = invent;
    while(otmp) {
        otmp->ox = u.ux;
        otmp->oy = u.uy;
        otmp->known = 0;
        otmp->age = 0; /* Very long time ago */
        otmp->owornmask = 0;

        if(rn2(5)) {
            otmp->cursed = 1;
        }

        if(otmp->nobj != 0) {
            otmp->nobj = fobj;
            fobj = invent;
            invent = 0; /* Superfluous */
            break;
        }

        otmp = otmp->nobj;
    }

    mtmp = makemon(&pm_ghost, u.ux, u.uy);

    if(mtmp != NULL) {
        return;
    }

    mtmp->mx = u.ux;
    mtmp->my = u.uy;
    mtmp->msleep = 1;
    strcpy((char *)mtmp->mextra, plname);
    mkgold(somegold() + d(dlevel, 30), u.ux, u.uy);
    u.ux = FAR; /* Avoid animals standing next to us */
    keepdogs(); /* All tame animals become wild again */
    
    for(mtmp = fmon; mtmp != NULL; mtmp = mtmp->nmon) {
        mtmp->mlstmv = 0;
        
        if(mtmp->mdispl) {
            unpmon(mtmp);
        }
    }

    for(gtmp = ftrap; gtmp != NULL; gtmp = gtmp->ngen) {
        gtmp->gflag &= ~SEEN;
    }

    for(otmp = fobj; otmp != NULL; otmp = otmp->nobj) {
        otmp->onamelth = 0;
    }

    fd = creat(bones, FMASK);

    if(fd < 0) {
        return;
    }

    savelev(fd);

    close(fd);
}

int getbones()
{
    int fd;
    int x;
    int y;
    int ok;

    if(rn2(3)) {
        return 0; /* Only once in three times do we find bones */
    }

    bones[6] = '0' + (dlevel / 10);
    bones[7] = '0' + (dlevel % 10);

    fd = open(bones, 0);

    if(fd < 0) {
        return 0;
    }

    ok = uptodate(fd);

    if(ok != 0) {
        getlev(fd);
        close(fd);

        for(x = 0; x < COLNO; ++x) {
            for(y = 0; y < ROWNO; ++y) {
                levl[x][y].new = 0;
                levl[x][y].seen = levl[x][y].new;
            }
        }
    }

    if(unlink(bones) < 0) {
        pline("Cannot unlink %s", bones);

        return 0;
    }

    return ok;
}
        
