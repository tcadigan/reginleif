#ifndef HACK_PRI_H_
#define HACK_PRI_H_

#include "config.h" /* schar */
#include "def.monst.h" /* monst */

void panic(char *str, ...);
void Tmp_at(schar x, schar y);
void unpmon(struct monst *mon);

#endif
