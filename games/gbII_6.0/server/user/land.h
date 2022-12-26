#ifndef LAND_H_
#define LAND_H_

#include "../server/ships.h"
#include "../server/vars.h"

void land(int, int, int, int, orbitinfo *);
int crash(shiptype *, double);
int docked(shiptype *);
int overloaded(shiptype *);
void go_thru_wormhole(shiptype *);

#endif /* LAND_H_ */
