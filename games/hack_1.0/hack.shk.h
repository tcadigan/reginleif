#ifndef HACK_SHK_H_
#define HACK_SHK_H_

#include "def.eshk.h" /* bill_x */
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

void setpaid();
void obfree(struct obj *obj, struct obj *merge);
int getprice(struct obj *obj);
int dopayobj(struct bill_x *bp);
int realhunger();
int inshop();
void addtobill(struct obj *obj);
void paybill();
char *shkname();
void shkdead();
void setshk();
char *plur(unsigned int n);
int online(int x, int y);
int shk_move();

#endif
