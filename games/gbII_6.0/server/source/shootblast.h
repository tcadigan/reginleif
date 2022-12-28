#ifndef SHOOTBLAST_H_
#define SHOOTBLAST_H_

#include "races.h"
#include "ships.h"

int shoot_ship_to_ship(shiptype *, shiptype *, int, int, int, char *,
                                  char *);
int shoot_planet_to_ship(racetype *, planettype *, shiptype *, int,
                                    char *, char *);
int shoot_ship_to_planet(shiptype *, planettype *, int, int, int,
                                    int, int, int, char *, char *);
int d_radiation(shiptype *, double, int, int, char const *, char *);
int do_damage(int, shiptype *, double, int, int, int, int, double,
                         char const *, char *);
void ship_disposition(shiptype *, int *, int *, int *);
int CEW_hit(double, int);
int Num_hits(double, int, int, double, int, int, int, int, int, int,
                        int, int);
int hit_odds(double, int *, double, int, int, int, int, int, int,
                        int, int);
int cew_hit_odds(double, int);
double gun_range(racetype *, shiptype *, int);
double tele_range(int, double);
int current_caliber(shiptype *);
void do_critical_hits(int, shiptype *, int *, int *, int, char *);
void do_collateral(shiptype *, int, int *, int *, int *, int *);
int getdefense(shiptype *);
double p_factor(double, double);
int planet_guns(unsigned int);
void mutate_sector(sectortype *);

#endif /* SHOOTBLAST_H_ */
