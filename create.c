/*
 * create.c
 * Larn is copyrighted 1986 by Noah Morgan.
 */

#include "create.h"

#include "global.h"
#include "header.h"
#include "io.h"
#include "monster.h"
#include "savelev.h"
#include "scores.h"

#include <curses.h>
#include <stdlib.h>
#include <unistd.h>

extern char larnlevels[];
extern char beenhere[];
extern char wizard;
extern char level;
extern short oldx;
extern short oldy;

static void fillroom(int arg, char what);

/*
 * makeplayer()
 *
 * Subroutine to create the player and the player's attributes this is
 * called at the beginning of the game and at no other time
 */
void makeplayer()
{
    int i;

    scbr();
    clear();

    /* Start player off with 15 hit points */
    c[HP] = 10;
    c[HPMAX] = c[HP];

    /* Player starts at level one */
    c[LEVEL] = 1;

    /* Total # spells starts off as 3 */
    c[SPELLS] = 1;
    c[SPELLMAX] = c[SPELLS];

    /* Start regeneration correctly */
    c[REGENCOUNTER] = 16;
    c[ECOUNTER] = 96;

    c[WIELD] = -1;
    c[WEAR] = c[WIELD];
    c[SHIELD] = c[WEAR];

    for(i = 0; i < 26; ++i) {
	iven[i] = 0;
    }

    /* He know protection, magic missile */
    spelknow[1] = 1;
    spelknow[0] = spelknow[1];

    if(c[HARDGAME] <= 0) {
	iven[0] = OLEATHER;
	iven[1] = ODAGGER;
	c[WEAR] = 0;
	ivenarg[0] = c[WEAR];
	ivenarg[1] = ivenarg[0];
    }

    playerx = rand() % (MAXX - 2) + 1;
    playery = rand() % (MAXY - 2) + 1;
    oldx = 0;
    oldy = 25;

    /* Time clock start at zero */
    gtime = 0;

    cbak[SPELLS] = -50;

    /* Make the attributes, i.e. str, int etc. */
    for(i = 0; i < 6; ++i) {
	c[i] = 12;
    }

    recalc();
}

/*
 * newcavelevel(int level)
 *
 * Function to enter a new level. this routine must be called anytime
 * the player changes levels. If that level is unknown it will be
 * created. A new set of monsters will be created for a new level, and
 * existing levels will get a few more monsters.
 *
 * Note: That it is here we remove genocided monsters from the present level.
 */
void newcavelevel(int x)
{
    int i;
    int j;

    /* Put the level back into storage */
    if(beenhere[(int)level]) {
	savelevel();
    }

    /* Get the new level and put in working storage */
    level = x;
    
    if(beenhere[x] == 0) {
	for(i = 0; i < MAXY; ++i) {
	    for(j = 0; j < MAXX; ++j) {
		mitem[j][i] = 0;
		know[j][i] = mitem[j][i];
	    }
	}
    }
    else {
	getlevel();
	sethp(0);

	/* Wipe out any genocided monsters */
	checkgen();

	return;
    }

    makemaze(x);
    makeobject(x);
    beenhere[x] = 1;
    sethp(1);

#if WIZID
    if(wizard || (x == 0)) {
	for(j = 0; j < MAXY; ++j) {
	    for(i = 0; i < MAXX; ++i) {
		know[i][j] = 1;
	    }
	}
    }
#else
    if(x == 0) {
	for(j = 0; j < MAXY; ++j) {
	    for(i = 0; i < MAXX; ++i) {
		know[i][j] = 1;
	    }
	}
    }
#endif

    /* Wipe out any genocided monsters */
    checkgen();
}

/*
 * makemaze(int level)
 *
 * Subroutine to make the caverns for a given level. Only walls are made.
 */
static int mx;
static int mxl;
static int mxh;
static int my;
static int myl;
static int myh;
static int tmp2;

void makemaze(int k)
{
    int i;
    int j;
    int tmp;
    int z;

    if((k > 1)
       && (((rand() % 17 + 1) <= 4)
	   || (k == (MAXLEVEL - 1))
	   || (k == (MAXLEVEL + MAXLEVEL - 1)))) {
	/* Read maze from data file */
	if(cannedlevel(k)) {
	    return;
	}
    }

    if(k == 0) {
	tmp = 0;
    }
    else {
	tmp = OWALL;
    }

    for(i = 0; i < MAXY; ++i) {
	for(j = 0; j < MAXX; ++j) {
	    item[j][i] = tmp;
	}
    }

    if(k == 0) {
	return;
    }

    eat(1, 1);

    /* Exit from dungeon */
    if(k == 1) {
	item[33][MAXY - 1] = 0;
    }

    /* Now for open spaces -- not on level 10 */
    if(k != (MAXLEVEL - 1)) {
	tmp2 = rand() % 3 + 1 + 3;

	for(tmp = 0; tmp < tmp2; ++tmp) {
	    my = rand() % 11 + 1 + 2;
	    myl = my - rand() % 2 + 1;
	    myh = my + rand() % 2 + 1;

	    if(k < MAXLEVEL) {
		mx = rand() % 44 + 1 + 5;
		mxl = mx - rand() % 4 + 1;
		mxh = mx + rand() % 12 + 1 + 3;
		z = 0;
	    }
	    else {
		mx = rand() % 60 + 1 + 3;
		mxl = mx - rand() % 2 + 1;
		mxh = mx + rand() % 2 + 1;
		z = makemonst(k);
	    }

	    for(i = mxl; i < mxh; ++i) {
		for(j = myl; j < myh; ++j) {
		    item[i][j] = 0;
		    mitem[i][j] = z;

		    if(mitem[i][j]) {
			hitp[i][j] = monster[z].hitpoints;
		    }
		}
	    }
	}
    }

    if(k != (MAXLEVEL - 1)) {
	my = rand() % (MAXY - 2) + 1;

	for(i = 1; i < (MAXX - 1); ++i) {
	    item[i][my] = 0;
	}
    }

    if(k > 1) {
	treasureroom(k);
    }
}

/*
 * Function to eat away a filled in maze
 */
void eat(int xx, int yy)
{
    int dir;
    int try;

    dir = rand() % 4 + 1;
    try = 2;

    while(try) {
	switch(dir) {
	case 1: /* West */
	    if(xx <= 2) {
		break;
	    }

	    if((item[xx - 1][yy] != OWALL) || (item[xx - 2][yy] != OWALL)) {
		break;
	    }

	    item[xx - 2][yy] = 0;
	    item[xx - 1][yy] = item[xx - 2][yy];
	    eat(xx- 2, yy);

	    break;
	case 2: /* East */
	    if(xx >= (MAXX - 3)) {
		break;
	    }

	    if((item[xx + 1][yy] != OWALL) || (item[xx + 2][yy] != OWALL)) {
		break;
	    }

	    item[xx + 2][yy] = 0;
	    item[xx + 1][yy] = item[xx + 2][yy];
	    eat(xx + 2, yy);

	    break;
	case 3: /* South */
	    if(yy <= 2) {
		break;
	    }

	    if((item[xx][yy - 1] != OWALL) || (item[xx][yy - 2] != OWALL)) {
		break;
	    }

	    item[xx][yy - 2] = 0;
	    item[xx][yy - 1] = item[xx][yy - 2];
	    eat(xx, yy - 2);

	    break;
	case 4: /* North */
	    if(yy >= (MAXY - 3)) {
		break;
	    }

	    if((item[xx][yy + 1] != OWALL) || (item[xx][yy + 2] != OWALL)) {
		break;
	    }

	    item[xx][yy + 2] = 0;
	    item[xx][yy + 1] = item[xx][yy + 2];
	    eat(xx, yy + 2);

	    break;
	}

	++dir;
	
	if(dir > 4) {
	    dir = 1;
	    --try;
	}
    }
}

/*
 * Function to read in a maze from a data file
 *
 * Format of maze data file:
 *   1st character = # of mazes in file (ascii digit)
 *   For each maze: 18 lines (first 17 used) 67 characters per line
 *
 * Special characters in maze data file:
 *   #    Wall
 *   D    Door
 *   .    Random monster
 *   ~    Eye of larn
 *   !    Cure dianthroritis
 *   -    Random object
 */
int cannedlevel(int k)
{
    char *row;
    char *lgetl();
    int i;
    int j;
    int it;
    int arg;
    int mit;
    int marg;

    if(lopen(larnlevels) < 0) {
	write(STDOUT_FILENO, "Can't open the maze data file\n", 30);
	died(-282);

	return 0;
    }

    i = lgetc();

    if(i <= '0') {
	died(-282);

	return 0;
    }

    /* Advance to desired maze */
    for(i = (18 * (rand() % (i - '0'))); i > 0; --i) {
	lgetl();
    }

    for(i = 0; i < MAXY; ++i) {
	row = lgetl();

	for(j = 0; j < MAXX; ++j) {
	    marg = 0;
	    arg = marg;
	    mit = arg;
	    it = mit;

	    switch(*row++) {
	    case '#':
		it = OWALL;

		break;
	    case 'D':
		it = OCLOSEDOOR;
		arg = rand() % 30 + 1;

		break;
	    case '~':
		if(k != (MAXLEVEL - 1)) {
		    break;
		}
		
		it = OLARNEYE;
		mit = rand() % 8 + DEMONLORD;
		marg = monster[mit].hitpoints;

		break;
	    case '!':
		if(k != (MAXLEVEL + MAXVLEVEL - 1)) {
		    break;
		}
		    
		it = OPOTION;
		arg = 21;
		mit = DEMONLORD + 7;
		marg = monster[mit].hitpoints;

		break;
	    case '.':
		if(k < MAXLEVEL) {
		    break;
		}

		mit = makemonst(k + 1);
		marg = monster[mit].hitpoints;

		break;
	    case '-':
		it = newobject(k + 1, &arg);

		break;
	    }

	    item[j][i] = it;
	    iarg[j][i] = arg;
	    mitem[j][i] = mit;
	    hitp[j][i] = marg;

#if WIZID
	    if(wizard) {
		know[j][i] = 1;
	    }
	    else {
		know[j][i] = 0;
	    }
#else
	    know[j][i] = 0;
#endif
	}
    }

    lrclose();

    return 1;
}

/*
 * Function to make a treasure room on a level
 * Level 10's treasure room has the eye in it and demon lords
 * Level V3 has potion of cure dianthroritis and demon prince
 */
void treasureroom(int lv)
{
    int tx;
    int ty;
    int xsize;
    int ysize;

    for(tx = (rand() % 10 + 2); tx < (MAXX - 10); tx += 10) {
	if((lv == (MAXLEVEL - 1))
	   || (lv == (MAXLEVEL + MAXVLEVEL - 1))
	   || ((rand() % 13 + 1) == 2)) {
	    xsize = rand() % 6 + 1 + 3;
	    ysize = rand() % 3 + 1 + 3;

	    /* Upper left corner of room */
	    ty = rand() % (MAXY - 9) + 1 + 1;
	    
	    if((lv == (MAXLEVEL - 1)) || (lv == (MAXLEVEL + MAXVLEVEL - 1))) {
		tx = tx + rand() % (MAXX - 24) + 1;
		troom(lv, xsize, ysize, tx, ty, rand() % 3 + 1 + 6);
	    }
	    else {
		troom(lv, xsize, ysize, tx, ty, rand() % 9 + 1);
	    }
	}
    }
}

/*
 * Subroutine to create a treasure room of any size at a given location room is
 * filled with objects and monsters.
 * The coordinate given is that of the upper left corner of the room
 */
void troom(int lv, int xsize, int ysize, int tx, int ty, int glyph)
{
    int i;
    int j;
    int tp1;
    int tp2;

    for(j = (ty - 1); j <= (ty + ysize); ++j) {
	/* Clear out space for room */
	for(i = (tx - 1); i <= (tx + xsize); ++i) {
	    item[i][j] = 0;
	}
    }

    for(j = ty; j < (ty + ysize); ++j) {
	/* Now put in the walls */
	for(i = tx; i < (tx + xsize); ++i) {
	    item[i][j] = 0;
	}
    }

    /* Locate the door on the treasure room */
    switch(rand() % 2 + 1) {
    case 1:
	i = tx + (rand() % xsize);
	j = ty + ((ysize - 1) * (rand() % 2));
	item[i][j] = OCLOSEDOOR;

	/* On horizontal walls */
	iarg[i][j] = glyph;

	break;
    case 2:
	i = tx + ((xsize - 1) * (rand() % 2));
	j = ty + (rand() % ysize);
	item[i][j] = OCLOSEDOOR;

	/* On vertical walls */
	iarg[i][j] = glyph;

	break;
    }

    tp1 = playerx;
    tp2 = playery;
    playery = ty + (ysize >> 1);

    if(c[HARDGAME] < 2) {
	for(playerx = (tx + 1); playerx <= (tx + xsize - 2); playerx += 2) {
	    j = rand() % 6 + 1;
	    
	    for(i = 0; i <= j; ++i) {
		something(lv + 2);
		createmonster(makemonst(lv + 1));
	    }
	}
    }
    else {
	for(playerx = (tx + 1); playerx <= (tx + xsize - 2); playerx += 2) {
	    j = rand() % 4 + 1;

	    for(i = 0; i <= j; ++i) {
		something(lv + 2);
		createmonster(makemonst(lv + 3));
	    }
	}
    }

    playerx = tp1;
    playery = tp2;
}

/*
 * ***********
 * MAKE_OBJECT
 * ***********
 * 
 * Subroutine to create the objects in the maze for the given level
 */
void makeobject(int j)
{
    int i;

    if(j == 0) {
	/* Entrance to the dungeon */
	fillroom(OENTRANCE, 0);

	/* The DND store */
	fillroom(ODNDSTORE, 0);

	/* College of Larn */
	fillroom(OSCHOOL, 0);

	/* 1st National bank of Larn */
	fillroom(OBANK, 0);

	/* Volcano shaft to temple */
	fillroom(OVOLDOWN, 0);

	/* The player's home and family */
	fillroom(OHOME, 0);

	/* The trading post */
	fillroom(OTRADEPOST, 0);

	/* The larn revenue service */
	fillroom(OLRS,  0);

	return;
    }

    /* Volcano shaft up from the temple */
    if(j == MAXLEVEL) {
	fillroom(OVOLUP, 0);
    }

    /* Make the fixed objects in the maze STAIRS */
    if((j > 0) && (j != (MAXLEVEL - 1)) && (j != (MAXLEVEL + MAXVLEVEL - 1))) {
	fillroom(OSTAIRSDOWN, 0);
    }

    if((j > 1) && (j != MAXLEVEL)) {
	fillroom(OSTAIRSUP, 0);
    }

    /* Make the random objects in the maze */
    fillmroom(rand() % 3, OBOOK, j);
    fillmroom(rand() % 3, OALTAR, 0);
    fillmroom(rand() % 3, OSTATUE, 0);
    fillmroom(rand() % 3, OPIT, 0);
    fillmroom(rand() % 3, OFOUNTAIN, 0);
    fillmroom((rand() % 3 + 1) - 2, OIVTELETRAP, 0);
    fillmroom(rand() % 2, OTHRONE, 0);
    fillmroom(rand() % 2, OMIRROR, 0);
    fillmroom(rand() % 2, OTRAPARROWIV, 0);
    fillmroom((rand() % 3 + 1) - 2, OIVDARTRAP, 0);
    fillmroom(rand() % 3, OCOOKIE, 0);

    if(j == 1) {
	fillmroom(1, OCHEST, j);
    }
    else {
	fillmroom(rand() % 2, OCHEST, j);
    }

    if((j != (MAXLEVEL - 1)) && (j != (MAXLEVEL + MAXVLEVEL - 1))) {
	fillmroom(rand() % 2, OIVTRAPDOOR, 0);
    }

    if(j <= 10) {
	fillmroom(rand() % 2, ODIAMOND, rand() % ((10 * j) + 1) + 1 + 10);
	fillmroom(rand() % 2, ORUBY, rand() % ((6 * j) + 1) + 1 + 6);
	fillmroom(rand() % 2, OEMERALD, rand() % ((4 * j) + 1) + 1 + 4);
	fillmroom(rand() % 2, OSAPPHIRE, rand() % ((3 * j) + 1) + 1 + 2);
    }

    for(i = 0; i < (rand() % 4 + 1 + 3); ++i) {
	/* Make a POTION */
	fillroom(OPOTION, newpotion());
    }

    for(i = 0; i < (rand() % 5 + 1 + 3); ++i) {
	/* Make a SCROLL */
	fillroom(OSCROLL, newscroll());
    }

    for(i = 0; i < (rand() % 12 + 12); ++i) {
	/* Make GOLD */
	fillroom(OGOLDPILE, (12 * (rand() % (j + 1) + 1)) + (j << 3) + 10);
    }

    if(j == 5) {
	/* Branch office of the bank */
	fillroom(OBANK2, 0);
    }

    /* A ring mail */
    froom(2, ORING, 0);

    /* A studded leather */
    froom(1, OSTUDLEATHER, 0);

    /* A splint mail */
    froom(3, OSPLINT, 0);

    /* A shield */
    froom(5, OSHIELD, rand() % 3);

    /* A battle axe */
    froom(2, OBATTLEAXE, rand() % 3);

    /* A long sword */
    froom(5, OLONGSWORD, rand() % 3);

    /* A flail */
    froom(5, OFLAIL, rand() % 3);

    /* Ring of regeneration */
    froom(4, OREGENRING, rand() % 3);

    /* Ring of protection */
    froom(1, OPROTRING, rand() % 3);

    /* Ring of strength + 4 */
    froom(2, OSTRRING, 4);

    /* A spear */
    froom(7, OSPEAR, rand() % 5 + 1);

    /* Orb of dragon slaying */
    froom(3, OORBOFDRAGON, 0);

    /* Scarab of negate spirit */
    froom(4, OSPIRITSCARAB, 0);

    /* Cube of undead control */
    froom(4, OCUBEOFUNDEAD, 0);

    /* Ring of extra regen */
    froom(2, ORINGOFEXTRA, 0);

    /* Device of antitheft */
    froom(3, ONOTHEFT, 0);

    /* Sword of slashing */
    froom(2, OSWORDOFSLASHING, 0);

    if(c[BESSMANN] == 0) {
	/* Bessman's flailing hammer */
	froom(4, OHAMMER, 0);
	c[BESSMANN] = 1;
    }

    if((c[HARDGAME] < 3) || ((rand() % 4 + 1) == 3)) {
	if(j > 3) {
	    /* Sunsword + 3 */
	    froom(3, OSWORD, 3);

	    /* A two handed sword */
	    froom(5, O2SWORD, rand() % 4 + 1);

	    /* Belt of striking */
	    froom(3, OBELT, 4);

	    /* Energy ring */
	    froom(3, OENERGYRING, 3);

	    /* Platemail + 5 */
	    froom(4, OPLATE, 5);
	}
    }
}

/*
 * Subroutine to fill in a number of objects of the same kind
 */
void fillmroom(int n, int arg, char what)
{
    int i;

    for(i = 0; i < n; ++i) {
	fillroom(what, arg);
    }
}

void froom(int n, int arg, char itm)
{
    if((rand() % 151 + 1) < n) {
	fillroom(itm, arg);
    }
}

/*
 * Subroutine to put an object into an empty room uses a random walk
 */
static void fillroom(int arg, char what)
{
    int x;
    int y;

#ifdef EXTRA
    ++c[FILLROOM];
#endif

    x = rand() % (MAXX - 2) + 1;
    y = rand() % (MAXY - 2) + 1;

    while(item[x][y]) {
#ifdef EXTRA
	/* Count up these random walks */
	++c[RANDOMWALK];
#endif

	x += (rand() % 3 + 1 - 2);
	y += (rand() % 3 + 1 - 2);

	if(x > (MAXX - 2)) {
	    x = 1;
	}

	if(x < 1) {
	    x = MAXX - 2;
	}

	if(y > (MAXY - 2)) {
	    y = 1;
	}

	if(y < 1) {
	    y = MAXY - 2;
	}
    }

    item[x][y] = what;
    iarg[x][y] = arg;
}

/*
 * Subroutine to put monsters into an empty room without walls or other monsters
 */
int fillmonst(char what)
{
    int x;
    int y;
    int trys;

    /* Max # of creation attempts */
    for(trys = 5; trys > 0; --trys) {
	x = rand() % (MAXX - 2) + 1;
	y = rand() % (MAXY - 2) + 1;

	if((item[x][y] == 0)
	   && (mitem[x][y] == 0)
	   && ((playerx != x) || (playery != y))) {
	    mitem[x][y] = what;
	    know[x][y] = 0;
	    hitp[x][y] = monster[(int)what].hitpoints;

	    return 0;
	}
    }

    /* Creation failure */
    return -1;
}

/*
 * Creates an entire set of monsters for a level. Must be done when entering a
 * new level. If sethp(1) then wipe out old monsters else level them there.
 */
void sethp(int flg)
{
    int i;
    int j;

    if(flg) {
	for(i = 0; i < MAXY; ++i) {
	    for(j = 0; j < MAXX; ++j) {
		stealth[j][i] = 0;
	    }
	}
    }

    /* If teleported and found level 1 then know level we are on */
    if(level == 0) {
	c[TELEFLAG] = 0;

	return;
    }

    if(flg) {
	j = rand() % 12 + 1 + 2 + (level >> 1);
    }
    else {
	j = (level >> 1) + 1;
    }

    for(i = 0; i < j; ++i) {
	fillmonst(makemonst(level));
    }

    positionplayer();
}

/*
 * Function to destroy all genocided monsters on the present level
 */
void checkgen()
{
    int x;
    int y;

    for(y = 0; y < MAXY; ++y) {
	for(x = 0; x < MAXX; ++x) {
	    if(monster[(int)mitem[x][y]].genocided) {
		/* No more monster */
		mitem[x][y] = 0;
	    }
	}
    }
}
