#ifndef HACK_OBJNAM_H_
#define HACK_OBJNAM_H_

#include "def.obj.h" /* obj */

char *doname(struct obj *obj);
char *xname(struct obj *obj);
char *aobjnam(struct obj *otmp, char *verb);
struct obj *readobjnam(char *bp);
void setan(char *str, char *buf);

#endif
