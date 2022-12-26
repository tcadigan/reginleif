#ifndef DOSECTOR_H_
#define DOSECTOR_H_

#include "vars.h"

void produce(startype *, planettype *, sectortype *);
void spread(planettype *, sectortype *, int, int);
void Migrate2(planettype *, int, int, sectortype *, int*);
void explore(planettype *, sectortype *, int, int, int);
void plate(sectortype *);

#endif /* DOSECTOR_H_ */
