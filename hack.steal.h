#ifndef HACK_STEAL_H_
#define HACK_STEAL_H_

#include "def.monst.h" /* monst */
#include "def.obj.h" /* obj */

int somegold();
int steal(struct monst *otmp);
void stealgold(struct monst *mtmp);
void mpickobj(struct monst *mtmp, struct obj *otmp);
void relobj(struct monst *mtmp, int show);

#endif
