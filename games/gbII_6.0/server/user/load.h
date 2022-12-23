#ifndef LOAD_H_
#define LOAD_H_

#include "../server/races.h"
#include "../server/ships.h"

void load(int, int, int, int);
void jettison(int, int, int);
int jettison_check(int, int, int, int);
void dump(int, int, int);
void transfer(int, int, int);
void mount(int, int, int);
void dismount(int, int, int);
void _mount(int, int, int, int);
void use_fuel(shiptype *, double);
void use_destruct(shiptype *, int);
void use_resource(shiptype *, int);
void use_popn(shiptype *, int, double);
void rcv_fuel(shiptype *, double);
void rcv_resource(shiptype *, int);
void rcv_destruct(shiptype *, int);
void rcv_popn(shiptype *, int, double);
void rcv_troops(shiptype *, int, double);
void do_transporter(racetype *, int, shiptype *);
int landed_on(shiptype *, int);
void unload_onto_alien_sector(int, int, planettype *, shiptype *,
                              sectortype *, int, int);

#endif /* LOAD_H_ */
