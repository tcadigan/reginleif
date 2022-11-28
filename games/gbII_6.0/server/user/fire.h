#ifndef FIRE_H_
#define FIRE_H_

void fire(int, int, int, int);
void bombard(int, int, int);
void defend(int, int, int);
void detonate(int, int, int);
int retal_strength(shiptype *);
int adjacent(int, int, int, int, planettype *);
int landed(shiptype *);
void check_overload(shiptype *, int, int *);
void check_retal_strength(shiptype *, int *);
int laser_on(shiptype *);

#endif /* FIRE_H_ */
