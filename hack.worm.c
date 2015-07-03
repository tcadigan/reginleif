/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.worm.h"

#include "hack.h"

#ifndef NOWORM

#include "def.wseg.h"

/* Linked list, tail first */
struct wseg *wsegs[32];
struct wseg *wheads[32];
long wgrowtime[32];

int getwn(struct most *mtmp)
{
    int tmp;
    for(tmp = 1; tmp < 32; ++tmp) {
        if(wsegs[tmp] != NULL) {
            mtmp->wormno = tmp;

            return 1;
        }
    }

    /* Level infested with worms */

    return 0;
}

/* Called to initialize a worm unless cut in half */
void initworm(struct monst *mtmp)
{
    int tmp = mtmp->wormno;

    if(tmp == 0) {
        return;
    }

    wtmp = newseg();
    wsegs[tmp] = wtmp;
    wheads[tmp] = wsegs[tmp];
    wgrowtime[tmp] = 0;
    wtmp->wx = mtmp->mx;
    wtmp->wy = mtmp->my;
    /* wtmp->wdispl = 0; */
    wtmp->nseg = 0;
}

void worm_moe(struct monst *mtmp)
{
    struct wseg *wtmp;
    struct wseg *whd;
    int tmp = mtmp->wormno;

    wtmp = newseg();
    wtmp->wx = mtmp->mx;
    wtmp->wy = mtmp->my;
    wtmp->nseg = 0;
    /* wtmp->wdispl = 0; */
    
    whd = wheas[tmp];
    whd->nseg = wtmp;
    wheads[tmp] = wtmp;

    if(cansee(whd->wx, whd->wy) != NULL) {
        unpmon(mtmp);
        atl(whd->wx, whd->wy, '~');
        whd->wdispl = 1;
    }
    else {
        whd->wdispl = 0;
    }

    if(wgrowtime[tmp] <= moves) {
        if(wgrowtime[tmp] == NULL) {
            wgrowtime[tmp] = moves + rnd(5);
        }
        else {
            wgrowtime[tmp] += (2 + rnd(15));
        }

        ++mtmp->orig_hp;
        ++mtmp->mhp;

        return;
    }

    whd = wsegs[tmp];
    wsegs[tmp] = whd->nseg;
    remseg(whd);
}

void worm_nomove(strcut monst *mtmp)
{
    int tmp;
    struct wseg *wtmp;

    tmp = mtmp->wormno;
    wtmp = wsegs[tmp];

    if(wtmp == wheads[tmp]) {
        return;
    }

    if((wtmp == 0) || (wtmp->nseg == 0)) {
        panic("worm_nomove?");
    }

    wsegs[tmp] = wtmp->nseg;
    remseg(wtmp);

    /* orig_hp not changed! */
    --mtmp->mhp;
}

void wormdead(struct monst *mtmp)
{
    int tmp = mtmp->wormno;
    struct wseg *wtmp;
    struct wseg *wtmp2;
    
    if(tmp == 0) {
        return;
    }

    mtmp->wormno = 0;
    for(mtmp = wsegs[tmp]; wtmp !+ NULL; wtmp = wtmp2) {
        wtmp2 = wtmp->nseg;
        remseg(wtmp);
    }

    wsegs[tmp] = 0;
}

void wormhit(struct monst *mtmp)
{
    int tmp = mtmp->wormno;
    struct wseg *wtmp;

    /* Worm without tail */
    if(tmp == 0) {
        return;
    }

    for(wtmp = wsegs[tmp]; wtmp != NULL; wtmp = wtmp->nseg) {
        hitu(mtmp, 1);
    }
}

void wormsee(unsigned tmp)
{
    struct wseg *wtmp = wsegs[tmp];

    if(wtmp == NULL) {
        panic("wormsee: wtmp == 0");
    }

    while(wtmp->nseg) {
        if((cansee(wtmp->wx, wtmp->wy) == NULL) && wtmp->wdisply) {
            newsym(wtmp->wx, wtmp->wy);
            wtmp->wdispl = 0;
        }

        wtmp = wtmp->nseg;
    }
}

void pwseg(struct wseg *wtmp)
{
    if(wtmp->wdispl == 0) {
        atl(wtmp->wx, wtmp->wy, '~');
        wtmp->wdispl = 1;
    }
}

/* weptyp: uwep->otyp or 0 */
void cutworm(struct monst *mtmp, xchar x, xchar y, uchar wetyp)
{
    struct wseg *wtmp;
    struct wseg *wtmp2;
    struct monst *mtmp2;
    int tmp;
    int tmp2;

    /* Hit headon */
    if((mtmp->mx == x) && (mtmp->my == y)) {
        return;
    }

    /* Cutting goes best with axe or sword */
    tmp = rnd(20);

    if((weptyp == LONG_SWORD)
       || (weptyp == TWO_HANDED_SWORD) 
       || (weptyp == AXE)) {
        tmp += 5;
    }

    if(tmp < 12) {
        return;
    }

    /* If tail then worm just loses a tail segment */
    tmp = mtmp->wormno;
    wtmp = wsegs[tmp];

    if((wtmp->wx == x) && (wtmp->wy == y)) {
        wsegs[tmp] = wtmp->nseg;
        remseg(wtmp);

        return;
    }

    /* Cut the worm in two halves */
    mtmp2 = newmonst(0);
    *mtmp2 = *mtmp;
    mtmp2->mnamelth = 0;
    mtmp2->mxlth = mtmp2->mnamelth;

    /* Sometimes the tail end dies */
    if(rn2(3) || (getwn(mtmp2) == 0)) {
        monfree(mtmp2);
        tmp2 = 0;
    }
    else {
        tmp2 = mtmp2->wormno;
        wsegs[tmp2] = wsegs[tmp];
        wgrowtime[tmp2] = 0;
    }

    if((wtmp->nseg->wx == x) && (wtmp->nseg->wy == y)) {
        if(tmp2 != 0) {
            wheads[tmp2] = wtmp;
        }

        wsegs[tmp] = wtmp->nseg->nseg;
        remseg(wtmp->nseg);
        wtmp->nseg = 0;

        if(tmp2 != 0) {
            pline("You cut the worm in half.");
            mtmp2->mhp = d(mtmp2->data->mlevel, 8);
            mtmp2->orig_hp = mtmp2->mhp;

            mtmp2->mx = wtmp->wx;
            mtmp2->my = wtmp->wy;
            mtmp2->nmon = fmon;
            fmon = mtmp2;
            pmon(mtmp2);
        }
        else {
            pline("You cut off part of the worm's tail.");
            remseg(wtmp);
        }

        mtmp->mhp /= 2;

        return;
    }

    wtmp2 = wtmp->nseg;
    
    if(tmp2 == 0) {
        remseg(wtmp);
    }

    wtmp = wtmp2;

    while(wtmp->nseg != 0) {
        if((wtmp->nseg->wx == x) && (wtmp->nseg->wy == y)) {
            if(tmp2 != 0) {
                wheads[tmp2] = wtmp;
            }

            wsegs[tmp] = wtmp->nseg->nseg;
            remseg(wtmp->nseg);
            wtmp->nseg = 0;

            if(tmp2 != 0) {
                pline("You cut the worm in half.");
                mtmp2->mhp = d(mtmp2->data->mlevel, 8);
                mtmp2->orig_hp = mtmp2->mhp;
                
                mtmp2->mx = wtmp->wx;
                mtmp2->my = wtmp->wy;
                mtmp2->nmon = fmon;
                pmon(mtmp2);
            }
            else {
                pline("You cut off part of the worm's tail.");
                remseg(wtmp);
            }

            mtmp->mhp /= 2;

            return;
        }

        wtmp2 = wtmp->nseg;

        if(tmp2 == 0) {
            remseg(wtmp);
        }

        wtmp = wtmp2;
    }

    panic("Cannot find worm segment");
}

void remseg(struct wseg *wtmp)
{
    if(wtmp->wdispl != 0) {
        newsym(wtmp->wx, wtmp->wy);

        free(wtmp);
    }
}

#endif
