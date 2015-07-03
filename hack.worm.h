#ifndef HACK_WORM_H_
#define HACK_WORM_H_

#include "config.h" /* xchar, uchar */
#include "def.monst.h" /* monst */

void cutworm(struct monst *mtmp, xchar x, xchar y, uchar wetyp);
void initworm(struct monst *mtmp);
int getwn(struct monst *mtmp);
void wormhit(struct monst *mtmp);

#endif
