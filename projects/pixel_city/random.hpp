#ifndef RANDOM_HPP_
#define RANDOM_HPP_

#define COIN_FLIP (random_val(2) == 0)

unsigned long random_val(int range);
unsigned long random_val();
void random_init(unsigned long seed);

#endif /* RANDOM_HPP_ */
