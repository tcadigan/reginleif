/*
 * object.c
 * Larn is copyrighted 1986 by Noah Morgan.
 */

#include "header.h"

/*
 * ***************
 * LOOK_FOR_OBJECT
 * ***************
 *
 * Subroutine to look for an object and give the player his options if
 * an object was found.
 */
void lookforobject()
{
    int i;
    int j;

    /* Can't find objects if time is stopped */
    if(c[TIMESTOP]) {
	return;
    }

    i = item[playerx][playery];

    if(i == 0) {
	return;
    }

    showcell(playerx, playery);
    cursors();
    yrepcount = 0;

    switch(i) {
    case OGOLDPILE:
    case OMAXGOLD:
    case OKGOLD:
    case ODGOLD:
	lprcat("\n\nYou have found some gold!");
	ogold(i);

	break;
    case OPOTION:
	lprcat("\n\nYou have found a magic potion");
	i = iarg[playerx][playery];

	if(potionname[i][0]) {
	    lprintf(" of %s", &potionname[i][1]);
	}
	
	opotion(i);

	break;
    case OSCROLL:
	lprcat("\n\nYou have found a magic scroll");
	i = iarg[playerx][playery];

	if(scrollname[i][0]) {
	    lprintf(" of %s", &scrollname[i][1]);
	}

	oscroll(i);

	break;
    case OALTAR:
	if(nearbymonst()) {
	    return;
	}

	lprcat("\n\nThere is a Holy Altar here!");
	oaltar();

	break;
    case OBOOK:
	lprcat("\n\nYou have found a book.");
	obook();

	break;
    case OCOOKIE:
	lprcat("\n\nYou have found a fortune cookie.");
	ocookie();

	break;
    case OTHRONE:
	if(nearbymonst()) {
	    return;
	}

	lprintf("\n\nThere is %s here!", objectname[i]);
	othrone(0);

	break;
    case OTHRONE2:
	if(nearbymonst()) {
	    return;
	}

	lprintf("\n\nthere is %s here!", objectname[i]);
	othrone(1);

	break;
    case ODEADTHRONE:
	lprintf("\n\nThere is %s here!", objectname[i]);
	odeadthrone();

	break;
    case OORB:
	lprcat("\n\nYou have found the Orb!!!!!");
	oorb();

	break;
    case OPIT:
	lprcat("\n\nYou're standing at the top of a pit.");
	opit();

	break;
    case OSTAIRSUP:
	lprcat("\n\nThere is a circular staircase here");

	/* Up */
	ostairs(1);

	break;
    case OELEVATORUP:
	lprcat("\n\nYou feel heavy for a moment, but the feeling disappears");

	/* Up */
	oelevator(1);

	break;
    case OFOUNTAIN:
	if(nearbymonst()) {
	    return;
	}

	lprcat("\n\nThere is a fountain here");
	ofountain();

	break;
    case OSTATUE:
	if(nearbymonst()) {
	    return;
	}

	lprcat("\n\nYou are standing in front of a statue");
	ostatue();

	break;
    case OCHEST:
	lprcat("\n\nThere is a chest here");
	ochest();

	break;
    case OIVTELETRAP:
	if(rnd(11) < 6) {
	    return;
	}

	item[playerx][playery] = OTELEPORTER;
	know[playerx][playery] = 1;
    case OTELEPORTER:
	lprcat("\nZaaaapp! You've been teleported!\n");
	beep();
	nap(3000);
	oteleport(0);

	break;
    case OSCHOOL:
	if(nearbymonst()) {
	    return;
	}

	lprcat("\n\nYou have found the College of Larn.");
	lprcat("\nDo you (g) go inside, or (i) stay here? ");
	i = 0;

	while((i != 'g') && (i != 'i') && (i != '\33')) {
	    i = getchar();
	}

	/* The college of Larn */
	if(i == 'g') {
	    oschool();
	}
	else {
	    lprcat(" stay here");
	}

	break;
    case OMIRROR:
	if(nearbymonst()) {
	    return;
	}

	lprcat("\n\nThere is a mirror here");
	omirror();

	break;
    case OBANK2:
    case OBANK:
	if(nearbymonst()) {
	    return;
	}

	if(i == OBANK) {
	    lprcat("\n\nYou have found the Bank of Larn.");
	}
	else {
	    lprcat("\n\nYou have for a branch office of the Bank of Larn.");
	}

	lprcat("\nDo you (g) go inside, or (i) stay here? ");
	j = 0;

	while((j != 'g') && (j != 'i') && (j != '\33')) {
	    j = getchar();
	}

	if(j == 'g') {
	    /* The Bank of Larn */
	    if(i == OBANK) {
		obank();
	    }
	    else {
		obank2();
	    }
	}
	else {
	    lprcat(" stay here");
	}

	break;
    case ODEADFOUNTAIN:
	if(nearbymonst()) {
	    return;
	}

	lprcat("\n\nThere is a dead fountain here");

	break;
    case ODNDSTORE:
	if(nearbymonst()) {
	    return;
	}

	lprcat("\n\nThere is a DND store here.");
	lprcat("\nDo you (g) go inside, or (i) stay here? ");
	i = 0;

	while((i != 'g') && (i != 'i') && (i != '\33')) {
	    i = getchar();
	}

	/* The DND Adventurers Store */
	if(i == 'g') {
	    dndstore();
	}
	else {
	    lprcat(" stay here");
	}

	break;
    case OSTAIRSDOWN:
	lprcat("\n\nThere is a circular staircase here");

	/* Down */
	ostairs(-1);

	break;
    case OELEVATORDOWN:
	lprcat("\n\nYou feel light for a moment, but the feeling disappears");

	/* Down */
	oelevator(-1);

	break;
    case OOPENDOOR:
	lprintf("\n\nYou have found %s", objectname[i]);
	lprcat("\nDo you (c) close it");
	iopts();
	i = 0;

	while((i != 'c') && (i != 'i') && (i != '\33')) {
	    i = getchar();
	}

	if((i == '\33') || (i == 'i')) {
	    ignore();

	    break;
	}

	lprcat("close");
	forget();
	item[playerx][playery] = OCLOSEDOOR;
	iarg[playerx][playery] = 0;
	playerx = lastpx;
	playery = lastpy;

	break;
    case OCLOSEDOOR:
	lprintf("\n\nYou have found %s", objectname[i]);
	lprcat("\nDo you (o) try to open it");
	iopts();
	i = 0;

	while((i != 'o') && (i != 'i') && (i != '\33')) {
	    i = getchar();
	}

	if((i == '\33') || (i == 'i')) {
	    ignore();
	    playerx = lastpx;
	    playery = lastpy;

	    break;
	}
	else {
	    lprcat("open");

	    if(rnd(11) < 7) {
		switch(iarg[playerx][playery]) {
		case 6:
		    c[AGGRAVATE] += rnd(400);

		    break;
		case 7:
		    lprcat("\nYou are jolted by an electric schock ");
		    lastnum = 274;
		    losehp(rnd(20));
		    bottomline();

		    break;
		case 8:
		    loselevel();

		    break;
		case 9:
		    lprcat("\nYou suddenly feel weaker ");

		    if(c[STRENGTH] > 3) {
			--c[STRENGTH];
		    }

		    bottomline();

		    break;
		default:

		    break;
		}

		playerx = lastpx;
		playery = lastpy;
	    }
	    else {
		forget();
		item[playerx][playery] = OOPENDOOR;
	    }
	}

	break;
    case OENTRANCE:
	lprcat("\nYou have found ");
	lprcat(objectname[OENTRANCE]);
	lprcat("\nDo you (g) go inside");
	iopts();
	i = 0;

	while((i != 'g') && (i != 'i') && (i != '\33')) {
	    i = getchar();
	}

	if(i == 'g') {
	    newcavelevel(1);
	    playerx = 33;
	    playery = MAXY - 2;
	    mitem[33][MAXY - 1] = 0;
	    know[33][MAXY - 1] = mitem[33][MAXY - 1];
	    item[33][MAXY - 1] = know[33][MAXY - 1];
	    draws(0, MAXX, 0, MAXY);
	    bot_linex();

	    return;
	}
	else {
	    ignore();
	}

	break;
    case OVOLDOWN:
	lprcat("\nYou have found ");
	lprcat(objectname[OVOLDOWN]);
	lprcat("\nDo you (c) climb down");
	iopts();
	i = 0;

	while((i != 'c') && (i != 'i') && (i != '\33')) {
	    i = getchar();
	}

	if((i == '\33') || (i == 'i')) {
	    ignore();

	    break;
	}

	if(level != 0) {
	    lprcat("\nThe shaft only extends 5 feet downward!");

	    return;
	}

	if(packweight() > (45 + (3 * (c[STRENGTH] + c[STREXTRA])))) {
	    lprcat("\nYou slip and fall down the shaft");
	    beep();
	    lastnum = 275;
	    losehp(30 + rnd(20));
	    bottomhp();
	}
	else {
	    lprcat("climb down");
	    nap(3000);
	    newcavelevel(MAXLEVEL);
	}

	/* Put the player near volcano shaft */
	for(i = 0; i < MAXY; ++i) {
	    for(j = 0; j < MAXX; ++j) {
		if(item[j][i] == OVOLUP) {
		    playerx = j;
		    playery = i;
		    j = MAXX;
		    i = MAXY;
		    positionplayer();
		}
	    }
	}

	draws(0, MAXX, 0, MAXY);
	bot_line();

	return;
    case OVOLUP:
	lprcat("\nYou have found ");
	lprcat(objectname[OVOLUP]);
	iopts();
	i = 0;

	while((i != 'c') && (i != 'i') && (i 1= '\33')) {
	    i = getchar();
	}

	if((i == '\33') || (i == 'i')) {
	    ignore();

	    break;
	}

	if(level != 11) {
	    lprcat("\nThe shaft only extends 8 feet upwards before you find a blockage!");

	    return;
	}

	if(packweight() > (45 + (5 * (c[STRENGTH] + c[STREXTRA])))) {
	    lprcat("\nYou slip and fall down the shaft");
	    beep();
	    lastnum = 275;
	    losehp(15 + rnd(20));
	    bottomhp();

	    return;
	}

	lprcat("climb up");
	lflush();
	nap(3000);
	newcavelevel(0);

	/* Put player near volcano shaft */
	for(i = 0; i < MAXY; ++i) {
	    for(j = 0; j < MAXX; ++j) {
		if(item[j][i] == OVOLDOWN) {
		    playerx = j;
		    playery = i;
		    j = MAXX;
		    y = MAXY;
		    positionplayer();
		}
	    }
	}

	draws(0, MAXX, 0, MAXY);
	bot_linex();

	return;
    case OTRAPARROWIV:
	/* For an arrow trap */
	if(rnd(17) < 13) {
	    return;
	}

	item[playerx][playery] = OTRAPARROW;
	know[playerx][playery] = 0;
    case OTRAPARROW:
	/* For an arrow trap */
	lprcat("\nYou are hit by an arrow");
	beep();
	lastnum = 259;
	losehp(rnd(10) + level);
	bottomhp();

	return;
    case OIVDRATRAP:
	/* For a dart trap */
	if(rnd(17) < 13) {
	    return;
	}

	item[playerx][playery] = ODARTRAP;
	know[playerx][playery] = 0;
    case ODARTRAP:
	/* For a dart trap */
	lprcat("\nYou are hit by a dart");
	beep();
	lastnum = 260;
	losehp(rnd(5));

	--c[STRENGTH];
	if(c[STRENGTH] < 3) {
	    c[STRENGTH] = 3;
	}

	bottomline();

	return;
    case OIVTRAPDOOR:
	/* For a trap door */
	if(rnd(17) < 13) {
	    return;
	}

	item[playerx][playery] = OTRAPDOOR;
	know[playerx][playery] = 1;
    case OTRAPDOOR:
	/* A trap door */
	lastnum = 272;

	if((level == (MAXLEVEL - 1)) || (level == (MAXLEVEL + MAXVLEVEL - 1))) {
	    lprcat("\nYou fell through a bottomless trap door!");
	    beep();
	    nap(3000);
	    died(271);
	}

	lprcat("\nYou fall through a trap door!");
	beep();

	/* For a trap door */
	losehp(rnd(5 + level));
	nap(2000);
	newcavelevel(level + 1);
	draws(0, MAXX, 0, MAXY);
	bot_linex();

	return;
    case OTRADEPOST:
	if(nearbymonst()) {
	    return;
	}

	lprcat("\nYou have found the Larn Trading Post.");
	lprcat("\nDo you (g) go inside, or (i) stay here? ");
	i = 0;

	while((i != 'g') && (i != 'i') && (i != '\33')) {
	    i = getchar();
	}

	if(i == 'g') {
	    otradepost();
	}
	else {
	    lprcat("stay here");
	}

	return;
    case OHOME:
	if(nearbymonst()) {
	    return;
	}

	lprcat("\nYou have found your way home.");
	lprcat("\nDo you (g) go inside, or (i) stay here? ");
	i = 0;

	while((i != 'g') && (i != 'i') && (i != '\33')) {
	    i = getchar();
	}

	if(i == 'g') {
	    ohome();
	}
	else {
	    lprcat("stay here");
	}

	return;
    case OWALL:

	break;
    case OANNIHILATION:
	/* Annihilated by sphere of annihilation */
	died(283);

	return;
    case OLRS:
	if(nearbymonst()) {
	    return;
	}

	lprcat("\n\nThere is an LRS office here.");
	lprcat("\nDo you (g) go inside, or (i) stay here? ");
	i = 0;

	while((i != 'g') && (i != 'i') && (i != '\33')) {
	    i = getchar();
	}

	if(i == 'g') {
	    /* The Larn Revenue Service */
	    olrs();
	}
	else {
	    lprcat(" stay here");
	}

	break;
    case :
	finditem(i);

	break;
    }
}

/* Function to say what object we found and ask if player wants to take it */
void finditem(int itm)
{
    int tmp;
    int i;

    lprintf("\n\nYou have found %s ", objectname[itm]);
    tmp = iarg[playerx][playery];

    switch(itm) {
    case ODIAMOND:
    case ORUBY:
    case OEMERALD:
    case OSAPPHIRE:
    case OSPIRITSCARAB:
    case OORBOFDRAGON:
    case OCUBEOFUNDEAD:
    case ONOTHEFT:
	
	break;
    default:
	if(tmp > 0) {
	    lprintf("+ %d", tmp);
	}
	else if(tmp < 0) {
	    lprintf(" %d", tmp);
	}
    }

    lprcat("\nDo you want to (t) take it");
    iopts();
    i = 0;

    while((i != 't') && (i != 'i') && (i != '\33')) {
	i = getchar();
    }

    if(i == 't') {
	lprcat("take");

	if(take(itm, tmp) == 0) {
	    forget();
	}

	return;
    }

    ignore();
}

/*
 * *******
 * OSTAIRS
 * *******
 *
 * Subroutine to process the stair cases.
 * If dir > 0 then up else down
 */
void ostairs(int dir)
{
    int k;

    lprcat("\nDo you (s) stay here ");

    if(dir > 0) {
	lprcat("(u) go up ");
    }
    else {
	lprcat("(d) go down ");
    }

    lprcat(" or (f) kick stairs? ");

    while(1) {
	switch(getchar()) {
	case '\33':
	case 's':
	case 'i':
	    lprcat("stay here");

	    return;
	case 'f':
	    lprcat("kick stairs");

	    if(rnd(2) == 1) {
		lprcat("\nI hope you feel better. Showing anger rids you of your frustration.");
	    }
	    else {
		k = rnd((level + 1) << 1);
		lprintf("\nYou hurt your foot dumb dumb! You suffer %d hit points", k);
		lastnum = 276;
		losehp(k);
		bottomline();
	    }

	    return;
	case 'u':
	    lprcat("go up");

	    if(dir < 0) {
		lprcat("\nThe stairs don't go up!");
	    }
	    else {
		if((level >= 2) && (level != 11)) {
		    k = level;
		    newcavelevel(level - 1);
		    draws(0, MAXX, 0, MAXY);
		    bot_linex();
		}
		else {
		    lprcat("\nThe stairs lead to a dead end!");
		}
	    }

	    return;
	case 'd':
	    lprcat("go down");

	    if(dir > 0) {
		lprcat("\nThe stairs don't go down!");
	    }
	    else {
		if((level != 0) && (level != 10) && (level != 13)) {
		    k = level;
		    newcavelevel(level + 1);
		    draws(0, MAXX, 0, MAXY);
		    bot_linex();
		}
		else {
		    lprcat("\nThe stairs lead to a dead end!");
		}
	    }

	    return;
	}
    }
}

/*
 * *********
 * OTELEPORT
 * *********
 *
 * Subroutine to handle a teleport trap +/- 1 level maximum
 */
void oteleport(int err)
{
    int tmp;

    /* Stuck in a rock */
    if(err) {
	if(rnd(151) < 3) {
	    died(264);
	}
    }
    
    /* Show ?? on bottomline if been teleported */
    c[TELEFLAG] = 1;
    
    if(level == 0) {
	tmp = 0;
    }
    else if(level < MAXLEVEL) {
	tmp = rnd(5) + level - 3;
	
	if(tmp >= MAXLEVEL) {
	    tmp = MAXLEVEL - 1;
	}
	
	if(tmp < 1) {
	    tmp = 1;
	}
    }
    else {
	tmp = rnd(3) + level - 2;
	
	if(tmp >= (MAXLEVEL + MAXVLEVEL)) {
	    tmp = MAXLEVEL + MAXVLEVEL - 1;
	}
	
	if(tmp < MAXLEVEL) {
	    tmp = MAXLEVEL;
	}
    }
    
    playerx = rnd(MAXX - 2);
    playery = rnd(MAXY - 2);
    
    if(level != tmp) {
	newcavelevel(tmp);
    }
    
    positionplayer();
    draws(0, MAXX, 0, MAXY);
    bot_linex();
}

/*
 * *******
 * OPOTION
 * *******
 *
 * Function to process a potion
 */
void opotion(int pot)
{
    lprcat("\nDo you (d) drink it, (t) take it");
    iopts();

    while(1) {
	switch(getchar()) {
	case '\33':
	case 'i':
	    ignore();

	    return;
	case 'd':
	    lprcat("drink\n");
	    forget();
	    quaffpotion(pot);

	    return;
	case 't':
	    lprcat("take\n");

	    if(take(OPOTION, pot) == 0) {
		forget();
	    }

	    return;
	}
    }
}

/* Function to drink a potion */
void quaffpotion(int pot)
{
    int i;
    int j;
    int k;

    /* Check for within bounds */
    if((pot < 0) || (pot >= MAXPOTION)) {
	return;
    }

    potionname[pot][0] = ' ';

    switch(pot) {
    case 9:
	lprcat("\nYou feel greedy...");
	nap(2000);

	for(i = 0; i < MAXY; ++i) {
	    for(j = 0; j < MAXX; ++j) {
		if((item[j][i] == OGOLDPILE) || (item[j][i] == OMAXGOLD)) {
		    know[j][i] = 1;
		    show1cell(j, i);
		}
	    }
	}

	showplayer();

	return;
    case 19:
	lprcat("\nYou feel greedy...");
	nap(2000);

	for(i = 0; i < MAXY; ++i) {
	    for(j = 0, j < MAXX; ++j) {
		k = item[j][i];

		if((k == ODIAMOND)
		   || (k == ORUBY)
		   || (k == OEMERALD)
		   || (k == OMAXGOLD)
		   || (k == OSAPPHIRE)
		   || (k == OLARNEYE)
		   || (k == OGOLDPILE)) {
		    know[j][i] = 1;
		    show1cell(j, i);
		}
	    }
	}

	showplayer();

	return;
    case 20:
	/* Instant healing */
	c[HP] = c[HPMAX];

	break;
    case 1:
	lprcat("\nYou feel better");

	if(c[HP] == c[HPMAX]) {
	    raisemhp(1);
	}
	else {
	    c[HP] += (rnd(20) + 20 + c[LEVEL]);

	    if(c[HP] > c[HPMAX]) {
		c[HP] = c[HPMAX];
	    }
	}

	break;
    case 2:
	lprcat("\nSuddenly, you feel much more skillful!");
	raiselevel();
	raisemhp(1);

	return;
    case 3:
	lprcat("\nYou feel strange for a moment");
	++c[rund(6)];

	break;
    case 4:
	lprcat("\nYou feel more self confident!");
	c[WISDOM] += rnd(2);

	break;
    case 5:
	lprcat("\nWow! You feel great!");

	if(c[STRENGTH] < 12) {
	    c[STRENGTH] = 12;
	}
	else {
	    ++c[STRENGTH];
	}

	break;
    case 6:
	lprcat("\nYour charm went up by one!");
	++c[CHARISMA];

	break;
    case 8:
	lprcat("\nYour intelligence went up by one!");
	++c[iNTELLIGENCE];

	break;
    case 10:
	for(i = 0; i < MAXY; ++i) {
	    for(j = 0; j < MAXX; ++j) {
		if(mitem[j][i]) {
		    know[j][i] = 1;
		    show1cell(j, i);
		}
	    }
	}

	/* Monster detection */
	return;
    case 12:
	lprcat("\nThis potion has not taste to it");

	return;
    case 15:
	lprcat("\nWOW!!! You feel Super-fantastic!!!");

	if(c[HERO] == 0) {
	    for(i = 0; i < 6; ++i) {
		c[i] += 11;
	    }
	}

	c[HERO] += 250;

	break;
    case 16:
	lprcat("\nYou have a greater intestinal constitude!");
	++c[CONSTITUTION];

	break;
    case 17:
	lprcat("\nYou now have incredibly bulging muscles!!!");

	if(c[GIANTSTR] == 0) {
	    c[STREXTRA] += 21;
	}
	
	c[GIANTSTR] += 700;

	break;
    case 18:
	lprcat("\nYou feel a chill run up your spine!");
	c[FIRERESISTANCE] += 1000;

	break;
    case 0:
	lprcat("\nYou fall asleep...");
	i = (rnd(11) - (c[CONSTITUTION] >> 2)) + 2;

	while(--i > 0) {
	    parse2();
	    nap(1000);
	}

	cursors();
	lprcat("\nYou woke up!");

	return;
    case 7:
	lprcat("\nYou become dizzy!");
	--c[STRENGTH];
	
	if(c[STRENGTH] < 3) {
	    c[STRENGTH] = 3;
	}

	break;
    case 11:
	lprcat("\nYou stagger a moment...");

	for(i = 0; i < MAXY; ++i) {
	    for(j = 0, j < MAXY; ++j) {
		know[j][i] = 0;
	    }
	}

	nap(2000);

	/* Potion of forgetfulness */
	draws(0, MAXX, 0, MAXY);
	
	return;
    case 13:
	/* Blindness */
	lprcat("\nYou can't see anything!");
	c[BLINDCOUNT] += 500;

	return;
    case 14:
	lprcat("\nYou feel confused");
	c[CONFUSE] += (20 + rnd(9));

	return;
    case 21:
	lprcat("\nYou don't seem to be affected");

	/* Cure dianthroritis */
	return;
    case 22:
	/* Poison */
	lprcat("\nYou feel a sickness engulf you");
	c[HALFDAM] += (200 + rnd(200));

	return;
    case 23:
	/* See invisible */
	lprcat("You feel your vision sharpen");
	c[SEEINVISIBLE] += (rnd(1000) + 400);
	monstnamelist[INVISIBLESTALKER] = 'I';

	return;
    }

    /* Show new stats */
    bottomline();

    return;
}

/*
 * *******
 * OSCROLL
 * *******
 *
 * Function to process a magic scroll
 */
void oscroll(int typ)
{
    lprcat("\nDo you ");

    if(c[BLINDCOUNT] == 0) {
	lprcat("(r) read it, ");
    }
    
    lprcat("(t) take it");
    iopts();

    while(1) {
	switch(getchar()) {
	case '\33':
	case 'i':
	    ignore();

	    return;
	case 'r':
	    if(c[BLINDCOUNT]) {
		break;
	    }

	    lprcat("read");
	    forget();

	    if((type == 2) || (typ == 15)) {
		show1cell(playerx, playery);
		cursors();
	    }

	    /* Destroy it */
	    read_scroll(typ);

	    return;
	case 't':
	    lprcat("take");

	    if(take(OSCROLL, typ) == 0) {
		forget();
	    }

	    /* Destroy it */
	    return;
	}
    }
}

/* Data for the function to read a scroll */
static int xh;
static int yh;
static int yl;
static int xl;

static char curse[] = {
    BLINDCOUNT,
    CONFUSE,
    AGGRAVATE,
    HASTEMONST,
    ITCHING,
    LAUGHING,
    DRAINSTRENGTH,
    CLUMSINESS,
    INFEEBLEMENT,
    HALFDAM
};

static char exten[] = {
    PROTECTIONTIME,
    DEXCOUNT,
    STRCOUNT,
    CHARMCOUNT,
    INVISIBILITY,
    CANCELLATION,
    HASTESELF,
    GLOBE,
    SCAREMONST,
    HOLDMONST,
    TIMESTOP
};

char time_change[] = {
    HASTESELF,
    HERO,
    ALTPRO,
    PROTECTIONTIME,
    DEXCOUNT,
    STRCOUNT,
    GIANTSTR,
    CHARMCOUNT,
    INVISIBILITY,
    CANCELLATION,
    HASTESELF,
    AGGRAVATE,
    SCAREMONST,
    STEALTH,
    AWARENESS,
    HOLDMONST,
    HASTEMONST,
    FIRERESISTANCE,
    GLOBE,
    SPIRITPRO,
    UNDEADPRO,
    HALFDAM,
    SEEINVISIBLE,
    ITCHING,
    CLUMSINESS,
    WTW
};

/* Function to adjust time when time warping and taking courses in school */
void adjtime(long tim)
{
    int j;

    /* Adjust time related parameters */
    for(j = 0; j < 26; ++j) {
	if(c[time_change[j]]) {
	    c[time_change[j]] -= tim;

	    if(c[time_change[j]] < 1) {
		c[time_change[j]] = 1;
	    }
	}
    }

    regen();
}

/* Function to read a scroll */
void read_scroll(int typ)
{
    int i;
    int j;

    /* Be sure we are within bounds */
    if((typ < 0) || (typ >= MAXSCROLL)) {
	return;
    }

    scrollname[typ][0] = ' ';

    switch(typ) {
    case 0:
	lprcat("\nYour armor glows for a momen");
	enchantarmor();

	return;
    case 1:
	lprcat("\nYour wepon clows for a moment");
	enchweapon();

	/* Enchant weapon */
	return;
    case 2:
	lprcat("\nYou have been granted enlighenment!");
	yh = min(playery + 7, MAXY);
	xh = min(playerx + 25, MAXX);
	yl = max(playery - 7, 0);
	xl = max(playerx - 25, 0);

	for(i = yl; i < yh; ++i) {
	    for(j = xl; j < xh; ++j) {
		know[j][i] = 1;
	    }
	}

	nap(2000);
	draws(xl, xh, yl, yh);

	return;
    case 3:
	lprcat("\nThis scroll seems to be blank");
	
	return;
    case 4:
	createmonster(makemonst(level + 1));

	/* This one creates a monster */
	return;
    case 5:
	something(level);

	/* Create artifact */
	return;
    case 6:
	c[AGGRAVATE] += 800;

	/* Aggravate monsters */
	return;
    case 7:
	/* Time warp */
	i = rnd(1000) - 850;
	
	gtime += i;

	if(i > 0) {
	    lprintf("\nYou went forward in time by %d mobuls", (i + 99) / 100);
	}
	else {
	    lprintf("\nYou wen backward in time by %d mobuls", -((i + 99) / 100));
	}

	/* Adjust time for time warping */
	adjtime(i);

	return;
    case 8:
	oteleport(0);

	/* Teleportation */
	return;
    case 9:
	c[AWARENESS] += 1800;

	/* Expanded awareness */
	return;
    case 10:
	c[HASTEMONST] += (rnd(55) + 12);

	/* Haste monster */
	return;
    case 11:
	for(i = 0; i < MAXY; ++i) {
	    for(j = 0; j < MAXX; ++j) {
		if(mitem[j][i]) {
		    hitp[j][i] = monster[mitem[j][i]].hitpoints;
		}
	    }
	}

	/* Monster healing */
	return;
    case 12:
	c[SPIRITPRO] += (300 + rnd(200));
	bottomline();

	/* Spirit protection */
	return;
    case 13:
	c[UNDEADPRO] += (300 + rnd(200));
	bottomline();

	/* Undead protection */
	return;
    case 14:
	c[STEALTH] += (250 + rnd(250));
	bottomline();

	/* Stealth */
	return;
    case 15:
	/* Magic mapping */
	lprcat("\nYou have been granted enlightenment!");

	for(i = 0; i < MAXY; ++i) {
	    for(j = 0; j < MAXX; ++j) {
		know[j][i] = 1;
	    }
	}

	nap(2000);
	draws(0, MAXX, 0, MAXY);

	return;
    case 16:
	/* Gem perfection */
	for(i = 0; i < 26; ++i) {
	    switch(iven[i]) {
	    case ODIAMOND:
	    case ORUBY:
	    case OEMERALD:
	    case OSAPPHIRE:
		j = ivenarg[i];
		j &= 255;
		j <<= 1;

		/* Double value */
		if(j > 255) {
		    j = 255;
		}

		ivenarg[i] = j;

		break;
	    }
	}

	break;
    case 18:
	/* Spell extension */
	for(i = 0; i < 11; ++i) {
	    c[exten[i]] <<= 1;
	}

	break;
    case 19:
	/* Identify */
	for(i = 0; i < 26; ++i) {
	    if(iven[i] == OPOTION) {
		potionname[ivenarg[i]][0] = ' ';
	    }

	    if(iven[i] == OSCROLL) {
		scrollname[ivenarg[i]][0] = ' ';
	    }
	}

	break;
    case 20:
	/* Remove curse */
	for(i = 0; i < 26; ++i) {
	    if(c[curse[i]]) {
		c[cursep[i]] = 1;
	    }
	}

	break;
    case 21:
	/* Scroll of annihilation */
	annihilate();

	break;
    case 22:
	/* Pulverization */
	godirect(22, 150, "The ray hits the %s", 0, ' ');

	break;
    case 23:
	/* Life protection */
	++c[LIFEPROT];

	break;
    }
}

void oorb()
{
}

void opit()
{
    int i;

    if(rnd(101) < 81) {
	if((rnd(70) > ((9 * c[DEXTERITY]) - packweight())) || (rnd(101) < 5)) {
	    if(level == (MAXLEVEL - 1)) {
		obottomless();
	    }
	    else if(level == (MAXLEVEL + MAXVLEVEL - 1)) {
		obottomless();
	    }
	    else {
		if(rnd(101) < 20) {
		    i = 0;
		    lprcat("\nYou fell into a pit! Your fall is cushioned by an unknown force\n");
		}
		else {
		    i = rnd((level * 3) + 3);
		    lprintf("\nYou fell into a pit! You suffer %d hit points damage", i);

		    /* If he dies scoreboard will say so */
		    lastnum = 261;
		}

		losehp(i);
		nap(2000);
		newcavelevel(level + 1);
		draws(0, MAXX, 0, MAXY);
	    }
	}
    }
}

void obottomless()
{
    lprcat("\nYou fell into a bottomless pit!");
    beep();
    nap(3000);
    died(262);
}

void oelevator(int dir)
{
#ifdef lint
    int x;
    x = dir;
    dir = x;
#endif
}

void ostatue()
{
}

void omirror()
{
}

void obook()
{
    lprcat("\nDo you ");

    if(c[BLINDCOUNT] == 0) {
	lprcat("(r) read it, ");
    }

    lprcat("(t) take it");
    iopts();

    while(1) {
	switch(getchar()) {
	case '\33':
	case 'i':
	    ignore();

	    return;
	case 'r':
	    if(c[BLINDCOUNT]) {
		break;
	    }

	    lprcat("read");

	    /* No more book */
	    readbook(iarg[playerx][playery]);
	    forget();

	    return;
	case 't':
	    lprcat("take");

	    /* No more book */
	    if(take(OBOOK, iarg[playerx][playery]) == 0) {
		forget();
	    }

	    return;

	}
    }
}

/* Function to read a book */
void readbook(int lev)
{
    int i;
    int tmp;

    if(lev <= 3) {
	if(splev[lev]) {
	    tmp = tmp;
	}
	else {
	    tmp = 1;
	}

	i = rund(tmp);
    }
    else {
	if(splev[lev] - 9) {
	    tmp = tmp;
	}
	else {
	    tmp = 1;
	}

	i = rnd(tmp) + 9;
    }

    spelknow[i] = 1;
    lprintf("\nSpell \"%s\":  %s\n%s",
	    spelcode[i],
	    spelname[i],
	    speldescript[i]);

    if(rnd(10) == 4) {
	lprcat("\nYou int went up by one!");
	++c[INTELLIGENCE];
	bottomline();
    }
}

void ocookie()
{
    char *p;

    lprcat("\nDo you (e) eat it, (t) take it");
    iopts();

    while(1) {
	switch(getchar()) {
	case '\33':
	case 'i':
	    ignore();

	    return;
	case 'e':
	    lprcat("eat\nThe cookie tasted good.");

	    /* No more cookie */
	    forget();

	    if(c[BLINDCOUNT]) {
		return;
	    }

	    p = fortune(fortfile);

	    if(!p) {
		return;
	    }

	    lprcat(" A message inside the cookies reads:\n");
	    lprcat(p);

	    return;
	case 't':
	    lprcat("take");

	    if(take(OCOOKIE, 0) == 0) {
		/* No more cookie */
		forget();
	    }

	    return;
	}
    }
}

/*
 * Routine to pick up some gold.
 * If arg == OMAXGOLD then the pile is worth 100x the argument
 */
void ogold(int arg)
{
    long i;

    i = iarg[playerx][playery];

    if(arg == OMAXGOLD) {
	i *= 100;
    }
    else if(arg == OKGOLD) {
	i *= 1000;
    }
    else if(arg == ODGOLD) {
	i *= 10;
    }

    lprintf("\nIt is worth %d!", i);
    c[GOLD] += i;
    bottomgold();

    /* Destroy gold */
    know[playerx][playery] = 0;
    item[playerx][playery] = know[playerx][playery];
}

void ohome()
{
    int i;

    /* Disable signals */
    for(i = 0; i < 26; ++i) {
	if(iven[i] == OPOTION) {
	    if(ivenarg[i] == 21) {
		/* Remove the potion of cure dianthroritis form inventory */
		iven[i] = 0;
		clear();
		lprcat("Congratulations. You found a potion of cure dianthroritis.\n");
		lprcat("\nFrankly, no one though you could do it. Boy! Did you surprise them!\n");

		if(gtime > TIMELIMIT) {
		    lprcat("\nThe doctor has the sad duty to inform you that your daughter has died!\n");
		    lprcat("You didn't make it in time. In your agony you kill the doctor,\nyour wife\n, and yourself! Too bad!\n");
		    nap(5000);
		    died(269);
		}
		else {
		    lprcat("\nThe doctor is now administering the potion, and in a few moments\n");
		    lprcat("You daughter should be well on her way to recovery.\n");
		    nap(6000);
		    lprcat("\nThe potion is");
		    nap(3000);
		    lprcat(" working! The doctor thinks that\n");
		    lprcat("your daughter will recover in a few days. Congratulations!\n");
		    beep();
		    nap(5000);
		    died(263);
		}
	    }
	}
    }

    while(1) {
	clear();
	lprintf("Welcome home %s. Latest word from the doctor is not good.\n", logname);

	if(gtime > TIMELIMIT) {
	    lprcat("\nThe doctor has the sad duty to inform you that your daughter died!\n");
	    lpract("You didn't make it in time. In your agony, you kill the doctor,\nyour wife, and yourself! Too bad!\n");
	    nap(5000);
	    died(269);
	}

	lprcat("\nThe diagnosis is confirmed as dianthroritis. He guesses that\n");
	lprcat("your daughter has only %d mobuls left in this world. It's up to you,\n",
	       (TIMELIMIT - gtime + 99) / 100);

	lprintf("%s, to find the only hope for your daughter, the very rare\n", logname);
	lprcat("potion of cure dianthroritis. It is rumored that only deep in the\n");
	lprcat("depths of the caves can this potion be found.\n\n\n");
	lprcat("\n     ----- press ");
	standout("return");
	lprcat(" to continue, ");
	standout("escape");
	lprcat(" to leave ----- ");
	i = getchar();

	while((i != '\33') && (i != '\n')) {
	    i = getchar();
	}

	if(i == '\33') {
	    drawscreen();

	    /* Enable signals */
	    nosignal = 0;

	    return;
	}
    }
}

/* Routine to save program space */
void iopts()
{
    lprcat(", or (i) to ignore it? ");
}

void ignore()
{
    lprcat("ignore\n");
}
	
