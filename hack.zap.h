#ifndef HACK_ZAP_H_
#define HACK_ZAP_H_

#include "def.monst.h" /* monst */

int dozap();
void miss(char *str, struct monst *mtmp);
struct monst *bhit(int ddx, int ddy, int range, char sym);
struct monst *boomhit(int dx, int dy);
char *exclam(int force);

#endif
