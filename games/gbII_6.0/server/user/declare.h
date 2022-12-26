#ifndef DECLARE_H_
#define DECLARE_H_

#include "../server/vars.h"

void invite(int, int, int, int, orbitinfo *);
void pledge(int, int, int, int, orbitinfo *);
void declare(int, int, int, int, orbitinfo *);
void vote(int, int, int, int, orbitinfo *);
void show_votes(int, int);

#endif /* DECLARE_H_ */
