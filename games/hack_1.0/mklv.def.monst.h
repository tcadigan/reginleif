/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#ifndef MKLV_DEF_MONST_H_
#define MKLV_DEF_MONST_H_

#include "config.h"
#include "mklev.h" /* coord */

struct monst {
    struct monst *nmon;
    struct permonst *data;
    unsigned int m_id;
    xchar mx;
    xchar my;
    /* If mdispl then pos where last displayed */
    xchar mdx;
    xchar mdy;
#define MTSZ 4
    coord mtrack[MTSZ]; /* Monster track */
    schar mhp;
    schar orig_hp;
    char mimic; /* Undetected mimic - this is its symbol */
    Bitfield(mdispl, 1); /* mdx, mdy valid */
    Bitfield(minvis, 1); /* Invisible */
    Bitfield(cham, 1); /* Shape-changer */
    Bitfield(mhide, 1); /* Hides beneath objects */
    Bitfield(mundetected, 1); /* Not seen in present hiding place */
    Bitfield(mspeed, 2);
    Bitfield(msleep, 1);
    Bitfield(mfroz, 1);
    Bitfield(mconf, 1);
    Bitfield(mflee, 1);
    Bitfield(mcan, 1); /* Has been cancelled */
    Bitfield(mtame, 1); /* Implies peaceful */
    Bitfield(mpeaceful, 1); /* does not attack unprovoked */
    Bitfield(isshk, 1); /* Is shopkeeper */
    Bitfield(isgd, 1); /* Is guard */
    Bitfield(mcansee, 1); /* cansee 1, temp.blinded 0, blind 0 */
    Bitfield(mblinded, 7); /* cansee 0, temp.blinded n, blind 0 */
    Bitfield(mtrapped, 1); /* Trapped in a pit or bear trap */
    Bitfield(mnamelth, 6); /* Length of name (following mxlth) */
#ifndef NOWORM
    Bitfield(wormno, 5); /* At most 31 worms on any level */
#endif
    unsigned int mtrapseen; /* Bitmap of traps we've been trapped in */
    long mlstmv; /* Prevent two moves at once */
    struct obj *minvent;
    long mgold;
    unsigned int mxlth; /* Length of following data */

    /* 
     * In order to prevent alignment problems mextra should
     * be (or follow) a long int
     */
    long mextra[1]; /* Monster dependent info */
};

#define newmonst(xl) (struct monst *)alloc((unsigned int)(xl) + sizeof(struct monst))

extern struct monst *fmon;

/* These are in mspeed */
#define MSLOW 1 /* Slow monster */

#define MFAST 2 /* Speeded monster */

#define NAME(mtmp) (((char *)mtmp->mextra) + mtmp->mxlth)

#endif
