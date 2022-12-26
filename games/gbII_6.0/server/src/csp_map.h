#ifndef CSP_MAP_H_
#define CSP_MAP_H_

#include "races.h"
#include "vars.h"

void CSP_map(int, int, int, int, planettype *);
int gettype(planettype *, int, int);
int getowner(planettype *, int, int);
char getsymbol(planettype *, int, int, racetype *, int);

#endif /* CSP_MAP_H_ */
