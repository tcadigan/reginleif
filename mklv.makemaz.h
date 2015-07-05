#ifndef MKLV_MAKEMAZ_H_
#define MKLV_MAKEMAZ_H_

#include "mklev.h" /* coord */

coord mazexy();
void makemaz();
void move(int *x, int *y, int dir);
int okay(int x, int y, int dir);
void walkfrom(int x, int y);

#endif
