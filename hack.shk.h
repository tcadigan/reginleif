#ifndef HACK_SHK_H_
#define HACK_SHK_H_

#include "def.obj.h" /* obj */

void addtobill();
void subfrombill(struct obj *obj);
void splitbill(struct obj *obj, struct obj *otmp);
char *plur(unsigned int n);
int inshop();
int dopay();
void obfree(struct obj *obj, struct obj *merge);

#endif
