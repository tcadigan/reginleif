/*
 * movem.c
 * Larn is copyrighted 1986 by Noah Morgan.
 *
 * Here are the functions in this file:
 *
 * movemonst()     Routine to move the monsters toward the player
 * movemt()        Function to move a monster at (x, y) -- must determine where
 * mmove()         Function to actually perform the monster movement
 * movesphere()    Function to look for and move spheres of annihilation
 */

#include "movem.h"

#include "header.h"

/*
 * movemonst()
 *
 * Routine to move the monsters toward the player
 *
 * This routing has the responsibility to determine which monsters are
 * to move, and call movemt() to do the move.
 *
 * Returns no value.
 */

static short w1[9];
static short w1x[9];
static short w1y[9];
static int tmp1;
static int tmp2;
static int tmp3;
static int tmp4;
static int distance;

void movemonst()
{
    int i;
    int j;

    /* No action if time is stopped */
    if(c[TIMESTOP]) {
	return;
    }

    if(c[HASTESELF]) {
	if((c[HASTESELF] & 1) == 0) {
	    return;
	}
    }

    /* Move the speheres of annihilation if any */
    if(spheres) {
	movesphere();
    }

    /* No action if monsters are held */
    if(c[HOLDMONST]) {
	return;
    }

    /* Determine window of monsters to move */
    if(c[AGGRAVATE]) {
	tmp1 = playery - 5;
	tmp2 = playery + 6;
	tmp3 = playerx - 10;
	tmp4 = playerx + 11;

	/* Depth of intelligent monster movement */
	distance = 40;
    }
    else {
	tmp1 = playery - 3;
	tmp2 = playery + 4;
	tmp3 = playerx - 10;
	tmp4 = playerx + 11;

	/* Depth of intelligent monster movement */
	distance = 17;
    }

    /* If on outside level monster can move in perimeter */
    if(level == 0) {
	if(tmp1 < 0) {
	    tmp1 = 0;
	}

	if(tmp2 > MAXY) {
	    tmp2 = MAXY;
	}

	if(tmp3 < 0) {
	    tmp3 = 0;
	}

	if(tmp4 > MAXX) {
	    tmp4 = MAXX;
	}
    }
    else { /* If in a dungeon monsters can't be on the perimeter (wall there) */
	if(tmp1 < 1) {
	    tmp1 = 1;
	}

	if(tmp2 > (MAXY - 1)) {
	    tmp2 = MAXY - 1;
	}

	if(tmp3 < 1) {
	    tmp3 = 1;
	}

	if(tmp4 > (MAXX - 1)) {
	    tmp4 = MAXX - 1;
	}
    }

    /* Now reset monster moved flags */
    for(j = tmp1; j < tmp2; ++j) {
	for(i = tmp3; i < tmp4; ++i) {
	    moved[i][j] = 0;
	}
    }

    moved[lasthx][lasthy] = 0;

    /* Who gets moved? Split for efficiency */
    if(c[AGGRAVATE] || !c[STEALTH]) {
	/* Look through all locations in window */
	for(j = tmp1; j < tmp2; ++j) {
	    for(i = tmp3; i < tmp4; ++i) {
		/* If there is a monster to move */
		if(mitem[i][j]) {
		    /* If it has not already been moved */
		    if(moved[i][j] == 0) {
			/* Go and move the monster */
			movemt(i, j);
		    }
		}
	    }
	}
    }
    else { /* Not aggravated and not stealth */
	/* Look through all locations in window */
	for(j = tmp1; j < tmp2; ++j) {
	    for(i = tmp3; i < tmp4; ++i) {
		/* If there is a monster to move */
		if(mitem[i][j]) {
		    /* If it has not already been moved */
		    if(moved[i][j] == 0) {
			/* If it is asleep due to stealth */
			if(stealth[i][j]) {
			    /* Go and move the monster */
			    movemt(i, j);
			}
		    }
		}
	    }
	}
    }

    /* Now move monster last hit by player if not already moved */
    if(mitem[lasthx][lasthy]) {
	/* If it has not already been moved */
	if(moved[lasthx][lasthy] == 0) {
	    movemt(lasthx, lasthy);
	    lasthx = w1x[0];
	    lasthy = w1y[0];
	}
    }
}

/*
 * movemt(int x, int y)
 *
 * Function to move a monster at (x, y) -- must determine where
 *
 * This routine is responsible for determining where one monster at
 * (x, y) will move to. Enter with the monster's coordinates in
 * (x, y).
 *
 * Returns no value
 */
static int tmpitem;
static int xl;
static int xh;
static int yl;
static int yh;

void movemt(int i, int j)
{
    int k;
    int m;
    int z;
    int tmp;
    int xtmp;
    int ytmp;
    int monst;

    /* For half speed monsters */
    monst = mitem[i][j];
    
    switch(monst) {
    case TROGLODYTE:
    case HOBGOBLIN:
    case METAMORPH:
    case XVART:
    case INVISIBLESTALKER:
    case ICELIZARD:
	if((gtime & 1) == 1) {
	    return;
	}
    }

    /* Choose destination randomly if scared */
    if(c[SCAREMONST]) {
	xl = i + rnd(3) - 2;

	if(xl < 0) {
	    xl = 0;
	}

	if(xl >= MAXX) {
	    xl = MAXX - 1;
	}

	yl = j + rnd(3) - 2;

	if(yl < 0) {
	    yl = 0;
	}

	if(yl >= MAXY) {
	    yl = MAXY - 1;
	}

	tmp = item[x][y];

	if(tmp != OWALL) {
	    if(mitem[xl][yl] == 0) {
		if((mitem[i][j] != VAMPIRE) || (tmpitem != OMIRROR)) {
		    if(tmp != OCLOSEDOOR) {
			mmove(i, j, xl, yl);
		    }
		}
	    }
	}

	return;
    }

    /* If smart monster */
    if(monster[monst].intelligence > (10 - c[HARDGAME])) {
	/* Intelligent movement here -- first setup screen array */
	xl = tmp3 - 2;
	yl = tmp1 - 2;
	xh = tmp4 + 2;
	yh = tmp2 + 2;
	vxy(&xl, &yl);
	vxy(&xh, &yh);

	for(k = yl; k < yh; ++k) {
	    for(m = xl; m < xh; ++m) {
		switch(item[m][k]) {
		case OWALL:
		case OPIT:
		case OTRAPARROW:
		case ODARTRAP:
		case OCLOSEDOOR:
		case OTRAPDOOR:
		case OTELEPORTER:
		    screen[m][k] = 127;

		    break;
		case OMIRROR:
		    if(mitem[m][k] == VAMPIRE) {
			screen[m][k] = 127;

			break;
		    }
		    
		default:
		    screen[m][k] = 0;

		    break;
		}
	    }
	}

	screen[playerx][playery] = 1;

	/* Now perform proximmity ripple from playerx, playery to monster */
	xl = tmp3 - 1;
	yl = tmp1 - 1;
	xh = tmp4 + 1;
	yh = tmp2 + 1;
	vxy(&xl, &yl);
	vxy(&xh, &yh);

	/* Only up to 20 squares away */
	int flag = 0;
	for(tmp = 1; tmp < distance; ++tmp) {
	    for(k = yl; k < yh; ++k) {
		for(m = xl; m < xh; ++m) {
		    /* If find proximity n advance it */
		    if(screen[m][k] == tmp) {
			/* Go around in a circle */
			for(z = 1; z < 9; ++z) {
			    xtmp = m + diroffx[z];
			    ytmp = k + diroffy[z];

			    if(screen[xtmp][ytmp] == 0) {
				screen[xtmp][ytmp] = tmp + 1;
			    }

			    if((xtmp == i) && (ytmp == j)) {
				flag = 1;

				break;
			    }
			}
		    }

		    if(flag) {
			break;
		    }
		}

		if(flag) {
		    break;
		}
	    }

	    if(flag) {
		break;
	    }
	}

	/* Did find connectivity */
	if(tmp < distance) {
	    /* Now select lowest value around playerx, playery */
	    /* Go around in a circle */
	    for(z = 1; z < 9; ++z) {
		xl = i + diroffx[z];
		yl = j + diroffy[z];

		if(screen[xl][yl] == tmp) {
		    if(!mitem[xl][yl]) {
			w1x[0] = xl;
			x1y[0] = yl;
			mmove(i, j, w1x, w1y);

			return;
		    }
		}
	    }
	}
    }

    /* Dumb monsters move here */
    xl = i - 1;
    yl = j - 1;
    xh = i + 2;
    yh = j + 2;

    if(i < playerx) {
	++xl;
    }
    else if(i > playerx) {
	--xh;
    }

    if(j < playery) {
	++yl;
    }
    else if(j > playery) {
	--yh;
    }

    for(k = 0; k < 9; ++k) {
	w1[k] = 10000;
    }

    for(k = xl; k < xh; ++k) {
	/* For each square compute distance to palyer */
	for(m = yl; m < yh; ++m) {
	    tmp = k - i + 4 + (3 * (m - j));
	    tmpitem = item[k][m];

	    if((tmpitem != OWALL) || ((k == playerx) && (m == playery))) {
		if(mitem[k][m] == 0) {
		    if((mitem[i][j] != VAPIRE) || (tmpitem != OMIRROR)) {
			if(tmpitem != OCLOSEDOOR) {
			    w1[tmp] = (playerx - k) * (playerx - k) + (playery - m) * (playery - m);
			    w1x[tmp] = k;
			    w1y[tmp] = m;
			}
		    }
		}
	    }
	}
    }

    tmp = 0;

    for(k = 1; k < 9; ++k) {
	if(w1[tmp] > w1[k]) {
	    tmp = k;
	}
    }

    if(w1[tmp] < 10000) {
	if((i != w1x[tmp]) || (j != w1y[tmp])) {
	    mmove(i, j, w1x[tmp], w1y[tmp]);
	}
    }
}

/*
 * mmove(int x, int y, int xd, int yd)
 *
 * Function to actuall perform the monster movement
 *
 * Enter with the from coordinates in (x, y) and the destination
 * coordinates in (xd, yd).
 */

void mmove(int aa, int bb, int cc, int dd)
{
    int tmp;
    int i;
    int flag;
    char *who;
    char *p;

    /* Set to 1 if monster hit by arrow trap */
    flag = 0;

    if((cc == playerx) && (dd == playery)) {
	hitplayer(aa, bb);
	moved[aa][bb] = 1;

	return;
    }

    i = item[cc][dd];

    if((i == OPIT) || (i == OTRAPDOOR)) {
	switch(mitem[aa][bb]) {
	case SPIRITNAGA:
	case PLATINUMDRAGON:
	case WRAITH:
	case VAPIRE:
	case SILVERDRAGON:
	case POLTERGEIST:
	case DEMONLORD:
	case DEMONLORD+1:
	case DEMONLORD+2:
	case DEMONLORD+3:
	case DEMONLORD+4:
	case DEMONLORD+5:
	case DEMONLORD+6:
	case DEMONPRINCE:

	    break;
	default:
	    /* Fell in a pit or trapdoor */
	    mitem[aa][bb] = 0;
	}
    }

    mitem[cc][dd] = mitem[aa][bb];
    tmp = mitem[cc][dd];

    if(i == OANNIHILATION) {
	/* Demons dispel spheres */
	if(tmp >= DEMONLORD+3) {
	    cursors();
	    lprintf("\nThe %s dispels the sphere!", monster[tmp].name);
	    /* Delete the sphere */
	    rmsphere(cc, dd);
	}
	else {
	    mitem[cc][dd] = 0;
	    tmp = mitem[cc][dd];
	    i = tmp;
	}
    }

    stealth[cc][dd] = 1;
    hitp[cc][dd] = hitp[aa][bb];

    if(hitp[cc][dd] < 0) {
	hitp[cc][dd] = 1;
    }

    mitem[aa][bb] = 0;
    moved[cc][dd] = 1;

    if(tmp == LEPRECHAUN) {
	switch(i) {
	case OGOLDPILE:
	case OMAXGOLD:
	case OKGOLD:
	case ODGOLD:
	case ODIAMOND:
	case ORUBY:
	case OEMERALD:
	case OSAPPHIRE:
	    /* Leprechaun takes gold */
	    item[cc][dd] = 0;
	}
    }

    /* If a troll regenerate him */
    if(tmp == troll) {
	if((gtime & 1) == 0) {
	    if(monster[tmp].hitpoints > hitp[cc][dd]) {
		++hitp[cc][dd];
	    }
	}
    }

    /* Arrow hits monster */
    if(i == OTRAPARROW) {
	who = "An arrow";
	hitp[cc][dd] -= (rnd(10) + level);
	
	if(hitp[cc][dd] <= 0) {
	    mitem[cc][dd] = 0;
	    flag = 2;
	}
	else {
	    flag = 1;
	}
    }

    /* Dart hits monster */
    if(i == ODARTRAP) {
	who = "A dart";
	hitp[cc][dd] -= rnd(6);
	
	if(hitp[cc][dd]) {
	    mitem[cc][dd] = 0;
	    flag = 2;
	}
	else {
	    flag = 1;
	}
    }

    /* Monster hits teleport trap */
    if(i == OTELEPORTER) {
	flag = 3;
	fillmonst(mitem[cc][dd]);
	mitem[cc][dd] = 0;
    }

    /* If blind don't show where monsters are */
    if(c[BLINDCOUNT]) {
	return;
    }

    if(know[cc][dd] & 1) {
	p = 0;
	
	if(flag) {
	    cursors();
	}
	
	switch(flag) {
	case 1:
	    p = "\n%s hits the %s";
	    
	    break;
	case 2:
	    p = "\n%s hits and kills the %s";
	    
	    break;
	case 3:
	    p = "\nThe %s%s gets teleported";
	    who = "";
	    
	    break;
	}
	
	if(p) {
	    lprintf(p, who, monster[tmp].name);
	    beep();
	}
    }
    
    /* if(yrepcount > 1) { */
    /*     know[aa][bb] &= 2; */
    /*     know[cc][dd] &= 2; */
    
    /*     return; */
    /* } */
    
    if(know[aa][bb] & 1) {
	show1cell(aa, bb);
    }
    
    if(know[cc][dd] & 1) {
	show1cell(cc, dd);
    }
}

/*
 * movsphere()
 *
 * Function to look for and move spheres of annihilation
 *
 * This function works on the sphere linked list, first duplication
 * the list (the act of moving changes the list), then processing each
 * sphere in order to move it. They eat anything in their way, include
 * stairs, volcanic shafts, potions, etc., except for upper level
 * demons, who can dispel spheres.
 *
 * No value is returned
 */

/* Maximum number of spheres movsphere can handle */
#define SPHMAX 20

void movsphere()
{
    int x;
    int y;
    int dir;
    int len;
    struct sphere *sp;
    struct sphere *sp2;
    sphere sph[SPHMAX];

    /* First duplicate sphere list */
    sp = 0;
    x = 0;
    
    /* Look through sphere list */
    for(sp2 = spheres; sp; sp2 = sp2->p) {
	/* Only in this level */
	if(sp2->lev == level) {
	    /* Copy the struct */
	    sph[x] = *sp2;
	    sph[x].p = 0;
	    ++x;

	    /* Link pointers */
	    if(x > 1) {
		sph[x - 2].p = &sph[x - 1];
	    }
	}
    }

    /* If any spheres, point to them */
    if(x) {
	sp = sph;
    }
    else { /* No spheres */
	return;
    }

    /* Look through sphere list */
    for(sp = sph; sp; sp = sp->p) {
	x = sp->x;
	y = sp->y;

	/* Not really there */
	if(item[x][y] != OANNIHILATION) {
	    continue;
	}

	/* Has sphere run out of gas? */
	--(sp->lifetime);
	
	if(sp->lifetime < 0) {
	    /* Delete sphere */
	    rmsphere(x, y);

	    continue;
	}

	/* Time to move the sphere */
	switch(rnd((int)max(7, c[INTELLIGENCE]) >> 1)) {
	case 1:
	case 2:
	    /* Change direction to a random one */
	    sp->dir = rnd(8);

	default:
	    /* Move in normal direction */
	    dir = sp->dir;
	    len = sp->lifetime;
	    rmsphere(x, y);
	    newsphere(x + diroffx[dir], y + diroffy[dir], dir, len);
	}
    }
}
