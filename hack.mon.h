#ifndef HACK_MON_H_
#define HACK_MON_H_

#include "def.monst.h" /* monst */

void setmangry(struct monst *mtmp);
int dist(int x, int y);
void mnexto(struct monst *mtmp);
int m_move(struct monst *mtmp, int after);
void rloc(struct monst *mtmp);
void replmon(struct monst *mtmp, struct monst *mtmp2);

#endif
