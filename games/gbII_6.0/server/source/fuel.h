#ifndef FUEL_H_
#define FUEL_H_

#include "vars.h"

void proj_fuel(int, int, int, int, orbitinfo *);
void fuel_output(int, int, double, double, double, double, int);
int do_trip(double, double);

#endif /* FUEL_H_ */
