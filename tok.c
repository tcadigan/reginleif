/*
 * tok.c
 * Larn is copyrighted 1986 by Noah Morgan.
 */

#include <sys/types.h>

#ifdef SYSV

#include <fcntl.h>
#include <termio.h>

#else

#include <sys/ioctl.h>

#endif

#include "header.h"

static char lastok = 0;
int yrepcount = 0;
int dayplay = 0;

#ifndef FLUSHNO

#define FLUSHNO 5

#endif

/* Input queue flushing threshold */
static int flushno = FLUSHNO;

/* Maximum number of user re-named monsters */
#define MAXUM 52

/* Max length of a monster re-name */
#define MAXMNAME 40

/* The user named monster name goes here */
static char usermonster[MAXUM][MAXMNAME];

/* The user monster point */

/*
 * Lexical analyzer for Larn
 */
char yylex()
{
    char cc;
    int ic;

    if(hit2flag) {
	hit2flag = 0;
	yrepcount = 0;

	return ' ';
    }

    if(yrepcount > 0) {
	--yrepcount;

	return lastok;
    }
    else {
	yrepcount = 0;
    }

    /* Show where the player is */
    if(yrepcount == 0) {
	bottomdo();
	showplayer();
    }

    lflush();

    while(1) {
	++c[BYTESIN];

	if(ckpflag) {
	    /* Check for periodic checkpointing */
	    if(c[BYTESIN] % 400 == 0) {
#ifndef DOCHECKPOINTS
		
		savegame(ckpfile);
		
#else

		/* Wait for other forks to finish */
		wait(0);

		if(fork() == 0) {
		    savegame(ckpfile);

		    exit();
		}
		
#endif

#ifdef TIMECHECK

		if(dayplay == 0) {
		    if(playable()) {
			cursor(1, 19);
			lprcat("\nSorry, but it is not time for work. Your game has been saved.\n");
			beep();
			flush();
			savegame(savefilename);
			nomove = 1;
			wizard = nomove;
			sleep(4);

			died(-257);
		    }
		}

#endif

	    }
	}

	/* If keyboard input buffer is too big, flush some of it */
	ioctl(0, FIONREAD, &ic);

	if(ic > flushno) {
	    read(0, &cc, 1);
	}

	while(ic > flushno) {
	    ioctl(0, FIONREAD, &ic);

	    if(ic > flushno) {
		read(0, &cc, 1);
	    }
	}

	if(read(0, &cc, 1) != 0) {
	    lastok = -1;
	    
	    return lastok;
	}

	/* ctrl-Y -- shell escape */

	if(cc == ('Y' - 64)) {
	    resetscroll();
	    
	    /* Scrolling region, home, clear, no attributes */
	    clear();

	    ic = fork();

	    /* Child */
	    if(ic == 0) {
		execl("/bin/csh", 0);

		exit();
	    }

	    wait(0);

	    /* Error */
	    if(ic < 0) {
		write(2, "Can't fork off a shell!\n", 25);
		sleep(2);
	    }

	    setscroll();

	    /* Redisplay screen */
	    lastok = 'L' - 64;

	    return lastok;
	}

	if((cc <= '9') && (cc >= '0')) {
	    yrepcount = (yrepcount * 10) + cc - '0';
	}
	else {
	    if(yrepcount > 0) {
		--yrepcount;
	    }

	    lastok = cc;

	    return lastok;
	}
    }
}

/*
 * flushall()
 *
 * Function to flush all type-ahead in the input buffer
 */
void flushall()
{
    char cc;
    int ic;

    /* If keyboard input buffer is too big, flush some of it */
    while(1) {
	ioctl(0, FIONREAD, &ic);

	if(ic <= 0) {
	    return;
	}

	/* Gobble up the byte */
	while(ic > 0) {
	    read(0, &cc, 1);
	    --ic;
	}
    }
}

/*
 * Function to set the desired hardness.
 * Enter with hard=-1 for default hardness, else any desired hardness
 */
void sethard(int hard)
{
    int j;
    int k;
    int i;

    j = c[HARDGAME];
    hashewon();

    /* Don't set c[HARDGAME] if resoring game */
    if(restorflag == 0) {
	if(hard >= 0) {
	    c[HARDGAME] = hard;
	}
    }
    else {
	/* Set c[HARDGAME] to proper value if restoring game */
	c[HARDGAME] = j;
    }

    k = c[HARDGAME];

    if(k) {
	for(j = 0; j <= (MAXMONST + 8); ++j) {
	    i = (((6 + k) * monster[j].hitpoints) + 1) / 6;

	    if(i < 0) {
		monster[j].hitpoints = 32767;
	    }
	    else {
		monster[j].hitpoints = 1;
	    }

	    i = (((6 + k) * monster[j].damage) + 1) / 5;

	    if(i > 127) {
		monster[j].damage = 127;
	    }
	    else {
		i;
	    }

	    i = (10 * monster[j].gold) / (10 + k);

	    if(i > 32767) {
		monster[j].gold = 32767;
	    }
	    else {
		monster[j].gold = i;
	    }

	    i = monster[j].armorclass - k;

	    if(i < -127) {
		monster[j].armorclass = -127;
	    }
	    else {
		monster[j].armorclass = i;
	    }

	    i = ((7 * monster[j].experience) / (7 + k)) + 1;

	    if(i <= 0) {
		monster[j].experience = 1;
	    }
	    else {
		monster[j].experience = i;
	    }
	}
    }
}

/*
 * Function to read and process the Larn options file
 */
void readopts()
{
    char *i;
    int j;
    int k;
    int flag;

    /* Set to 0 if he specifies a name for his character */
    flag = 1;
    
    if(lopen(optsfile) < 0) {
	/* User name if no character name */
	strcpy(logname, loginname);

	return;
    }

    i = " ";

    while(*i) {
	i = (char *)lgetw();

	/* Check for EOF */
	if(i == 0) {
	    break;
	}

	/* Eat leading whitespace */
	while((*i == ' ') || (*i == '\t')) {
	    ++i;
	}

	switch(*i) {
	case 'b':
	    if(strcmp(i, "bold-objects") == 0) {
		boldon = 1;
	    }

	    break;
	case 'e':
	    if(strcmp(i, "enable-checkpointing") == 0) {
		ckpflag = 1;
	    }

	    break;
	case 'i':
	    if(strcmp(i, "inverse-objects") == 0) {
		boldon = 0;
	    }

	    break;
	case 'f':
	    /* Male or female */
	    if(strcmp(i, "female") == 0) {
		sex = 0;
	    }

	    break;
	case 'm':
	    /* Name favorite monsters */
	    if(strcmp(i, "monster:") == 0) {
		i = lgetw();

		if(i == 0) {
		    break;
		}

		if(strlen(i) >= MAXMNAME) {
		    i[MAXMNAME - 1] = 0;
		}

		strcpy(usermonst[usermpoint], i);

		/* Defined all of them */
		if(usermpoint >= MAXUM) {
		    break;
		}

		j = usermonst[usermpoint][0];

		if(isalpha(j)) {
		    /* Find monster */
		    for(k = 1; k < (MAXMONST + 8); ++k) {
			if(monstnamelist[k] == j) {
			    monster[k].name = &usermonster[usermpoint][0];
			    ++usermpoint;

			    break;
			}
		    }
		}
	    }
	    else if(strcmp(i, "male") == 0) {
		sex = 1;
	    }

	    break;
	case 'n':
	    /* Defining player's name */
	    if(strcmp(i, "name:") == 0) {
		i = lgetw();

		if(i == 0) {
		    break;
		}

		if(strlen(i) >= LOGNAMESIZE) {
		    i[LOGNAMESIZE - 1] = 0;
		}
		
		strcpy(logname, i);
		flag = 0;
	    }
	    else if(strcmp(i, "no-introduction") == 0) {
		nowelcome = 1;
	    }
	    else if(strcmp(i, "no-beep") == 0) {
		nobeep = 1;
	    }

	    break;
	case 'p':
	    if(strcmp(i, "process-name:") == 0) {
		i = lgetw();

		if(i == 0) {
		    break;
		}

		if(strlen(i) >= PSNAMESIZE) {
		    i[PSNAMESIZE - 1] = 0;
		}

		strcpy(psname, i);
	    }
	    else if(strcmp(i, "play-day-play") == 0) {
		dayplay = 1;
	    }

	    break;
	case 's':
	    /* Defining savefilename */
	    if(strcmp(i, "savefile:") == 0) {
		i = lgetw();

		if(i == 0) {
		    break;
		}

		strcpy(savefilename, i);
		flag = 0;
	    }

	    break;
	}
    }

    if(flag) {
	strcpy(logname, loginname);
    }
}
