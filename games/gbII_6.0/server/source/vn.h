#ifndef VN_H_
#define VN_H_

#include "ships.h"
#include "vars.h"

void do_vn(shiptype *);
void planet_do_vn(shiptype *, planettype *);
void vn_brain(int, int, int, int, orbitinfo *);
void vn_mad();

#endif /* VN_H_ */
