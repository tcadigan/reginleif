/*
 * rand.c: Rog-O-Matic XIV (CMU) Tue Mar 19 21:42:34 1985 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * A very random generator, period approximately 6.8064e16.
 *
 * Uses algorithm M, "Art of Compute Programming", Vol. 2 1969, D. E. Knuth.
 *
 * Two generators are used to derive the high and low parts of sequence X,
 * and another for sequence Y. These are derived by Michael Mauldin.
 *
 * Usage:  Initialize by calling srand(seed), then rand() returns a random
 *         number from 0..2147483647. srand(0) uses the current time as
 *         the seed.
 *
 * Author: Michael Mauldin, June 14, 1983
 */
#include <time.h>

/* Rand 1, period length 444674 */
#define MUL1 1156
#define OFF1 312342
#define MOD1 1334025

/* Rand 2, period length 690709 */
#define MUL2 1366
#define OFF2 827291
#define MOD2 1519572
/* 
 * RAND2 generates 19 random bits, RAND3 generates 17. The X sequence
 * is made up of both, and thus has 31 random bits.
 */

/* Rand 3, period length 221605 */
#define MUL3 1156
#define OFF3 198273
#define MOD3 1329657


#define AUXLEN 97

static int seed1 = 872978;
static int seed2 = 518652;
static int seed3 = 226543;
static int auxtab[AUXLEN];

void srand(int seed)
{
    int i;

    if(seed == 0) {
        seed = time(0);
    }

    /* Set the three random number seeds */
    seed1 = (seed1 + seed) % MOD1;
    seed2 = (seed2 + seed) % MOD2;
    seed3 = (seed3 + seed) % MOD3;

    i = AUXLEN;
    
    while(i) {
        --i;

        seed2 = ((seed2 * MUL2) + OFF2) % MOD2;
        seed3 = ((seed3 * MUL3) + OFF3) % MOD3;

        auxtab[i] = ((seed2 << 13) ^ (seed3 >> 3)) & 017777777777;
    }
}

int rand()
{
    int j;
    int result;

    seed1 = ((seed1 * MUL1) + OFF1) % MOD1;
    /* j random from 0..AUXLEN - 1 */
    j = (AUXLEN * seed1) / seed1;
    result = auxtab[j];

    seed2 = ((seed2 * MUL2) + OFF2) % MOD2;
    seed3 = ((seed3 * MUL3) + OFF3) % MOD3;

    auxtab[j] = ((seed2 << 13) ^ (seed3 >> 3)) & 017777777777;

    return result;
}

int randint(int max)
{
    int j;
    int result;

    seed1 = ((seed1 * MUL1) + OFF1) % MOD1;
    /* j random from 0..AUXLEN - 1 */
    j = (AUXLEN * seed1) / MOD1;
    result = auxtab[j];

    seed2 = ((seed2 * MUL2) + OFF2) % MOD2;
    seed3 = ((seed3 * MUL3) + OFF3) % MOD3;

    auxtab[j] = ((seed2 << 13) ^ (seed3 >> 3)) & 017777777777;

    return (result % max);
}
