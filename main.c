/*
 * main.c
 */

#include "main.h"

#include <pwd.h>
#include <sys/types.h>

#include "header.h"

static char copyright[] = "\nLarn is copyrighted 1986 by Noah Morgan.\n";

/* Line counter for showstr() */
int srccount = 0;

/* If 1 then don't lookforobject() next round */
int dropflag = 0;

/* Random monster creation counter */
int rmst = 80;

/* The player's login user id number */
int userid;

/* If nomove then don't count next iteration as a move */
char nowelcome = 0;
char nomove = 0;

/* If viewflag then we have done a 99 stay here and don't showcell in the main loop */
static char viewflag = 0;

/* 1 means restore has been done */
char restorflag = 0;

static char cmdhelp[] = "\
Cmd line format: larn [slicnh] [-o<optsfile>] [-##] [++]\n\
  -s   Show the scoreboard\n\
  -l   Show the logfile (wizard id only)\n\
  -i   Show scoreboard with inventories of dead characters\n\
  -c   Create new scoreboard (wizard id only)\n\
  -n   suppress welcome message on starting game\n\
  -##  Specify level of difficulty (example -5)\n\
  -h   Print this help text\n\
  ++   Restore game from checkpoint file\n\
  -o<optsfile>   Specify .larnopts filename to be used instead of \"~/.larnopts\"\n\
";

#ifdef VT100
static char *termtypes[] = {
    "vt100",
    "vt101",
    "vt102",
    "vt103",
    "vt125",
    "vt131",
    "vt140",
    "vt180",
    "vt220",
    "vt240",
    "vt241",
    "vt320",
    "vt340",
    "vt341"
};
#endif

/*
 * ************
 * MAIN PROGRAM
 * ************
 */
int main(int argc, char *argv[])
{
    int i;
    int j;
    int hard;
    char *ptr = 0;
    char *ttype;
    struct passwd *pwe;

    /* First task is to identify the player */
#ifdef VT100
    /* Setup the terminal (find out what type) for termcap */
    init_term();
#endif

    /* Try to get the login name */
    ptr = getlogin();

    if((ptr == 0) || (*ptr == 0)) {
	/* Can we get it from /etc/passwd? */
	pwe = getpwuid(getuid());

	if(pwe) {
	    ptr = pwe->pw_name;
	}
	else {
	    ptr = getenv("USER");

	    if(ptr == 0) {
		ptr = getenv("LOGNAME");

		if(ptr == 0) {
		    write(2, "Can't find your logname. Who are you?\n", 38);

		    exit();
		}
	    }
	}
    }
    
    if(ptr == 0) {
	write(2, "Can't find your logname. Who are you?\n", 38);
	
	exit();
    }
    
    if(strlen(ptr) == 0) {
	write(2, "Can't find your logname. Who are you?\n", 38);

	exit();
    }

    /* Second task is to prepare the pathnames the player will need */
    /* Save loginname of the user for logging purposes */
    strcpy(loginname, ptr);

    /* This will be overwritten with the play's name */
    strcpy(logname, ptr);

    ptr = getenv("HOME");

    if(ptr == 0) {
	ptr = ".";
    }

    strcpy(savefilename, ptr);

    /* Save file name in home directory */
    strcat(savefilename, "/Larn.sav");

    /* The .larnopts filename */
    sprintf(optsfile, "%s/.larnopts", ptr);

    /* Now malloc the memory for the dungeon */
    cell = (struct cel *)malloc(sizeof(struct cel) * (MAXLEVEL + MAXVLEVEL) * MAXX * MAXY);

    /* If malloc failure */
    if(cell == 0) {
	died(-285);
    }

    /* Output buffer */
    lpbuf = malloc((5 * BUFBIG) >> 2);

    /* Output buffer */
    inbuffer = malloc((5 * MAXIBUF) >> 2);

    /* malloc() failure */
    if((lpbuf == 0) || (inbuffer == 0)) {
	died(-285);
    }

    lcreat((char *)0);

    /* Set the initial clock */
    newgame();
    hard = -1;

#ifdef VT100
    /* Check terminal type to avoid users who have not vt100 type terminals */
    ttype = getenv("TERM");
    j = 0;
    
    for(i = 0; i < (sizeof(termtypes) / sizeof(char *)); ++i) {
	if(strcmp(ttype, termtypes[i]) == 0) {
	    j = 0;

	    break;
	}
    }

    if(j) {
	lprcat("Sorry, Larn needs a VT100 family terminal for all its features.\n");
	lflush();

	exit();
    }
#endif

    /* Now make scoreboard if it is not there (don't clear) */
    /* Not there */
    if(access(scorefile, 0) == -1) {
	makeboard();
    }

    /* Now process the command line arguments */
    for(i = 1; i < argc; ++i) {
	if(argv[i][0] == '-') {
	    switch(argv[i][1]) {
	    case 's':
		/* Show scoreboard */
		showscores();

		exit();
	    case 'l':
		/* Show log file */
		diedlog();

		exit();
	    case 'i':
		/* Show all scoreboard */
		showallscores();

		exit();
	    case 'c':
		/* Anyone with password can create scoreboard */
		lprcat("Preparing to initialize the scoreboard.\n");

		/* Make new scoreboard */
		if(getpassword() != 0) {
		    makeboard();
		    lprc('\n');
		    showscores();
		}

		exit();
	    case 'n':
		/* No welcome message */
		nowelcome = 1;
		argv[i][0] = 0;

		break;
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
		/* For hardness */
		sscanf(&argv[i][1], "%d", &hard);

		break;
	    case 'h':
		/* Print out ocmmand line arguments */
		write(1, cmdhelp, sizeof(cmdhelp));

		exit();
	    case 'o':
		/* Specify a .larnopts filename */
		strncpy(optsfile, argv[i] + 2, 127);

		break;
	    default:
		printf("Unknown option <%s>\n", argv[i]);

		exit();
	    }
	}

	if(argv[i][1] == '+') {
	    clear();
	    restorflag = 1;

	    if(argv[i][1] == '+') {
		/* Restore checkpointed game */
		hitflag = 1;
		restoregame(ckpfile);
	    }

	    i = argc;
	}
    }

    /* Read the options file if there is one */
    readopts();

#ifdef UIDSCORE
    /* Obtain the user's effective id number */
    userid = geteuid();
#else
    /* Obtain the player's id number */
    userid = getplid(logname);
#endif

    if(userid < 0) {
	write(2, "Can't obtain playerid\n", 22);

	exit();
    }

#ifdef HIDEBYLINK
    /* This section of code changes the program to look like something else to ps */
    /* If a different process name only */
    if(strcmp(psname, argv[0])) {
	i = access(psname, 1);

	if(i < 0) {
	    /* Link not there */
	    if(link(argv[0], psname) >= 0) {
		argv[0] = psname;
		execv(psname, argv);
	    }
	}
	else {
	    unlink(psname);
	}
    }

    for(i = 1; i < argc; ++i) {
	/* Zero the argument to avoid ps snooping */
	szero(argv[i]);
    }
#endif

    /* Restore game if need to */
    if(access(savefilename, 0) == 0) {
	clear();
	restorflag = 1;
	hitflag = 1;

	/* Restore last game */
	restoregame(savefilename);
    }

    /* Trap all needed signals */
    sigsetup();

    /* Set up the desired difficulty */
    sethard(hard);

    /* Setup the terminal special mode */
    setupvt100();

    /* Create new game */
    if(c[HP] == 0) {
	/* Make the character that will paly */
	makeplayer();

	/* Make the dungeon */
	newcavelevel(0);

	/* Tell signals that we are in the welcome screen */
	predostuff = 1;

	/* Welcome the player to the game */
	if(nowelcome == 0) {
	    welcome();
	}
    }

    /* Show the initial dungeon */
    drawscreen();

    /* Tell the trap functions that they must do a showplayer from here on */
    predostuff = 2;

    /* Games should be run niced */
    /* nice(1); */

    hit2flag = 0;
    yrepcount = hit2flag;

    while(1) {
	/* See if there is an object here */
	if(dropflag == 0) {
	    lookforobject();
	}
	else {
	    /* Don't show if just dropped an item */
	    dropflag = 0;
	}

	/* Move the monsters */
	if(hitflag == 0) {
	    if(c[HASTEMONST]) {
		movemonst();
	    }
	    
	    movemonst();
	}

	/* Show stuff around player */
	if(viewflag == 0) {
	    showcell(playerx, playery);
	}
	else {
	    viewflag = 0;
	}

	if(hit3flag) {
	    flushall();
	}

	hit3flag = 0;
	hitflag = hit3flag;
	nomove = 1;

	/* Update bottom line */
	bot_linex();

	/* Get commands and make moves */
	while(nomove) {
	    if(hit3flag) {
		flushall();
	    }

	    nomove = 0;
	    parse();
	}
	
	/* Regenerate hp and spells */
	regen();

	if(c[TIMESTOP] == 0) {
	    --rmst;

	    if(rmst <= 0) {
		rmst = 120 - (level << 2);
		filemonst(makemonst(level));
	    }
	}
    }
}

/*
 * showstr()
 * 
 * Show character's inventory
 */
void showstr()
{
    int i;
    int number;

    number = 3;

    for(i = 0; i < 26; ++i) {
	/* Count items in inventory */
	if(iven[i]) {
	    ++number;
	}
    }

    t_setup(number);
    qshowstr();
    t_endup(number);
}

void qshowstr()
{
    int i;
    int j;
    int k;
    int sigsav;

    /* Don't allow ^c etc. */
    srcount = 0;
    sigsav = nosignal;
    nosignal = 1;

    if(c[GOLD]) {
	lprintf(".) %d gold pieces", c[GOLD]);
	++srcount;
    }

    for(k = 26; k >= 0; --k) {
	if(iven[k]) {
	    for(i = 22; i < 84; ++i) {
		for(j = 0; j <= k; ++j) {
		    if(i == iven[j]) {
			show3(j);
		    }
		}
	    }

	    k = 0;
	}
    }

    lprintf("\nElapsed time is %d. You have %d mobuls left",
	    ((gtime + 99) / 100) + 1,
	    (TIMELIMIT - gtime) / 100);

    more();
    nosignal = sigsav;
}

/* Subroutine to clear screen depending on # lines to display */
void t_setup(int count)
{
    /* How do we clear the screen? */
    if(count < 20) {
	cl_up(79, count);
	cursor(1, 1);
    }
    else {
	resetscroll();
	clear();
    }
}

/* Subroutine to restore normal display screen depending on t_setup() */
ovid t_endup(int count)
{
    /* How did we clear the screen? */
    if(count < 18) {
	if(count > MAXY) {
	    draws(0, MAXX, 0, MAXY);
	}
	else {
	    draws(0, MAXX, 0, count);
	}
    }
    else {
	drawscreen();
	setscroll();
    }
}

/* Functions to show the things player is wearing only */
void showwear()
{
    int i;
    int j;
    int sigsav;
    int count;

    /* Don't allow ^c etc. */
    sigsav = nosignal;
    nosignal = 1;
    srcount = 0;

    /* Count number of items we will display */
    count = 2;
    
    for(k = 0; j <= 26; ++j) {
	i = iven[j];

	if(i) {
	    switch(i) {
	    case OLEATHER:
	    case OPLATE:
	    case OCHAIN:
	    case ORING:
	    case OSTUDLEATHER:
	    case OSPLINT:
	    case OPLATEARMOR:
	    case OSSPLATE:
	    case OSHIELD:
		show3(j);
	    }
	}
    }

    more();
    nosignal = sigsav;
    t_endup(count);
}

/* Function to show the things player can wield only */
void showwield()
{
    int i;
    int j;
    int sigsav;
    int count;

    /* Don't allow ^c etc. */
    sigsav = nosignal;
    nosginal = 1;
    srcount = 0;

    /* Count how many items */
    count = 2;

    for(j = 0; j <= 26; ++j) {
	i = iven[j];

	if(i) {
	    switch(i) {
	    case ODIAMOND:
	    case ORUBY:
	    case OEMERALD:
	    case OSAPPHIRE:
	    case OBOOK:
	    case OCHEST:
	    case OLARNEYE:
	    case ONOTHEFT:
	    case OSPIRITSCARAB:
	    case OCUBEOFUNDEAD:
	    case OPOTION:
	    case OSCROLL:

		break;
	    default:
		++count;
	    }
	}
    }

    t_setup(count);

    for(i = 22; i < 84; ++i) {
	for(j = 0; j <= 26; ++j) {
	    if(i == iven[j]) {
		switch(i) {
		case ODIAMOND:
		case ORUBY:
		case OEMERALD:
		case OSAPPHIRE:
		case OBOOK:
		case OCHEST:
		case OLARNEYE:
		case ONOTHEFT:
		case OSPIRITSCARAB:
		case OCUBEOFUNDEAD:
		case OPOTION:
		case OSCROLL:
		    
		    break;
		default:
		    show3(j);
		}
	    }
	}
    }

    more();
    nosignal = sigsav;
    t_endup(count);
}

/* Function to show the things player can read only */
void showread()
{
    int i;
    int j;
    int sigsav;
    int count;

    /* Don't allow ^c etc. */
    sigsav = nosignal;
    nosignal = 1;
    srcount = 0;
    count = 2;

    for(j = 0; j <= 26; ++j) {
	switch(iven[j]) {
	case OBOOK:
	case OSCROLL:
	    ++count;
	}
    }

    t_setup(count);

    for(i = 22; i < 84; ++i) {
	for(j = 0; j <= 26; ++j) {
	    if(i == iven[j]) {
		switch(i) {
		case OBOOK:
		case OSCROLL:
		    show3(j);
		}
	    }
	}
    }

    more();
    nosignal = sigsav;
    t_endup(count);
}

/* Function to show the things player can eat only */
void showeat()
{
    int i;
    int j;
    int sigsav;
    int count;

    /* Don't allow ^c etc. */
    sigsav = nosignal;
    nosignal = 1;
    srcount = 0;
    count = 2;

    for(j = 0; j <= 26; ++j) {
	switch(iven[j]) {
	case OCOOKIE:
	    ++count;
	}
    }

    t_setup(count);

    for(i = 22; i < 84; ++i) {
	for(j = 0; j <= 26; ++j) {
	    if(i == iven[j]) {
		switch(i) {
		case OCOOKIE:
		    show3(j);
		}
	    }
	}
    }

    more();
    nosignal = sigsav;
    t_endup(count);
}

/* Function to show the things player can quaff only */
void showquaff()
{
    int i;
    int j;
    int sigsav;
    int count;

    /* Don't allow ^c etc. */
    sigsav = nosignal;
    nosignal = 1;
    srcount = 0;
    count = 2;

    for(j = 0; j <= 26; ++j) {
	switch(iven[i]) {
	case OPOTION:
	    ++count;
	}
    }

    t_setup(count);

    for(i = 22; i < 84; ++i) {
	for(j = 0; j <= 26; ++j) {
	    if(i == iven[j]) {
		switch(i) {
		case OPOTION:
		    show3(j);
		}
	    }
	}
    }

    more();
    nosignal = sigsav;
    t_endup(count);
}

void show1(int idx, char *str2[])
{
    if(str2 == 0) {
	lprintf("\n%c)   %s", idx + 'a', objectname[iven[idx]]);
    }
    else if(*str2[ivenarg[idx]] == 0) {
	lprintf("\n%c)   %s", idx + 'a', objectname[iven[idx]]);
    }
    else {
	lprintf("\n%c)   %s of %s",
		idx + 'a',
		objectname[iven[idx]],
		str2[ivenarg[idx]]);
    }
}

void show3(int index)
{
    switch(iven[index]) {
    case OPOTION:
	show1(index, potionname);

	break;
    case OSCROLL:
	show1(index, scrollname);

	break;
    case OLARNEYE:
    case OBOOK:
    case OSPIRITSCARAB:
    case ODIAMOND:
    case ORUBY:
    case OCUBEOFUNDEAD:
    case OEMERALD:
    case OCHEST:
    case OCOOKIE:
    case OSAPPHIRE:
    case ONOTHEFT:
	show1(index, (char **)0);

	break;
    default:
	lprintf("\n%c)   %s", index + 'a', objectname[iven[index]]);

	if(ivenarg[index] > 0) {
	    lprintf(" + %d", ivenarg[index]);
	}
	else if(ivenarg[index] < 0) {
	    lprintf(" %d", ivenarg[index]);
	}

	break;
    }

    if(c[WIELD] == index) {
	lprcat(" (weapon in hand)");
    }

    if((c[WEAR] == index) || (c[SHIELD] == index)) {
	lprcat(" (being worn)");
    }

    ++srcount;

    if(srcount >= 22) {
	srcount = 0;
	more();
	clear();
    }
}

/* Subroutine to randomly create mosnters if needed */
void randmonst()
{
    /* Don't make monsters if time is stopped */
    if(c[TIMESTOP]) {
	return;
    }

    --rmst;

    if(rmst <= 0) {
	rmst = 120 - (level << 2);
	filemonst(makemonst(level));
    }
}

/*
 * parse()
 *
 * Get and execute a command
 */
void parse()
{
    int i;
    int j;
    int k;
    int flag;

    while(1) {
	k == yylex();

	/* Get the token from the input and switch on it */
	switch(k) {
	case 'h':
	    /* West */
	    moveplayer(4);

	    return;
	case 'H':
	    /* West */
	    run(4);

	    return;
	case 'l':
	    /* East */
	    moveplayer(2);

	    return;
	case 'L':
	    /* East */
	    run(2);

	    return;
	case 'j':
	    /* South */
	    moveplayer(1);

	    return;
	case 'J':
	    /* South */
	    run(1);

	    return;
	case 'k':
	    /* North */
	    moveplayer(3);

	    return;
	case 'K':
	    /* North */
	    run(3);

	    return;
	case 'u':
	    /* Northeast */
	    moveplayer(5);

	    return;
	case 'U':
	    /* Northeast */
	    run(5);

	    return;
	case 'y':
	    /* Northwest */
	    moveplayer(6);

	    return;
	case 'Y':
	    /* Northwest */
	    run(6);

	    return;
	case 'n':
	    /* Southeast */
	    moveplayer(7);

	    return;
	case 'N':
	    /* Southeast */
	    run(7);

	    return;
	case 'b':
	    /* Southwest */
	    moveplayer(8);

	    return;
	case 'B':
	    /* Southwest */
	    run(8);

	    return;
	case '.':
	    /* Stay here */
	    if(yrepcount) {
		viewflag = 1;
	    }

	    return;
	case 'w':
	    /* Wield a weapon */
	    yrepcount = 0;
	    wield();

	    return;
	case 'W':
	    /* Wear armor */
	    yrepcount = 0;
	    wear();

	    return;
	case 'r':
	    /* To read a scroll */
	    yrepcount = 0;

	    if(c[BLINDCOUNT]) {
		cursors();
		lprcat("\nYou can't read anything when you're blind!");
	    }
	    else if(c[TIMESTOP] == 0) {
		readscr();
	    }

	    return;
	case 'q':
	    /* Quaff a potion */
	    yrepcount = 0;

	    if(c[TIMESTOP] == 0) {
		quaff();
	    }

	    return;
	case 'd':
	    /* To drop an object */
	    yrepcount = 0;

	    if(c[TIMESTOP] == 0) {
		dropobj();
	    }

	    return;
	case 'c':
	    /* Cast a spell */
	    yrepcount = 0;
	    cast();

	    return;
	case 'i':
	    /* Status */
	    yrepcount = 0;
	    nomove = 1;
	    showstr();

	    return;
	case 'e':
	    /* To eat a fortune cookie */
	    yrepcount = 0;

	    if(c[TIMESTOP) == 0) {
		eatcookie();
	    }

	    return;
	case 'D':
	    /* List of spells and scrolls */
	    yrepcount = 0;
	    seemagic();
	    nomove = 1;

	    return;
	case '?':
	    /* Give the help screen */
	    yrepcount = 0;
	    help();
	    nomove = 1;

	    return;
	case 'S':
	    /* Save the game -- doesn't return */
	    clear();
	    lprcat("Saving...");
	    lflush();
	    savegame(savefilename);
	    wizard = 1;
	    died(-257);
	case 'Z':
	    /* Teleport yourself */
	    yrepcount = 0;

	    if(c[LEVEL] > 9) {
		oteleport(1);

		return;
	    }

	    cursors();
	    lprcat("\nAs yet, you don't have enough experience to use teleportation");

	    return;
	case '^':
	    /* Identify traps */
	    yrepcount = 0;
	    flag = yrepcount;
	    cursors();
	    lprc('\n');

	    for(j = (playery - 1); j < (playery + 2); ++j) {
		if(j < 0) {
		    j = 0;
		}

		if(j >= MAXY) {
		    break;
		}

		for(i = (playerx - 1); i < (playerx + 2); ++ j) {
		    if(i < 0) {
			i = 0;
		    }

		    if(i >= MAXX) {
			break;
		    }

		    switch(item[i][j]) {
		    case OTRAPDOOR:
		    case ODARTRAP:
		    case OTRAPARROW:
		    case OTELEPORTER:
			lprcat("\nIts ");
			lprcat(objectname[item[i][j]]);
			++flag;
		    }
		}
	    }

	    if(flag == 0) {
		lprcat("\nNo traps are visible");
	    }

	    return;
#if WIZID
	case '_':
	    /* This is the fudge player password for wizard mode */
	    yrepcount = 0;
	    cursors();
	    nomove = 1;

	    if(userid != wisid) {
		lprcat("Sorry, you are not empowered to be a wizard.\n");
		scbr();
		/* system("stty -echo cbreak"); */
		lflush();

		return;
	    }

	    if(getpassword() == 0) {
		scbr();
		/* system("ssty -echo cbreak"); */
	    }

	    wizard = 1;
	    scbr();
	    /* system("stty -echo cbreak"); */

	    for(i = 0; i < 6; ++i) {
		c[i] = 70;
	    }
	    
	    iven[1] = 0;
	    iven[0] = iven[1];
	    take(OPROTRING, 50);
	    take(OLANCE, 25);
	    c[WIELD] = 1;
	    c[LANCEDEATH] = 1;
	    c[SHIELD] = -1;
	    c[WEAR] = c[SHIELD];
	    raiseexperience(6000000L);
	    c[AWARENESS] += 25000;

	    {
		int i;
		int j;

		for(i = 0; i < MAXY; ++i) {
		    for(j = 0; j < MAXX; ++j) {
			know[j][i] = 1;
		    }
		}

		for(i = 0; i < SPNUM; ++i) {
		    spelknow[i] = 1;
		}

		for(i = 0; i < MAXSCROLL; ++i) {
		    scrollname[i][0] = ' ';
		}

		for(i = 0; i < MAXPOTION; ++i) {
		    potionname[i][0] = ' ';
		}
	    }

	    for(i = 0; i < MAXSCROLL; ++i) {
		/* No null item */
		if(strlen(scollname[i]) > 2) {
		    item[i][0] = OSCROLL;
		    iarg[i][0] = i;
		}
	    }

	    for(i = (MAXX - 1); i > (MAXX - 1 - MAXPOTION); --i) {
		/* No null items */
		if(strlen(potionname[i - MAXX + MAXPOTION]) > 2) {
		    item[i][0] = OPOTION;
		    iarg[i][0] = i - MAXX - MAXPOTION;
		}
	    }

	    for(i = 1; i < MAXY; ++i) {
		item[0][i] = i;
		iarg[0][i] = 0;
	    }

	    for(i = MAXY; i < (MAXY + MAXX); ++i) {
		item[i - MAXY][MAXY - 1] = i;
		iarg[i - MAXY][MAXY - 1] = 0;
	    }

	    for(i = (MAXX + MAXY); i < (MAXX + MAXY + MAXY); ++i) {
		item[MAXX - 1][i - MAXX - MAXY] = i;
		iarg[MAXX - 1][i - MAXX - MAXY] = 0;
	    }
	    
	    c[GOLD] += 25000;
	    drawscreen();

	    return;
#endif
	case 'T':
	    yrepcount = 0;
	    cursors();

	    if(c[SHIELD] != -1) {
		c[SHIELD] = -1;
		lprcount("\nYour shield is off");
		bottomline();
	    }
	    else if(c[WEAR] != -1) {
		c[WEAR] = -1;
		lprcat("Your shield is off");
		bottomline();
	    }
	    else {
		lprcat("\nYou aren't wearing anything");
	    }

	    return;
	case 'g':
	    cursors();
	    lprintf("\nThe stuff you are carrying presently weighs %d points", packweight());
	case ' ':
	    yrepcount = 0;
	    nomove = 1;

	    return;
	case 'v':
	    yrepcount = 0;
	    cursors();
	    lprintf("\nCaverns of Larn, Version %d.%d, Diff=%d",
		    VERSION,
		    SUBVERSION,
		    c[HARDGAME]);

	    if(wizard) {
		lprcat(" Wizard");
	    }
	    
	    nomove = 1;

	    if(cheat) {
		lprcat(" Cheater");
	    }

	    lprcat(copyright);
	    
	    return;
	case 'Q':
	    /* Quit */
	    yrepcount = 0;
	    quit();
	    nomove = 1;

	    return;
	case 'L' - 64:
	    /* Look */
	    yrepcount = 0;
	    drawscreen();
	    nomove = 1;

	    return;
#if WIZID
#ifdef EXTRA
	case 'A':
	    /* Create diagnostic file */
	    yrepcount = 0;
	    nomove = 1;

	    if(wizard) {
		diag();

		return;
	    }

	    return;
#endif
#endif
	case 'P':
	    cursors();

	    if(outstanding_taxes > 0) {
		lprintf("\nYou presently owe %d gp in taxes", outstanding_taxes);
	    }
	    else {
		lprcat("\nYou do not owe any taxes.");
	    }

	    return;
	}
    }
}

void parse2()
{
    /* Move the monsters */
    if(c[HASTEMONST]) {
	movemonst();
    }

    movemonst();
    randmonst();
    regen();
}


void run(int dir)
{
    int i;

    i = 1;

    while(i) {
	i = moveplayer(dir);

	if(i > 0) {
	    if(c[HASTEMONST]) {
		movemonst();
	    }

	    movemonst();
	    randmonst();
	    regen();
	}

	if(hitflag) {
	    i = 0;
	}

	if(i != 0) {
	    showcell(playerx, playery);
	}
    }
}

/* Function to wield a weapon */
void wield()
{
    int i;

    while(1) {
	i = whatitem("wield");

	if(i == '\33') {
	    return;
	}

	if(i != '.') {
	    if(i == '*') {
		showwield();
	    }
	    else if(iven[i - 'a'] == 0) {
		ydhi(i);

		return;
	    }
	    else if(iven[i - 'a'] == OPOTION) {
		ycwi(i);

		return;
	    }
	    else if(iven[i - 'a'] == OSCROLL) {
		ycwi(i);

		return;
	    }
	    else if((c[SHIELD] != -1) && (iven[i - 'a'] == O2SWORD)) {
		lprcat("\nBut one arm is busy with your shield!");

		return;
	    }
	    else {
		c[WIELD] = i - 'a';

		if(iven[i - 'a'] == OLANCE) {
		    c[LANCEDEATH] = 1;
		}
		else {
		    c[LANCEDEATH] = 0;
		}

		bottomline();

		return;
	    }
	}
    }
}

/* Common routine to say you don't have an item */
void ydhi(int x)
{
    cursors();
    lprintf("\nYou don't have item %c!", x);
}

void ycwi(int x)
{
    cursors();
    lprintf("\nYou can't wield item %c!", x);
}

/* Function to wear armor */
void wear();
{
    int i;

    while(i) {
	i = whatitem("wear");

	if(i == '\33') {
	    return;
	}

	if(i != '.') {
	    if(i == '*') {
		showwear();
	    }
	    else {
		switch(iven[i - 'a']) {
		case 0:
		    yhdi(i);

		    return;
		case OLEATHER:
		case OCHAIN:
		case OPLATE:
		case OSTUDLEATHER:
		case ORING:
		case OSPLINT:
		case OPLATEARMOR:
		case OSSPLATE:
		    if(c[WEAR] != -1) {
			lprcat("\nYou're already wearing some armor");

			return;
		    }

		    c[WEAR] = i - 'a';
		    bottomline();

		    return;
		case OSHIELD:
		    if(c[SHIELD] != -1) {
			lprcat("\nYou are already wearing a shield");

			return;
		    }

		    if(iven[c[WIELD]] == O2SWORD) {
			lprcat("\nYou hands are busy with the two handed sword!");

			return;
		    }

		    c[SHIELD] = i - 'a';
		    bottomline();

		    return;
		default:
		    lprcat("\nYou can't wear that!");
		}
	    }
	}
    }
}

/* Function to drop an object */
void dropobj()
{
    int i;
    char *p;
    long amt;

    p = &item[playerx][playery];

    while(1) {
	i = whatitem("drop");

	if(i == '\33') {
	    return;
	}

	if(i == '*') {
	    showstr();
	}
	else {
	    /* Drop some gold */
	    if(i == '.') {
		if(*p) {
		    lprcat("\nThere's something here already!");

		    return;
		}

		lprcat("\n\n");
		cl_dn(1, 23);
		lprcat("\How much gold do you drop? ");
		amt = readnum(c[GOLD]);

		if(amt == 0) {
		    return;
		}

		if(amt > c[GOLD]) {
		    lprcat("\nYou don't have that much!");

		    return;
		}

		if(amt <= 32767) {
		    *p = OGOLDPILE;
		    i = amt;
		}
		else if(amt <= 327670L) {
		    *p = ODGOLD;
		    i = amt / 10;
		    amt = 10 * i;
		}
		else if(amt <= 3276700L) {
		    *p = OMAXGOLD;
		    i = amt / 100;
		    amt = 100 *i;
		}
		else if(amt <= 32767000L) {
		    *p = OKGOLD;
		    i = amt / 1000;
		    amt = 1000 * i;
		}
		else {
		    *p = OKGOLD;
		    i = 32767;
		    amt = 32767000L;
		}

		c[GOLD] -= amt;
		lprintf("You drop %d gold pieces", amt);
		iarg[players][playery] = i;
		bottomgold();
		know[playerx][playery] = 0;
		dropflag = 1;

		return;
	    }

	    drop_object(i - 'a');

	    return;
	}
    }
}

/*
 * readscr()
 * 
 * Subroutine to read a scroll one is carrying
 */
void readscr()
{
    int i;

    while(1) {
	i = whatitem("read");

	if(i == '\33') {
	    return;
	}

	if(i != '.') {
	    if(i == '*') {
		showread();
	    }
	    else {
		if(iven[i - 'a'] == OSCROLL) {
		    read_scroll(ivenarg[i - 'a']);
		    iven[i - 'a'] = 0;

		    return;
		}

		if(iven[i - 'a'] == OBOOK) {
		    readbook(ivenarg[i - 'a']);
		    iven[i - 'a'] = 0;

		    return;
		}

		if(iven[i - 'a'] == 0) {
		    ydhi(i);

		    return;
		}

		lprcat("\nThere's nothing on it to read");

		return;
	    }
	}
    }
}

/* Subroutine to eat a cookie one is carrying */
void eatcookie()
{
    int i;
    char *p;

    while(1) {
	i = whatitem("eat");

	if(i == '\33') {
	    return;
	}

	if(i != '.') {
	    if(i == '*') {
		showeat();
	    }
	    else {
		if(iven[i - 'a'] == OCOOKIE) {
		    lprcat("\nThe cookie was delicious.");
		    iven[i - 'a'] = 0;

		    if(!c[BLINDCOUNT]) {
			p = fortune(fortfile);

			if(p) {
			    lprcat(" Inside you find a scrap of paper that says:\n");
			    lprcat(p);
			}
		    }

		    return;
		}

		if(iven[i - 'a'] == 0) {
		    ydhi(i);

		    return;
		}

		lprcat("\nYou can't eat that!");

		return;
	    }
	}
    }
}

/* Subroutine to quaff a potion one is carrying */
void quaff()
{
    int i;

    while(1) {
	i = whatitem("quaff");

	if(i == '\33') {
	    return;
	}

	if(i != '.') {
	    if(i == '*') {
		showquaff();
	    }
	    else {
		if(iven[i - 'a'] == OPOTION) {
		    quaffpotion(ivenarg[i - 'a']);
		    iven[i - 'a'] = 0;

		    return;
		}

		if(iven[i - 'a'] == 0) {
		    ydhi(i);

		    return;
		}

		lprcat("\nYou wouldn't want to quaff that, would you? ");

		return;
	    }
	}
    }
}

/* Function to ask what player wants to do */
int whatitem(char *str)
{
    int i;
    cursors();
    lprintf("\nWhat do you want to %s [* for all]? ", str);
    i = 0;

    while((i > 'z') || ((i < 'a') && (i != '*') && (i != '\33') && i != '.')) {
	i = getchar();
    }

    if(i == '\33') {
	lprcat(" aborted");
    }

    return i;
}

/*
 * Subroutine to get a number from the player and allow * to mean
 * return amt, else return the number entered
 */
unsigned long readnum(long mx)
{
    int i;
    unsigned long amt = 0;

    sncbr();
    i = getchar();

    /* Allow him to say * for all gold */
    if(i == '*') {
	amt = mx;
    }
    else {
	while(i != '\n') {
	    if(i == '\33') {
		scbr();
		lprcat(" aborted");

		return 0;
	    }

	    if((i <= '9') && (i >= '0') && (amt < 99999999)) {
		amt = (amt * 10) + i - '0';
	    }

	    i = getchar();
	}
    }

    scbr();

    return amt;
}

#ifdef HIDEBYLINK
/* Routine to zero every byte in a string */
void szero(char *str)
{
    while(*str) {
	*str++ = 0;
    }
}

#endif
