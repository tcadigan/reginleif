#ifndef DOPLANET_H_
#define DOPLANET_H_

#include "ships.h"
#include "vars.h"

int doplanet(int, planettype *, int);
int moveship_onplanet(shiptype *, planettype *);
void terraform(shiptype *, planettype *);
void do_plow(shiptype *, planettype *);
void do_dome(shiptype *, planettype *);
void do_quarry(shiptype *, planettype *);
void do_recover(planettype *, int, int);
double est_production(sectortype *);
void squeeze_target(shiptype *);
int is_target(shiptype *, int, int, int);
void do_inf(shiptype *, planettype *);

/* New code by Kharush. */
void check_overflow(planettype *, int, int, unsigned char, int);

#endif /* DOPLANET_H_ */
