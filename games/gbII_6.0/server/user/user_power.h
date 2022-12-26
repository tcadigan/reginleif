#ifndef __USER_POWER_H__
#define __USER_POWER_H__

#include "../server/races.h"

void block(int, int, int, int, orbitinfo *);
void user_power(int, int, int, int, orbitinfo *);
void prepare_output_line(racetype *, racetype *, int, int);

#endif /* POWER_H_ */
