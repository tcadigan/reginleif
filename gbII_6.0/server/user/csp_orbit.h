#ifndef CSP_ORBIT_H_
#define CSP_ORBIT_H_

void csp_orbit(int, int, orbitinfo *);
void csp_DispStar(int, int, int, int, startype *, int, char *);
void csp_DispPlanet(int, int, int, int, int planettype *, char *,
                               racetype *, char *);
void csp_DispShip(int, int, placetype *, shiptype *, planettype *,
                             int char *);

#endif /* CSP_ORBIT_H_ */
