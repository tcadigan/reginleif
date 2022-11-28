#ifndef DOSHIP_H_
#define DOSHIP_H_

void doship(shiptype *, int);
void doloc(shiptype *);
void domass(shiptype *);
void domass(shiptype *);
void doown(shiptype *);
void domissile(shiptype *);
void do_mine(int, int);
void do_sweeper(int);
void doabm(shiptype *);
void do_repair(shiptype *);
void do_habitat(shiptype *);
void do_canister(shiptype *);
void do_greenhouse(shiptype *);
void do_mirror(shiptype *);
void do_god(shiptype *);
int do_ap(shiptype *);
void gastype(int, char *);
double crew_factor(shiptype *);
double ap_planet_factor(planettype *);
void do_oap(shiptype *);
int do_weapon_planet(shiptype *, unsigned int *, unsigned int *);
void do_seti(shiptype *);
int kill_ship(int, shiptype *);
void do_omcl(shiptype *);

#endif /* DOSHIP_H_ */
