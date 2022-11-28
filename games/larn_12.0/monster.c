/*
 * monster.c
 * Larn is copyrighted 1986 by Noah Morgan.
 *
 * This file contains the following functions:
 * createmonster()    Function to create a monster next to the player
 * cgood()            Function to check location for empiness
 * createitem()       Routine to place an item next to the player
 * cast()             Subroutine called by parse to case a spell for the player
 * speldamage()       Function to perform spell functions case by the player
 * loseint()          Routine to decrement your int (intelligence) if > 3
 * isconfuse()        Routine to check to see if player is confused
 * nospell()          Routine to return 1 if a spell doesn't affact a monster
 * fullhit()          Function to return full damage against a monster (aka web)
 * direct()           Routine to direct spell damage 1 square in 1 direction
 * godirect()         Function to perform missile attacks
 * ifblind()          Routine to put "monster" of the monster name into lastmonst
 * tdirect()          Routine to teleport away a monster()
 * omnidirect()       Routine to damage all monsters 1 square from player
 * dirsub()           Routine to ask for direction, then modify (x, y) for it
 * vxy()              Routine to verify/fix (*x, *y) for being within bounds
 * dirpoly()          Routine to ask for a direction and polymorph a monster
 * hitmonster()       Fuction to hit a monster at the designated coordinates
 * hitm()             Function to just hit a monster at a given coordinates
 * hitplayer()        Function for the monster to hit the player from (x, y)
 * dropsomething()    Function to create an object when a monster dies
 * dropgold()         Function to drop some gold around player
 * something()        Function to create a random item around player
 * newobject()        Routine to return a randomly selected new object
 * spattack()         Function to process special attacks from monsters
 * checkloss()        Routine to subtract hp from used and flag bottomline display
 * annihilate()       Routine to annihilate monsters around player, playerx, playery
 * newsphere()        Function to create a new sphere of annihilation
 * rmsphere()         Function to delete a sphere of annihilation from list
 * sphboom()          Function to perform the effects of a sphere detonation
 * genmonst()         Function to ask for monster and genocide from game
 */

#include "monster.h"

#include "create.h"
#include "display.h"
#include "global.h"
#include "header.h"
#include "io.h"
#include "main.h"
#include "nap.h"
#include "object.h"
#include "scores.h"
#include "tok.h"

#include <ctype.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>

/* Used for alter reality */
struct isave {
    /* 0 = item, 1 = monster */
    char type;

    /* Item number of monster number */
    char id;

    /* The type of item or hitpoints of monster */
    short arg;
};

static int dirsub(int *x, int *y);

/*
 * createmonster(int monstno)
 * 
 * Function to create a monster next to the player
 *
 * Enter with the monster number (1 to MAXMONST + 8)
 *
 * Returns no value
 */
void createmonster(int mon)
{
    int x;
    int y;
    int k;
    int i;

    /* Check for monster number out of bounds */
    if((mon < 1) || (mon > (MAXMONST + 8))) {
	beep();
	lprintf("\nCan't createmonst(%d)\n", mon);
	nap(3000);

	return;
    }

    /* Genocided? */
    while(monster[mon].genocided && (mon < MAXMONST)) {
	++mon;
    }

    /* Choose direction, then try all */
    k = rand() % 8 + 1;
    
    for(i = -8; i < 0; ++i) {
	/* Wraparound the diroff arrays */
	if(k > 8) {
	    k = 1;
	}

	x = playerx + diroffx[k];
	y = playery + diroffy[k];

	/* If we can create here */
	if(cgood(x, y, 0, 1)) {
	    mitem[x][y] = mon;
	    hitp[x][y] = monster[mon].hitpoints;
	    know[x][y] = 0;
	    stealth[x][y] = know[x][y];

	    switch(mon) {
	    case ROTHE:
	    case POLTERGEIST:
	    case VAMPIRE:
		stealth[x][y] = 1;
	    }

	    return;
	}
	
	++k;
    }
}

/*
 * int cgood(int x, int y, int itm, int monst)
 *
 * Function to check location for emptiness
 *
 * Routine to return TRUE if a location does not have itm or monst
 * there, returns FALSE (0) otherwise.
 *
 * Enter with itm or monst TRUE or FALSE if check it.
 *
 * Example:
 *   if itm == TRUE, check for no item at this location
 *   if monst == TRUE, check for no monster at this location
 *
 * This routine will return FALSE if at a wall of the dungeon exit on level 1
 */
int cgood(int x, int y, int itm, int monst)
{
    /* Within bounds? */
    if((y >= 0) && (y <= (MAXY - 1)) && (x >= 0) && (x <= (MAXX - 1))) {
	/* Can't make anything on walls */
	if(item[x][y] != OWALL) {
	    /* Is it free of items? */
	    if((itm == 0) || (item[x][y] == 0)) {
		/* Is it free of monsters? */
		if((monst == 0) || (mitem[x][y] == 0)) {
		    /* Not exit to level 1 */
		    if((level != 1) || (x != 33) || (y != (MAXY - 1))) {
			return 1;
		    }
		}
	    }
	}
    }

    return 0;
}

/*
 * createitem(int it, int arg)
 *
 * Routine to place an item next to the player
 *
 * Enter with the item number and its argument (iven[], ivenarg[])
 *
 * Returns no value, thus we don't know about createitem() failures.
 */
void createitem(int it, int arg)
{
    int x;
    int y;
    int k;
    int i;

    /* No such object */
    if(it >= MAXOBJ) {
	return;
    }

    /* Choose direction, then try all */
    k = rand() % 8 + 1;

    for(i = -1; i < 0; ++i) {

	/* Wraparound the diroff arrays */
	if(k > 8) {
	    k = 1;
	}

	x = playerx + diroffx[k];
	y = playery + diroffy[k];

	/* If we can create here */
	if(cgood(x, y, 1, 0)) {
	    item[x][y] = it;
	    know[x][y] = 0;
	    iarg[x][y] = arg;

	    return;
	}
	
	++k;
    }
}

/*
 * cast()
 *
 * Subroutine called by parse to cast a spell for the user
 *
 * No arguments and no return value
 */
static char eys[] = "\nEnter your spell: ";

void cast()
{
    int i;
    int j;
    int a;
    int b;
    int d;

    cursors();

    if(c[SPELLS] <= 0) {
	lprcat("\nYou don't have any spells!");

	return;
    }

    lprcat(eys);
    --c[SPELLS];
    a = getchar();

    while(a == 'D') {
	seemagic(-1);
	cursors();
	lprcat(eys);
	a = getchar();
    }

    /* To escape casting a spell */
    if(a == '\33') {
	lprcat(aborted);
	++c[SPELLS];

	return;
    }

    /* To escape casting a spell */
    b = getchar();

    if(b == '\33') {
	lprcat(aborted);
	++c[SPELLS];

	return;
    }

    d = getchar();

    /* To escape casting a spell */
    if(d == '\33') {
	lprcat(aborted);
	++c[SPELLS];

	return;
    }

#ifdef EXTRA
    ++c[SPELLCAST];
#endif

    /* Seq search for his spell, hash? */
    lprc('\n');
    j = -1;
    
    for(i = 0; i < SPNUM;++i) {
	if((spelcode[i][0] == a)
	   && (spelcode[i][1] == b)
	   && (spelcode[i][2] == d)) {
	    if(spelknow[i]) {
		speldamage(i);
		j = 1;
		i = SPNUM;
	    }
	}
    }

    if(j == -1) {
	lprcat(" Nothing happened ");
    }

    bottomline();
}

/*
 * speldamage(int x)
 *
 * Function to perform spell functions cast by they player
 *
 * Enter with the spell number, returns no value.
 *
 * Please ensure that there are 2 spaces before all messages here
 */
void speldamage(int x)
{
    int i;
    int j;
    int clev;
    int xl;
    int xh;
    int yl;
    int yh;
    char *p;
    char *kn;
    char *pm;

    /* No such spell */
    if(x >= SPNUM) {
	return;
    }

    /* Not if time stopped */
    if(c[TIMESTOP]) {
	lprcat(" It didn't seem to work");

	return;
    }

    clev = c[LEVEL];

    if(((rand() % 23 + 1) == 7) || ((rand() % 18 + 1) > c[INTELLIGENCE])) {
	lprcat(" It didn't work!");

	return;
    }

    if(((clev * 3) + 2) < x) {
	lprcat(" Nothing happens. You seem inexperienced at this");

	return;
    }

    switch(x) {
	/* --- LEVEL 1 SPELLS --- */
    case 0:
	/* Protection field + 2 */
	if(c[PROTECTIONTIME] == 0) {
	    c[MOREDEFENCES] += 2;
	}

	c[PROTECTIONTIME] += 250;

	return;
    case 1:
	/* Magic missile */
	i = rand() % (((clev + 1) << 1)) + 1 + clev + 3;

	if(clev >= 2) {
	    godirect(x, i, " Your missiles hit the %s", 100, '+');
	}
	else {
	    godirect(x, i, " Your missile hit the %s", 100, '+');
	}

	return;
    case 2:
	/* Dexterity */
	if(c[DEXCOUNT] == 0) {
	    c[DEXTERITY] += 3;
	}

	c[DEXCOUNT] += 400;
	
	return;
    case 3:
	/* Sleep */
	i = rand() % 3 + 1 + 1;
	p = " While the %s slept, you smashed it %d times";
	direct(x, fullhit(i), p, i);

	return;
    case 4:
	/* Charm monster */
	c[CHARMCOUNT]  += (c[CHARISMA] << 1);

	return;
    case 5:
	/* Sonic spear */
	godirect(x, rand() % 10 + 1 + 15 + clev, " The sound damages the %s", 70, '@');

	return;
	/* --- LEVEL 2 SPELLS --- */
    case 6:
	/* web */
	i = rand() % 3 + 1 + 2;
	p = " While the %s is entangled, you hit %d times";
	direct(x, fullhit(i), p, i);

	return;
    case 7:
	/* Strength */
	if(c[STRCOUNT] == 0) {
	    c[STREXTRA] += 3;
	}

	c[STRCOUNT] += (150 + rand() % 100 + 1);

	return;
    case 8:
	/* Enlightenment */
	yl = playery - 5;
	yh = playery + 6;
	xl = playerx - 15;
	xh = playerx + 16;

	/* Check bounds */
	vxy(&xl, &yl);
	vxy(&xh, &yh);

	/* Enlightenment */
	for(i = yl; i < yh; ++i) {
	    for(j = xl; j <= xh; ++j) {
		know[j][i] = 1;
	    }
	}

	draws(xl, xh + 1, yl, yh + 1);

	return;
    case 9:
	/* Healing */
	raisehp(20 + (clev << 1));

	return;
    case 10:
	/* Cure blindness */
	c[BLINDCOUNT] = 0;

	return;
    case 11:
	createmonster(makemonst(level + 1) + 8);

	return;
    case 12:
	if((rand() % 11 + 1 + 7) <= c[WISDOM]) {
	    direct(x, rand() % 20 + 1 + 20 + clev, " The %s believed!", 0);
	}
	else {
	    lprcat(" It didn't believe the illusions!");
	}

	return;
    case 13:
	/* If he has the amulet of invisibility then add more time */
	j = 0;

	for(i = 0; i < 26; ++i) {
	    if(iven[i] == OAMULET) {
		j += ivenarg[i];
	    }
	}

	c[INVISIBILITY] += ((j << 7) + 12);

	return;
	/* --- LEVEL 3 SPELLS --- */
    case 14:
	/* Fireball */
	godirect(x, rand() % (25 + clev) + 1 + 25 + clev, " The fireball hits the %s", 40, '*');

	return;
    case 15:
	/* Cold */
	godirect(x, rand() % 25 + 1 + 20 + clev, " Your cone of cold strikes the %s", 60, 'O');

	return;
    case 16:
	/* Polymorph */
	dirpoly(x);

	return;
    case 17:
	/* Cancellation */
	c[CANCELLATION] += (5 + clev);

	return;
    case 18:
	/* Haste self */
	c[HASTESELF] += (7 + clev);

	return;
    case 19:
	/* Cloud kill */
	omnidirect(x, 30 + rand() % 10 + 1, " The %s gasps for air");

	return;
    case 20:
	/* Vaporize rock */
	xh = min(playerx + 1, MAXX - 2);
	yh = min(playery + 1, MAXY - 2);

	for(i = max(playerx - 1, 1); i < xh; ++i) {
	    for(j = max(playery - 1, 1); j <= yh; ++j) {
		kn = &know[i][j];
		pm = &mitem[i][j];
		p = &item[i][j];
		
		switch(*p) {
		case OWALL:
		    if(level < (MAXLEVEL + MAXVLEVEL - 1)) {
			*kn = 0;
			*p = *kn;
		    }
		    
		    break;
		case OSTATUE:
		    if(c[HARDGAME] < 3) {
			*p = OBOOK;
			iarg[i][j] = level;
			*kn = 0;
		    }

		    break;
		case OTHRONE:
		    *pm = GNOMEKING;
		    *kn = 0;
		    *p = OTHRONE2;
		    hitp[i][j] = monster[GNOMEKING].hitpoints;

		    break;
		case OALTAR:
		    *pm = DEMONPRINCE;
		    *kn = 0;
		    hitp[i][j] = monster[DEMONPRINCE].hitpoints;

		    break;
		}

		switch(*pm) {
		case XORN:
		    /* Xorn takes damage from vpr */
		    ifblind(i, j);
		    hitm(i, j, 20);

		    break;
		}
	    }
	}

	return;
	/* --- LEVEL 4 SPELLS */
    case 21:
	/* Dehydration */
	direct(x, 100 + clev, " The %s shrivels up", 0);

	return;
    case 22:
	/* Lightning */
	godirect(x, rand() % 25 + 1 + 20 + (clev << 1), " A lightning bolt hits the %s", 1, '~');

	return;
    case 23:
	/* Drain life */
	i = min(c[HP] - 1, c[HP] / 2);
	direct(x, i + i, "", 0);
	c[HP] -= i;

	break;
    case 24:
	/* Globe of invulnerability */
	if(c[GLOBE] == 0) {
	    c[MOREDEFENCES] += 10;
	}

	c[GLOBE] += 200;
	loseint();

	return;
    case 25:
	/* Flood */
	omnidirect(x, 32 + clev, " The %s struggles for air in your flood!");

	return;
    case 26:
	/* Finger of death */
	if((rand() % 151 + 1) == 63) {
	    beep();
	    lprcat("\nYour heart stopped!\n");
	    nap(4000);
	    died(270);

	    return;
	}

	if(c[WISDOM] > (rand() % 10 + 1 + 10)) {
	    direct(x, 2000, " The %s's heart stopped", 0);
	}
	else {
	    lprcat(" It didn't work");
	}

	return;
	/* --- LEVEL 5 SPELLS --- */
    case 27:
	/* Scare monster */
	c[SCAREMONST] += (rand() % 10 + 1 + clev);

	return;
    case 28:
	/* Hold monster */
	c[HOLDMONST] += (rand() % 10 + 1 + clev);

	return;
    case 29:
	/* Time stop */
	c[TIMESTOP] += (rand() % 20 + 1 + (clev << 1));

	return;
    case 30:
	/* Teleport away */
	tdirect(x);

	return;
    case 31:
	/* Magic fire */
	omnidirect(x, 35 + rand() % 10 + 1 + clev, " The %s cringes from the flame");

	return;
	/* --- LEVEL 6 SPELLS */
    case 32:
	/* Sphere of annihilation */
	if(((rand() % 23 + 1) == 5) && (wizard == 0)) {
	    beep();
	    lprcat("\nYou have been enveloped by the zone of nothingness!\n");
	    nap(4000);
	    died(258);

	    return;
	}

	xl = playerx;
	yl = playery;
	loseint();

	/* Get the direction of sphere */
	i = dirsub(&xl, &yl);

	/* Make a sphere */
	newsphere(xl, yl, i, rand() % 20 + 1 + 11);

	return;
    case 33:
	/* Genocide */
	genmonst();
	spelknow[33] = 0;
	loseint();

	return;
    case 34:
	/* Summon demon */
	if((rand() % 100 + 1) > 30) {
	    direct(x, 150, " Th demon strikes at the %s", 0);

	    return;
	}

	if((rand() % 100 + 1) > 15) {
	    lprcat(" Nothing seems to have happened");

	    return;
	}

	return;
    case 35:
	/* Walk through walls */
	c[WTW] += (rand() % 10 + 1 + 5);

	return;
    case 36:
	/* Alter reality */
	{
	    /* Pointer to item save structure */
	    struct isave *save;

	    /* # items saved */
	    int sc;

	    sc = 0;
	    save = (struct isave *)malloc(sizeof(struct isave) * MAXX * MAXY * 2);

	    /* Save all items and monsters */
	    for(j = 0; j < MAXY; ++ j) {
		for(i = 0; i < MAXX; ++i) {
		    xl = item[i][j];

		    if(xl && (xl != OWALL) && (xl != OANNIHILATION)) {
			save[sc].type = 0;
			save[sc].id = item[i][j];
			save[sc].arg = iarg[i][j];
			++sc;
		    }

		    if(mitem[i][j]) {
			save[sc].type = 1;
			save[sc].id = mitem[i][j];
			save[sc].arg = hitp[i][j];
			++sc;
		    }

		    item[i][j] = OWALL;
		    mitem[i][j] = 0;

		    if(wizard) {
			know[i][j] = 1;
		    }
		    else {
			know[i][j] = 0;
		    }
		}
	    }

	    eat(1, 1);

	    if(level == 1) {
		item[33][MAXY - 1] = 0;
	    }

	    j = rand() % (MAXY - 2) + 1;
	    
	    for(i = 1; i < (MAXX - 1); ++i) {
		item[i][j] = 0;
	    }

	    /* Put object back in level */
	    while(sc > 0) {
		--sc;

		if(save[sc].type == 0) {
		    int trys;

		    trys = 100;
		    j = 1;
		    i = j;

		    while((--trys > 0) && item[i][i]) {
			i = rand() % (MAXX - 1) + 1;
			j = rand() % (MAXY - 1) + 1;
		    }

		    if(trys) {
			item[i][j] = save[sc].id;
			iarg[i][j] = save[sc].arg;
		    }
		}
		else { /* Put monsters back in */
		    int trys;

		    trys = 100;
		    j = 1;
		    i = j;

		    while((--trys > 0)
			  && ((item[i][j] == OWALL) || mitem[i][j])) {
			i = rand() % (MAXX - 1)+ 1;
			j = rand() % (MAXY - 1) + 1;
		    }

		    if(trys) {
			mitem[i][j] = save[sc].id;
			hitp[i][j] = save[sc].arg;
		    }
		}
	    }

	    loseint();
	    draws(0, MAXX, 0, MAXY);

	    if(wizard == 0) {
		spelknow[36] = 0;
	    }

	    free((char *)save);
	    positionplayer();
	}
	
	return;
    case 37:
	/* Permanence */
	adjtime(-99999L);
	
	/* Forget */
	spelknow[37] = 0;
	loseint();

	return;
    default:
	lprintf(" spell %d not available!", x);
	beep();

	return;
    }
}

/*
 * loseint()
 * 
 * Routine to subtract 1 from you int (intelligence) if > 3
 *
 * No arguments and no return value
 */
void loseint()
{
    --c[INTELLIGENCE];
    
    if(c[INTELLIGENCE] < 3) {
	c[INTELLIGENCE] = 3;
    }
}

/*
 * isconfuse()
 *
 * Routine to hceck to see if player is confused
 *
 * This routine prints out a message saying "You can't aim your magic!"
 *
 * Returns 0 if not confused, non-zero (time remaining confused) if confused
 */
int isconfuse()
{
    if(c[CONFUSE]) {
	lprcat(" You can't aim your magic!");
	beep();
    }

    return c[CONFUSE];
}

/*
 * nospell(int x, int monst)
 *
 * Routine to return 1 if a spell doesn't affect a monster
 *
 * Subroutine to return 1 if the spell can't affect the monster,
 * otherwise returns 0
 *
 * Enter with the spell number in x, and the monster number in monst
 */
int nospell(int x, int monst)
{
    int tmp;

    /* Bad spell or monst */
    if((x >= SPNUM) || (monst >= (MAXMONST + 8)) || (monst < 0) || (x < 0)) {
	return 0;
    }

    tmp = spelweird[monst - 1][x];

    if(tmp == 0) {
	return 0;
    }

    cursors();
    lprc('\n');
    lprintf(spelmes[tmp], monster[monst].name);

    return 1;
}

/*
 * fullhit(int xx)
 * 
 * Function to return full damage against a monster (aka web)
 *
 * Function to return hp damage to monster due to a number of full hits
 *
 * Enter with the number of full hits being done
 */
int fullhit(int xx)
{
    int i;

    /* Fullhits are out of range */
    if((xx < 0) || (xx > 20)) {
	return 0;
    }

    /* Lance of death */
    if(c[LANCEDEATH]) {
	return 10000;
    }

    i = xx * ((c[WCLASS] >> 1) + c[STRENGTH] + c[STREXTRA] - c[HARDGAME] - 12 + c[MOREDAM]);

    if(i >= 1) {
	return i;
    }
    else {
	return xx;
    }
}

/*
 * direct(int spnum, int dam, char *str, int arg)
 *
 * Routine to direct spell damage 1 square int 1 dir
 *
 * Routine to ask for a direction to a spell and then hit the monster.
 *
 * Enter with the spell number in spnum, the damage to be done in dam,
 * lprintf format string in std, and lprintf's argument in arg.
 *
 * Returns no value
 */
void direct(int spnum, int dam, char *str, int arg)
{
    int x;
    int y;
    int m;

    /* Bad arguments */
    if((spnum < 0) || (spnum >= SPNUM) || (str == 0)) {
	return;
    }

    if(isconfuse()) {
	return;
    }

    dirsub(&x, &y);
    m = mitem[x][y];

    if(item[x][y] == OMIRROR) {
	/* Sleep */
	if(spnum == 3) {
	    lprcat("You fall sleep! ");
	    beep();
	    arg += 2;

	    while(arg-- > 0) {
		parse2();
		nap(1000);
	    }

	    return;
	}
	else if(spnum == 6) { /* Web */
	    lprcat("You got stuck in your own web! ");
	    beep();
	    arg += 2;

	    while(arg-- > 0) {
		parse2();
		nap(1000);
	    }

	    return;
	}
	else {
	    lastnum = 278;
	    lprintf(str, "spell caster (that's you)", arg);
	    beep();
	    losehp(dam);

	    return;
	}
    }

    if(m == 0) {
	lprcat(" There wasn't anything there!");

	return;
    }

    ifblind(x, y);

    if(nospell(spnum, m)) {
	lasthx = x;
	lasthy = y;

	return;
    }

    lprintf(str, lastmonst, arg);
    hitm(x, y, dam);
}

/*
 * godirect(int spnum, int dam, char *str, int delay, char cshow)
 *
 * Function to perform missile attacks
 *
 * Fuction to hit in a direction from a missigle weapon and have it
 * keep on going in that direction until its power is exhausted.
 *
 * Enter with the spell number in spnum, the power of the weapon in
 * hp, lprintf format string in str, the # of milliseconds to delay
 * between locations in delay, and the character to represent the
 * weapon in cshow.
 *
 * Returns no value.
 */
void godirect(int spnum, int dam, char *str, int delay, char cshow)
{
    char *p;
    int x;
    int y;
    int m;
    int dx;
    int dy;

    /* Bad args */
    if((spnum < 0) || (spnum >= SPNUM) || (str == 0) || (delay < 0)) {
	return;
    }

    if(isconfuse()) {
	return;
    }

    dirsub(&dx, &dy);
    x = dx;
    y = dy;
    dx = x - playerx;
    dy = y - playery;
    x = playerx;
    y = playery;

    while(dam > 0) {
	x += dx;
	y += dy;

	/* Out of bounds */
	if((x > (MAXX - 1)) || (y > (MAXY - 1)) || (x < 0) || (y < 0)) {
	    dam = 0;

	    break;
	}

	/* If energy hits player */
	if((x == playerx) && (y == playery)) {
	    cursors();
	    lprcat("\nYou are hit by your own magic!");
	    beep();
	    lastnum = 278;
	    losehp(dam);

	    return;
	}

	/* If not blind show effect */
	if(c[BLINDCOUNT] == 0) {
	    cursor(x + 1, y + 1);
	    lprc(cshow);
	    nap(delay);
	    show1cell(x, y);
	}

	m = mitem[x][y];

	/* Is there a monster there? */
	if(m) {
	    ifblind(x, y);

	    if(nospell(spnum, m)) {
		lasthx = x;
		lasthy = y;

		return;
	    }

	    cursors();
	    lprc('\n');
	    lprintf(str, lastmonst);
	    dam -= hitm(x, y, dam);
	    show1cell(x, y);
	    nap(1000);
	    x -= dx;
	    y -= dy;
	}
	else {
	    p = &item[x][y];

	    switch(*p) {
	    case OWALL:
		cursors();
		lprc('\n');
		lprintf(str, "wall");

		/* Enough damage? */
		if(dam >= (50 + c[HARDGAME])) {
		    /* Not on V3 */
		    if(level < (MAXLEVEL + MAXVLEVEL - 1)) {
			if((x < (MAXX - 1)) && (y < (MAXY - 1)) && x && y) {
			    lprcat(" The wall crumbles");
			    *p = 0;
			    know[x][y] = 0;
			    show1cell(x, y);
			}
		    }
		}

		dam = 0;
		
		break;
	    case OCLOSEDOOR:
		cursors();
		lprc('\n');
		lprintf(str, "door");

		if(dam >= 40) {
		    lprcat(" The door is blasted apart");
		    *p = 0;
		    know[x][y] = 0;
		    show1cell(x, y);
		}

		dam = 0;

		break;
	    case OSTATUE:
		cursors();
		lprc('\n');
		lprintf(str, "statue");

		if(c[HARDGAME] < 3) {
		    if(dam > 44) {
			lprcat(" The status crumbles");
			*p = OBOOK;
			iarg[x][y] = level;
			know[x][y] = 0;
			show1cell(x, y);
		    }
		    
		    dam = 0;

		    break;
		}

		dam = 0;

		break;
	    case OTHRONE:
		cursors();
		lprc('\n');
		lprintf(str, "throne");

		if(dam > 39) {
		    mitem[x][y] = GNOMEKING;
		    hitp[x][y] = monster[GNOMEKING].hitpoints;
		    *p = OTHRONE2;
		    know[x][y] = 0;
		    show1cell(x, y);
		}
		
		dam = 0;
		
		break;
	    case OMIRROR:
		dx *= -1;
		dy *= -1;

		break;
	    }

	    dam -= (3 + (c[HARDGAME] >> 1));
	}
    }
}

/*
 * ifblind(int x, int y)
 *
 * Routine to put "monster" or the monster name into lastmonst
 *
 * Subroutine to copy the word "monster" int lastmonst if the player is blind.
 *
 * Enter with the coordinates (x, y) of the monster
 *
 * Returns no value.
 */
void ifblind(int x, int y)
{
    char *p;

    /* Verify correct x, y coordinates */
    vxy(&x, &y);

    if(c[BLINDCOUNT]) {
	lastnum = 279;
	p = "monster";
    }
    else {
	lastnum = mitem[x][y];
	p = monster[lastnum].name;
    }

    strcpy(lastmonst, p);
}

/*
 * tdirect(int spnum)
 *
 * Routine to teleport away a monster
 *
 * Routine to ask for a direction to spell and then teleport away monster
 *
 * Enter with the spell number that wants to teleport away
 *
 * Returns no value
 */
void tdirect(int spnum)
{
    int x;
    int y;
    int m;

    /* Bad args */
    if((spnum < 0) || (spnum >= SPNUM)) {
	return;
    }
    
    if(isconfuse()) {
	return;
    }

    dirsub(&x, &y);

    m = mitem[x][y];

    if(m == 0) {
	lprcat(" There wasn't anything there!");

	return;
    }

    ifblind(x, y);

    if(nospell(spnum, m)) {
	lasthx = x;
	lasthy = y;

	return;
    }

    fillmonst(m);
    know[x][y] = 0;
    mitem[x][y] = know[x][y];
}

/* 
 * omindirect(int sp, int dam, char *str)
 *
 * Routine to damage all monsters 1 square from player
 *
 * Routine to cast a spell and then hit the monster in all directions.
 *
 * Enter with the spell number in sp, the damage done to each square
 * in dam, and the lprintf string to identify the spell in str.
 *
 * Returns no value.
 */
void omnidirect(int spnum, int dam, char *str)
{
    int x;
    int y;
    int m;

    /* Bad args */
    if((spnum < 0) || (spnum >= SPNUM) || (str == 0)) {
	for(x = (playerx - 1); x < (playerx + 2); ++x) {
	    for(y = (playery - 1); y < (playery + 2); ++y) {
		m = mitem[x][y];

		if(m) {
		    if(nospell(spnum, m) == 0) {
			ifblind(x, y);
			cursors();
			lprc('\n');
			lprintf(str, lastmonst);
			hitm(x, y, dam);
			nap(800);
		    }
		    else {
			lasthx = x;
			lasthy = y;
		    }
		}
	    }
	}
    }
}

/*
 * static dirsub(int *x, int *y)
 *
 * Routine to ask for direction, then modify (x, y) for it
 *
 * Function to ask for a direction and modify an (x, y) for that direction
 *
 * Enter with the origination coordinates in (x, y)
 *
 * Returns index into diroffx[] (0 - 8)
 */
static int dirsub(int *x, int *y)
{
    int i;
    int flag = 0;
    
    lprcat("\nIn what direction? ");
    i = 0;

    while(1) {
	switch(getchar()) {
	case 'b':
	    ++i;
	case 'n':
	    ++i;
	case 'y':
	    ++i;
	case 'u':
	    ++i;
	case 'h':
	    ++i;
	case 'k':
	    ++i;
	case 'l':
	    ++i;
	case 'j':
	    ++i;

	    flag = 1;
	}

	if(flag) {
	    break;
	}
    }

    *x = playerx + diroffx[i];
    *y = playery + diroffy[i];
    vxy(x, y);

    return i;
}

/*
 * vxy(int *x, int *y)
 *
 * Routine to verify/fix coordinates for being within bounds
 *
 * Function to verify (x, y) are within the bounds for a level.
 *
 * If *x or *y is not within the absolute bounds for a leve, fix them
 * so that they are on the level.
 *
 * Returns TRUE if it was out of bounds, and the (x, y) in the calling
 * routine are affected.
 */
int vxy(int *x, int *y)
{
    int flag = 0;

    if(*x < 0) {
	*x = 0;
	++flag;
    }

    if(*y < 0) {
	*y = 0;
	++flag;
    }

    if(*x >= MAXX) {
	*x = MAXX - 1;
	++flag;
    }

    if(*y >= MAXY) {
	*y = MAXY - 1;
	++flag;
    }

    return flag;
}

/*
 * dirpoly(int spnum)
 *
 * Routine to ask for a direction and polymorph a monster
 *
 * Subroutine to polymorph a monster and ask for the direct it's in
 *
 * Enter with the spell number in spnum
 *
 * Returns no value
 */
void dirpoly(int spnum)
{
    int x;
    int y;
    int m;

    /* Bad args */
    if((spnum < 0) || (spnum >= SPNUM)) {
	return;
    }

    /* If he is confused, he can't aim his magic */
    if(isconfuse()) {
	return;
    }

    dirsub(&x, &y);

    if(mitem[x][y] == 0) {
	lprcat(" There wasn't anything there!");

	return;
    }

    ifblind(x, y);

    if(nospell(spnum, mitem[x][y])) {
	lasthx = x;
	lasthy = y;

	return;
    }

    mitem[x][y] = MAXMONST + 7;
    m = mitem[x][y];

    while(monster[m].genocided) {
	mitem[x][y] = rand() % (MAXMONST + 7) + 1;
	m = mitem[x][y];
    }

    hitp[x][y] = monster[m].hitpoints;

    /* Show the new monster */
    show1cell(x, y);
}

/*
 * hitmonster(int x, int y)
 *
 * Function to hit a monster at the designated coordinates
 *
 * This routine is used for a bash and slash type attack on a monster
 *
 * Enter with the coordinates of the monster in (x, y)
 *
 * Returns no value
 */
void hitmonster(int x, int y)
{
    int tmp;
    int monst;
    int damag;
    int flag;

    /* Not if time stopped */
    if(c[TIMESTOP]) {
	return;
    }

    /* Verify coordinates are within range */
    vxy(&x, &y);
    monst = mitem[x][y];

    if(monst == 0) {
	return;
    }

    hit3flag = 1;
    ifblind(x, y);
    tmp = monster[monst].armorclass + c[LEVEL] + c[DEXTERITY] + (c[WCLASS] / 4) - 12;
    cursors();

    /* Need at least random chance to hit */
    if(((rand() % 20 + 1) < (tmp - c[HARDGAME])) || ((rand() % 71 + 1) < 5)) {
	lprcat("\nYou hit");
	flag = 1;
	damag = fullhit(1);

	if(damag < 9999) {
	    damag = rand() % damag + 1 + 1;
	}
    }
    else {
	lprcat("\nYou missed");
	flag = 0;
    }

    lprcat(" the ");
    lprcat(lastmonst);

    /* If the last monster was hit */
    if(flag) {
	if((monst == RUSTMONSTER)
	   || (monst == DISENCHANTRESS)
	   || (monst == CUBE)) {
	    if(c[WIELD] > 0) {
		if(ivenarg[c[WIELD]] > -10) {
		    lprintf("\nYou weapon is dulled by the %s", lastmonst);
		    beep();
		    --ivenarg[c[WIELD]];
		}
	    }
	}
    }

    if(flag) {
	hitm(x, y, damag);
    }

    if(monst == VAMPIRE) {
	if(hitp[x][y] < 25) {
	    mitem[x][y] = BAT;
	    know[x][y] = 0;
	}
    }
}

/*
 * hitm(int x, int y, int amt)
 * 
 * Function to just hit a monster at a given coordinate
 *
 * Returns the number of hitpoints the monster absorbed
 *
 * This routine is used to specifically damage a monster at a location
 * (x, y). Called by hitmonst(x,y)
 */
int hitm(int x, int y, int amt)
{
    int monst;
    int hpoints;
    int amt2;

    /* Verify coordinates are within range */
    vxy(&x, &y);

    /* Save initial damage so we can return it */
    amt2 = amt;
    monst = mitem[x][y];

    /* If half damage curse adjust damage points */
    if(c[HALFDAM]) {
	amt >>= 1;
    }

    if(amt <= 0) {
	amt = 1;
	amt2 = amt;
    }

    lasthx = x;
    lasthy = y;

    /* Make sure hitting monster breaks stealth condition */
    stealth[x][y] = 1;

    /* Hit a monster breaks hold monster spell */
    c[HOLDMONST] = 0;

    /* If a dragon and orb(s) of dragon slaying */
    switch(monst) {
    case WHITEDRAGON:
    case REDDRAGON:
    case GREENDRAGON:
    case BRONZEDRAGON:
    case PLATINUMDRAGON:
    case SILVERDRAGON:
	amt *= (1 + (c[SLAYING] << 1));

	break;
    }

    /* Invincible monster fix is here */
    if(hitp[x][y] > monster[monst].hitpoints) {
	hitp[x][y] = monster[monst].hitpoints;
    }

    hpoints = hitp[x][y];

    if(hpoints <= amt) {
#ifdef EXTRA
	++c[MONSTKILLED];
#endif
	lprintf("\nThe %s died!", lastmonst);
	raiseexperience(monster[monst].experience);
	amt = monster[monst].gold;

	if(amt > 0) {
	    dropgold(rand() % amt + 1 + amt);
	}

	dropsomething(monst);
	disappear(x, y);
	bottomline();

	return hpoints;
    }

    hitp[x][y] = hpoints - amt;

    return amt2;
}

/*
 * hitplayer(int x, int y)
 *
 * Function for the monster to hit the player from (x, y)
 *
 * Function for the monster to hit the player with monster at location (x, y)
 *
 * Returns nothing of value
 */
void hitplayer(int x, int y)
{
    int dam;
    int tmp;
    int mster;
    int bias;

    /* Verify coordinates are within range */
    vxy(&x, &y);
    mster = mitem[x][y];
    lastnum = mster;

    /* Spirit nagas and poltergeists do nothing if scarab of negate spirit */
    if(c[NEGATESPIRIT] || c[SPIRITPRO]) {
	if((mster == POLTERGEIST) || (mster == SPIRITNAGA)) {
	    return;
	}
    }

    /* If undead and cube of undead control */
    if(c[CUBEOFUNDEAD] || c[UNDEADPRO]) {
	if((mster == VAMPIRE) || (mster == WRAITH) || (mster == ZOMBIE)) {
	    return;
	}
    }

    if((know[x][y] & 1) == 0) {
	know[x][y] = 1;
	show1cell(x, y);
    }

    bias = c[HARDGAME] + 1;
    hit3flag = 1;
    hit2flag = hit3flag;
    hitflag = hit2flag;
    yrepcount = 0;
    cursors();
    ifblind(x, y);

    if(c[INVISIBILITY]) {
	if((rand() % 33 + 1) < 20) {
	    lprintf("\nThe %s misses wildly", lastmonst);

	    return;
	}
    }

    if(c[CHARMCOUNT]) {
	if(((rand() % 30 + 1) + (5 * monster[mster].level) - c[CHARISMA]) < 30) {
	    lprintf("\nThe %s is awestruck at your magnificence!", lastmonst);

	    return;
	}
    }

    if(mster == BAT) {
	dam = 1;
    }
    else {
	dam = monster[mster].damage;

	if(dam < 1) {
	    dam += ((rand() % 1 + 1) + monster[mster].level);
	}
	else {
	    dam += ((rand() % dam + 1) + monster[mster].level);
	}
    }

    tmp = 0;

    if(monster[mster].attack > 0) {
	if((dam + bias + 8) > c[AC]) {
	    if(spattack(monster[mster].attack, x, y)) {
		flushall();

		return;
	    }
	}
    }
    else {
	if(c[AC] > 0) {
	    if((rand() % c[AC] + 1) == 1) {
		if(spattack(monster[mster].attack, x, y)) {
		    flushall();

		    return;
		}
	    }
	}
	else {
	    if((rand() % 1 + 1) == 1) {
		if(spattack(monster[mster].attack, x, y)) {
		    flushall();

		    return;
		}
	    }
	}
    }

    tmp = 1;
    bias -= 2;
    cursors();

    if((dam + bias) > c[AC]) {
	lprintf("\n The %s hit you ", lastmonst);
	tmp = 1;
	dam -= c[AC];

	if(dam < 0) {
	    dam = 0;
	}

	if(dam > 0) {
	    losehp(dam);
	    bottomhp();
	    flushall();
	}
    }
    else {
	if(c[AC] > 0) {
	    if((rand() % c[AC] + 1) == 1) {
		lprintf("\n The %s hit you ", lastmonst);
		tmp = 1;
		dam -= c[AC];

		if(dam < 0) {
		    dam = 0;
		}

		if(dam > 0) {
		    losehp(dam);
		    bottomhp();
		    flushall();
		}
	    }
	}
	else {
	    if((rand() % 1 + 1) == 1) {
		lprintf("\n The %s hit you ", lastmonst);
		tmp = 1;
		dam -= c[AC];

		if(dam < 0) {
		    dam = 0;
		}

		if(dam > 0) {
		    losehp(dam);
		    bottomhp();
		    flushall();
		}
	    }
	}
    }

    if(tmp == 0) {
	lprintf("\n The %s missed ", lastmonst);
    }
}

/*
 * dropsomething(int monst)
 *
 * Function to create an object when a monster dies
 *
 * Function to create an object near the player when certain monsters are killed
 *
 * Enter with the monster number
 *
 * Returns nothing of value
 */
void dropsomething(int monst)
{
    switch(monst) {
    case ORC:
    case NYMPH:
    case ELF:
    case TROGLODYTE:
    case TROLL:
    case ROTHE:
    case VIOLETFUNGI:
    case PLATINUMDRAGON:
    case GNOMEKING:
    case REDDRAGON:
	something(level);

	return;
    case LEPRECHAUN:
	if((rand() % 101 + 1) >= 75) {
	    creategem();
	}

	if((rand() % 5 + 1) == 1) {
	    dropsomething(LEPRECHAUN);
	}

	return;
    }
}

/*
 * dropgold(int amount)
 *
 * Function to drop some gold around player
 *
 * Enter with the number of gold pieces to drop
 *
 * Returns nothing of value
 */
void dropgold(int amount)
{
    if(amount > 250) {
	createitem(OMAXGOLD, amount / 100);
    }
    else {
	createitem(OGOLDPILE, amount);
    }
}

/*
 * something(int level)
 *
 * Function to create a random item around player
 *
 * Function to create an item from a designed probability around player
 *
 * Enter with the cave level on which something is to be dropped
 *
 * Returns nothing of value
 */
void something(int level)
{
    int j;
    int i;

    /* Correct level? */
    if((level < 0) || (level > (MAXLEVEL + MAXVLEVEL))) {
	return;
    }

    /* Possibly more than one item */
    if((rand() % 101 + 1) < 8) {
	something(level);
    }

    j = newobject(level, &i);
    createitem(j, i);
}

/*
 * newobject(int lev, int *i)
 *
 * Routine to return a randomly selected new object
 *
 * Routine to return a randomly selected object to be created
 *
 * Returns the object number created and set *i for its argument
 *
 * Enter with the cave level and a pointer to the item's arg
 */
static char nobjtab[] = {
    0,
    OSCROLL,
    OSCROLL,
    OSCROLL,
    OSCROLL,
    OPOTION,
    OPOTION,
    OPOTION,
    OPOTION,
    OGOLDPILE,
    OGOLDPILE,
    OGOLDPILE,
    OGOLDPILE,
    OBOOK,
    OBOOK,
    OBOOK,
    OBOOK,
    ODAGGER,
    ODAGGER,
    ODAGGER,
    OLEATHER,
    OLEATHER,
    OLEATHER,
    OREGENRING,
    OPROTRING,
    OENERGYRING,
    ODEXRING,
    OSTRRING,
    OSPEAR,
    OBELT,
    ORING,
    OSTUDLEATHER,
    OSHIELD,
    OFLAIL,
    OCHAIN,
    O2SWORD,
    OPLATE,
    OLONGSWORD
};

int newobject(int lev, int *i)
{
    int tmp = 32;
    int j;

    /* Correct level? */
    if((level < 0) || (level > (MAXLEVEL + MAXVLEVEL))) {
	return 0;
    }

    if(lev > 6) {
	tmp = 37;
    }
    else if(lev > 4) {
	tmp = 35;
    }

    /* The object type */
    tmp = rand() % tmp + 1;

    j = nobjtab[tmp];

    switch(tmp) {
    case 1:
    case 2:
    case 3:
    case 4:
	*i = newscroll();

	break;
    case 5:
    case 6:
    case 7:
    case 8:
	*i = newpotion();

	break;
    case 9:
    case 10:
    case 11:
    case 12:
	*i = rand() % ((lev + 1) * 10) + 1 + (lev * 10) + 10;

	break;
    case 13:
    case 14:
    case 15:
    case 16:
	*i = lev;

	break;
    case 17:
    case 18:
    case 19:
	*i = ndgg[rand() % 13];

	if(!*i) {
	    return 0;
	}

	break;
    case 20:
    case 21:
    case 22:
	if(c[HARDGAME]) {
	    *i = nlpts[rand() % 13];
	}
	else {
	    *i = nlpts[rand() % 12];
	}

	if(!*i) {
	    return 0;
	}

	break;
    case 23:
    case 32:
    case 35:
	*i = rand() % ((lev / 3) + 1);

	break;
    case 24:
    case 26:
	*i = rand() % ((lev / 4) + 1) + 1;

	break;
    case 25:
	*i = rand() % ((lev / 4) + 1);

	break;
    case 27:
	*i = rand() % ((lev / 2) + 1) + 1;

	break;
    case 30:
    case 33:
	*i = rand() % ((lev / 2) + 1);

	break;
    case 28:
	*i = rand() % ((lev / 3) + 1);

	if(*i == 0) {
	    return 0;
	}

	break;
    case 29:
    case 31:
	*i = rand() % ((lev / 2) + 1);

	if(*i == 0) {
	    return 0;
	}

	break;
    case 34:
	*i = nch[rand() % 10];

	break;
    case 36:
	if(c[HARDGAME]) {
	    *i = nplt[rand() % 4];
	}
	else {
	    *i = nplt[rand() % 12];
	}

	break;
    case 37:
	if(c[HARDGAME]) {
	    *i = nsw[rand() % 6];
	}
	else {
	    *i = nsw[rand() % 13];
	}

	break;
    }

    return j;
}

/*
 * spattack(int atckno, int xx, int yy)
 *
 * Function to process special attacks from monsters
 *
 * Enter with the special attack number, and the coordinates (xx, yy)
 * of the monster that is special attacking
 *
 * Returns 1 if must do a show1cell(xx, yy) upon return, 0 otherwise
 *
 * atckno    monster            effect
 * -------------------------------------------------------------------
 *      0    none               None
 *      1    rust monster       Eat armor
 *      2    hell hound         Breath light fire
 *      3    dragon             Breathe fire
 *      4    giant centipede    Weakening sting
 *      5    white dragon       Cold breath
 *      6    wraith             Drain level
 *      7    waterlord          Water gusher
 *      8    leprechaun         Steal gold
 *      9    disenchantress     Disenchant weapon or armor
 *     10    ice lizard         Hits with barbed tail
 *     11    umber hulk         Confusion
 *     12    spirit naga        Cast spells taken from special attacks
 *     13    platinum dragon    Psionics
 *     14    nymph              Steal objects
 *     15    bugbear            Bite
 *     16    osequip            Bite
 */

/*
 * char rustarm[ARMORTYPES][2];
 * Special array for maximum rust damage to armor from rustmonster
 * Format is:
 *   { armor type, minimum attribute }
 */
#define ARMORTYPES 6

static char rustarm[ARMORTYPES][2] = {
    { OSTUDLEATHER, -2 },
    {        ORING, -4 },
    {       OCHAIN, -5 },
    {      OSPLINT, -6 },
    {       OPLATE, -8 },
    {  OPLATEARMOR, -9 }
};

static char spsel[] = { 1, 2, 3, 5, 6, 8, 9, 11, 13, 14 };

int spattack(int x, int xx, int yy)
{
    int i;
    int j = 0;
    int k;
    int m;
    char *p = 0;

    if(c[CANCELLATION]) {
	return 0;
    }

    /* Verify x, y coordinates */
    vxy(&xx, &yy);

    switch(x) {
    case 1:
	/* Rust your armor, j = 1 when rusting has occurred */
	k = c[WEAR];
	m = k;
	i = c[SHIELD];

	if(i != -1) {
	    if(--ivenarg[i] < -1) {
		ivenarg[i] = -1;
	    }
	    else {
		j = 1;
	    }
	}

	if((j == 0) && (k != -1)) {
	    m = iven[k];

	    for(i = 0; i < ARMORTYPES; ++i) {
		/* Find his armor in table */
		if(m == rustarm[i][0]) {
		    if(--ivenarg[k] < rustarm[i][1]) {
			ivenarg[k] = rustarm[i][1];
		    }
		    else {
			j = 1;
		    }

		    break;
		}
	    }
	}

	/* If rusting did not occur */
	if(j == 0) {
	    switch(m) {
	    case OLEATHER:
		p = "\nThe %s hit you -- You're luck you have leather on";

		break;
	    case OSSPLATE:
		p = "\nThe %s hit you -- You're fortunate to have stainless steel armor!";

		break;
	    }
	}
	else {
	    beep();
	    p = "\nThe %s hit you -- Your armor feels weaker";
	}

	break;
    case 2:
	i = rand() % 15 + 1 + 8 - c[AC];
	p = "\nThe %s breathes fire at you!";

	if(c[FIRERESISTANCE]) {
	    p = "\nThe %s's flame doesn't phase you!";
	}
	else if(p) {
	    lprintf(p, lastmonst);
	    beep();
	}

	checkloss(i);

	return 0;
    case 3:
	i = rand() % 20 + 1 + 25 - c[AC];
	p = "\nthe %s breathes fire at you!";
	
	if(c[FIRERESISTANCE]) {
	    p = "\nThe %s's flame doesn't phase you!";
	}
	else if(p) {
	    lprintf(p, lastmonst);
	    beep();
	}

	checkloss(i);

	return 0;
    case 4:
	if(c[STRENGTH] > 3) {
	    p = "\nThe %s stung you! You feel weaker";
	    beep();
	    --c[STRENGTH];
	}
	else {
	    p = "\nThe %s stung you!";
	}

	break;
    case 5:
	p = "\nThe %s blasts you with his cold breath";
	i = rand() % 15 + 1 + 18 - c[AC];

	if(p) {
	    lprintf(p, lastmonst);
	    beep();
	}

	checkloss(i);

	return 0;
    case 6:
	lprintf("\nThe %s drains you of your life energy!", lastmonst);
	loselevel();
	beep();

	return 0;
    case 7:
	p = "\nThe %s got you with a gusher!";
	i = rand() % 15 + 1 + 25 - c[AC];

	if(p) {
	    lprintf(p, lastmonst);
	    beep();
	}

	checkloss(i);

	return 0;
    case 8:
	/* He has a device of no theft */
	if(c[NOTHEFT]) {
	    return 0;
	}

	if(c[GOLD]) {
	    p = "\nThe %s hit you -- Your purse feel lighter";

	    if(c[GOLD] > 32767) {
		c[GOLD] >>= 1;
	    }
	    else {
		c[GOLD] -= (rand() % (1 + (c[GOLD] >> 1)) + 1);
	    }

	    if(c[GOLD] < 0) {
		c[GOLD] = 0;
	    }
	}
	else {
	    p = "\nThe %s couldn't find any gold to steal";
	}
	
	lprintf(p, lastmonst);
	disappear(xx, yy);
	beep();
	bottomgold();

	return 1;
    case 9:
	/* Disenchant */
	j = 50;

	while(1) {
	    i = rand() % 26;
	    m = iven[i];

	    /* Randomly select item */
	    if((m > 0)
	       && (ivenarg[i] > 0)
	       && (m != OSCROLL)
	       && (m != OPOTION)) {
		ivenarg[i] -= 3;

		if(ivenarg[i] < 0) {
		    ivenarg[i] = 0;
		}

		lprintf("\nThe %s hits you -- you feel a sense of loss", lastmonst);
		srcount = 0;
		beep();
		show3(i);
		bottomline();

		return 0;
	    }

	    if(--j <= 0) {
		p = "\nThe %s nearly misses";

		break;
	    }

	    break;
	}

	break;
    case 10:
	p = "\nThe %s hit you with his barbed tail";
	i = rand() % 25 + 1 - c[AC];

	if(p) {
	    lprintf(p, lastmonst);
	    beep();
	}

	checkloss(i);

	return 0;
    case 11:
	p = "\nThe %s has confused you";
	beep();
	c[CONFUSE] += (10 + rand() % 10 + 1);

	break;
    case 12:
	/* Performs any number of other special attacks */
	
	return spattack(spsel[rand() % 10], xx, yy);
    case 13:
	p = "\nThe %s flattens you with his psionics!";
	i = rand() % 15 + 1 + 30 - c[AC];

	if(p) {
	    lprintf(p, lastmonst);
	    beep();
	}

	checkloss(i);

	return 0;
    case 14:
	/* He has a device of no theft */
	if(c[NOTHEFT]) {
	    return 0;
	}

	if(emptyhanded() == 1) {
	    p = "\nThe %s couldn't find anything to steal";

	    break;
	}

	lprintf("\nThe %s picks your pocket and takes:", lastmonst);
	beep();

	if(stealsomething() == 0) {
	    lprcat(" nothing");
	    disappear(xx, yy);
	}

	bottomline();

	return 1;
    case 15:
	i = rand() % 10 + 1 + 5 - c[AC];
	p = "\nThe %s bit you!";

	if(p) {
	    lprintf(p, lastmonst);
	    beep();
	}

	checkloss(i);

	return 0;
    case 16:
	i = rand() % 15 + 1 + 10 - c[AC];
	p = "\nThe %s bit you!";

	if(p) {
	    lprintf(p, lastmonst);
	    beep();
	}

	checkloss(i);

	return 0;
    }

    if(p) {
	lprintf(p, lastmonst);
	bottomline();
    }

    return 0;
}

/*
 * checkloss(int x)
 *
 * Routine to subtract hp from user and flag bottomline display
 *
 * Routine to subtract hitpoints from the user and flag the bottomline display
 *
 * Enter with the number of hit points to lose
 * 
 * Note: if x > c[HP] this routine could kill the player!
 */
void checkloss(int x)
{
    if(x > 0) {
	losehp(x);
	bottomhp();
    }
}

/*
 * annihilate()
 *
 * Routine to annihilate all monsters around player (playerx, playery)
 *
 * Gives player experience, but no dropped objects
 *
 * Returns the experience gained from all monsters killed
 */
int annihilate()
{
    int i;
    int j;
    long k;
    char *p;

    k = 0;

    for(i = (playerx - 1); i <= (playerx + 1); ++i) {
	for(j = (playery - 1); j <= (playery + 1); ++j) {
	    /* If not out of bounds */
	    if(!vxy(&i, &j)) {
		p = &mitem[i][j];

		/* If not out of bounds */
		if(*p) {
		    if(*p < (DEMONLORD + 2)) {
			k += monster[(int)*p].experience;
			know[i][j] = 0;
			*p = know[i][j];
		    }
		    else {
			lprintf("\nThe %s barely escapes being annihilated!", monster[(int)*p].name);

			/* Lose half hit points */
			hitp[i][j] = (hitp[i][j] >> 1) + 1;
		    }
		}
	    }
	}
    }

    if(k > 0) {
	lprcat("\nYou hear loud screams of agnoy!");
	raiseexperience(k);
    }

    return k;
}

/*
 * newsphere(int x, int y, int dir, int lifetime)
 *
 * Function to create a new sphere of annihilation
 *
 * Enter with the coordinates for the sphere in (x, y), the direction
 * (0 - 8 diroffx format) in dir, and the lifespan of the sphere in
 * lifetime (in turns)
 *
 * Returns the number of spheres currently in existence
 */
int newsphere(int x, int y, int dir, int life)
{
    int m;
    struct sphere *sp;

    sp = (struct sphere *)malloc(sizeof(struct sphere));

    /* Can't malloc, therefore failure */
    if(sp == 0) {
	return c[SPHCAST];
    }

    /* No movement if direction not found */
    if(dir >= 9) {
	dir = 0;
    }

    /* Don't go out of bounds */
    if(level == 0) {
	vxy(&x, &y);
    }
    else {
	if(x < 1) {
	    x = 1;
	}
	
	if(x >= (MAXX - 1)) {
	    x = MAXX - 2;
	}
	
	if(y < 1) {
	    y = 1;
	}

	if(y >= (MAXY - 1)) {
	    y = MAXY - 2;
	}
    }

    m = mitem[x][y];

    /* Demons dispel spheres */
    if(m >= (DEMONLORD + 4)) {
	/* Show the demon (ha ha) */
	know[x][y] = 1;
	show1cell(x, y);
	cursors();
	lprintf("\nThe %s dispel the sphere!", monster[m].name);
	beep();

	/* Removes any spheres that are here */
	rmsphere(x, y);

	return c[SPHCAST];
    }

    /* Disenchantress cancels spheres */
    if(m == DISENCHANTRESS) {
	cursors();
	lprintf("\nThe %s causes cancellation of the sphere!", monster[m].name);
	beep();

	/* Blow up stuff around sphere */
	sphboom(x, y);

	/* Remove any spheres that are here */
	rmsphere(x, y);

	return c[SPHCAST];
    }

    /* Cancellation cancels spheres */
    if(c[CANCELLATION]) {
	cursors();
	lprcat("\nAs the cancellation takes effect, you hear a great earth shaking blast!");
	beep();

	/* Blow up stuff around sphere */
	sphboom(x, y);

	/* Remove any spheres that are here */
	rmsphere(x, y);

	return c[SPHCAST];
    }

    /* Collision of spheres detonates spheres */
    if(item[x][y] == OANNIHILATION) {
	cursors();
	lprcat("\nTwo spheres of annihilation collide! You hear a great earth shaking blast!");
	beep();
	rmsphere(x, y);

	/* Blow up stuff around sphere */
	sphboom(x, y);

	/* Remove any spheres that are here */
	rmsphere(x, y);

	return c[SPHCAST];
    }

    /* Collision of sphere and player! */
    if((playerx == x) && (playery == y)) {
	cursors();
	lprcat("\nYou have been enveloped by the zone of nothingness!\n");
	beep();

	/* Remove any spheres that are here */
	rmsphere(x, y);
	nap(4000);
	died(258);
    }

    item[x][y] = OANNIHILATION;
    mitem[x][y] = 0;
    know[x][y] = 1;

    /* Show the new sphere */
    show1cell(x, y);
    sp->x = x;
    sp->y = y;
    sp->lev = level;
    sp->dir = dir;
    sp->lifetime = life;
    sp->p = 0;

    /* If first node in this sphere list */
    if(spheres == 0) {
	spheres = sp;
    }
    else {
	/* Add sphere to beginning of linked list */
	sp->p = spheres;
	spheres = sp;
    }

    /* One more sphere in the world */
    ++c[SPHCAST];

    return c[SPHCAST];
}

/*
 * rmsphere(int x, int y)
 *
 * Function to delete a sphere of annihilation from list
 *
 * Enter with the coordinates of the sphere (on current level)
 *
 * Returns the number of spheres currently in existence
 */
int rmsphere(int x, int y)
{
    struct sphere *sp;
    struct sphere *sp2 = 0;

    for(sp = spheres; sp; sp = sp->p) {
	/* Is sphere on this level? */
	if(level == sp->lev) {
	    /* Locate sphere at this location */
	    if((x == sp->x) && (y == sp->y)) {
		mitem[x][y] = 0;
		item[x][y] = mitem[x][y];
		know[x][y] = 1;

		/* Show the now missing sphere */
		show1cell(x, y);
		--c[SPHCAST];

		if(sp == spheres) {
		    sp2 = sp;
		    spheres = sp->p;
		    free((char *)sp2);
		}
		else {
		    sp2->p = sp->p;
		    free((char *)sp);
		}

		break;
	    }
	}
	
	sp2 = sp;
    }

    /* Return number of spheres in the world */
    return c[SPHCAST];
}

/*
 * sphboom(int x, int y)
 *
 * Function to perform the effects of a sphere detonation
 *
 * Enter with the coordinates of the blast
 *
 * Returns no value
 */
void sphboom(int x, int y)
{
    int i;
    int j;

    if(c[HOLDMONST]) {
	c[HOLDMONST] = 1;
    }

    if(c[CANCELLATION]) {
	c[CANCELLATION] = 1;
    }

    for(j = max(1, x - 2); j < min(x + 3, MAXX - 1); ++j) {
	for(i = max(1, y - 2); i < min(y + 3, MAXY - 1); ++i) {
	    mitem[j][i] = 0;
	    item[j][i] = mitem[j][i];
	    show1cell(j, i);

	    if((playerx == j) && (playery == i)) {
		cursors();
		beep();
		lprcat("\nYou were too close to the sphere!");
		nap(3000);

		/* Player killed in explosion */
		died(283);
	    }
	}
    }
}

/*
 * genmonst()
 *
 * Function to ask for monster and genocide from game
 *
 * This is done by setting a flag in the monster[] structure
 */
void genmonst()
{
    int i;
    int j;

    cursors();
    lprcat("\nGenocide what monster? ");
    i = 0;

    while(!isalpha(i) && (i != ' ')) {
	i = getchar();
    }

    lprc(i);

    /* Search for the monster type */
    for(j = 0; j < MAXMONST; ++j) {
	/* Have we found it? */
	if(monstnamelist[j] == i) {
	    /* Genocided from game */
	    monster[i].genocided = 1;

	    lprintf(" There will be no more %s's", monster[j].name);

	    /* Now wipe out monsters on this level */
	    newcavelevel(level);
	    draws(0, MAXX, 0, MAXY);
	    bot_linex();

	    return;
	}
    }

    lprcat(" You sense failure!");
}
