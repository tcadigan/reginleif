/*
 * nap.c
 * Larn is copyrighted 1986 by Noah Morgan.
 */

#include "nap.h"

#include "io.h"

#include <curses.h>
#include <unistd.h>

/*
 * Routine to take a nap for n milliseconds
 */
void nap(int x)
{
    /* Eliminate chance for infinite loop */
    if(x <= 0) {
	return;
    }

    lflush();

    if(x > 999) {
	sleep(x / 1000);
    }
    else {
	napms(x);
    }
}
