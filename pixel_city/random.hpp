#ifndef RANDOM_HPP_
#define RANDOM_HPP_

#define COIN_FLIP (RandomVal(2) == 0)

unsigned long RandomVal(int range);
unsigned long RandomVal(void);
void RandomInit(unsigned long seed);

#endif /* RANDOM_HPP_ */
