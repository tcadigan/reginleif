#ifndef MAX_H_
#define MAX_H_

#include "races.h"
#include "ships.h"

int maxsupport(racetype *, sectortype *, double, int);
double compatibility(planettype *, racetype *);
double gravity(planettype *);
char *prin_ship_orbits(shiptype *);

#endif /* MAX_H_ */
