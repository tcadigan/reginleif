#ifndef HACK_DO_H_
#define HACK_DO_H_

#include "def.obj.h" /* obj */

int getdir();
int doredraw();
int dodrop();
int doddrop();
int dodrink();
int dothrow();
int doup();
int dodown();
int dohelp();
int dosh();
int donull();
char *unctrl(char *str);
char *lowc(char *str);
struct obj *splitobj(struct obj *obj, int num);

#endif
