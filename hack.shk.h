#ifndef HACK_SHK_H_
#define HACK_SHK_H_

#include "def.obj.h" /* obj */

#ifdef QUEST
void subfrombill();
void splitbill();
void dopay();
void doinvbill();
#else
void subfrombill(struct obj *obj);
void splitbill(struct obj *obj, struct obj *otmp);
int dopay();
void doinvbill(int cl);
#endif

void obfree(struct obj *obj, struct obj *merge);
int inshop();
void addtobill();
void paybill();
char *shkname();
void shkdead();
void setshk();
char *plur(unsigned int n);
int online(int x, int y);
int shk_move();

#endif
