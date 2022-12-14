#ifndef RST_H_
#define RST_H_

#include "vars.h"

void rst(int, int, int, int, orbitinfo *);
void ship_report(int, int, int, unsigned char [], int);
void plan_getrships(int, int, int, int);
void star_getrships(int, int, int);
int Getrship(int, int, int);
void Free_rlist(void);
int listed(int, char *);

extern reportdata *rd;

#endif /* RST_H_ */
