/*
 * The Mersenne Twister by Matsumoto and Nishimura <matumoto@math.keio.ac.jp>
 * It sets new standards for the period, quality and speed of random number
 * generators. The incredible period is 2^19937 - 1, a number with about 6000
 * digits; the 32-bit random numbers exhibit best possible equidistribution
 * properties in dimensions up to 623; and it's fast, very fast.
 *
 */

#include "random.hpp"

#include <random>

static std::mt19937 generator;

unsigned long random_val(void)
{
    return generator();
}

unsigned long random_val(int range)
{
    return (range ? (random_val() % range) : 0);
}

void random_init(unsigned long seed)
{
    generator.seed(seed);
}
