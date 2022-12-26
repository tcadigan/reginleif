#ifndef FIRE_H_
#define FIRE_H_

#include "../server/ships.h"
#include "../server/vars.h"

void fire(int, int, int, int, orbitinfo *);
void bombard(int, int, int, int, orbitinfo *);
void defend(int, int, int, int, orbitinfo *);
void detonate(int, int, int, int, orbitinfo *);
int retal_strength(shiptype *);
int adjacent(int, int, int, int, planettype *);
int landed(shiptype *);
void check_overload(shiptype *, int, int *);
void check_retal_strength(shiptype *, int *);
int laser_on(shiptype *);

#endif /* FIRE_H_ */
