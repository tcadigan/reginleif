#ifndef GETPLACE_H_
#define GETPLACE_H_

#include "ships.h"

placetype Getplace(int, int, char const *, int);
placetype Getplace2(int, int, char const *, placetype *, int, int);
char *Dispshiploc_brief(shiptype *);
char *Dispshiploc(shiptype *);
char const *Dispplace(int, int, placetype *);
int testship(int, int, shiptype *);
char const *Dispplace_brief(int, int, placetype *);

#endif /* GETPLACE_H_ */
