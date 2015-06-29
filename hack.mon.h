#ifndef HACK_MON_H_
#define HACK_MON_H_

#include "def.monst.h" /* monst */
#include "mklev.h" /* coord */

void setmangry(struct monst *mtmp);
int dist(int x, int y);
void mnexto(struct monst *mtmp);
int m_move(struct monst *mtmp, int after);
void rloc(struct monst *mtmp);
void replmon(struct monst *mtmp, struct monst *mtmp2);
void relmon(struct monst *mtmp);
int mfndpos(struct monst *mon, coord poss[9], int info[9], int flag);
void rescham();

#endif
