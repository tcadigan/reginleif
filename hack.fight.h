#ifndef HACK_FIGHT_H_
#define HACK_FIGHT_H_

#include "config.h" /* boolean */
#include "def.monst.h" /* monst */
#include "def.obj.h" /* obj */

boolean hmon(struct monst *mon, struct obj *obj, int thrown);
int hitmm(struct monst *magr, struct monst *mdef);
int hitu(struct monst *mtmp, int dam);
void mondied(struct monst *mdef);
void monstone(struct monst *mdef);

#endif
