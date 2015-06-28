#ifndef HACK_SHK_H_
#define HACK_SHK_H_

#include "def.obj.h" /* obj */

void addtobill();
char *plur(unsigned int n);
int inshop();
int dopay();
void splitbill(struct obj *obj, struct obj *otmp);
void obfree(struct obj *obj, struct obj *merge);

#endif
