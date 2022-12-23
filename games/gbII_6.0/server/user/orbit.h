#ifndef ORBIT_H_
#define ORBIT_H_

#include "../server/races.h"
#include "../server/ships.h"
#include "../server/vars.h"

void orbit(int, int, int);
void DispStar(int, int, int, startype *, int, char *, orbitinfo *);
void DispPlanet(int, int, int, planettype *, char *, racetype *,
                           char *, orbitinfo *);
void DispShip(int, int, placetype *, shiptype *, planettype *, int,
                         char *, orbitinfo *);

#endif /* ORBIT_H_ */
