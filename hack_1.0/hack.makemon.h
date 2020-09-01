#ifndef HACK_MAKEMON_H_
#define HACK_MAKEMON_H_

#include "config.h" /* xchar */
#include "def.monst.h" /* monst */
#include "def.permonst.h" /* permonst */
#include "mklev.h" /* coord */

struct monst *makemon(struct permonst *ptr, int x, int y);
int goodpos(int x, int y);
coord enexto(xchar xx, xchar yy);

#endif
