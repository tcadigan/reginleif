#ifndef OAUX2_H_
#define OAUX2_H_

#include "odefs.h"

int true_item_value(pob item);
void gain_level();
void movecursor(int *x, int *y, int dx, int dy);
int p_immune(int dtype);
void tacplayer(struct monster *m);
int statmod(int stat);

#endif
