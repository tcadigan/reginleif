#ifndef HACK_DO_H_
#define HACK_DO_H_

#include "config.h" /* boolean */
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
unsigned char *lowc(unsigned char *str);
struct obj *splitobj(struct obj *obj, int num);
void goto_level(int newlevel, boolean at_stairs);
int child(int wt);
void dropx(struct obj *obj);
int drop(struct obj *obj);
void pluslvl();
void strange_feeling(struct obj *obj);
void rhack(unsigned char *cmd);

#endif
