#ifndef HACK_DO_NAME_H_
#define HACK_DO_NAME_H_

#include "def.monst.h" /* monst */

char *monnam(struct monst *mtmp);
char *Monnam(struct monst *mtmp);
int ddocall();
void docall(struct obj *obj);
int do_mname();

#endif
