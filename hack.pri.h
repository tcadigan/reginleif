#ifndef HACK_PRI_H_
#define HACK_PRI_H_

#include "config.h" /* schar, xchar */
#include "def.monst.h" /* monst */
#include "def.obj.h" /* obj */

void panic(char *str, ...);
void Tmp_at(schar x, schar y);
void unpmon(struct monst *mon);
void on_scr(int x, int y);
void unpobj(struct obj *obj);
void newsym(int x, int y);
void atl(int x, int y, int ch);
void pru();
void prl(int x, int y);
char news0(xchar x, xchar y);
void nose1(int x, int y);
void prl1(int x, int y);
void docrt();
void prme();
void at(xchar x, xchar y, char ch);
void cls();
void pmon(struct monst *mon);
void docorner(int xmin, int ymax);

#endif
