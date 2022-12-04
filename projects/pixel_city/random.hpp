#ifndef RANDOM_HPP_
#define RANDOM_HPP_

#define COIN_FLIP (randomVal(2) == 0)

unsigned long randomVal(int range);
unsigned long randomVal(void);
void randomInit(unsigned long seed);

#endif /* RANDOM_HPP_ */
