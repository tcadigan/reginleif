#ifndef MOVESHIP_H_
#define MOVESHIP_H_

#include "ships.h"
#include "vars.h"

void Moveship(shiptype *, int, int, int);
void msg_OOF(shiptype *);
int followable(shiptype *, shiptype *);
int do_merchant(shiptype *, planettype *);
int clearhyper(shiptype *);

#endif /* MOVESHIP_H_ */
