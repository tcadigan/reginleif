#ifndef HACK_SHK_H_
#define HACK_SHK_H_

#include "def.obj.h" /* obj */

#ifdef QUEST
void subfrombill();
void splitbill();
void dopay();
#else
void subfrombill(struct obj *obj);
void splitbill(struct obj *obj, struct obj *otmp);
int dopay();
#endif

void obfree(struct obj *obj, struct obj *merge);
int inshop();
void addtobill();
void paybill();
char *shkname();
void setshk();
char *plur(unsigned int n);

#endif
