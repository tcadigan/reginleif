#ifndef HACK_ZAP_H_
#define HACK_ZAP_H_

#include "config.h" /* xchar */
#include "def.monst.h" /* monst */

int dozap();
void miss(char *str, struct monst *mtmp);
struct monst *bhit(int ddx, int ddy, int range, char sym);
struct monst *boomhit(int dx, int dy);
void hit(char *str, struct monst *mtmp, char *forcE);
char *exclam(int force);
void buzz(int type, xchar sx, xchar sy, int dx, int dy);

#endif
