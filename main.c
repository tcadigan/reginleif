// Rogue
// Exploring the dungeons of doom
// Copyright (C) 1980 by Michael Tor and Glenn Wichman
// All rights reserved
//
// @(#)main.c 3.27 (Berkeley) 6/15/81
//
// Revision History
// ================
// 22 Dec 81  DPK  Added set[ug]id code to main
//  1 Jan 82  DPK  Added code to print out rogue news on startup.
//                 If RNOTS is defined, the file is opened and printed

#define _XOPEN_SOURCE 700

#include "main.h"

#include "chase.h"
#include "command.h"
#include "daemon.h"
#include "daemons.h"
#include "init.h"
#include "io.h"
#include "list.h"
#include "machdep.h"
#include "misc.h"
#include "newlevel.h"
#include "options.h"
#include "pack.h"
#include "rip.h"
#include "save.h"
#include "weapons.h"

#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#ifdef __APPLE__
#undef LOADAV
#endif

#ifdef LOADAV
#include <bsd/nlist.h>
#endif

#ifdef UCOUNT
#include <utmpx.h>
#endif

#ifdef CHECKTIME
// Times we've gone over in checkout()
static int num_checks;
#endif

int main(int argc, char **argv, char **envp)
{
    char *env;
    struct passwd *pw;
    struct linked_list *item;
    struct object *obj;
    int lowtime;
    long now;

    /* TC_DEBUG: Start */
    FILE *output;
    output = fopen("debug.txt", "w");
    fclose(output);
    /* TC_DEBUG: Finish */
    
    // Lower priority slightly
    if(getuid() != 0) {
        nice(1);
    }

#ifdef BRL
    setgid(getgid());
    setuid(getuid());
#endif

    // Check for print-score option
    if((argc == 2) && (strcmp(argv[1], "-s") == 0)) {
	waswizard = TRUE;
	score(0, -1, NULL);
	exit(0);
    }

#ifdef RNOTES
    roguenotes();
#endif

    // Check to see if he is a wizard
    if((argc >= 2) && (argv[1][0] == '\0')) {
	FILE *input;
	input = fopen("/dev/tty", "r");

	printf("Wizard's password: ");

	struct termios terminal;
	tcgetattr(fileno(input), &terminal);
	terminal.c_lflag &= ~ECHO;
	tcsetattr(fileno(input), TCSANOW, &terminal);

	char *line = NULL;
	size_t read_length = 0;
	ssize_t line_length;

	line_length = getline(&line, &read_length, input);

	tcgetattr(fileno(input), &terminal);
	terminal.c_lflag &= ECHO;
	tcsetattr(fileno(input), TCSANOW, &terminal);

	line[line_length - 1] = '\0';

	if(strcmp(PASSWD, crypt(line, "mT")) == 0) {
	    wizard = TRUE;
	    ++argv;
	    --argc;
	}

	free(line);
    }

    // Get home and options from environment
    env = getenv("HOME");
    if(env != NULL) {
	strcpy(home, env);
    }
    else {
        pw = getpwuid(getuid());
        
        if(pw != NULL) {
            strcpy(home, pw->pw_dir);
        }
        else {
            home[0] = '\0';
        }
    }
    
    strcat(home, "/");

    strcpy(file_name, home);
    strcat(file_name, "rogue.sav");

    env = getenv("ROGUEOPTS");
    if(env != NULL) {
	parse_opts(env);
    }
    if((env == NULL) || (whoami[0] == '\0')) {
        pw = getpwuid(getuid());
	if(pw == NULL) {
	    printf("Say, who the hell are you?\n");
	    exit(1);
	}
	else {
	    strucpy(whoami, pw->pw_name, strlen(pw->pw_name));
        }
    }
    if((env == NULL) || (fruit[0] == '\0')) {
	strcpy(fruit, "slime-mold");
    }

#ifdef MAXUSERS
    if(too_much() && !wizard && !author()) {
	printf("Sorry, %s, but the system is too loaded now.\n", whoami);
	printf("Try again later.  Meanwhile, why not enjoy a%s %s?\n",
               vowelstr(fruit),
               fruit);
	exit(1);
    }
#endif
    
    if(argc == 2) {
        // Note: restore() will never return
	if(!restore(argv[1], envp)) {
	    exit(1);
        }
    }
    time(&now);
    lowtime = (int)now;

    if(wizard && (getenv("SEED") != NULL)) {
        dnum = atoi(getenv("SEED"));
    }
    else {
        dnum = lowtime + getpid();
    }

    if(wizard) {
	printf("Hello %s, welcome to dungeon #%d\n", whoami, dnum);
    }
    else {
	printf("Hello %s, just a moment while I dig the dungeon...\n", whoami);
    }
    
    fflush(stdout);
    seed = dnum;
    
    // Shake the dice
    srand(seed);
    // Roll up the rogue
    init_player();
    // Set up probabilities
    init_things();
    // Set up the names of scrolls
    init_names();
    // Set up colors of potions
    init_colors();
    // Set up stone settings of rings
    init_stones();
    // Set up materials of wands
    init_materials();

    // Start up cursor package
    initscr();
    setup();

    // Set up windows
    cw = newwin(LINES, COLS, 0, 0);
    mw = newwin(LINES, COLS, 0, 0);
    hw = newwin(LINES, COLS, 0, 0);
    waswizard = wizard;

    // Draw current level
    new_level();
    
    // Start up daemons and fuses
    start_daemon(doctor, 0, AFTER);
    fuse(swander, 0, WANDERTIME, AFTER);
    start_daemon(stomach, 0, AFTER);
    start_daemon(runners, 0, AFTER);

    // Give the rogue his weaponry, first a mace
    item = new_item(sizeof *obj);
    obj = (struct object *)item->l_data;
    obj->o_type = WEAPON;
    obj->o_which = MACE;
    init_weapon(obj, MACE);
    obj->o_hplus = 1;
    obj->o_dplus = 1;
    obj->o_flags |= ISKNOW;
    add_pack(item, TRUE);
    cur_weapon = obj;

    // Now a +1 bow
    item = new_item(sizeof *obj);
    obj = (struct object *)item->l_data;
    obj->o_type = WEAPON;
    obj->o_which = BOW;
    init_weapon(obj, BOW);
    obj->o_hplus = 1;
    obj->o_dplus = 0;
    obj->o_flags |= ISKNOW;
    add_pack(item, TRUE);

    // Now some arrows
    item = new_item(sizeof *obj);
    obj = (struct object *)item->l_data;
    obj->o_type = WEAPON;
    obj->o_which = ARROW;
    init_weapon(obj, ARROW);
    obj->o_count = 25+rnd(15);
    obj->o_hplus = obj->o_dplus = 0;
    obj->o_flags |= ISKNOW;
    add_pack(item, TRUE);

    // And his suit of armor
    item = new_item(sizeof *obj);
    obj = (struct object *)item->l_data;
    obj->o_type = ARMOR;
    obj->o_which = RING_MAIL;
    obj->o_ac = a_class[RING_MAIL] - 1;
    obj->o_flags |= ISKNOW;
    cur_armor = obj;
    add_pack(item, TRUE);

    // Give him some food too
    item = new_item(sizeof *obj);
    obj = (struct object *)item->l_data;
    obj->o_type = FOOD;
    obj->o_count = 1;
    obj->o_which = 0;
    add_pack(item, TRUE);

    playit();

    return 0;
}

// endit:
//     Exit the program abnormally
void endit(int paramter)
{
    fatal("Ok, if you want to exit that badly, I'll have to allow it\n");
}

// fatal:
//     Exit the program, printing a message
int fatal(char *s)
{
    clear();
    move(LINES - 2, 0);
    printw("%s", s);
    wrefresh(stdscr);
    endwin();
    exit(0);
}

// rnd:
//     Pick a very random number
int rnd(int range)
{
    if(range == 0) {
        return 0;
    }
    else {
        return (abs(rand()) % range);
    }
}

// roll:
//     Roll a number of dice
int roll(int number, int sides)
{
    int dtotal = 0;

    while(number--) {
	dtotal += (rnd(sides) + 1);
    }
    
    return dtotal;
}

#ifdef SIGTSTP
// tstp:
//     Handle stop and start signals
void tstp(int parameter)
{
    mvcur(0, COLS - 1, LINES - 1, 0);
    endwin();
    fflush(stdout);
    kill(0, SIGTSTP);
    signal(SIGTSTP, tstp);
    crmode();
    noecho();
    clearok(curscr, TRUE);
    touchwin(cw);
    wrefresh(cw);
    // Flush input
    raw();
    noraw();
}
#endif

// setup:
//     Something...
int setup()
{
#ifndef DUMP
    signal(SIGHUP, auto_save);
    signal(SIGILL, auto_save);
    signal(SIGTRAP, auto_save);
    signal(SIGABRT, auto_save);
    signal(SIGFPE, auto_save);
    signal(SIGBUS, auto_save);
    signal(SIGSEGV, auto_save);
    signal(SIGSYS, auto_save);
    signal(SIGPIPE, auto_save);
    signal(SIGTERM, auto_save);
#endif
    signal(SIGINT, quit);
#ifndef DUMP
    signal(SIGQUIT, endit);
#endif
#ifdef SIGTSTP
    signal(SIGTSTP, tstp);
#endif
#ifdef CHECKTIME
    if(!author()) {
	signal(SIGALRM, checkout);
	alarm(CHECKTIME * 60);
	num_checks = 0;
    }
#endif
    
    // cbreak mode
    crmode();
    // Echo off
    noecho();

    return 0;
}

// playit:
//     The main loop of the program. Loop until the game is over,
//     refreshing things and looking at the proper times
int playit()
{
    char *opts;

    // Parse environment declaration of options
    opts = getenv("ROGUEOPTS");
    if(opts != NULL) {
	parse_opts(opts);
    }

    oldpos = player.t_pos;
    oldrp = roomin(&player.t_pos);
    while(playing) {
        // Command execution
	command();
    }
    
    endit(0);

    return 0;
}

#ifdef MAXUSERS
// too_much:
//     See if the system is being used too much for this game
int too_much()
{
#ifdef MAXLOAD
    double avec[3];
    loadav(avec);
    return (avec[2] > (MAXLOAD / 10.0));
#else
    return (ucount() > MAXUSERS);
#endif
}

// author:
//     See if a user is an author of the program
int author()
{
    switch(getuid()) {
	case 0:
	    return TRUE;
	default:
	    return FALSE;
    }
}
#endif

#ifdef CHECKTIME
// checkout:
//     Something...
void checkout(int parameter)
{
    static char *msgs[] = {
	"The load is too high to be playing.  Please leave in %d minutes",
	"Please save your game.  You have %d minutes",
	"Last warning.  You have %d minutes to leave"
    };
    int checktime;

    signal(SIGALRM, checkout);
    if(too_much()) {
	if (num_checks == 3) {
	    fatal("Sorry. You took to long. You are dead\n");
        }
	checktime = CHECKTIME / (num_checks + 1);
        int args[] = { checktime };
	chmsg(msgs[num_checks++], args);
	alarm(checktime * 60);
    }
    else {
	if(num_checks) {
	    chmsg("The load has dropped back down.  You have a reprieve.", NULL);
	    num_checks = 0;
	}
	alarm(CHECKTIME * 60);
    }
}

// chmsg:
//     checkout()'s version of msg(). If we are in the middle of a shell,
//     do a printf instead of a msg() to avoid the refresh
int chmsg(char *fmt, void *args)
{
    if(in_shell) {
	printf(fmt, args);
	putchar('\n');
	fflush(stdout);
    }
    else {
	msg(fmt, args);
    }

    return 0;
}
#endif

#ifdef LOADAV
struct nlist avenrun = {
    {"_avenrun"}
};

// ladav:
//     Something...
int loadav(double *avg)
{
    int kmem;

    kmem = open("/dev/kmem", 0);

    if(kmem < 0) {
        avg[2] = 0.0;
        avg[1] = 0.0;
        avg[0] = 0.0;

        return 0;
    }
    
    nlist(NAMELIST, &avenrun);
    
    if(avenrun.n_type == 0) {
        avg[2] = 0.0;
        avg[1] = 0.0;
        avg[0] = 0.0;
        
	return 0;
    }
    
    lseek(kmem, (long) avenrun.n_value, 0);
    read(kmem, avg, 3 * sizeof (double));

    return 0;
}
#else
int loadav(double *avg)
{
    return 0;
}
#endif

#ifdef UCOUNT
struct utmpx buf;

// ucount:
//     Something...
int ucount()
{
    struct utmpx *up;
    FILE *utmp;
    int count;

    utmp = fopen(UTMP, "r");
    if(utmp == NULL) {
	return 0;
    }

    up = &buf;
    count = 0;

    while(fread(up, 1, sizeof (*up), utmp) > 0) {
	if(buf.ut_user[0] != '\0') {
	    ++count;
        }
    }
    
    fclose(utmp);
    return count;
}
#endif

#ifdef RNOTES
// roguenotes:
//     Something...
int roguenotes()
{
     FILE *notef;
     char buf[512];
     short nread;

     notef = fopen(RNOTES, "R");
     if(notef != NULL) {
         nread = fread(buf, 1, sizeof(buf), notef);
         while(nread != 0) {
             fwrite(buf, 1, nread, stdout);
             nread = fread(buf, 1, sizeof(buf), notef);
         }
         fclose(notef);
         printf("\n[Press return to continue]");
         fflush(stdout);
         wait_for('\n');
     }

     return 0;
}
#endif
