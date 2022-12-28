#ifndef MAP_H_
#define MAP_H_

#include "races.h"
#include "vars.h"

void map(int, int, int, int, orbitinfo *);
void show_map(int, int, int, int, planettype *, int, int);
char desshow(int, int, planettype *, int, int, racetype *);

#endif /* MAP_H_ */
