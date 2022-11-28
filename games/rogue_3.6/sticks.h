#ifndef STICKS_H_
#define STICKS_H_

#include "rogue.h"

int fix_stick(struct object *cur);
int do_zap(bool gotdir);
int drain(int ymin, int ymax, int xmin, int xmax);
char *charge_str(struct object *obj);

#endif
