#ifndef CSP_DUMP_H_
#define CSP_DUMP_H_

#include "vars.h"

void csp_planet_dump(int, int, int, int, orbitinfo *);
void csp_ship_dump(int, int, int, int, orbitinfo *);
void csp_ship_report(int, int, int, unsigned char[], int);
void CSP_star_dump(int, int, int, int, orbitinfo *);
void CSP_ship_list(int, int, int, int, orbitinfo *);
void CSP_sectors(int, int, int, int, orbitinfo *);
void csp_univ_dump(int, int, int, int, orbitinfo *);

#endif /* CSP_DUMP_H_ */
