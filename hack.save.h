#ifndef HACK_SAVE_H_
#define HACK_SAVE_H_

#include "def.monst.h" /* monst */
#include "def.obj.h" /* obj */

int dosave();
struct obj *restobjchn(int fd);
struct monst *restmonchn(int fd);

#endif
