#ifndef PROF_H_
#define PROF_H_

#include "../server/races.h"
#include "../server/vars.h"

void whois(int, int, int, int, orbitinfo *);
void treasury(int, int, int, int, orbitinfo *);
void profile(int, int, int, int, orbitinfo *);
char *Estimate_f(double, racetype *, int);
char *Estimate_i(int, racetype *, int);
int round_perc(int, racetype *, int);

#endif /* PROF_H_ */
