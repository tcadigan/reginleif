/*
 * moreobj.c
 * Larn is copyrighted 1986 by Noah Morgan.
 *
 * Routings in this file:
 *
 * oaltar()
 * othrone()
 * ochest()
 * ofountain()
 */

#include "header.h"

/*
 * ******
 * OALTAR
 * ******
 *
 * Subroutine to process an altar object
 */
void oaltar()
{
    unsigned long k;

    lprcat("\nDo you (p) pray (d) desecrate");
    iopts();

    while(1) {
	while(1) {
	    switch(getchar()) {
	    case 'p':
		lprcat(" pray\nDo you (m) give money or (j) just pray? ");

		while(1) {
		    switch(getchar()) {
		    case 'j':
			if(rnd(100) < 75) {
			    lprcat("\nnothing happens");
			}
			else if(rnd(13) < 4) {
			    if(c[WEAR]) {
				lprcat("\nYou feel your armor vibrate for a moment");
			    }

			    enchantarmor();
			    
			    return;
			}
			else if(rnd(43) == 10) {
			    if(c[WIELD]) {
				lprcat("\nYou feel your weapon vibrate for a moment");
			    }

			    enchweapon();

			    return;
			}
			else {
			    createmonster(makemonst(level + 1));
			}

			return;
		    case 'm':
			lprcat("\n\n");
			cursor(1, 24);
			cltoeoln();
			cursor(1, 23);
			cltoeoln();
			lprcat("how much do you donate? ");

			k = readnum((long)c[GOLD]);

			if(c[GOLD] < k) {
			    lprcat("\nYou don't have that much!");

			    return;
			}

			c[GOLD] -= k;

			if((k < (c[GOLD] / 10)) || (k < rnd(50))) {
			    createmonster(makemonst(level + 1));
			    c[AGGRAVATE] += 200;
			}
			else if(rnd(101) > 50) {
			    ohear();

			    return;
			}
			else if(rnd(43) == 5) {
			    if(c[WEAR]) {
				lprcat("\nYou feel your armor vibrate for a moment");
			    }

			    enchantarmor();

			    return;
			}
			else if(rnd(43) == 8) {
			    if(c[WIELD]) {
				lprcat("\nYou feel your weapon vibrate for a moment");
			    }

			    enchweapon();

			    return;
			}
			else {
			    lprcat("\nThank you.");
			}

			bottomline();

			return;
		    case '\33':

			return;
		    }
		}

	    case 'd':
		lprcat(" desecrate");

		if(rnd(100) < 60) {
		    createmonster(makemonst(level + 2) + 8);
		    c[AGGRAVATE] += 2500;
		}
		else if(rnd(101) < 30) {
		    lprcat("\nThe altar crumbles into a pile of dust before your eyes");

		    /* Remeber to destroy the altar */
		    forget();
		}
		else {
		    lprcat("\nnothing happens");
		}

		return;
	    case 'i':
	    case '\33':
		ignore();

		if(rnd(100) < 30) {
		    createmonster(makemonst(level + 1));
		    c[AGGRAVATE] += rnd(450);
		}
		else {
		    lprcat("\nnothing happens");
		}

		return;
	    }
	}
    }
}

/*
 * Function to case a +3 protection on the player
 */
static void ohear()
{
    lprcat("\nYou have been heard!");

    if(c[ALTPRO] == 0) {
	c[MOREDEFENSES] += 3;

	/* Protection field */
	c[ALTPRO] += 500;
	bottomline();
    }
}

/*
 * *******
 * OTHRONE
 * *******
 *
 * Subroutine to process a throne object
 */
void othrone(int arg)
{
    int i;
    int k;

    lprcat("\nDo you (p) pry off jewels, (s) sit down");
    iopts();

    while(1) {
	while(1) {
	    switch(getchar()) {
	    case 'p':
		lprcat(" pry off");
		k = rnd(101);

		if(k < 25) {
		    for(i = 0; i < rnd(4); ++i) {
			/* Gems pop off the throne */
			creategem();
		    }

		    item[playerx][playery] = ODEADTHRONE;
		    know[playerx][playery] = 0;
		}
		else if((k < 40) && (arg == 0)) {
		    createmonster(GNOMEKING);
		    item[playerx][playery] = OTHRONE2;
		    know[players][playery] = 0;
		}
		else {
		    lprcat("\nnothing happens");
		}

		return;
	    case 's':
		lprcat(" sit down");
		k = rnd(101);

		if((k < 30) && (arg == 0)) {
		    createmonster(GNOMEKING);
		    item[playerx][playery] = OTHRONE2;
		    know[playerx][playery] = 0;
		}
		else if(k < 35) {
		    lprcat("\nZaaaapp! You've been teleported!\n");
		    beep();
		    oteleport(0);
		}
		else {
		    lprcat("\nnothing happens");
		}

		return;
	    case 'i':
	    case '\33':
		ignore();

		return;
	    }
	}
    }
}

void odeadthrone()
{
    int k;

    lprcat("\nDo you (s) sit down");
    iopts;

    while(1) {
	while(1) {
	    switch(getchar()) {
	    case 's':
		lprcat(" sit down");
		k = rnd(101);

		if(k < 35) {
		    lprcat("\nZaaaappp! You've been teleported!\n");
		    beep();
		    oteleport(0);
		}
		else {
		    lprcat("\nnothing happens");
		}

		return;
	    case 'i':
	    case '\33':
		ignore();

		return;
	    }
	}
    }
}

/*
 * ******
 * OCHEST
 * ******
 *
 * Subroutine to process a throne object
 */
void ochest()
{
    int i;
    int k;

    lprcat("\nDo you (t) take it, (o) try to open it");
    iopts;

    while(1) {
	while(1) {
	    switch(getchar()) {
	    case 'o':
		lprcat(" open it");
		k = rnd(101);

		if(k < 40) {
		    lprcat("\nThe chest explodes as you open it");
		    beep();
		    i = rnd(10);

		    /* In case he dies */
		    lastnum = 281;

		    lprintf("\nYou suffer %d hit points damage!", i);
		    checkloss(i);

		    /* See if he gets a curse */
		    switch(rnd(10)) {
		    case 1:
			c[ITCHING] += (rnd(1000) + 100);
			lprcat("\nYou feel an irritation spread over your skin!");
			beep();

			break;
		    case 2:
			c[CLUMSINESS] += (rnd(1600) + 200);
			lprcat("\nYou begin to lose hand eye coordination!");
			beep();
			
			break;
		    case 3:
			c[HALFDAM] += (rnd(1600) + 200);
			beep();
			lprcat("\nA sickness engulfs you!");

			break;
		    }

		    know[playerx][playery] = 0;
		    item[playerx][playery] = know[playerx][playery];

		    /* Gems from the chest */
		    dropgold(rnd((110 * iarg[playerx][playery]) + 200));

		    for(i = 0; i < rnd(4); ++i) {
			something(iarg[playerx][playery] + 2);
		    }
		}
		else {
		    lprcat("\nnothing happens");
		}

		return;
	    case 't':
		lprcat(" take");

		if(take(OCHEST, iarg[playerx][playery]) == 0) {
		    know[playerx][playery] = 0;
		    item[playerx][playery] = know[playerx][playery];
		}

		return;
	    case 'i':
	    case '\33':
		ignore();

		return;
	    }
	}
    }
}

/*
 * *********
 * OFOUNTAIN
 * *********
 */
void ofountain()
{
    int x;

    cursors();
    lprcat("\nDo you (d) drink, (w) wash yourself");
    iopts();

    while(1) {
	while(1) {
	    switch(getchar()) {
	    case 'd':
		lprcat("drink");
		
		if(rnd(1501) < 2) {
		    lprcat("\nOops! You seem to have caught the dreadful sleep!");
		    beep();
		    lflush();
		    sleep(3);
		    died(280);
		    
		    return;
		}
		
		x = rnd(100);
		
		if(x < 7) {
		    c[HALFDAM] += (200 + rnd(200));
		    lprcat("\nYou feel a sickness coming on");
		}
		else if(x < 13) {
		    /* See invisible */
		    quaffpotion(23);
		}
		else if(x < 45) {
		    lprcat("\nnothing seems to have happened");
		}
		else if(rnd(3) != 2) {
		    /* Change char levels upward */
		    fntchange(1);
		}
		else {
		    /* Change char levels downward */
		    fntchange(-1);
		}
		
		if(rnd(12) < 3) {
		    lprcat("\nThe fountains bubbling slowly quiets");
		    
		    /* Dead fountain */
		    item[playerx][playery] = ODEADFOUNTAIN;
		    
		    know[playerx][playery] = 0;
		}
		
		return;
	    case '\33':
	    case 'i':
		ignore();
		
		return;
	    case 'w':
		lprcat("wash yourself");
		
		if(rnd(100) < 11) {
		    x = rnd((level << 2) + 2);
		    lprintf("\nOh no! The water was foul! You suffer %d hit points!", x);
		    lastnum = 273;
		    losehp(x);
		    bottomline();
		    cursors();
		}
		else if(rnd(100) < 29) {
		    lprcat("\nYou got the dirt off!");
		}
		else if(rnd(100) < 31) {
		    lprcat("\nThis water seems to be hard water! The dirt didn't come off!");
		}
		else if(rnd(100) < 34) {
		    /* Make water lord */
		    createmonster(WATERLORD);
		}
		else {
		    lprcat("\nnothing seems to have happened");
		}
		
		return;
	    }
	}
    }
}

/*
 * A subroutine to raise or loewr character levels.
 * If x > 0 they are raised.
 * If x < 0 they are lowered.
 */
void fntchange(int how)
{
    long j;

    lprc('\n');

    switch(rnd(9)) {
    case 1:
	lprcat("Your strength");
	fch(how, &c[0]);

	break;
    case 2:
	lprcat("Your intelligence");
	fch(how, &c[1]);

	break;
    case 3:
	lprcat("Your wisdom");
	fch(how, &c[2]);

	break;
    case 4:
	lprcat("Your constitution");
	fch(how, &c[3]);

	break;
    case 5:
	lprcat("Your dexterity");
	fch(how, &c[4]);

	break;
    case 6:
	lprcat("Your charm");
	fch(how, &c[5]);

	break;
    case 7:
	j = rnd(level + 1);

	if(how < 0) {
	    lprintf("You lost %d hit point", j);

	    if(j > 1) {
		lprcat("s!");
	    }
	    else {
		lprc('!');
	    }

	    losemhp((int)j);
	}
	else {
	    lprintf("You gain %d hit point", j);
	    
	    if(j > 1) {
		lprcat("s!");
	    }
	    else {
		lprc('!');
	    }

	    raisemhp((int)j);
	}

	bottomline();

	break;
    case 8:
	j = rnd(level + 1);

	if(how > 0) {
	    lprintf("You just gained %d spell", j);
	    raisemspells((int)j);

	    if(j > 1) {
		lprcat("s!");
	    }
	    else {
		lprc('!');
	    }
	}
	else {
	    lprintf("You just lost %d spell", j);
	    losemspells((int)j);

	    if(j > 1) {
		lprcat("s!");
	    }
	    else {
		lprc('!');
	    }
	}

	bottomline();

	break;
    case 9:
	j = 5 * rnd((level + 1) * (level + 1));

	if(how < 0) {
	    lprintf("You just lost %d experience point", j);

	    if(j > 1) {
		lprcat("s!");
	    }
	    else {
		lprc('!');
	    }

	    loseexperience(j);
	}
	else {
	    lprintf("You just gained %d experience point", j);

	    if(j > 1) {
		lprcat("s!");
	    }
	    else {
		lprc('!');
	    }

	    raiseexperience(j);
	}

	break;
    }

    cursors();
}

/*
 * ***
 * FCH
 * ***
 * 
 * Subroutine to process an up/down of a character attribute for ofountain
 */
static void fch(int how, long *x)
{
    if(how < 0) {
	lprcat(" went down by one!");
	--(*x);
    }
    else {
	lprcat(" went up by one!");
	(*x)++;
    }

    bottomline();
}
