#ifndef HACK_WORN_H_
#define HACK_WORN_H_

#include "def.obj.h" /* obj */

void setworn(struct obj *obj, long mask);
void setnotworn(struct obj *obj);

#endif
