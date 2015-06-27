#ifndef HACK_MAKEMON_H_
#define HACK_MAKEMON_H_

#include "def.monst.h" /* monst */
#include "def.permonst.h" /* permonst */

struct monst *makemon(struct permonst *ptr, int x, int y);

#endif
