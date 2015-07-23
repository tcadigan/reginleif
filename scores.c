/*
 * scores.c
 * Larn is copyrighted 1986 by Noah Morgan.
 *
 * Functions in this file are:
 * 
 * readboard()        Function to read in the scoreboard into a static buffer
 * writeboard()       Function to write the scoreboard from readboard()'s buffer
 * makeboard()        Function to create a new scoreboard (wipe out old one)
 * hashewon()         Function to return 1 if player has won a game before, else 0
 * paytaxes()         Function to pay taxes if any are due
 * winshou()          Subroutine to print out the winning scoreboard
 * shou()             Subroutine to print out the non-winners scoreboard
 * showscores()       Function to show the scoreboard on the terminal
 * showallscores()    Function to show score and the iven lists that go with them
 * sortboard()        Function to sort the scoreboard
 * newscore()         Function to add entry to scoreboard
 * new1sub()          Subroutine to put player into a
 * new2sub()          Subroutine to put a player into a
 * died()             Subroutine to record who played Larn, and what the score was
 * diedsub()          Subroutine to print out a line showing player when he is killed
 * diedlog()          Subroutine to read a log file and print it out in ascii format
 * getplid(name)      Function to get player's id # from id file
 */

#include <sys/types.h>
#include <sys/times.h>
#include <sys/stat.h>

#include "header.h"

/* This is the structure for the scoreboard */
struct scofmt {
    /* The score of the player */
    long score;

    /* The user id of the player */
    long suid;

    /* The number of the monster that killed player */
    short what;

    /* The level player was on when he died */
    short level;

    /* The level of difficulty player played at */
    short hardlev;

    /* The relative ordering place of this entry */
    short order;

    /* The name of the character */
    char who[40];

    /* This is the inventory list of the character */
    char sciv[26][2];
};

/* This is the structure for the winning scoreboard */
struct wscofmt {
    /* The score of the player */
    long score;

    /* The time used in mobuls to win the game */
    long timeused;

    /* Taxes he owes to LRS */
    long taxes;

    /* The user id numer of the player */
    long suid;

    /* The level of difficulty player played at */
    short hardlev;

    /* The relative ordering place of this entry */
    short order;

    /* The name of the character */
    char who[40];
};

/* 102 bytes struct for the log file */
struct log_fmt {
    /* The player's score */
    long score;

    /* Time when game was over */
    long diedtime;

    /* Level in caves */
    short cavelev;

    /* Difficulty player played at */
    short diff;

#ifdef EXTRA
    /* Real time of game in seconds */
    long elapsedtime;

    /* Bytes input and output */
    long bytout;
    long bytin;

    /* Number of moves made by player */
    long moves;

    /* Armor class of player */
    short ac;

    /* Player's hitpoints */
    short hp;
    short hpmax;

    /* Cpu time needed in seconds */
    short cputime;

    /* Monsters killed and spells cast */
    short killed;
    short spused;

    /* Usage of the cpu in % */
    short usage;

    /* Player level */
    short lev;

#endif
    
    /* Player name */
    char who[12];

    /* What happened to player */
    char what[46];
};

/* The structure for the scoreboard */
static struct scofmt sco[SCORESIZE];

/* Struct for the winning scoreboard */
static struct scofmt winr[SCORESIZE];

/* Structure for the log file */
static struct log_fmt logg;

static char *whydead[] = {
    "quit",
    "suspended",
    "self-annihilated",
    "shot by an arrow",
    "hit by a dart",
    "fell into a pit",
    "fell into a bottomless pit",
    "a winner",
    "trapped in solid rock",
    "killed by a missing save file",
    "killed by an old save file",
    "caught by the greedy cheater checker trap",
    "killed by a protected save file",
    "killed his family and committed suicide",
    "erased by a wayward finger",
    "fell through a bottomless trap door",
    "fell through a trap door",
    "drank some poisonous water",
    "fried by an electric shock",
    "slipped on a volcano shaft",
    "killed by a stupid act of frustration",
    "attacked by a revolting demon",
    "hit by his own magic",
    "demolished by an unseen attacker",
    "fell into the dreaful sleep",
    "killed by an exploding chest",
    "killed by a missing maze data file", /* 26 */
    "annihilated in a sphere",
    "died a post mortem death",
    "wasted by a malloc() failure"
};

/*
 * readboard()
 *
 * Function to read in the scoreboard into a static buffer.
 *
 * Returns -1 if unable to write the scoreboard, returns 0 if all is OK
 */
int readboard()
{
    if(lopen(scorefile) < 0) {
	lprcat("Can't read scoreboard\n");
	lflush();

	return -1;
    }

    lrfill((char *)sco, sizeof(sco));
    lrfill((char *)winr, sizeof(winr));
    lrclose();
    lcreat((char *)0);

    return 0;
}

/*
 * writeboard()
 *
 * Funciton to write the scoreboard from readboard()'s buffer
 *
 * Returns -1 if unable to write the scoreboard, returns 0 if all is OK
 */
int writeboard()
{
    set_score_output();

    if(lcreat(scorefile) < 0) {
	lprcat("Can't write scoreboar\n");
	lflush();

	return -1;
    }

    lwrite((char *)sco, sizeof(sco));
    lwrite((char *)winr, sizeof(winr));
    lwclose();
    lcreat((char *)0);

    return 0;
}

/*
 * makeboard()
 *
 * Function to create a new scoreboard (wipe out old one)
 *
 * Returns -1 if unable to write the scoreboard, returns 0 if all is OK
 */
int makeboard()
{
    int i;

    for(i = 0; i < SCORESIZE; ++i) {
	sco[i].score = 0;
	winr[i].score = sco[i].score;
	winr[i].taxes = winr[i].score;
    }

    if(writeboard()) {
	return -1;
    }

    chmod(scorefile, 0660);

    return 0;
}

/*
 * hashewon()
 *
 * Function to return 1 if player has won a game before, else 0
 *
 * This function also sets c[HARDGAME] to appropriate value -- 0 if not a
 * winner, otherwise the next level of difficulty listed in the winners
 * scoreboard. This function also sets outstanding_taxes to the value in the
 * winners scoreboard.
 */
int hashewon()
{
    int i;

    c[HARDGAME] = 0;

    /* Can't find scoreboard */
    if(readboard() < 0) {
	return 0;
    }

    /* Search through winners scoreboard */
    for(i = 0; i < SCOREBOARD; ++i) {
	if(winr[i].suid == userid) {
	    if(winr[i].score > 0) {
		c[HARDGAME] = winr[i].hardlev + 1;
		outstanding_taxes = winr[i].taxes;

		return 1;
	    }
	}
    }

    return 0;
}

/*
 * paytaxes()
 *
 * Function to pay taxes if any are due
 *
 * Enter with the amount (in gp) to pay on the taxes.
 * Returns amount actually paid.
 */
long paytaxes(long x)
{
    int i;
    long amt;

    if(x < 0) {
	return 0;
    }

    if(readboard() < 0) {
	return 0;
    }

    for(i = 0; i < SCORESIZE; ++i) {
	/* Look for player's winning entry */
	if(winr[i].suid == userid) {
	    /* Search for a winning entry for the player */
	    if(winr[i].score > 0) {
		amt = winr[i].taxes;

		/* Don't overpay taxes (Ughhhh) */
		if(x < amt) {
		    amt = x;
		}

		winr[i].taxes -= amt;
		outstanding_taxes -= amt;

		if(writeboard() < 0) {
		    return 0;
		}

		return amt;
	    }
	}
    }

    /* Couldn't find user on winning scoreboard */
    return 0;
}

/*
 * winshou()
 *
 * Subroutine to print out the winning scoreboard
 *
 * Returns the number of players on scoreboard that were shown
 */
int winshou()
{
    struct wscofmt *p;
    int i;
    int j;
    int count;

    /* Is there anyone on the scoreboard? */
    j = 0;
    count = 0;

    for(i = 0; i < SCORESIZE; ++i) {
	if(winr[i].score != 0) {
	    ++j;

	    break;
	}
    }

    if(j) {
	lprcat("\n  Score   Difficulty   Time Needed  Larn Winners List\n");

	/* This loop is needed to print out the winners in order */
	for(i = 0; i < SCORESIZE; ++i) {
	    for(j = 0; j < SCORESIZE; ++j) {
		/* Pointer to the scoreboard entry */
		p = &winr[j];

		if(p->order == i) {
		    if(p->score) {
			++count;
			lprintf("%10d     %2d     %5d Mobuls  %s \n",
				(long)p->score,
				(long)p->hardlev,
				(long)p->timeused,
				p->who);
		    }

		    break;
		}
	    }
	}
    }

    /* Return number of people on scoreboard */
    return count;
}

/*
 * shou(int x)
 *
 * Subroutine to print out the non-winners scoreboard
 *
 * Enter with 0 to list the scores, enter with 1 to list inventories too
 * Returns the number of players on scoreboard that were shown
 */
int count(int x)
{
    int i;
    int j;
    int n;
    int k;
    int count;

    /* Is the scoreboard empty? */
    j = 0;
    count = 0;

    for(i = 0; i < SCORESIZE; ++i) {
	if(sco[i].score != 0) {
	    ++j;

	    break;
	}
    }

    if(j) {
	lprcat("\n   Score   Difficulty   Larn Visitor Log\n");

	/* Be sure to print them out in order */
	for(i = 0; i < SCORESIZE; ++i) {
	    for(j = 0; j < SCORESIZE; ++j) {
		if(sco[j].order == i) {
		    if(sco[j].score) {
			++count;
			lprintf("%10d    %2d      %s ",
				(long)sco[j].score,
				(long)sco[j].hardlev,
				sco[j].who);

			if(sco[j].what < 256) {
			    lprintf("killed by a %s",
				    monster[sco[j].what].name);
			}
			else {
			    lprintf("%s", whydead[sco[j].what - 256]);
			}

			if(x != 263) {
			    lprintf(" on %s", levelname[sco[j].level]);
			}

			if(x) {
			    for(n = 0; n < 26; ++n) {
				iven[n] = sco[j].sciv[n][0];
				ivenarg[n] = sco[j].sciv[n][1];
			    }

			    for(k = 1; k < 99; ++k) {
				for(n = 0; n < 26; ++n) {
				    if(k == iven[n]) {
					srcount = 0;
					show3(n);
				    }
				}
			    }

			    lprcat("\n\n");
			}
			else {
			    lprc('\n');
			}
		    }

		    j = SCORESIZE;
		}
	    }
	}
    }

    /* Return the number of players just shown */
    return count;
}

/*
 * showscores()
 *
 * Function to show the scoreboard on the terminal
 *
 * Returns nothing of value
 */
static char esb[] = "The scoreboard is empty.\n";

void showscores()
{
    int i;
    int j;

    lflush();
    lcreat((char *)0);

    if(readboard() < 0) {
	return;
    }

    i = winshou();
    j = shou();

    if((i + j) == 0) {
	lprcat(esb);
    }
    else {
	lprc('\n');
    }

    lflush();
}

/*
 * showallscores()
 *
 * Function to show scores and the iven lists that go with them
 *
 * Returns nothing of value
 */
void showallscores()
{
    int i;
    int j;

    lflush();
    lcreat((char *)0);

    if(readboard() < 0) {
	return;
    }

    /* Not wielding or wearing anything */
    c[SHIELD] = -1;
    c[WIELD] = c[SHIELD];
    c[WEAR] = c[WIELD];

    for(i = 0; i < MAXPOTION; ++i) {
	potionname[i][0] = ' ';
    }

    for(i = 0; i < MAXSCROLL; ++i) {
	scrollname[i][0] = ' ';
    }

    i = winshou();
    j = shou(1);

    if((i + j) == 0) {
	lprcat(esb);
    }
    else {
	lprc('\n');
    }

    lflush();
}

/*
 * sortboard()
 *
 * Function to sort the scoreboard
 *
 * Returns 0 if no sorting done, else returns 1
 */
int sortboard()
{
    int i;
    int j;
    int pos;
    long jdat;

    for(i = 0; i < SCORESIZE; ++i) {
	winr[i].order = -1;
	sco[i].order = winr[i].order;
    }

    pos = 0;

    while(pos < SCORESIZE) {
	jdat = 0;

	for(i = 0; i < SCORESIZE; ++i) {
	    if((sco[i].order < 0) && (sco[i].score >= jdat)) {
		j = i;
		jdat = sco[i].score;
	    }
	}

	sco[j].order = pos;
	++pos;
    }

    pos = 0;

    while(pos < SCORESIZE) {
	jdat = 0;

	for(i = 0; i < SCORESIZE; ++i) {
	    if((winr[i].order < 0) && (winr[i].score >= jdat)) {
		j = i;
		jdat = winr[i].score;
	    }
	}

	winr[i].order = pos;
	++pos;
    }

    return 1;
}

/*
 * newscore(long score, int winner, int whyded, char *whoo)
 *
 * Function to add entry to scoreboard
 *
 * Enter with the total score in gp in score, player's name in whoo, died()
 * reason # in whyded, and TRUE/FALSE in winner if a winner
 *
 * ex. newscore(1000, "player 1", 32, 0);
 */
void newscore(long score, int winner, int whyded, char *whoo)
{
    int i;
    long taxes;

    /* Do the scoreboard */
    if(readboard() < 0) {
	return;
    }

    /* If a winner then delete all non-winning scores */
    if(cheat) {
	/* If he cheated, don't let him win */
	winner = 0;
    }

    if(winner) {
	for(i = 0; i < SCORESIZE; ++i) {
	    if(sco[i].suid == userid) {
		sco[i].score = 0;
	    }
	}

	taxes = score * TAXRATE;

	/* Bonus for winning */
	score += (100000 * c[HARDGAME]);

	/* If he has a slot on the winning scoreboard update it if greater score */
	for(i = 0; i < SCORESIZE; ++i) {
	    if(winr[i].suid == userid) {
		new1sub(score, i, whoo, taxes);

		return;
	    }
	}

	/* He had no entry. Look for last entry and see if he has a greater score */
	for(i = 0; i < SCORESIZE; ++i) {
	    if(winr[i].order == (SCORESIZE - 1)) {
		new1sub(score, i, whoo, taxes);

		return;
	    }
	}
    }
    else if(!cheat) { /* For not winning scoreboard */
	/* If he has a slot on the scoreboard update it if greater score */
	for(i = 0; i < SCORESIZE; ++i) {
	    if(sco[i].suid == userid) {
		new2sub(score, i, whoo, whyded);

		return;
	    }
	}

	/* He had no entry. Look for last entry and see if he hsa a greater score */
	for(i = 0; i < SCORESIZE; ++i) {
	    if(sco[i].order == (SCORESIZE - 1)) {
		new2sub(score, i, whoo, whyded);

		return;
	    }
	}
    }
}

/*
 * new1sub(long score, int i, char *whoo, long taxes)
 *
 * Subroutine to put player into a winning scoreboard entry if his score is high
 * enough
 *
 * Enter with the total score in gp in score, players name in whoo, died()
 * reason # in whyded, and TRUE/FALSE in winner if a winner slot in scoreboard
 * in i, and the tax bill in taxes.
 * 
 * Returns nothing of value
 */
void new1sub(long score, int i, char *whoo, long taxes)
{
    struct wscofmt *p;

    p = &winr[i];
    p->taxes += taxes;

    if((score >= p->score) || (c[HARDGAME] > p->hardlev)) {
	strcpy(p->who, whoo);
	p->score = score;
	p->hardlev = c[HARDGAME];
	p->suid = userid;
	p->timeused = gtime / 100;
    }
}

/*
 * new2sub(long score, int i, char *whoo, int whyded)
 *
 * Subroutine to put player into a non-winning scoreboard entry if his score is
 * high enough
 *
 * Enter with the total score in gp in score, player's name in whoo, died()
 * reason # in whyded, and slot in scoreboard in i.
 *
 * Returns nothing of value
 */
void new2sub(long score, int i, char *whoo, int whyded)
{
    int j;
    struct scofmt *p;

    p = &sco[i];

    if((score >= p->score) || (c[HARDGAME] -> p->hardlev)) {
	strcpy(p->who, whoo);
	p->score = score;
	p->what = whyded;
	p->hardlev = c[HARDGAME];
	p->suid = userid;
	p->level = level;

	for(j = 0; j < 26; ++j) {
	    p->sciv[j][0] = iven[j];
	    p->sciv[j][1] = ivenarg[j];
	}
    }
}

/*
 * died(int x)
 *
 * Subroutine to record who played Larn, and what the score was
 *
 * if x < 0 then don't show scores
 * died() never returns! (unless c[LIFEPROT] and reincarnatable death!)
 *
 *     < 256        Killed by the monster number
 *     256          Quit
 *     257          Suspended
 *     258          Self-annihilated
 *     259          Shot by an arrow
 *     260          Hit by a dart
 *     261          Fell into a pit
 *     262          Fell into a bottomless pit
 *     263          A winner
 *     264          Trapped in solid rock
 *     265          Killed by a missing save file
 *     266          Killed by an old save file
 *     267          Caught by the greedy cheater checker trap
 *     268          Killed by a protected save file
 *     269          Killed his family and killed himself
 *     270          Erased by a wayward finger
 *     271          Fell through a bottomless trap door
 *     272          Fell through a trap door
 *     273          Drank some poisonous water
 *     274          Fried by an electric shock
 *     275          Slipped on a volcano shaft
 *     276          Killed by a stupid act of frustration
 *     277          Attacked by a revolting demon
 *     278          Hit by his own magic
 *     279          Demolished by an unseen attacker
 *     280          Fell into the dreaful sleep
 *     281          Killed by an exploding chest
 *     282          Killed by a missing maze data file
 *     283          Killed by a sphere of annihilation
 *     284          Died a post mortem death
 *     285          malloc() failure
 *     300          Quick quit -- don't put on scoreboard
*/
static int scorerror;

void died(int x)
{
    int f;
    int win;
    char ch;
    char *mod;
    long zzz;
    int i;
    struct tms cputime;
    int flag;

    flag = 1;

    /* If life protection */
    if(c[LIFEPROT] > 0) {
	if(x > 0) {
	    switch(x) {
	    case 256:
	    case 257:
	    case 262:
	    case 263:
	    case 265:
	    case 266:
	    case 267:
	    case 268:
	    case 269:
	    case 271:
	    case 282:
	    case 284:
	    case 285:
	    case 300:
		/* Can't be saved */
		flag = 0;
	    }
	}
	else {
	    switch(-x) {
	    case 256:
	    case 257:
	    case 262:
	    case 263:
	    case 265:
	    case 266:
	    case 267:
	    case 268:
	    case 271:
	    case 282:
	    case 284:
	    case 285:
	    case 300:
		/* Can't be saved */
		flag = 0;
	    }
	}

	if(flag) {
	    --c[LIFEPROT];
	    c[HP] = 1;
	    --c[CONSTITUTION];
	    cursors();
	    lprcat("\nYou feel wiiieeeeerrrrrd all over! ");
	    beep();
	    lflush();
	    sleep(4);

	    /* Only case where died() returns */
	    return;
	}
    }

    clearvt100();
    lflush();
    f = 0;

    /* Remove checkpoint file if used */
    if(ckpflag) {
	unlink(ckpfile);
    }

    /* If we are not to display the scores */
    if(x < 0) {
	++f;
	x = -x;
    }

    /* For quick exit of saved game */
    if((x == 300) || (x == 257)) {
	exit();
    }

    if(x == 263) {
	win = 1;
    }
    else {
	win = 0;
    }

    c[GOLD] += c[BANKACCOUNT];
    c[BANKACCOUNT] = 0;

    /* Now enter the player at the end of the scoreboard */
    newscore(c[GOLD], logname, x, win);

    /* Print out the score line */
    diedsub(x);
    lflush();
    set_score_output();

    /* Wizards can't score */
    if((wizard == 0) && (c[GOLD] > 0)) {
#ifndef NOLOG
	/* Append to file */
	if(lappend(logfile) < 0) {
	    /* And can't create new log file */
	    if(lcreat(logfile) < 0) {
		lcreat((char *)0);
		lprcat("\nCan't open record file: I can't post your score.\n");
		sncbr();
		resetscroll();
		lflush();

		exit();
	    }

	    chmod(logfile, 0660);
	}

	strcpy(logg.who, loginname);
	logg.score = c[GOLD];
	logg.diff = c[HARDGAME];

	if(x < 256) {
	    ch = *monster[x].name;

	    if((ch == 'a')
	       || (ch == 'e')
	       || (ch == 'i')
	       || (ch == 'o')
	       || (ch == 'u')) {
		mod = "an";
	    }
	    else {
		mod = "a";
	    }

	    sprintf(logg.what, "killed by %s %s", mod, monster[x].name);
	}
	else {
	    sprintf(logg.what, "%s", whydead[x - 256]);
	}

	logg.cavelev = level;

	/* Get cpu time -- write out score info */
	time(&zzz);
	logg.diedtime = zzz;

#ifdef EXTRA
	/* Get cpu time -- write out score info */
	times(&cputime);
	i = ((cputime.tms_utime + cputime.tms_stime) / 60) + c[CPUTIME];
	logg.cputime = i;
	logg.lev = c[LEVEL];
	logg.ac = c[AC];
	logg.hpmax = c[HPMAX];
	logg.hp = c[HP];
	logg.elapsedtime = ((zzz - initialtime) + 59) / 60;
	logg.usage = (10000 * i) / (zzz - initialtime);
	logg.bytin = c[BYTESIN];
	logg.bytout = c[BYTESOUT];
	logg.moves = c[MOVESMADE];
	logg.spused = c[SPELLCAST];
	logg.killed = c[MONSTKILLED];
#endif
	lwrite((char *)&logg, sizeof(struct log_fmt));
	lwclose();
#endif

	/* Now for the scoreboard maintenance -- not for a suspended game */
	if(x != 257) {
	    if(sortboard()) {
		scorerror = writeboard();
	    }
	}
    }

    if((x == 256) || (x == 257) || (f != 0)) {
	exit();
    }

    /* If we updated the scoreboard */
    if(scorerror == 0) {
	showscores();
    }

    if(x == 263) {
	mailbill();
    }

    exit();
}

/*
 * diedsub(int x)
 *
 * Subroutine to print out the line showing the player when he is killed
 */
void diedsub(int x)
{
    char ch;
    char *mod;

    lprintf("Score: %d, Diff: %d,  %s ",
	    (long)c[GOLD],
	    (long)c[HARDGAME],
	    logname);

    if(x < 256) {
	ch = *monster[x].name;

	if((ch == 'a')
	   || (ch == 'e')
	   || (ch == 'i')
	   || (ch == 'o')
	   || (ch == 'u')) {
	    mod = "an";
	}
	else {
	    mod = "a";
	}

	lprintf("killed by %s %s", mod, monster[x].name);
    }
    else {
	lprintf("%s", whydead[x - 256]);
    }

    if(x != 263) {
	lprintf(" on %s\n", levelname[level]);
    }
    else {
	lprc('\n');
    }
}

/*
 * diedlog()
 *
 * Subroutine to read a log file and print it out in ascii format
 */
void diedlog()
{
    int n;
    char *p;
    struct stat stbuf;

    lcreat((char *)0);

    if(lopen(logfile) < 0) {
	lprintf("Can't locate log file <%s>\n", logfile);

	return;
    }

    if(fstat(fd, &stbuf) < 0) {
	lprintf("Can't stat log file <%s>\n", logfile);

	return;
    }

    for(n = (stbuf.st_size / sizeof(struct log_fmt)); n > 0; --n) {
	lrfill((char *)&logg, sizeof(struct log_fmt));
	p = ctime(&logg.diedtime);
	p[16] = '\n';
	p[17] = 0;
	lprintf("Score: %d, Diff: %d,  %s %s on %d at %s",
		(long)logg.score,
		(long)logg.diff,
		logg.who,
		logg.what,
		(long)logg.cavelev,
		p + 4);

#ifdef EXTRA
	if(logg.moves <= 0) {
	    logg.moves = 1;
	}

	lprintf("  Experience Level: %d,  AC: %d,  HP: %d/%d,  Elapsed Time: %d minutes\n",
		(long)logg.lev,
		(long)logg.ac,
		(long)logg.hp,
		(long)logg.hpmax,
		(long)logg.elapsedtime);

	lprintf("  CPU time used: %d seconds,  Machine usage: %d.%02d%%\n",
		(long)logg.cputime,
		(long)(logg.usage / 100),
		(long)(logg.usage % 100));

	lprintf("  BYTES in: %d, out: %d, moves: %d, deaths: %d, spells cast: %d\n",
		(long)logg.bytin,
		(long)logg.bytout,
		(long)logg.moves,
		(long)logg.killed,
		(long)logg.spused);

	lprintf("  out bytes per move: %d,  time per move: %d ms\n",
		(long)(logg.bytout / logg.moves),
		(long)((logg.cputime * 1000) / logg.moves));
#endif
    }

    lflush();
    lrclose();

    return;
}

#ifndef UIDSCORE
/*
 * getplid(char *nam)
 *
 * Function to get players id # form id file
 *
 * Enter with the name of the player's character in name.
 *
 * Returns the id # of the player's character, or -1 if failure.
 *
 * This routine will try to find the name in the id file, if it's not there, it
 * will try to make a new entry int he file. Only returns -1 if can't find him
 * in the file, and can't make a new entry in the file.
 *
 * Format of playerids file:
 *     Id # in ascii    \n     character name     \n
*/

/* Player id # if previously done */
static int havepid = -1;

int getplid(char *nam)
{
    int fd7;
    int high = 999;
    int no;
    char *p;
    char *p2;
    char name[80];

    /* Already did it */
    if(havepid != -1) {
	return havepid;
    }

    /* Flush any pending I/O */
    lflush();

    /* Append a '\n' to name */
    sprintf(name, "%s\n", nam);

    /* No file, make it */
    if(lopen(playerids) < 0) {
	fd7 = creat(playerids, 0666);

	/* Can't make it */
	if(fd7 < 0) {
	    return -1;
	}

	close(fd7);

	/* Now append new playerid record to file */

	/* Can't open file for append */
	if(lappend(playerids) < 0) {
	    return -1;
	}

	/* New id # and name */
	++high;
	lprintf("%d\n%s", high, name);
	lwclose();

	/* Re-open terminal channel */
	lcreat((char *) 0);

	return high;
    }

    /* Now search for the name in the player id file */
    while(1) {
	p = lgetl();

	/* EOF? */
	if(p == NULL) {
	    break;
	}

	/* The id # */
	no = atoi(p);
	p2 = lgetl();
	
	/* EOF? */
	if(p2 == NULL) {
	    break;
	}

	/* Accumulate highest id # */
	if(no > high) {
	    high = no;
	}

	/* We found him */
	if(strcmp(p2, name) == 0) {
	    /* His id number */
	    return no;
	}
    }

    /* If we get here, we didn't find him in the file -- put him there */
    lrclose();

    /* Can't open file for append */
    if(lappend(playerids) < 0) {
	return -1;
    }

    /* New id # and name */
    ++high;
    lprintf("%d\n%s", high, name);
    lwclose();

    /* Re-open terminal channel */
    lcreat((char *)0);

    return high;
}

#endif
