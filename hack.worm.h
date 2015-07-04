#ifndef HACK_WORM_H_
#define HACK_WORM_H_

#include "config.h" /* xchar, uchar */
#include "def.monst.h" /* monst */
#include "def.wseg.h" /* wseg */

void cutworm(struct monst *mtmp, xchar x, xchar y, uchar wetyp);
void initworm(struct monst *mtmp);
int getwn(struct monst *mtmp);
void wormhit(struct monst *mtmp);
void wormdead(struct monst *mtmp);
void worm_move(struct monst *mtmp);
void worm_nomove(struct monst *mtmp);
void wormsee(unsigned int tmp);
void pwseg(struct wseg *wtmp);

#endif
