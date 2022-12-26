#ifndef RELATION_H_
#define RELATION_H_

#include "races.h"
#include "vars.h"

void relation(int, int, int, int, orbitinfo *);
void csp_relation(int, int);
char const *allied(racetype *, int, int, int);
int isallied(racetype *, int, int, int);

#endif /* RELATION_H_ */
