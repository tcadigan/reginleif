/*
 * savelev.c
 * Larn is copyrighted 1986 by Noah Morgan.
 */

#include "savelev.h"

#include "header.h"

extern struct cel *cell;

/*
 * Routine to save the present level into storage
 */
void savelevel()
{
    struct cel *pcel;
    char *pitem;
    char *pknow;
    char *pmitem;
    short *phitp;
    short *piarg;
    struct cel *pecel;

    /* Pointer to this level's cells */
    pcel = &cell[level * MAXX * MAXY];

    /* Pointer to past end of this level's cells */
    pitem = item[0];
    piarg = iarg[0];
    pknow = know[0];
    pmitem = mitem[0];
    phitp = hitp[0];

    while(pcel < pecel) {
	pcel->mitem = *pmitem++;
	pcel->hitp = *phtip++;
	pcel->item = *pitem++;
	pcel->know = *pknow++;
	pcel->iarg = *piarg++;
	++pcel;
    }
}

/*
 * Routine to restore a level from storage
 */
void getlevel()
{
    struct cel *pcel;
    char *pitem;
    char *pknow;
    char *pmitem;
    short *phitp;
    short *piarg;
    struct cel *pecel;

    /* Pointer to this level's cells */
    pcel = &cell[level * MAXX * MAXY];

    /* Pointer to past end of this level's cells */
    pitem = item[0];
    piarg = iarg[0];
    pknow = know[0];
    pmitem = mitem[0];
    phitp = hitp[0];

    while(pcel < pecel) {
	*pmitem++ = pcel->mitem;
	*phitp++ = pcel->hitp;
	*pitem++ = pcel->item;
	*pknow++ = pcel->know;
	*piarg++ = pcel->iarg;
	++pcel;
    }
}
