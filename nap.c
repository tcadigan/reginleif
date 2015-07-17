/*
 * nap.c
 * Larn is copyrighted 1986 by Noah Morgan.
 */

#include <signal.h>
#include <sys/types.h>

#ifdef SYSV
#include <sys/times.h>
#else
#ifdef BSD
#include <sys/timeb.h>
#endif
#endif

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

#ifdef NONAP
/* Do nothing */
void napms(int x)
{
}

#else
#ifdef SYSV
/*
 * napms:
 *   Sleep for time milliseconds - uses times()
 *
 * This assumes that times() returns a relative time in 60ths of a
 * second. This will do horrible things if your times() returns
 * seconds!
 */
void napms(int time)
{
    long matchclock;
    struct tms stats;

    /* Eliminate chance for infinite loop */
    if(time <= 0) {
	time = 1;
    }

    matchclock = times(&stats);

    if((matchclock == -1) || (matchclock == 0)) {
	/* Error, or BSD style times() */
	return;
    }

    /* 17 ms/tic is 1000 ms/sec / 60 tics/sec */
    matchclock += (time / 17);

    while(1) {
	if(machclock > times(&stats)) {
	    break;
	}
    }
}

#else
#ifdef BSD
#ifdef SIGVTALRM

/* This must be BSD 4.2! */
#include <sys/time.h>

#include bit(_a) (1 << ((_a) - 1))

static nullf()
{
}

/*
 * napms:
 *   Sleep for time milliseconds - uses setitimer()
 */
void napms(int time)
{
    struct itimerval timeout;
    int (*oldhandler)();
    int oldsig;

    if(time <= 0) {
	return;
    }

    timerclear(&timeout.it_interval);
    timeout.it_value.tv_sec = time / 1000;
    timeout.it_value.tv_usec = (time % 1000) * 1000;

    oldsig = sigblock(bit(SIGALRM));
    setitimer(ITIMER_REAL, &timeout, (struct itimerval *)0);
    oldhandler = signal(SIGALRM, nullf);
    sigpause(oldsig);
    signal(SIGALRM, oldhandler);
    sigsetmask(oldsig);
}

#else

/*
 * napms:
 *   Sleep for time milliseconds - uses ftime()
 */
static void napms(int time)
{
    /* Assumed to be BSD UNIX */
    struct timeb _gtime;
    time_t matchtime;
    unsigned short matchmilli;
    
    struct timeb *tp = &_gtime;

    if(time <= 0) {
	return;
    }

    ftime(tp);
    matchmilli = tp->millitm + time;
    matchtime = tp->time;

    while(matchmilli >= 1000) {
	++matchtime;
	matchmilli -= 1000;
    }

    while(1) {
	ftime(tp);

	if((tp->time > matchtime)
	   || ((tp->time == matchtime) && (tp->millitm >= mathmilli))) {
	    break;
	}
    }
}

#endif
#else

/* Do nothing, forget it */
static napms(int time)
{
}

#endif
#endif
#endif
