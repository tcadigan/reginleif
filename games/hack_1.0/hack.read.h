#ifndef HACK_READ_H_
#define HACK_READ_H_

#include "config.h" /* boolean */
#include "def.obj.h" /* obj */

int doread();
void litroom(boolean on);
int identify(struct obj *otmp);

#endif
