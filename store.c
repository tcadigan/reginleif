/*
 * store.c
 * Larn is copyrights 1986 by Noah Morgan.
 *
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided that
 * the above copyright notice and this paragraph are duplicated in all such
 * forms and that any documentation, advertising materials, and other materials
 * related to such distribution and use acknowledge that the software was
 * developed by the University of California, Berkeley. The name of the
 * University may not be used to endorse or promote products derived from this
 * software without specific prior written permission. THIS SOFTWARE IS PROVIDED
 * ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, WITHOUT
 * LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * @(#)store.c    5.2 (Berkeley) 7/22/88
 */

#include "store.h"

#include "display.h"
#include "global.h"
#include "header.h"
#include "io.h"
#include "main.h"
#include "nap.h"
#include "object.h"
#include "scores.h"

#include <curses.h>

static void banktitle(char *str);
static void dnditem(int i);
static void nogold();
static void outofstock();
static void handsfull();

static int dndcount = 0;
static int dnditm = 0;

/* This is the data for the stuff in the dnd store */
/* Number of items in the dnd inventory table */
int maxitm = 83;

struct _itm itm[90] = {
    /* Cost    memory     iven name    iven arg     how*/
    /* gp      pointer    iven[]       ivenarg[]    many*/
    {     2,          0,         OLEATHER,  0, 3 },
    {    10,          0,     OSTUDLEATHER,  0, 2 },
    {    40,          0,            ORING,  0, 2 },
    {    85,          0,           OCHAIN,  0, 2 },
    {   220,          0,          OSPLINT,  0, 1 },
    {   400,          0,           OPLATE,  0, 1 },
    {   900,          0,      OPLATEARMOR,  0, 1 },
    {  2600,          0,         OSSPLATE,  0, 1 },
    {   150,          0,          OSHIELD,  0, 1 },
    {     2,          0,          ODAGGER,  0, 3 },
    {    20,          0,           OSPEAR,  0, 3 },
    {    80,          0,           OFLAIL,  0, 2 },
    {   150,          0,       OBATTLEAXE,  0, 2 },
    {   450,          0,       OLONGSWORD,  0, 2 },
    {  1000,          0,          O2SWORD,  0, 2 },
    {  5000,          0,           OSWORD,  0, 1 },
    { 16500,          0,           OLANCE,  0, 1 },
    {  6000,          0, OSWORDOFSLASHING,  0, 0 },
    { 10000,          0,          OHAMMER,  0, 0 },
    {   150,          0,        OPROTRING,  1, 1 },
    {    85,          0,         OSTRRING,  1, 1 },
    {   120,          0,         ODEXRING,  1, 1 },
    {   120,          0,      OCLEVERRING,  1, 1 },
    {   180,          0,      OENERGYRING,  0, 1 },
    {   125,          0,         ODAMRING,  0, 1 },
    {   220,          0,       OREGENRING,  0, 1 },
    {  1000,          0,     ORINGOFEXTRA,  0, 1 },
    {   200,          0,            OBELT,  0, 1 },
    {   400,          0,          OAMULET,  0, 1 },
    {  6500,          0,     OORBOFDRAGON,  0, 0 },
    {  5500,          0,    OSPIRITSCARAB,  0, 0 },
    {  5000,          0,    OCUBEOFUNDEAD,  0, 0 },
    {  6000,          0,         ONOTHEFT,  0, 0 },
    {   590,          0,           OCHEST,  6, 1 },
    {   200,          0,            OBOOK,  8, 1 },
    {    10,          0,          OCOOKIE,  0, 3 },
    {    20, potionname,          OPOTION,  0, 6 },
    {    90, potionname,          OPOTION,  1, 5 },
    {   520, potionname,          OPOTION,  2, 1 },
    {   100, potionname,          OPOTION,  3, 2 },
    {    50, potionname,          OPOTION,  4, 2 },
    {   150, potionname,          OPOTION,  5, 2 },
    {    70, potionname,          OPOTION,  6, 1 },
    {    30, potionname,          OPOTION,  7, 7 },
    {   200, potionname,          OPOTION,  8, 1 },
    {    50, potionname,          OPOTION,  9, 1 },
    {    80, potionname,          OPOTION, 10, 1 },
    {    30, potionname,          OPOTION, 11, 3 },
    {    20, potionname,          OPOTION, 12, 5 },
    {    40, potionname,          OPOTION, 13, 3 },
    {    35, potionname,          OPOTION, 14, 2 },
    {   520, potionname,          OPOTION, 15, 1 },
    {    90, potionname,          OPOTION, 16, 2 },
    {   200, potionname,          OPOTION, 17, 2 },
    {   220, potionname,          OPOTION, 18, 4 },
    {    80, potionname,          OPOTION, 19, 6 },
    {   370, potionname,          OPOTION, 20, 3 },
    {    50, potionname,          OPOTION, 22, 1 },
    {   150, potionname,          OPOTION, 23, 3 },
    {   100, scrollname,          OSCROLL,  0, 2 },
    {   125, scrollname,          OSCROLL,  1, 2 },
    {    60, scrollname,          OSCROLL,  2, 4 },
    {    10, scrollname,          OSCROLL,  3, 4 },
    {   100, scrollname,          OSCROLL,  4, 3 },
    {   200, scrollname,          OSCROLL,  5, 2 },
    {   110, scrollname,          OSCROLL,  6, 1 },
    {   500, scrollname,          OSCROLL,  7, 2 },
    {   200, scrollname,          OSCROLL,  8, 2 },
    {   250, scrollname,          OSCROLL,  9, 4 },
    {    20, scrollname,          OSCROLL, 10, 5 },
    {    30, scrollname,          OSCROLL, 11, 3 },
    {   340, scrollname,          OSCROLL, 12, 1 },
    {   340, scrollname,          OSCROLL, 13, 1 },
    {   300, scrollname,          OSCROLL, 14, 2 },
    {   400, scrollname,          OSCROLL, 15, 2 },
    {   500, scrollname,          OSCROLL, 16, 2 },
    {  1000, scrollname,          OSCROLL, 17, 1 },
    {   500, scrollname,          OSCROLL, 18, 1 },
    {   340, scrollname,          OSCROLL, 19, 2 },
    {   220, scrollname,          OSCROLL, 20, 3 },
    {  3900, scrollname,          OSCROLL, 21, 0 },
    {   610, scrollname,          OSCROLL, 22, 1 },
    {  3000, scrollname,          OSCROLL, 23, 0 }
};

/* Function for the dnd store */
void dnd_2hed()
{
    lprcat("Welcome to the Larn Thrift Shoppe. We stock items expolers find useful\n");
    lprcat(" in their adventures. Feel free to brwose to your heart's content.\n");
    lprcat("Also be advised, if you break 'em, you pay for 'em.");
}

void dnd_hed()
{
    int i;

    for(i = dnditm; i < (26 + dnditm); ++i) {
	dnditem(i);
    }

    cursor(50, 18);
    lprcat("You have ");
}

void dndstore()
{
    int i;

    dnditm = 0;

    /* Disable signals */
    nosignal = 1;
    clear();
    dnd_2hed();

    if(outstanding_taxes > 0) {
	lprcat("\n\nThe Larn Revenue Service has ordered us to not do business with tax evaders.\n");
	beep();
	lprintf("They have also told us that you owe %d gp in back taxes, and as we must\n",
		outstanding_taxes);

	lprcat("comply with the law, we cannot serve you at this time. Sooo sorry.\n");
	cursors();
	lprcat("\nPress ");
	standout();
	lprcat("escape");
	standend();
	lprcat(" to leave: ");
	lflush();
	i = 0;

	while(i != '\33') {
	    i = getchar();
	}

	drawscreen();
	nosignal = 0;

	/* Enable signals */
	return;
    }

    dnd_hed();

    while(1) {
	cursor(59, 18);
	lprintf("%d gold pieces", c[GOLD]);
	cltoeoln();

	/* Erase to eod */
	cl_dn(1, 20);
	lprcat("\nEnter your transaction [");
	standout();
	lprcat("space");
	standend();
	lprcat(" for more, ");
	standout();
	lprcat("escape");
	standend();
	lprcat(" to leave]? ");
	i = 0;

	while(((i < 'a') || (i > 'z'))
	      && (i != ' ')
	      && (i != '\33')
	      && (i != 12)) {
	    i = getchar();
	}

	if(i == 12) {
	    clear();
	    dnd_2hed();
	    dnd_hed();
	}
	else if(i == '\33') {
	    drawscreen();

	    /* Enable signals */
	    nosignal = 0;

	    return;
	}
	else if(i == ' ') {
	    cl_dn(1, 4);
	    dnditm += 26;

	    if(dnditm >= maxitm) {
		dnditm = 0;
		dnd_hed();
	    }
	}
	else {
	    /* Buy something */
	    /* Echo the byte */
	    lprc(i);
	    i += (dnditm - 'a');

	    if(i >= maxitm) {
		outofstock();
	    }
	    else if(itm[i].qty <= 0) {
		outofstock();
	    }
	    else if(pocketful()) {
		handsfull();
	    }
	    else if(c[GOLD] < (itm[i].price * 10)) {
		nogold();
	    }
	    else {
		if(itm[i].mem != 0) {
		    *itm[i].mem[(int)itm[i].arg] = ' ';
		}
		
		c[GOLD] -= (itm[i].price * 10);
		--itm[i].qty;
		take(itm[i].obj, itm[i].arg);
		
		if(itm[i].qty == 0) {
		    dnditem(i);
		}

		nap(1001);
	    }
	}
    }
}

/* Function for the player's hands are full */
static void handsfull()
{
    lprcat("\nYou can't carry anything more!");
    lflush();
    nap(2200);
}

static void outofstock()
{
    lprcat("\nSorry, but we are out of that item.");
    lflush();
    nap(2200);
}

static void nogold()
{
    lprcat("\nYou don't have enough gold to pa for that!");
    lflush();
    nap(2200);
}

/*
 * dnditem(int i)
 *
 * To print the item list; used in dndstore() enter with the index into itm
 */
static void dnditem(int i)
{
    int j;
    int k;

    if(i >= maxitm) {
	return;
    }

    j = ((i & 1) * 40) + 1;
    k = ((i % 26) >> 1) + 5;
    cursor(j, k);

    if(itm[i].qty == 0) {
	lprintf("%39s", "");

	return;
    }

    lprintf("%c) ", (i % 26) + 'a');

    if(itm[i].obj == OPOTION) {
	lprcat("potion of ");
	lprintf("%s", &potionname[(int)itm[i].arg][1]);
    }
    else if(itm[i].obj == OSCROLL) {
	lprcat("scroll of ");
	lprintf("%s", &scrollname[(int)itm[i].arg][1]);
    }
    else {
	lprintf("%s", objectname[(int)itm[i].obj]);
    }

    cursor(j + 31, k);
    lprintf("%6d", (itm[i].price * 10));
}

/* For the college of Larn */
/* The list of courses taken */
char course[26] = {0};
char sourcetime[] = { 10, 15, 10, 20, 10, 10, 10, 5 };

/* Function to display the header info for the school */
void sch_hed()
{
    clear();
    lprcat("The college of Larn offers the exciting opportunity of higher education to \n");
    lprcat("all inhabitants of the caves. Here is a list of the class schedule:\n\n\n");
    lprcat("\t\t    Course Name \t       Time Needed\n\n");

    /* Line 7 of crt */
    if(course[0] == 0) {
	lprcat("\t\ta)  Fighters Training I        10 mobuls");
	lprc('\n');
    }
    if(course[1] == 0) {
	lprcat("\t\tb)  Fighters Training II       15 mobuls");
	lprc('\n');
    }
    if(course[2] == 0) {
	lprcat("\t\tc)  Introduction to Wizardy    10 mobuls");
	lprc('\n');
    }
    if(course[3] == 0) {
	lprcat("\t\td)  Applied Wizardry           20 mobuls");
	lprc('\n');
    }
    if(course[4] == 0) {
	lprcat("\t\te)  Behavioral Psychology      10 mobuls");
	lprc('\n');
    }
    if(course[5] == 0) {
	lprcat("\t\tf)  Faith for Today            10 mobuls");
	lprc('\n');
    }
    if(course[6] == 0) {
	lprcat("\t\tg)  Contemporary Dance         10 mobuls");
	lprc('\n');
    }
    if(course[7] == 0) {
	lprcat("\t\th)  History of Larn             5 mobuls");
	lprc('\n');
    }

    lprcat("\n\n\tAll courses cost 250 gold pieces.");
    cursor(30, 18);
    lprcat("You are presently carrying ");
}

void oschool()
{
    int i;
    long time_used;

    /* Disable signals */
    nosignal = 1;
    sch_hed();

    while(1) {
	cursor(57, 18);
	lprintf("%d gold pieces. ", c[GOLD]);
	cursors();
	lprcat("\nWhat is your choice [");
	standout();
	lprcat("escape");
	standend();
	lprcat(" to leave]? ");
	yrepcount = 0;
	i = 0;

	while(((i < 'a') || (i > 'h')) && (i != '\33') && (i != 12)) {
	    i = getchar();
	}

	if(i == 12) {
	    sch_hed();
	    
	    continue;
	}
	else if(i == '\33') {
	    nosignal = 0;
	    drawscreen();

	    /* Enable signals */
	    return;
	}

	lprc(i);

	if(c[GOLD] < 250) {
	    nogold();
	}
	else if(course[i - 'a']) {
	    lprcat("\nSorry, but that class is filled.");
	    nap(1000);
	}
	else if(i <= 'h') {
	    c[GOLD] -= 250;
	    time_used = 0;

	    switch(i) {
	    case 'a':
		c[STRENGTH] += 2;
		++c[CONSTITUTION];
		lprcat("\nYou feel stronger!");
		cl_line(16, 7);

		break;
	    case 'b':
		if(course[0] == 0) {
		    lprcat("\nSorry, but this class has a prerequisite of Fighters Training I");
		    c[GOLD] += 250;
		    time_used = -10000;

		    break;
		}

		lprcat("\nYou feel much stronger!");
		cl_line(16, 8);
		c[STRENGTH] += 2;
		c[CONSTITUTION] += 2;

		break;
	    case 'c':
		c[INTELLIGENCE] += 2;
		lprcat("\nThe task before you now seems more attainable!");
		cl_line(16, 9);

		break;
	    case 'd':
		if(course[2] == 0) {
		    lprcat("\nSorry, but this class has a prerequisite of Introduction of Wizardry");
		    c[GOLD] += 250;
		    time_used = -10000;

		    break;
		}

		lprcat("\nThe task before you now seems more attainable!");
		cl_line(16, 10);
		c[INTELLIGENCE] += 2;

		break;
	    case 'e':
		c[CHARISMA] += 3;
		lprcat("\nYou now feel like a born leader!");
		cl_line(16, 11);

		break;
	    case 'f':
		c[WISDOM] += 2;
		lprcat("\nYou now feel more confident that you can find the potion in time!");
		cl_line(16, 12);

		break;
	    case 'g':
		c[DEXTERITY] += 3;
		lprcat("\nYou feel like dancing!");
		cl_line(16, 13);

		break;
	    case 'h':
		++c[INTELLIGENCE];
		lprcat("\nYou instructor told you that the Eye of Larn is rumored to be guarded\n");
		lprcat("by a platinum dragon who possesses psionic abilities.");
		cl_line(16, 14);

		break;
	    }

	    time_used += (sourcetime[i- 'a'] * 100);

	    if(time_used > 0) {
		gtime += time_used;

		/* Remeber that he has taken that course */
		++course[i - 'a'];

		/* He regenerated */
		c[HP] = c[HPMAX];
		c[SPELLS] = c[SPELLMAX];

		/* Cure blindness too! */
		if(c[BLINDCOUNT]) {
		    c[BLINDCOUNT] = 1;
		}

		/* End confusion */
		if(c[CONFUSE]) {
		    c[CONFUSE] = 1;
		}

		/* Adjust parameters for time change */
		adjtime(time_used);
	    }

	    nap(1000);
	}
    }
}

/* For the First National Bank of Larn */
/* Last time he was in bank */
int lasttime = 0;

void obank()
{
    banktitle("    Welcome to the First National Bank of Larn.");
}

void obank2()
{
    banktitle("Welcome to the 5th level branch office ofthe First National Bank of Larn.");
}

static void banktitle(char *str)
{
    /* Disable signals */
    nosignal = 1;
    clear();
    lprcat(str);

    if(outstanding_taxes > 0) {
	int i;

	lprcat("\n\nThe Larn Revenue Service has ordered that your account be frozen until all\n");
	beep();
	lprintf("levied taxes have been paid. They have also told us that you owe %d gp in\n",
		outstanding_taxes);

	lprcat("taxes, and we must comply with them. We cannot serve yo uat this time. Sorry.\n");
	lprcat("We suggest you go to the LRS office and pay your taxes.\n");
	cursors();
	lprcat("\nPress ");
	standout();
	lprcat("escape");
	standend();
	lprcat(" to leave: ");
	lflush();
	i = 0;

	while(i != '\33') {
	    i = getchar();
	}

	drawscreen();
	
	/* Enable signals */
	nosignal = 0;

	return;
    }

    lprcat("\n\n\tGemstone\t      Appraisal\t\tGemstone\t     Appraisal");
    obanksub();

    /* Enable signals */
    nosignal = 0;
    drawscreen();
}

/* Function to put interest on your bank account */
void ointerest()
{
    int i;

    if(c[BANKACCOUNT] < 0) {
	c[BANKACCOUNT] = 0;
    }
    else if((c[BANKACCOUNT] > 0) && (c[BANKACCOUNT] < 500000)) {
	/* Mobuls elapsed */
	i = (gtime - lasttime) / 100;

	while((i-- > 0) && (c[BANKACCOUNT] < 500000)) {
	    c[BANKACCOUNT] += c[BANKACCOUNT] / 250;
	}

	/* Interest limit */
	if(c[BANKACCOUNT] > 500000) {
	    c[BANKACCOUNT] = 500000;
	}
    }

    lasttime = (gtime / 100) * 1000;
}

/* The reference to screen location for each */
static short gemorder[26] = {0};

/* The appraisal of the gems */
static long gemvalue[26] = {0};

void obanksub()
{
    unsigned long amt;
    int i;
    int k;

    /* Credit any needed interest */
    ointerest();
    k = 0;

    for(i = 0; i < 26; ++i) {
	switch(iven[i]) {
	case OLARNEYE:
	case ODIAMOND:
	case OEMERALD:
	case ORUBY:
	case OSAPPHIRE:
	    if(iven[i] == OLARNEYE) {
		gemvalue[i] = 250000 - (((gtime * 7) / 100) * 100);

		if(gemvalue[i] < 50000) {
		    gemvalue[i] = 50000;
		}
	    }
	    else {
		gemvalue[i] = (255 & ivenarg[i]) * 100;
		gemorder[i] = k;
		cursor(((k % 2) * 40) + 1, (k >> 1) + 4);
		lprintf("%c) %s", i + 'a', objectname[(int)iven[i]]);
		cursor(((k % 2) * 40) + 33, (k >> 1) + 4);
		lprintf("%5d", gemvalue[i]);
		++k;
	    }
	}
    }

    cursor(31, 17);
    lprintf("You have %8d gold pieces in the bank.", c[BANKACCOUNT]);
    cursor(40, 18);
    lprintf("You have %8d gold pieces", c[GOLD]);

    if((c[BANKACCOUNT] + c[GOLD]) >= 500000) {
	lprcat("\nNote: Larndom law states that only deposits under 500,000 gp can earn interest.");
    }

    while(1) {
	cl_dn(1, 20);
	lprcat("\nYour wish? [(");
	standout();
	lprcat("d");
	standend();
	lprcat(") deposit, (");
	standout();
	lprcat("w");
	standend();
	lprcat(") withdraw, (");
	standout();
	lprcat("s");
	standend();
	lprcat(") sell a stone, or ");
	standout();
	lprcat("escape");
	standend();
	lprcat("] ");
	yrepcount = 0;
	i = 0;

	while((i != 'd') && (i != 'w') && (i != 's') && (i != '\33')) {
	    i = getchar();
	}

	switch(i) {
	case 'd':
	    lprcat("deposit\nHow much? ");
	    amt = readnum(c[GOLD]);

	    if(amt > c[GOLD]) {
		lprcat(" You don't have that much.");
		nap(2000);
	    }
	    else {
		c[GOLD] -= amt;
		c[BANKACCOUNT] += amt;
	    }

	    break;
	case 'w':
	    lprcat("withdraw\nHow much? ");
	    amt = readnum(c[BANKACCOUNT]);

	    if(amt > c[BANKACCOUNT]) {
		lprcat("\nYou don't have that much in the bank!");
		nap(2000);
	    }
	    else {
		c[GOLD] += amt;
		c[BANKACCOUNT] -= amt;
	    }

	    break;
	case 's':
	    lprcat("\nWhich stone would you like to sell? ");
	    i = 0;

	    while(((i < 'a') || (i > 'z')) && (i != '*')) {
		i = getchar();
	    }

	    if(i == '*') {
		for(i = 0; i < 26; ++i) {
		    if(gemvalue[i]) {
			c[GOLD] += gemvalue[i];
			iven[i] = 0;
			gemvalue[i] = 0;
			k = gemorder[i];
			cursor((k % 2) * 40 + 1, (k >> 1) + 4);
			lprintf("%39s", "");
		    }
		}
	    }
	    else {
		i = i - 'a';

		if(gemvalue[i] == 0) {
		    lprintf("\nItem %c is not a gemstone!", i + 'a');
		    nap(2000);

		    break;
		}

		c[GOLD] += gemvalue[i];
		iven[i] = 0;
		gemvalue[i] = 0;
		k = gemorder[i];
		cursor(((k % 2) * 40) + 1, (k >> 1) + 4);
		lprintf("%39s", "");
	    }

	    break;
	case '\33':

	    return;
	}

	cursor(40, 17);
	lprintf("%8d", c[BANKACCOUNT]);
	cursor(49, 18);
	lprintf("%8d", c[GOLD]);
    }
}

/* Subroutine to appraise any stone for the bank */
void appraise(int gemstone)
{
    int j;
    int amt;

    for(j = 0; j < 26; ++j) {
	if(iven[j] == gemstone) {
	    lprintf("\nI see you have %s", objectname[gemstone]);

	    if(gemstone == OLARNEYE) {
		lprcat(" I must commend you. I didn't think\nyou could get it.");
	    }

	    lprcat(" Shall I appraise it for you? ");
	    yrepcount = 0;

	    if(getyn() == 'y') {
		lprcat("yes.\n Just one moment please \n");
		nap(1000);

		if(gemstone == OLARNEYE) {
		    amt = 250000 - (((gtime * 7) / 100) * 100);

		    if(amt < 50000) {
			amt = 50000;
		    }
		}
		else {
		    amt = (255 & ivenarg[j]) * 100;
		}

		lprintf("\nI can see this is an excellent stone, It is worth %d",
			amt);

		lprintf("\nWould you like to sell it to us? ");
		yrepcount = 0;

		if(getyn() == 'y') {
		    lprcat("yes\n");
		    c[GOLD] += amt;
		    iven[j] = 0;
		}
		else {
		    lprcat("no thank you.\n");
		}

		if(gemstone == OLARNEYE) {
		    lprcat("It is, of course, your privilege to keep the stone\n");
		}
	    }
	    else {
		lprcat("no\nOk.\n");
	    }
	}
    }
}

/* Function for the trading post */
static void otradhead()
{
    clear();
    lprcat("Welcom to Larn Trading Post. We buy items that explorers no longer fins\n");
    lprcat("useful. Since the condition of the items you bring in is not certain,\n");
    lprcat("and we incur great expense in reconditioning the items, we usually pay\n");
    lprcat("only 20% of their value were they do be new. If the items are badly\n");
    lprcat("damaged, we will only pay 10% of their new value.\n\n");
}

void otradepost()
{
    int i;
    int j;
    int value = 0;
    int isub;
    int izarg;

    dndcount = 0;
    dnditm = dndcount;

    /* Disable signals */
    nosignal = 1;
    resetscroll();
    otradhead();

    while(1) {
	lprcat("\nWhat item do yo uwant to see to us [");
	standout();
	lprcat("*");
	standend();
	lprcat(" for what list, or ");
	standout();
	lprcat("escape");
	standend();
	lprcat("]? ");
	i = 0;

	while((i > 'z')
	      && ((i < 'a') && (i != '*') && (i != '\33') && (i != '.'))) {
	    i = getchar();
	}

	if(i == '\33') {
	    setscroll();
	    recalc();
	    drawscreen();

	    /* Enable signals */
	    nosignal = 0;

	    return;
	}

	isub = i - 'a';
	j = 0;

	if(iven[isub] == OSCROLL) {
	    if(scrollname[ivenarg[isub]][0] == 0) {
		/* Can't sell unidentified item */
		j = 1;
		cnsitm();
	    }
	}

	if(iven[isub] == OPOTION) {
	    if(potionname[ivenarg[isub]][0] == 0) {
		/* Can't sell unidentified item */
		j = 1;
		cnsitm();
	    }
	}

	if(!j) {
	    if(i == '*') {
		clear();
		qshowstr();
		otradhead();
	    }
	}
	else if(iven[isub] == 0) {
	    lprintf("\nYou don't have item %c!", isub + 'a');
	}
	else {
	    for(j = 0; j < maxitm; ++j) {
		if((itm[j].obj == iven[isub])
		   || (iven[isub] == ODIAMOND)
		   || (iven[isub] == ORUBY)
		   || (iven[isub] == OEMERALD)
		   || (iven[isub] == OSAPPHIRE)) {
		    srcount = 0;
		    
		    /* Show what the item was */
		    show3(isub);

		    if((iven[isub] == ODIAMOND)
		       || (iven[isub] == ORUBY)
		       || (iven[isub] == OEMERALD)
		       || (iven[isub] == OSAPPHIRE)) {
			value = 20 * ivenarg[isub];
		    }
		}
		else if((itm[j].obj == OSCROLL) || (itm[i].obj == OPOTION)) {
		    value = 2 * itm[j + ivenarg[isub]].price;
		}
		else {
		    izarg = ivenarg[isub];

		    /* Appreciate if a +n object */
		    value = itm[j].price;

		    if(izarg >= 0) {
			value *= 2;
		    }

		    while(izarg-- > 0) {
			value = 14 * (67 + value) / 10;

			if(value >= 500000) {
			    break;
			}
		    }
		}

		lprintf("\nItem (%c) is worth %d gold pieces to us. Do you want to sell it? ",
			i,
			value);

		yrepcount = 0;

		if(getyn() == 'y') {
		    lprcat("yes\n");
		    c[GOLD] += value;

		    if(c[WEAR] == isub) {
			c[WEAR] = -1;
		    }

		    if(c[WIELD] == isub) {
			c[WIELD] = -1;
		    }

		    if(c[SHIELD] == isub) {
			c[SHIELD] = -1;
		    }

		    adjustcvalues(iven[isub], ivenarg[isub]);
		    iven[isub] = 0;
		}
		else {
		    lprcat("no thanks.\n");
		}
		
		/* Get out of the inner loop */
		j = maxitm + 100;
	    }

	    if(j <= (maxitm + 2)) {
		lprcat("\nSo sorry, but we are not authorized to accept that item.");
	    }
	}
    }
}

void cnsitm()
{
    lprcat("\nSorry, we can't accept unidentified objects.");
}

/* For the Larn Revenue Service */
void olrs()
{
    int i;
    int first;
    unsigned long amt;
    
    /* Disable signals */
    nosignal = 1;
    first = nosignal;
    clear();
    resetscroll();
    cursor(1, 4);
    lprcat("Welcome to the Larn Revenue Service district office. How can we help you?");

    while(1) {
	if(first) {
	    first = 0;
	}
	else {
	    cursors();
	    lprcat("\n\nYour wish? [(");
	    standout();
	    lprcat("p");
	    standend();
	    lprcat(") pay taxes, or ");
	    standout();
	    lprcat("escape");
	    standend();
	    lprcat("] ");
	    yrepcount = 0;
	    i = 0;

	    while((i != 'p') && (i != '\33')) {
		i = getchar();
	    }

	    switch(i) {
	    case 'p':
		lprcat("pay taxes\nHow much? ");
		amt = readnum(c[GOLD]);

		if(amt > c[GOLD]) {
		    lprcat(" You don't have that much.\n");
		}
		else {
		    c[GOLD] -= paytaxes(amt);
		}

		break;
	    case '\33':
		/* Enable signals */
		nosignal = 0;
		setscroll();
		drawscreen();

		return;
	    }
	}

	cursor(1, 6);

	if(outstanding_taxes > 0) {
	    lprintf("You presently owe %d gp in taxes. ", outstanding_taxes);
	}
	else {
	    lprcat("You do not owe us any taxes. ");
	}

	cursor(1, 8);

	if(c[GOLD] > 0) {
	    lprintf("You have %6d gp. ", c[GOLD]);
	}
	else {
	    lprcat("You have no gold pieces. ");
	}
    }
}
