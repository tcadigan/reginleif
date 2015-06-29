#ifndef HACK_DOG_H_
#define HACK_DOG_H_

#include "config.h" /* xchar */
#include "def.monst.h" /* monst */
#include "def.obj.h" /* obj */

void keepdogs();
void losedogs();
int tamedog(struct monst *mtmp, struct obj *obj);
int inroom(xchar x, xchar y);
void initedog(struct monst *mtmp);

#endif
