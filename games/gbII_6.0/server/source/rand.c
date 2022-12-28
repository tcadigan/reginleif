/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * Random number generator
 *   double double_rand() - This returns a random number between 0 and 1
 *   int int_rand(low, hi) - This returns an integer random number between low
 *                           and hi, inclusive.
 *   int round_rand(double) - Returns double rounded to integer, with
 *                            proportional chance of rounding up or down.
 *   int rposneg() - Either -1 or 1
 *
 * #ident  "@(#)rand.c  1.5 11/5/93 "
 *
 * $Header: /var/cvs/gbp/GB+/server/rand.c,v 1.4 2007/07/06 17:30:26 gbp Exp $
 *
 * static char *ver = "@(#)       $RCSfile: rand.c,v $ $Revision: 1.4 $";
 */
#include "rand.h"

#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "tweakables.h"

double double_rand(void);
int int_rand(int, int);
int round_rand(double);
int rposneg(void);

static int firsttime = 1;

void init_rand(void)
{
#ifdef RAND
    int i;
    int j;

    srand(time(NULL) + (5 * getpid()));
    j = rand() / 32000;

    for (i = 0; i < j; ++i) {
        rand();
    }

#elif RANDOM
    int i;
    int j;
    srandom(time(NULL) + (5 * getpid()));
    j = random() / 32000;

    for (i = 0; i < j; ++i) {
        random();
    }

#elif RAND48
    int i;
    int j;

    srand48(time(NULL) + (5 * getpid()));
    j = lrand48() / 3200;

    for (i = 0; i < j; ++i) {
        lrand48();
    }

#else
#error "Must choose pseudo-random number generator, see tweakables.h"

#endif
}

double double_rand(void)
{
    if (firsttime) {
        init_rand();
        firsttime = 0;
    }

#ifdef RAND
    return ((double)rand() / (RAND_MAX + 1.0));

#elif RANDOM
    return ((double)random() / (RAND_MAX + 1));

#elif RAND48
    return drand48();

#else
#error "Must choose pseudo-random number generator, see tweakables.h"

#endif
}

int gb_rand(void)
{
    if (firsttime) {
        init_rand();
        firsttime = 0;
    }

#ifdef RAND
    return rand();

#elif RANDOM
    return random();

#elif RAND48
    return lrand48();

#else
#error "Must choose pseudo-random number generator, see tweakables.h"

#endif
}

int int_rand(int low, int hi)
{
    if (firsttime) {
        init_rand();
        firsttime = 0;
    }

#ifdef RAND
    if (hi <= low) {
        return low;
    } else {
        return ((rand() % (hi - low + 1)) + low);
    }

#elif RANDOM
    if (hi <= low) {
        return low;
    } else {
        return ((random() % (hi - low + 1)) + low);
    }

#elif RAND48
    if (hi <= low) {
        return low;
    } else {
        return ((lrand48() % (hi - low + 1)) + low);
    }

#else
#error "Must choose pseudo-random number generator, see tweakables.h"

#endif
}

int round_rand(double x)
{
    if (firsttime) {
        init_rand();
        firsttime = 0;
    }

    if (double_rand() > (x - (double)(int)x)) {
        return (int)x;
    } else {
        return (int)(x + 1);
    }
}

int rposneg(void)
{
    if (firsttime) {
        init_rand();
        firsttime = 0;
    }

#ifdef RAND
    if (rand() & 256) {
        return -1;
    } else {
        return 1;
    }

#elif RANDOM
    if (random() & 256) {
        return -1;
    } else {
        return 1;
    }

#elif RAND48
    if (mrand48() < 0) {
        return -1;
    } else {
        return 1;
    }

#else
    #error "Must choose pseudo-random number generator, see tweakables.h"

#endif
}
