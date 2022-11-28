#ifndef HACK_MKOBJ_H_
#define HACK_MKOBJ_H_

#include "def.obj.h" /* obj */

void mkgold(int num, int x, int y);
struct obj *mkobj(int let);
void mkobj_at(int let, int x, int y);
struct obj *mksobj(int let, int otyp);
void mksobj_at(int let, int otyp, int x, int y);
int weight(struct obj *obj);
int letter(int c);

#endif
