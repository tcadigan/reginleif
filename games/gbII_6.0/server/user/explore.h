#ifndef EXPLORE_H_
#define EXPLORE_H_

#include "../server/races.h"

void colonies_at_star(int, int, racetype *, int, int);
void colonies(int, int, int, int);
void distance(int, int, int);
void star_locations(int, int, int);
void exploration(int, int, int);
void tech_status(int, int, int);
void tech_report_star(int, int, startype *, int, int *, double *,
                                 double *);

#endif /* EXPLORE_H_ */
