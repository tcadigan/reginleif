#ifndef BUILD_H_
#define BUILD_H_

#include "races.h"
#include "ships.h"
#include "vars.h"

void upgrade(int, int, int, int, orbitinfo *);
void make_mod(int, int, int, int, orbitinfo *);
void build(int, int, int, int, orbitinfo *);
int getcount(int, char *);
int can_build_at_planet(int, int, startype *, planettype *);
int get_build_type(char *);
int can_build_this(int, racetype *, char *);
int can_build_on_ship(int, racetype *, shiptype *, char *);
int can_build_on_sector(int, racetype *, planettype *, sectortype *, int, int,
                        char *);
int build_at_ship(int, int, racetype *, shiptype *, int *, int *);
void autoload_at_planet(int, shiptype *, planettype *, sectortype *, int *,
                        double *);
void autoload_at_ship(int, shiptype *, shiptype *, int *, double *);
void initialize_new_ship(int, int, racetype *, shiptype *, double, int, int);
int create_ship_by_planet(int, int, racetype *, shiptype *, planettype *, int,
                          int, int, int);
int create_ship_by_ship(int, int, racetype *, int, startype *, planettype *,
                        shiptype *, shiptype *);
double getmass(shiptype *);
int ship_size(shiptype *);
double cost(shiptype *);
void system_cost(double *, double *, int, int);
double complexity(shiptype *);
void Getship(shiptype *, int, racetype *);
void Getfactship(shiptype *, shiptype *);
int Shipcost(int, racetype *);
void sell(int, int, int, int, orbitinfo *);
void bid(int, int, int, int, orbitinfo *);
int shipping_cost(int, int, double *, int);
int inship(shiptype *);

#ifdef MARKET
extern char const *Commod[];
#endif

#endif /* BUILD_H_ */
