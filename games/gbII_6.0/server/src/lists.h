#ifndef LISTS_H_
#define LISTS_H_

#include "ships.h"
#include "vars.h"

void insert_sh_univ(struct stardata *, shiptype *);
void insert_sh_star(startype *, shiptype *);
void insert_sh_plan(planettype *, shiptype *);
void insert_sh_ship(shiptype *, shiptype *);
void remove_sh_star(shiptype *);
void remove_sh_plan(shiptype *);
void remove_sh_ship(shiptype *, shiptype *);
double Getcomplexity(int);
int ShipCompare(void const *, void const *);
void SortShips(void);
void insert_sh_fleet(int, int, shiptype *, int);
void remove_sh_fleet(int, int, shiptype *);

extern int ShipVector[NUMSTYPES];

#endif /* LISTS_H_ */
