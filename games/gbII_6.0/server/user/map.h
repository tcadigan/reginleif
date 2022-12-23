#ifndef MAP_H_
#define MAP_H_

#include "../server/races.h"
#include "../server/vars.h"

void map(int, int, int);
void show_map(int, int, int, int, planettype *, int, int);
char desshow(int, int, planettype *, int, int, racetype *);

#endif /* MAP_H_ */
