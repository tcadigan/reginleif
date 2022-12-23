#ifndef PROF_H_
#define PROF_H_

#include "../server/races.h"

void whois(int, int, int);
void treasury(int, int);
void profile(int, int, int);
char *Estimate_f(double, racetype *, int);
char *Estimate_i(int, racetype *, int);
int round_perc(int, racetype *, int);

#endif /* PROF_H_ */
