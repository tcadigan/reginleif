/*
 * global.c
 * Larn is copyrighted 1986 by Noah Morgan.
 *
 * raiselevel()          subroutine to raise the player by one level
 * loselevel()           subroutine to lower the player by one level
 * raiseexperience(x)    subroutine to increase experience points
 * loseexperience(x)     subroutine to lose experience points
 * losehp(x)             subroutine to remove hit points form the player
 * losemhp(x)            subroutine to remove max # hit points from the player
 * raisehp(x)            subroutine to gain hit points
 * raisemhp(x)           subroutine to gain maximum hit points
 * losespells(x)         subroutine to lose spells
 * losemspells(x)        subroutine to lose maximum spells
 * raisespells(x)        subroutine to gain spells
 * raisemspells(x)       subroutine to gain maximum spells
 * recalc()              function to recalculate the armor class of the player
 * makemonst(lev)        function to return monster number for a randomly selected
 *                         monster
 * positionplayer()      function to be sure player is not in a wall
 * quit()                subroutine to ask if the player really wants to quit
 */

#include "global.h"

#include "display.h"
#include "header.h"
#include "io.h"
#include "main.h"
#include "monster.h"
#include "nap.h"
#include "scores.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern int score[];
extern int srcount;
extern int dropflag;

/* The random number seed */
extern short playerx;
extern short playery;
extern short lastnum;
extern char cheat;
extern char level;
extern char monstnamelist[];
extern char lastmonst[];
extern char *what[];
extern char *who[];
extern char winner[];
extern char logname[];
extern char monstlevel[];
extern char sciv[SCORESIZE + 1][26][2];
extern char *potionname[];
extern char *scrollname[];

/*
 * ***********
 * RAISE LEVEL
 * ***********
 *
 * raiselevel()
 * 
 * Subroutine to raise the player level uses the skill[] array to find
 * level boundaries uses c[EXPERIENCE], c[LEVEL]
 */
void raiselevel()
{
    if(c[LEVEL] < MAXPLEVEL) {
	raiseexperience((long)(skill[c[LEVEL]] - c[EXPERIENCE]));
    }
}

/*
 * **********
 * LOSE LEVEL
 * **********
 *
 * loselevel()
 *
 * Subroutine to lower the player's character level by one
 */
void loselevel()
{
    if(c[LEVEL] > 1) {
	loseexperience((long)(c[EXPERIENCE] - skill[c[LEVEL] - 1] + 1));
    }
}

/*
 * ****************
 * RAISE EXPERIENCE
 * ****************
 *
 * raiseexperience()
 *
 * Subroutine to increase experience points
 */
void raiseexperience(long x)
{
    int i;
    int tmp;

    i = c[LEVEL];
    c[EXPERIENCE] += x;

    while((c[EXPERIENCE] >= skill[c[LEVEL]]) && (c[LEVEL] < MAXPLEVEL)) {
	tmp = (c[CONSTITUTION] - c[HARDGAME]) >> 1;
	++c[LEVEL];
	
	if(tmp > 0) {
	    raisemhp((int)((rand() % 3 + 1) + (rand() % tmp + 1)));
	}
	else {
	    raisemhp((int)((rand() % 3 + 1) + (rand() % 1 + 1)));
	}

	raisemspells((int)rand() % 3);

	if(c[LEVEL] < (7 - c[HARDGAME])) {
	    raisemhp((int)(c[CONSTITUTION] >> 2));
	}
    }

    if(c[LEVEL] != i) {
	cursors();
	beep();

	/* If we changed levels */
	lprintf("\nWelcome to level %d", (long)c[LEVEL]);
    }
    
    bottomline();
}

/*
 * ****************
 * LOOSE EXPERIENCE
 * ****************
 *
 * loseexperience()
 *
 * Subroutine to lose experience points
 */
void loseexperience(long x)
{
    int i;
    int tmp;

    i = c[LEVEL];
    c[EXPERIENCE] -= x;

    if(c[EXPERIENCE] < 0) {
	c[EXPERIENCE] = 0;
    }

    while(c[EXPERIENCE] < skill[c[LEVEL] - 1]) {
	/* Down one level */
	--c[LEVEL];

	if(c[LEVEL] <= 1) {
	    c[LEVEL] = 1;
	}

	/* Lose hpoints */
	tmp = (c[CONSTITUTION] - c[HARDGAME]) >> 1;

	/* Lose hpoints */
	if(tmp > 0) {
	    losemhp((int)(rand() % tmp + 1));
	}
	else {
	    losemhp((int)(rand() % 1 + 1));
	}

	if(c[LEVEL] < (7 - c[HARDGAME])) {
	    losemhp((int)(c[CONSTITUTION] >> 2));
	}

	/* Lose spells */
	losemspells((int)rand() % 3);
    }

    if(i != c[LEVEL]) {
	cursors();
	beep();
	lprintf("\nYou went down to level %d!", (long)c[LEVEL]);
    }

    bottomline();
}

/*
 * ********
 * LOOSE HP
 * ********
 *
 * losehp(x)
 * losemhp(x)
 * 
 * Subroutine to remove hitpoints form the player
 * WARNING -- Will kill player if hp goes to zero
 */
void losehp(int x)
{
    c[HP] -= x;

    if(c[HP] <= 0) {
	beep();
	lprcat("\n");
	nap(3000);
	died(lastnum);
    }
}

void losemhp(int x)
{
    c[HP] -= x;

    if(c[HP] < 1) {
	c[HP] = 1;
    }

    c[HPMAX] -= x;

    if(c[HPMAX] < 1) {
	c[HPMAX] = 1;
    }
}

/*
 * ********
 * RAISE HP
 * ********
 * 
 * raisehp(x)
 * raisemhp(x)
 *
 * Subroutine to gain maximum hit points
 */
void raisehp(int x)
{
    c[HP] += x;

    if(c[HP] > c[HPMAX]) {
	c[HP] = c[HPMAX];
    }
}

void raisemhp(int x)
{
    c[HPMAX] += x;
    c[HP] += x;
}

/*
 * ************
 * RAISE SPELLS
 * ************
 *
 * raisespells(x)
 * raisemspells(x)
 *
 * Subroutine to gain maximum spells
 */
void raisespells(int x)
{
    c[SPELLS] += x;

    if(c[SPELLS] > c[SPELLMAX]) {
	c[SPELLS] = c[SPELLMAX];
    }
}

void raisemspells(int x)
{
    c[SPELLMAX] += x;
    c[SPELLS] += x;
}

/*
 * ************
 * LOOSE SPELLS
 * ************
 * 
 * losespells(x)
 * losemspells(x)
 *
 * Subroutine to lose maximum spells
 */
void losespells(int x)
{
    c[SPELLS] -= x;

    if(c[SPELLS] < 0) {
	c[SPELLS] = 0;
    }
}

void losemspells(int x)
{
    c[SPELLMAX] -= x;

    if(c[SPELLMAX] < 0) {
	c[SPELLMAX] = 0;
    }

    c[SPELLS] -= x;

    if(c[SPELLS] < 0) {
	c[SPELLS] = 0;
    }
}

/*
 * makemonst(int lev)
 *
 * Function to return monster number for a randomly selected monster
 * for the given cave level
 */
int makemonst(int lev)
{
    int tmp;
    int x;

    if(lev < 1) {
	lev = 1;
    }

    if(lev > 12) {
	lev = 12;
    }

    tmp = WATERLORD;

    if(lev < 5) {
	while(tmp == WATERLORD) {
	    x = monstlevel[lev - 1];

	    if(x) {
		tmp = rand() % x + 1;
	    }
	    else {
		tmp = 1;
	    }
	}
    }
    else {
	while(tmp == WATERLORD) {
	    x = monstlevel[lev - 1] - monstlevel[lev - 4];

	    if(x) {
		tmp = rand() % x + 1 + monstlevel[lev - 4];
	    }
	    else {
		tmp = rand() % 1 + 1 + monstlevel[lev - 4];
	    }
	}
    }

    /* Genocided? */
    while(monster[tmp].genocided && (tmp < MAXMONST)) {
	++tmp;
    }

    return tmp;
}

/*
 * positionplayer()
 * 
 * Funciton to be sure player is not in a wall
 */
void positionplayer()
{
    int try;

    try = 2;

    while((item[playerx][playery] || mitem[playerx][playery]) && try) {
	++playerx;

	if(playerx >= (MAXX - 1)) {
	    playerx = 1;

	    ++playery;

	    if(playery >= (MAXY - 1)) {
		playery = 1; --try;
	    }
	}
    }

    if(try == 0) {
	lprcat("Failure in positionplayer\n");
    }
}

/*
 * recalc()
 *
 * Function to recalculate the armor class of the player
 */
void recalc()
{
    int i;
    int j;
    int k;

    c[AC] = c[MOREDEFENCES];

    if(c[WEAR] >= 0) {
	switch(iven[c[WEAR]]) {
	case OSHIELD:
	    c[AC] += (2 + ivenarg[c[WEAR]]);

	    break;
	case OLEATHER:
	    c[AC] += (2 + ivenarg[c[WEAR]]);

	    break;
	case OSTUDLEATHER:
	    c[AC] += (3 + ivenarg[c[WEAR]]);

	    break;
	case ORING:
	    c[AC] += (5 + ivenarg[c[WEAR]]);

	    break;
	case OCHAIN:
	    c[AC] += (6 + ivenarg[c[WEAR]]);

	    break;
	case OSPLINT:
	    c[AC] += (7 + ivenarg[c[WEAR]]);

	    break;
	case OPLATE:
	    c[AC] += (9 + ivenarg[c[WEAR]]);

	    break;
	case OPLATEARMOR:
	    c[AC] += (10 + ivenarg[c[WEAR]]);

	    break;
	case OSSPLATE:
	    c[AC] += (12 + ivenarg[c[WEAR]]);

	    break;
	}
    }

    if(c[SHIELD] >= 0) {
	if(iven[c[SHIELD]] == OSHIELD) {
	    c[AC] += (2 + ivenarg[c[SHIELD]]);
	}
    }

    if(c[WIELD] < 0) {
	c[WCLASS] = 0;
    }
    else {
	i = ivenarg[c[WIELD]];

	switch(iven[c[WIELD]]) {
	case ODAGGER:
	    c[WCLASS] = 3 + i;

	    break;
	case OBELT:
	    c[WCLASS] = 7 + i;

	    break;
	case OSHIELD:
	    c[WCLASS] = 8 + i;

	    break;
	case OSPEAR:
	    c[WCLASS] = 10 + i;

	    break;
	case OFLAIL:
	    c[WCLASS] = 14 + i;

	    break;
	case OBATTLEAXE:
	    c[WCLASS] = 17 + i;

	    break;
	case OLANCE:
	    c[WCLASS] = 19 + i;

	    break;
	case OLONGSWORD:
	    c[WCLASS] = 22 + i;

	    break;
	case O2SWORD:
	    c[WCLASS] = 26 + i;

	    break;
	case OSWORDOFSLASHING:
	    c[WCLASS] = 32 + i;

	    break;
	case OHAMMER:
	    c[WCLASS] = 35 + i;

	    break;
	default:
	    c[WCLASS] = 0;
	}
    }

    c[WCLASS] += c[MOREDAM];

    /* Now for regeneration abilities based on rings */
    c[REGEN] = 1;
    c[ENERGY] = 0;
    j = 0;

    for(k = 25; k > 0; --k) {
	if(iven[k]) {
	    j = k;
	    k = 0;
	}
    }

    for(i = 0; i <= j; ++i) {
	switch(iven[i]) {
	case OPROTRING:
	    c[AC] += (ivenarg[i] + 1);

	    break;
	case ODAMRING:
	    c[WCLASS] += (ivenarg[i] + 1);

	    break;
	case OBELT:
	    c[WCLASS] += ((ivenarg[i] << 1) + 2);

	    break;
	case OREGENRING:
	    c[REGEN] += (ivenarg[i] + 1);

	    break;
	case ORINGOFEXTRA:
	    c[REGEN] += (5 * (ivenarg[i] + 1));

	    break;
	case OENERGYRING:
	    c[ENERGY] += (ivenarg[i] + 1);

	    break;
	}
    }
}

/*
 * quit()
 * 
 * Subroutine to ask if the player really wants to quit
 */
void quit()
{
    int i;

    cursors();
    strcpy(lastmonst, "");
    lprcat("\n\ndo you really want to quit?");

    while(1) {
	i = getchar();

	if(i == 'y') {
	    died(300);

	    return;
	}

	if((i == 'n') || (i == '\33')) {
	    lprcat(" no");
	    lflush();

	    return;
	}

	lprcat("\n");
	setbold();
	lprcat("Yes");
	resetbold();
	lprcat(" or ");
	setbold();
	lprcat("No");
	resetbold();
	lprcat(" please? Do you want to quit? ");
    }
}

/*
 * Function to ask --more-- then the user must enter a space
 */
void more()
{
    lprcat("\n --- press ");
    standout();
    lprcat("space");
    standend();
    lprcat(" to continue --- ");

    while(1) {
	if(getchar() == ' ') {
	    break;
	}
    }
}

/*
 * Function to put something in the player's inventory.
 * Returns 0 if success, 1 if a failure
 */
int take(int itm, int arg)
{
    int i;
    int limit;

    /* cursors(); */
    limit = 15 + (c[LEVEL] >> 1);

    if(limit > 26) {
	limit = 26;
    }

    for(i = 0; i < limit; ++i) {
	if(iven[i] == 0) {
	    iven[i] = itm;
	    ivenarg[i] = arg;
	    limit = 0;

	    switch(itm) {
	    case OPROTRING:
	    case ODAMRING:
	    case OBELT:
		limit = 1;

		break;
	    case ODEXRING:
		c[DEXTERITY] += (ivenarg[i] + 1);
		limit = 1;

		break;
	    case OSTRRING:
		c[STREXTRA] += (ivenarg[i] + 1);
		limit = 1;

		break;
	    case OCLEVERRING:
		c[INTELLIGENCE] += (ivenarg[i] + 1);
		limit = 1;

		break;
	    case OHAMMER:
		c[DEXTERITY] += 10;
		c[STREXTRA] += 10;
		c[INTELLIGENCE] -= 10;
		limit = 1;

		break;
	    case OORBOFDRAGON:
		++c[SLAYING];

		break;
	    case OSPIRITSCARAB:
		++c[NEGATESPIRIT];

		break;
	    case OCUBEOFUNDEAD:
		++c[CUBEOFUNDEAD];

		break;
	    case ONOTHEFT:
		++c[NOTHEFT];

		break;
	    case OSWORDOFSLASHING:
		c[DEXTERITY] += 5;
		limit = 1;

		break;
	    }

	    lprcat("\nYou pick up:");
	    srcount = 0;
	    show3(i);

	    if(limit) {
		bottomline();

		return 0;
	    }
	}
    }

    lprcat("\nYou can't carry anything else");

    return 1;
}

/* 
 * Subroutine to drop an object.
 * Returns 1 if something there already else 0
 */
int drop_object(int k)
{
    int itm;

    if((k < 0) || (k < 25)) {
	return 0;
    }

    itm = iven[k];
    cursors();

    if(itm == 0) {
	lprintf("\nYou don't have item %c! ", k + 'a');

	return 1;
    }

    if(item[playerx][playery]) {
	beep();
	lprcat("\nThere's something here already");

	return 1;
    }

    /* Not in entrance */
    if((playery == (MAXY - 1)) && (playerx == 33)) {
	return 1;
    }

    item[playerx][playery] = itm;
    iarg[playerx][playery] = ivenarg[k];
    srcount = 0;
    lprcat("\n You drop:");

    /* Show what item you dropped */
    show3(k);

    know[playerx][playery] = 0;
    iven[k] = 0;

    if(c[WIELD] == k) {
	c[WIELD] = -1;
    }

    if(c[WEAR] == k) {
	c[WEAR] = -1;
    }

    if(c[SHIELD] == k) {
	c[SHIELD] = -1;
    }

    adjustcvalues(itm, ivenarg[k]);

    /* Say dropped an item so won't ask to pick it up right away */
    dropflag = 1;

    return 0;
}

/*
 * Function to enchant armor player is currently wearing
 */
void enchantarmor()
{
    int tmp;

    if(c[WEAR] < 0) {
	if(c[SHIELD] < 0) {
	    cursors();
	    beep();
	    lprcat("\nYou feel a sense of loss");

	    return;
	}
	else {
	    tmp = iven[c[SHIELD]];

	    if(tmp != OSCROLL) {
		if(tmp != OPOTION) {
		    ++ivenarg[c[SHIELD]];
		    bottomline();
		}
	    }
	}
    }

    tmp = iven[c[WEAR]];

    if(tmp != OSCROLL) {
	if(tmp != OPOTION) {
	    ++ivenarg[c[WEAR]];
	    bottomline();
	}
    }
}

/*
 * Function to enchant a weapon presently being wielded
 */
void enchweapon()
{
    int tmp;

    if(c[WIELD] < 0) {
	cursors();
	beep();
	lprcat("\nYou feel a sense of loss");

	return;
    }

    tmp = iven[c[WIELD]];

    if(tmp != OSCROLL) {
	if(tmp != OPOTION) {
	    ++ivenarg[c[WIELD]];

	    if(tmp == OCLEVERRING) {
		++c[INTELLIGENCE];
	    }
	    else if(tmp == OSTRRING) {
		++c[STREXTRA];
	    }
	    else if(tmp == ODEXRING) {
		++c[DEXTERITY];
	    }

	    bottomline();
	}
    }
}

/*
 * Routine to tell if player can carry one more thing.
 * Returns 1 if pockets are full, else 0
 */
int pocketful()
{
    int i;
    int limit;

    limit = 15 + (c[LEVEL] >> 1);

    if(limit > 26) {
	limit = 26;
    }

    for(i = 0; i < limit; ++i) {
	if(iven[i] == 0) {
	    return 0;
	}
    }

    return 1;
}

/*
 * Function to return 1 if a monster is next to the palyer else returns 0
 */
int nearbymonst()
{
    int tmp;
    int tmp2;

    for(tmp = (playerx - 1); tmp < (playerx + 2); ++tmp) {
	for(tmp2 = (playery - 1); tmp2 < (playery + 2); ++tmp2) {
	    /* If monster nearby */
	    if(mitem[tmp][tmp2]) {
		return 1;
	    }
	}
    }

    return 0;
}

/*
 * Function to steal an item from the player's pockets.
 * Returns 1 if steals something else returns 0
 */
int stealsomething()
{
    int i;
    int j;

    j = 100;

    while(1) {
	i = rand() % 26;

	if(iven[i]) {
	    if(c[WEAR] != i) {
		if(c[WIELD] != i) {
		    if(c[SHIELD] != i) {
			srcount = 0;
			show3(i);
			adjustcvalues(iven[i], ivenarg[i]);
			iven[i] = 0;

			return 1;
		    }
		}
	    }
	}

	--j;

	if(j <= 0) {
	    return 0;
	}
    }
}

/*
 * Function to return 1 if player carries nothing else return 0
 */
int emptyhanded()
{
    int i;

    for(i = 0; i < 26; ++i) {
	if(iven[i]) {
	    if(i != c[WIELD]) {
		if(i != c[WEAR]) {
		    if(i != c[SHIELD]) {
			return 0;
		    }
		}
	    }
	}
    }

    return 1;
}

/*
 * Function to create a gem on a square near the player
 */
void creategem()
{
    int i;
    int j;

    switch(rand() % 4 + 1) {
    case 1:
	i = ODIAMOND;
	j = 50;

	break;
    case 2:
	i = ORUBY;
	j = 40;

	break;
    case 3:
	i = OEMERALD;
	j = 30;

	break;
    default:
	i = OSAPPHIRE;
	j = 20;

	break;
    }

    createitem(i, rand() % j + 1 + (j / 10));
}

/*
 * Function to change character levels as needed when dropping an
 * object that affects these characteristics
 */
void adjustcvalues(int itm, int arg)
{
    int flag;

    flag = 0;

    switch(itm) {
    case ODEXRING:
	c[DEXTERITY] -= (arg + 1);
	flag = 1;

	break;
    case OSTRRING:
	c[STREXTRA] -= (arg + 1);
	flag = 1;

	break;
    case OCLEVERRING:
	c[INTELLIGENCE] -= (arg + 1);
	flag = 1;

	break;
    case OHAMMER:
	c[DEXTERITY] -= 10;
	c[STREXTRA] -= 10;
	c[INTELLIGENCE] += 10;
	flag = 1;

	break;
    case OSWORDOFSLASHING:
	c[DEXTERITY] -= 5;
	flag = 1;

	break;
    case OORBOFDRAGON:
	--c[SLAYING];

	return;
    case OSPIRITSCARAB:
	--c[NEGATESPIRIT];

	return;
    case ONOTHEFT:
	--c[NOTHEFT];

	return;
    case OLANCE:
	c[LANCEDEATH] = 0;

	return;
    case OPOTION:
    case OSCROLL:

	return;
    default:
	flag = 1;
    }

    if(flag) {
	bottomline();
    }
}

/*
 * Function to read a string from token input "string".
 * Returns a pointer to the string
 */
void gettockstr(char *str)
{
    int i;
    int j;

    i = 50;

    while(getchar() != '"') {
	--i;

	if(i <= 0) {
	    break;
	}
    }

    i = 36;
    --i;
    
    while(i > 0) {
	j = getchar();

	if(j != '"') {
	    *str++ = j;
	}
	else {
	    i = 0;
	}
	
	--i;
    }

    *str = 0;

    i = 50;

    /* If end due to too long, then find closing quote */
    if(j != '"') {
	while(getchar() != '"') {
	    --i;

	    if(i <= 0) {
		break;
	    }
	}
    }
}

/*
 * Function to ask user for a password (no echo).
 * Returns 1 if entered correctly, 0 if not
 */
static char gpwbuf[33];

int getpassword()
{
    int i;
    int j;
    char *gpwp;
    extern char *password;

    scbr();
    /* system("stty -echo cbreak"); */
    gpwp = gpwbuf;
    lprcat("\nEnter Password: ");
    lflush();
    i = strlen(password);

    for(j = 0; j < i; ++j) {
	if(read(STDIN_FILENO, gpwp, 1) == -1) {
	    exit(1);
	}
	
	++gpwp;
	gpwbuf[i] = 0;
    }

    sncbr();
    /* system("stty echo -cbreak"); */

    if(strcmp(gpwbuf, password) != 0) {
	lprcat("\nSorry\n");
	lflush();

	return 0;
    }
    else {
	return 1;
    }
}

/*
 * Subroutine to get a yes or no response from the user.
 * returns y or n
 */
int getyn()
{
    int i;

    i = 0;

    while((i != 'y') && (i != 'n') && (i != '\33')) {
	i = getchar();
    }

    return i;
}

/*
 * Function to calculate the pack weight of the player.
 * Returns the number of pounds the player is carrying
 */
int packweight()
{
    int i;
    int j;
    int k;

    k = c[GOLD] / 1000;
    j = 25;

    while((iven[j] == 0) && (j > 0)) {
	--j;
    }

    for(i = 0; i < j; ++i) {
	switch(iven[i]) {
	case 0:

	    break;
	case OSSPLATE:
	case OPLATEARMOR:
	    k += 40;

	    break;
	case OPLATE:
	    k += 35;

	    break;
	case OHAMMER:
	    k += 30;

	    break;
	case OSPLINT:
	    k += 26;

	    break;
	case OSWORDOFSLASHING:
	case OCHAIN:
	case OBATTLEAXE:
	case O2SWORD:
	    k += 23;

	    break;
	case OLONGSWORD:
	case OSWORD:
	case ORING:
	case OFLAIL:
	    k += 20;

	    break;
	case OLANCE:
	case OSTUDLEATHER:
	    k += 15;

	    break;
	case OLEATHER:
	case OSPEAR:
	    k += 8;

	    break;
	case OORBOFDRAGON:
	case OBELT:
	    k += 4;

	    break;
	case OSHIELD:
	    k += 7;

	    break;
	case OCHEST:
	    k += (30 + ivenarg[i]);

	    break;
	default:
	    ++k;
	}
    }

    return k;
}
