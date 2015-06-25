#ifndef HACK_APPLY_H_
#define HACK_APPLY_H_

#include "def.monst.h" /* monst */
#include "def.obj.h" /* obj */

void use_camera(struct obj *obj);
void use_ice_box(struct obj *obj);
void use_magic_whistle(struct obj *obj);
void use_whistle(struct obj *obj);
struct monst *bchit(int ddx, int ddy, int trange, char sym);

#endif
