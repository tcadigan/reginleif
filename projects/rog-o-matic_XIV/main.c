/*
 * main.c: Rog-O-Matic XIV (CMU) Sun Jul 6 00:22:00 1986 -mlm
 */

/*
 *=========================================================================
 * Rog-O-Matic XIV
 * Automatically exploring the dungeons of doom
 * Copyright (C) 1985 by Appel, Jacobson, Hamey, and Mauldin
 *
 * The right is granted to any person, univeristy, or company
 * to copy, modify, or distribute (for free) these files,
 * provided that any person receiving a copy notifies Michael Mauldin
 *
 * (1) By electronic mail to Mauldin@CMU-CS-A.ARPA
 * or
 * (2) By US mail to:
 *    Michael Mauldin
 *    Dept. of Computer Science
 *    Carnegie-Mellon University
 *    Pittsburgh, PA 15213
 *
 * All other rights, including those of publication and sale, are reserved.
 *=========================================================================
 */

/*
 *************************************************************************
 * EDITLOG
 *  LastEditDate = Sun Jul 6 00:22:00 1986 - Michael Mauldin
 *  LastFileName = /usre3/mlm/src/rog/ver14/main.c
 *
 * History:
 *     I.    Andrew Appel and Guy Jacobson, 10/81 [created]
 *     II.   Andrew Appel and Guy Jacobson, 1/82  [added search]
 *     III.  Michael Mauldin, 3/82                [added termcap]
 *     IV.   Michael Mauldin, 3/82                [searching]
 *     V.    Michael Mauldin, 4/82                [cheat mode]
 *     VI.   Michael Mauldin, 4/82                [object database]
 *     VII.  All three, 5/82                      [running away]
 *     VIII. Michael Mauldin, 9/82                [improved cheating]
 *     IX.   Michael Mauldin, 10/82               [replaced termcap]
 *     X.    Mauldin, Hamey, 11/82                [Fixes, Rogue 5.2]
 *     XI.   Mauldin, 11/82                       [Fixes, Score lock]
 *     XII.  Hamey, Mauldin, 6/83                 [Fixes, New Replay]
 *     XIII. Mauldin, Hamey, 11/83                [Fixes, Rogue 5.3]
 *     XIV.  Mauldin, 1/85                        [Fixes, UT mods]
 *
 * General:
 *
 * This is the main routine for the player process, which decodes the
 * Rogue output and sends commands back. This process is execl'd by the
 * rogomatic process (cf. setup.c) which also execl's the Rogue process,
 * conveniently conneting the two via two pipes.
 *
 * Source Files:
 *
 *     arms.c        Armor, weapon, and ring handling functions
 *     command.c     Effector interface, sends commands to Rogue
 *     database.c    Memeory for objects "discovered"
 *     debug.c       Contains the debugging functions
 *     explore.c     Path searching functions, exploration
 *     findscore.c   Reads Rogue scoreboard
 *     io.c          I/O functions, sensory interface
 *     main.c        Main program for 'player' (this file)
 *     mess.c        Handles messages from Rogue
 *     monsters.c    Monster handling utilities
 *     mover.c       Creates command string to accomplish moves
 *     rooms.c       Room specific functions, new levels
 *     scorefile.c   Score file handling utilities
 *     search.c      Does shortest path
 *     setup.c       Main program for 'rogomatic'
 *     strategy.c    Makes high level decisions
 *     survival.c    Find cycles and places to run to
 *     tactics.c     Medium level intelligence
 *     things.c      Builds commands, part of Effector interface
 *     titlepage.c   Prints the animated copyright notice
 *     utility.c     Miscellaneous Unix (tm) functions
 *     worth.c       Evaluates the items in the pack
 *
 * Include Files:
 *
 *     globals.h     External definitions for all global variables
 *     install.h     Machine dependent DEFINES
 *     termtokens.h  Defines various tokens to/from Rogue
 *     types.h       Global DEFINES, macros, and typedefs.
 *
 * Other files which may be included with your distribution include:
 *     rplot         A shell script, prints a scatter plot of Rog's scores
 *     rgmplot.c     A program used by rplot
 *     datesub.l     A program used by rplot
 *     histplot.c    A program which plots a histogram of Rgm's scores
 *
 * Acknowledgements:
 *
 *  The UTexas modifictaions include in this distribution
 *  came from Dan Reynolds, and are included by permission.
 *  Rog-O-Matic's first total winner against 5.3 was
 *  on a UTexas computer.
 *************************************************************************
 */
#include "main.h"

#include <ctype.h>
#include <curses.h>
#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "arms.h"
#include "command.h"
#include "database.h"
#include "debug.h"
#include "explore.h"
#include "globals.h"
#include "install.h"
#include "io.h"
#include "learn.h"
#include "ltm.h"
#include "mess.h"
#include "monsters.h"
#include "pack.h"
#include "replay.h"
#include "rooms.h"
#include "search.h"
#include "strategy.h"
#include "survival.h"
#include "termtokens.h"
#include "things.h"
#include "types.h"
#include "utility.h"

/* Global data - see globals.h for current definitions */

/* Files */
FILE *fecho = NULL;      /* Game record file 'echo' option */
FILE *frogue = NULL;     /* Pipe from Rogue process */
FILE *logfile = NULL;    /* File for score log */
FILE *realstdout = NULL; /* Real stdout for Emacs, terse modes */
FILE *snapshot = NULL;   /* File for snapshot command */
FILE *trogue = NULL;     /* Pipe to Rogue process */

/* Characters */
char logfilename[100]; /* Name of log file */
char afterid = '\0';   /* Letter of obj after identify */
char genelock[100];    /* Gene pool lock file */
char genelog[100];     /* Genetic learning log file */
char genepool[100];    /* Gene pool */
char *genocide;        /* List of monster to be genocided */
char genocided[100];   /* List of monsters genocided */
char lastcmd[64];      /* copy of las command sent to Rogue */
char lastname[64];     /* Name of last postion/scroll/wand */
char nextid = '\0';    /* Next object to identify */
char screen[24][80];   /* Map of current Rogue screen */
char sumline[128];     /* Termination message for Rogomatic */
char ourkiller[64];    /* How we died */
char versionstr[32];   /* Version of Rogue being used */
char *parmstr;         /* Point to process arguments */

/* Integers */
int aggravated = 0;       /* True if we have aggravated the level */
int agoalc = NONE;        /* Goal square to arch from (col) */
int agoalr = NONE;        /* Goal square to arch from (row) */
int arglen = 0;           /* Length in bytes of argument space */
int ammo = 0;             /* How many missiles? */
int arrowshot = 0;        /* True if an arrow show us last turn */
int atrow;                /* Current position of the Rogue (@) */
int atcol;                /* Current position of the Rogue (@) */
int atrow0;               /* Position at start of turn */
int atcol0;               /* Position at start of turn */
int attempt = 0;          /* Number of times we searched whole level */
int badarrow = 0;         /* True if cursed/lousy arrow in hand */
int beingheld = 0;        /* True if a fungus has ahold of us */
int beingstalked = 0;     /* True if recent hit by invisible stalker */
int blinded = 0;          /* True if blinded */
int blindir = 0;          /* Last direction we moved when blind */
int cancelled = 0;        /* True ==> recently zapped with cancel */
int cecho = 0;            /* Last kind of message to echo file */
int cheat = 0;            /* True ==> cheat, use bugs, etc. */
int checkrange = 0;       /* True ==> check range */
int chicken = 0;          /*  True ==> check run away code */
int compression = 0;      /* True ==> move more than one square/turn */
int confused = 0;         /* True if we are confused */
int cosmic = 0;           /* True if we are hallucinating */
int currentarmor = NONE;  /* Index of our armor */
int currentweapon = NONE; /* Index of our weapon */
int cursedarmor = 0;      /* True if our armor is cursed */
int cursedweapon = 0;     /* True if we are wielding cursed weapon */
int darkdir = NONE;       /* Direction of monster being arched */
int darkturns = 0;        /* Distance to monster being arched */
int debugging = D_NORMAL; /* Debugging options in effect */
int didreadmap = 0;       /* Last level we read a map on */
int doorlist[40];         /* List of doors on this level */
int doublehasted = 0;     /* True if we double hasted (Rogue 3.6) */
int droppedscare = 0;     /* True if we dropped 'scare' on this level */
int emacs = 0;            /* True ==> format output for Emacs */
int exploredlevel = 0;    /* We completely explored this level */
int floating = 0;         /* True if we are levitating */
int foughtmonster = 0;    /* True if recently fought a monster */
int foundarrowtrap = 0;   /* Found arrow trap this level */
int foundtrapdoor = 0;    /* Found trap door this level */
int goalc = NONE;         /* Current goal square (col) */
int goalr = NONE;         /* Current goal square (row) */
int goodarrow = 0;        /* True if good (magic) arrow in hand */
int goodweapon = 0;       /* True if weapon in hand >= 100 */
int gplusdam = 1;         /* Our plus damage from strength */
int gplushit = 0;         /* Our plus to hit from strength */
int hasted = 0;           /* True if hasted */
int hitstokill = 0;       /* Number of times we hit last monster killed */
int interrupted = 0;      /* True if at commandtop from onintr() */
int knowident = 0;        /* Found an identify scroll? */
int larder = 1;           /* How much food? */
int lastate = 0;          /* Time we last ate */
int lastdamage = 0;       /* Amount of last hit by a monster */
int lastdrop = NONE;      /* Last object we tried to drop */
int lastfoodlevel = 1;    /* Last level we found food */
int lastmonster = NONE;   /* Last mosnter we tried to hit */
int lastobj = NONE;       /* What we did last use */
int lastwand = NONE;      /* Index of last wand */
int leftring = NONE;      /* Index of our left ring */
int logdigested = 0;      /* True if log file has been read by replay */
int logging = 0;          /* True if keeping record of game */
int lyinginwait = 0;      /* True if we waited for a monster */
int maxobj = 22;          /* How much we can carry */
int missedstairs = 0;     /* True if we search everywhere */
int morecount = 0;        /* Number of messages since last command */
int msgonscreen = 0;      /* Set implies message at top */
int newarmor = 1;         /* Change in armor status? */
int *newdoors = NULL;     /* New doors on screen */
int newring = 1;          /* Change in ring status? */
int newweapon = 1;        /* Change in weapon status? */
int nohalf = 0;           /* True ==> no halftime show */
int noterm = 0;           /* True ==> no user watching */
int objcount = 0;         /* Number of objects */
int ourscore = 0;         /* Final score when killed */
int playing = 1;          /* True if still playing game */
int poorarrow = 0;        /* True if arrow as missed */
int protected = 0;        /* True if we protected our armor */
int putonseeinv = 0;      /* Turn when last put on see invisible ring */
int quitat = BOGUS;       /* Score to beat, quit if within 10% more */
int redhands = 0;         /* True if we have red hands */
int replaying = 0;        /* True if replaying old game */
int revvideo = 0;         /* True if in rev. video mode */
int rightring = NONE;     /* Index of our right ring */
int rogpid = 0;           /* Pid of rogue process */
int room[9];              /* Flags for each room */
int row;                  /* Current cursor position */
int col;                  /* Current cursor position */
int scrmap[24][80];       /* Flags bits for level map */
int singlestep = 0;       /* True ==> go one turn */
int slowed = 0;           /* True ==> recent zapped with slow monster */
int stairrow;             /* Position of stairs on this level */
int staircol;             /* Position of stairs on this level */
int startecho = 0;        /* True ==> turn on echoing on startup */
int teleported = 0;       /* Number of times teleported this level */
int terse = 0;            /* True ==> terse mode */
int transparent = 0;       /* True ==> user command mode */
int trapc = NONE;         /* Location of arrow trap, this level (col) */
int trapr = NONE;         /* Location of arrow trap, this level (row) */
int urocnt = 0;           /* Un-identified Rogue Object count */
int usesynch = 0;         /* Set when the inventory is correct */
int usingarrow = 0;       /* True ==> wielding an arrow from a trap */
int version;              /* Rogue version, integer */
int wplusdam = 2;         /* Our plus damage from weapon bonus */
int wplushit = 1;         /* Our plus hit from weapon bonus */
int zone = NONE;          /* Current screen zone, 0..8 */
int zonemap[9][9];        /* Map of zone's connections */

/* Functions */
void (*istat)();

/* Stuff list, list of objects on this level */
stuffrec slist[MAXSTUFF];
int slistlen = 0;

/* Monster list, list of monsters on this level */
monrec mlist[MAXMONST];
int mlistlen = 0;

char targetmonster = '@'; /* Monster we are arching at */

/* Monster attribute and Long term memory arrays */
attrec monatt[26];      /* Monster attributes */
lrnrec ltm;             /* Long term memory -- general */
ltmrec monhist[MAXMON]; /* Long term memory -- creatures */
int nextmon = 0;        /* Length of LTM */
int monindex[27];       /* Index into monhist array */

/* Genetic learning parameters (and defaults) */
int geneid = 0;   /* Id of genotype */
int genebest = 0; /* Best score of genotype */
int geneavg = 0;  /* Average score of genotype */
int k_srch = 50;  /* Propensity for searching for traps */
int k_door = 50;  /* Propensity for searching for doors */
int k_rest = 50;  /* Propensity for resting */
int k_arch = 50;  /* Propensity for firing arrows */
int k_exper = 50; /* Level * 10 on which to experiment with items */
int k_run = 50;   /* Propensity for retreating */
int k_wake = 50;  /* Propensity for waking things up */
int k_food = 50;  /* Propensity for hoarding food (affects rings) */
int knob[MAXKNOB] = { 50, 50, 50, 50, 50, 50, 50, 50 };
char *knob_name[MAXKNOB] = {
    "trap searching:   ",
    "door searching:   ",
    "resting:          ",
    "using arrows:     ",
    "experimenting:    ",
    "retreating:       ",
    "waking monsters:  ",
    "hoarding food:    "
};

/* Door search map */
int timessearched[24][80];
int timestosearch;
int searchstartr = NONE;
int searchstartc = NONE;
int reusepsd = 0;
int new_mark = 1;
int new_findroom = 1;
int new_search = 1;
int new_stairs = 1;
int new_arch = 1;

/* Results of last call to makemove() */
int ontarget = 0;
int targetrow = NONE;
int targetcol = NONE;

/* Rog-O-Matic's model of his stats */
int Level = 0;
int MaxLevel = 0;
int Gold = 0;
int Hp = 12;
int Hpmax = 12;
int Str = 16;
int Strmax = 16;
int Ac = 6;
int Exp = 0;
int Explev = 1;
int turns = 0;

/* The message about his state of hunger */
char Ms[30];

/* Miscellaneous movement tables */
int deltrc[8] = { 1, -79, -80, -81, -1, 79, 10, 81 };
int deltc[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
int deltr[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
char keydir[8] = { 'l', 'u', 'k', 'y', 'h', 'b', 'j', 'n' };
int movedir;

/* Map characters on screen into object types */
stuff translate[128] = {
    none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj,   /* \00x */
    none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj,   /* \01x */
    none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj,   /* \02x */
    none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj,   /* \03x */
    none_obj, potion_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, /* \04x */
    hitter_obj, hitter_obj, gold_obj, none_obj, amulet_obj, none_obj, none_obj, wand_obj, /* \05x */
    none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj,   /* \06x */
    none_obj, none_obj, food_obj, none_obj, none_obj, ring_obj, none_obj, scroll_obj, /* \07x */
    none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj,   /* \10x */
    none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj,   /* \11x */
    none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj,   /* \12x */
    none_obj, none_obj, none_obj, armor_obj, none_obj, armor_obj, none_obj, none_obj, /* \13x */
    none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj,   /* \14x */
    none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj,   /* \15x */
    none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj,   /* \16x */
    none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj, none_obj    /* \17x */
};

/* Inventory, contents of our pack */
invrec inven[MAXINV];
int invcount = 0;

/* Time history */
timerec timespent[50];

/* End of game messages */
char *termination = "perditus";
char *gamename = "Rog-O-Matic";
char *roguename = "Rog-O-Matic                             ";

/* Used by onintr() to restart Rgm at top of command loop */
jmp_buf commandtop;

/*
 * Main program
 */
int main(int argc, char *argv[])
{
    char ch;
    char *s;
    char msg[128];
    int startingup = 1;
    int i;


    /* Initialize some storage */
    sprintf(lastcmd, "i");
    sprintf(ourkiller, "unknown");

    i = 80 * 24;

    while(i) {
        --i;
        screen[0][i] = ' ';
    }

    /*
     * The first argument to player is a two character string encoding
     * the file file descriptors of the pipe ends. See setup.c for call.
     *
     * If we get 'ZZ', then we are replaying an old game, and there are
     * no pipes to read/write.
     */
    if(argv[1][0] == 'Z') {
        replaying = 1;
        gamename = "Iteratum Rog-O-Maticus";
        termination = "finis";
        strcpy(logfilename, argv[4]);
        startreplay(&logfile, logfilename);
    }
    else {
        frogue = fdopen(argv[1][0] - 'a', "r");
        trogue = fdopen(argv[1][1] - 'a', "w");
        setbuf(trogue, (char *)NULL);
    }

    /* The second argument to player is the process id of Rogue */
    if(argc > 2) {
        rogpid = atoi(argv[2]);
    }

    /* The third argument is an option list */
    if(argc > 3) {
        sscanf(argv[3],
               "%d,%d,%d,%d,%d,%d,%d,%d",
               &cheat,
               &noterm,
               &startecho,
               &nohalf,
               &emacs,
               &terse,
               &transparent,
               &quitat);
    }

    /* The fourth argument is the Rogue name */
    if(argc > 4) {
        strcpy(roguename, argv[4]);
    }
    else {
        sprintf(roguename, "Rog-O-Matic %s", RGMVER);
    }

    /* Now count argument space and assign a global pointer to it */
    arglen = 0;
    for(i = 0; i < argc; ++i) {
        int len = strlen(argv[i]);
        arglen += (len + 1);

        while(len >= 0) {
            argv[i][len--] = ' ';
        }
    }

    parmstr = argv[0];
    --arglen;

    /* If we are in one-line mode, then squirrel away stdout */
    if(emacs || terse) {
        realstdout = fdopen(dup(fileno(stdout)), "w");
        freopen("/dev/null", "w", stdout);
    }

    /* Initialize the Curses package */
    initscr();
    crmode();
    noecho();

    /* Start logging? */
    if(startecho) {
        toggleecho();
    }

    /* Clear the screen */
    clear();

    /* Figure out Rogue version */
    getrogver();

    if(!replaying) {
        /* Get long term memory of version */
        restoreltm();

        /* Start genetic learning */
        startlesson();
    }

    /* Give a hello message */
    if(replaying) {
        sprintf(msg,
                " Replaying log file %s, version %s.",
                logfilename,
                versionstr);
    }
    else {
        sprintf(msg,
                " %s: version %s, genotype %d, quit at %d.",
                roguename,
                versionstr,
                geneid,
                quitat);
    }

    if(emacs) {
        fprintf(realstdout, "%s  (%%b)", msg);
        fflush(realstdout);
    }
    else if(terse) {
        fprintf(realstdout, "%s\n", msg);
        fflush(realstdout);
    }
    else {
        saynow(msg);
    }

    /*
     * Now that we have the version figured out, we can properly
     * interpret the screen. Force a redraw by sending a redraw
     * screen command (^L for old, ^R for new).
     *
     * Also identify wands (/), so that we can differentiate
     * older Rogue 3.6 form Rogue 3.6 with extra magic...
     */
    if(version < RV53A) {
        sendnow("%c//;", ctrl('l'));
    }
    else {
        sendnow("%c;", ctrl('r'));
    }

    /*
     * If we are not replaying an old game, we must position the
     * input after the next form feed, which signals the start of
     * the level drawing.
     */
    if(!replaying) {
        ch = GETROGUECHAR;

        while(((int)ch != CL_TOK) && ((int)ch != EOF)) {
            ch = GETROGUECHAR;
        }
    }

    /* Note: if we are replyaing, the logfile is now in synch */

    /* Read the input up to the end of first command */
    getrogue(ill, 2);

    /* Identify all 26 monsters */
    if(!replaying) {
        for(ch = 'A'; ch <= 'Z'; ++ch) {
            send("/%c", ch);
        }
    }

    /*
     * Signal handling. On an interrupt, Rogomatic goes into trasparent
     * mode and clear what state information it can. this code is styled
     * after that in "UNIX Programming -- Second Edition" by Brian
     * Kernigan and Dennis Ritchie. I sure wouldn't have thought of it.
     */
    /* save original status */
    istat = signal(SIGINT, SIG_IGN);

    /* save stack position */
    setjmp(commandtop);
    if(istat != SIG_IGN) {
        signal(SIGINT, onintr);
    }

    if(interrupted) {
        saynow("Interrupt [enter command]:");
        interrupted = 0;
        transparent = 1;
    }

    if(transparent) {
        noterm = 0;
    }

    while(playing) {
        refresh();

        /* If we have any commands to send, send them */
        while(resend()) {
            if(startingup) {
                showcommand(lastcmd);
            }

            sendnow(";");
            getrogue(ill, 2);
        }

        /* All monsters identified */
        if(startingup) {
            /* Do version specific things */
            versiondep();

            /* Clear starting flag */
            startingup = 0;
        }

        /* In case we died */
        if(!playing) {
            break;
        }

        /*
         * No more sotred commands, so either get a command from the
         * user (if we are in transparent mode or the user has typed
         * something), or let the strategize module try its luck. If
         * strategize fails we wait for the user to type something. If
         * there is no user (noterm mode) then user ROGQUIT to signal a
         * quit command.
         */
        if((transparent && !singlestep)
           || (!emacs && charsavail())
           || !strategize()) {
            if(noterm) {
                ch = ROGQUIT;
            }
            else {
                ch = getch();
            }

            switch(ch) {
            case '?':
                givehelp();

                break;
            case '\n':
                if(terse) {
                    printsnap(realstdout);
                    fflush(realstdout);
                }
                else {
                    singlestep = 1;
                    transparent = 1;
                }

                break;
            case 'H':
            case 'J':
            case 'K':
            case 'L':
            case 'Y':
            case 'U':
            case 'B':
            case 'N':
            case 'h':
            case 'j':
            case 'k':
            case 'l':
            case 'y':
            case 'u':
            case 'b':
            case 'n':
                /* Rogue command characters */
                command(T_OTHER, "%c", ch);
                transparent = 1;

                break;
            case 'f':
                ch = getch();

                for(s = "hjklyubnHJKLYUBN"; *s; ++s) {
                    if(ch == *s) {
                        if(version < RV53A) {
                            command(T_OTHER, "f%c", ch);
                        }
                        else {
                            command(T_OTHER, "%c", ctrl(ch));
                        }
                    }
                }

                transparent = 1;

                break;
            case '\f':
                redrawscreen();

                break;
            case 'm':
                dumpmonstertable();

                break;
            case 'M':
                dumpmazedoor();

                break;
            case '>':
                if((atrow == stairrow) && (atcol == staircol)) {
                    command(T_OTHER, ">");
                }

                transparent = 1;

                break;
            case '<':
                if((atrow == stairrow)
                   && (atcol == staircol)
                   && (have(amulet_obj) != NONE)) {
                    command(T_OTHER, "<");
                }

                transparent = 1;

                break;
            case 't':
                transparent = !transparent;

                break;
            case ')':
                markcycles(DOPRINT);
                at(row, col);

                break;
            case '+':
                setpsd(DOPRINT);
                at(row, col);

                break;
            case 'A':
                attempt = (attempt + 1) % 5;
                saynow("Attempt %d", attempt);

                break;
            case 'G':
                mvprintw(0,
                         0,
                         "%d: Sr %d Dr %d Re %d Ar %d Ex %d Rn %d Wk %d Fd %d, %d/%d",
                         geneid,
                         k_srch,
                         k_door,
                         k_rest,
                         k_arch,
                         k_exper,
                         k_run,
                         k_wake,
                         k_food,
                         genebest,
                         geneavg);

                clrtoeol();
                at(row, col);

                break;
            case ':':
                chicken = !chicken;

                if(chicken) {
                    saynow("chicken");
                }
                else {
                    saynow("aggresive");
                }

                break;
            case '~':
                if(replaying) {
                    saynow("Replaying log file %s, version %s.",
                           logfilename,
                           versionstr);
                }
                else {
                    saynow(" %s: version %s, genotype %d, quit at %d.",
                           roguename,
                           versionstr,
                           geneid,
                           quitat);
                }

                break;
            case '[':
                at(0, 0);
                printw("%s = %d, %s = %d, %s = %d, %s = %d.",
                       "hitstokill",
                       hitstokill,
                       "goodweapon",
                       goodweapon,
                       "usingarrow",
                       usingarrow,
                       "goodarrow",
                       goodarrow);

                clrtoeol();
                at(row, col);
                refresh();

                break;
            case '-':
                saynow(statusline());

                break;
            case '`':
                clear();
                summary((FILE *)NULL, '\n');
                pauserogue();

                break;
            case '|':
                clear();
                timehistory((FILE *)NULL, '\n');
                pauserogue();

                break;
            case 'r':
                resetinv();
                saynow("Inventory reset.");

                break;
            case 'i':
                clear();
                dumpinv((FILE *)NULL);
                pauserogue();

                break;
            case '/':
                dosnapshot();

                break;
            case '(':
                clear();
                dumpdatabase();
                pauserogue();

                break;
            case 'c':
                cheat = !cheat;

                if(cheat) {
                    saynow("cheating");
                }
                else {
                    saynow("righteous");
                }

                break;
            case 'd':
                toggledebug();

                break;
            case 'e':
                toggleecho();

                break;
            case '!':
                dumpstuff();

                break;
            case '@':
                dumpmonster();

                break;
            case '#':
                dumpwalls();

                break;
            case '%':
                clear();
                havearmor(1, DOPRINT, ANY);
                pauserogue();

                break;
            case ']':
                clear();
                havearmor(1, DOPRINT, RUSTPROOF);
                pauserogue();

                break;
            case '=':
                clear();
                havering(1, DOPRINT);
                pauserogue();

                break;
            case '$':
                clear();
                haveweapon(1, DOPRINT);
                pauserogue();

                break;
            case '^':
                clear();
                havebow(1, DOPRINT);
                pauserogue();

                break;
            case '{':
                promptforflags();

                break;
            case '&':
                saynow("Object count is %d.", objcount);

                break;
            case '*':
                blinded = !blinded;

                if(blinded) {
                    saynow("blinded");
                }
                else {
                    saynow("sighted");
                }

                break;
            case 'C':
                cosmic = !cosmic;

                if(cosmic) {
                    saynow("cosmic");
                }
                else {
                    saynow("boring");
                }

                break;
            case 'E':
                dwait(D_ERROR, "Testing the ERROR trap...");

                break;
            case 'F':
                dwait(D_FATAL, "Testing the FATAL trap...");

                break;
            case 'R':
                if(replaying) {
                    positionreplay();
                    getrogue(ill, 2);

                    if(transparent) {
                        singlestep = 1;
                    }
                }
                else {
                    saynow("Replay position only works in replay mode.");
                }

                break;
            case 'S':
                quitrogue("saved", Gold, SAVED);
                playing = 0;

                break;
            case 'Q':
                quitrogue("user typing quit", Gold, FINISHED);
                playing = 0;

                break;
            case ROGQUIT:
                dwait(D_ERROR, "Strategize failed, gave up.");
                quitrogue("gave up", Gold, SAVED);

                break;
            }
        }
        else {
            singlestep = 0;
        }
    }

    if(!replaying) {
        /* Save new long term memory */
        saveltm(Gold);

        /* End genetic learning */
        endlesson();
    }

    /* Print termination messages */
    at(23, 0);
    clrtoeol();
    refresh();
    endwin();
    nocrmode();
    noraw();
    echo();

    if(emacs) {
        if(*sumline) {
            fprintf(realstdout, " %s", sumline);
        }
    }
    else if(terse) {
        if(*sumline) {
            fprintf(realstdout, "%s\n", sumline);
        }

        fprintf(realstdout, "%s %s est.\n", gamename, termination);
    }
    else {
        if(*sumline) {
            printf("%s\n", sumline);
        }

        printf("%s %s est.\n", gamename, termination);
    }

    /* Rename log file, if it is open */
    if(logging) {
        char lognam[128];

        /* Make up a new log file name */
        sprintf(lognam, "%.4s.%d.%d", ourkiller, MaxLevel, ourscore);

        /* Close the open file */
        toggleecho();

        /* Rename the log file */
        if(link(ROGUELOG, lognam) == 0) {
            unlink(ROGUELOG);
            printf("Log file left on %s\n", lognam);
        }
        else {
            printf("Log file left on %s\n", ROGUELOG);
        }
    }

    exit(0);
}

/*
 * onintr: The SIGINT handler. Pass interrupts to main loop, setting
 * transparent mode. Also send some synchronization characters to Rogue,
 * and reset some goal variables.
 */
void onintr()
{
    /* Tell rogue we don't want to quit */
    sendnow("n\033");

    /* Print out everything */
    if(logging) {
        fflush(fecho);
    }

    /* Clear terminal output */
    refresh();

    /* Clear command queue */
    clearsendqueue();

    /* Don't believe ex */
    setnewgoal();

    /* Drop into transparent mode */
    transparent = 1;

    /* Mark as an interrupt */
    interrupted = 1;

    /* Allow commands */
    noterm = 0;

    /* Back to command Process */
    longjmp(commandtop, 0);
}

/*
 * startlesson: Genetic learning algorithm, pick a genotype to
 * test this game, and set the parameters (or "knobs") accordingly.
 */
void startlesson()
{
    sprintf(genelog, "%s/GeneLog%d", RGMDIR, version);
    sprintf(genepool, "%s/GenePool%d", RGMDIR, version);
    sprintf(genelock, "%s/Genelock%d", RGMDIR, version);

    /* Start random number generator */
    srand(0);

    /* disable interrupts */
    critical();

    /* Serialize access to the gene pool */
    /* Lock the gene pool */
    if(lock_file(genelock, MAXLOCK)) {
        /* Open the gene log file */
        if(openlog(genelog) == NULL) {
            saynow("coulf not open file %s", genelog);
        }

        /* Read the gene pool */
        if(!readgenes(genepool)) {
            /* Random starting point */
            initpool(MAXKNOB, 20);
        }

        /* Select a genotype */
        setknobs(&geneid, knob, &genebest, &geneavg);

        /* Write out the gene pool */
        writegenes(genepool);

        /* Close the gene log file */
        closelog();

        /* Unlock the gene pool */
        unlock_file(genelock);
    }
    else {
        fprintf(stderr, "Cannot lock gene pool to read '%s'\n", genepool);
    }

    /* Re-enable interrupts */
    uncritical();

    /* Cache the parameters for easier use */
    k_srch = knob[K_SRCH];
    k_door = knob[K_DOOR];
    k_rest = knob[K_REST];
    k_arch = knob[K_ARCH];
    k_exper = knob[K_EXPER];
    k_run = knob[K_RUN];
    k_wake = knob[K_WAKE];
    k_food = knob[K_FOOD];
}

/*
 * endlesson: If killed, total winner, or quit for socreboard,
 * evaluate the performance of the genotype and save in genepool.
 */
void endlesson()
{
    if((geneid > 0)
       && (stlmatch(termination, "perditus")
           || stlmatch(termination, "victorius")
           || stlmatch(termination, "callidus"))) {
        /* Disbale interrupts */
        critical();

        /* Lock the score file */
        if(lock_file(genelock, MAXLOCK)) {
            /* Open the gene log file */
            openlog(genelog);

            /* Read the gene pool */
            if(readgenes(genepool)) {
                /* Add the trial to the pool */
                evalknobs(geneid, Gold, Level);

                /* Write out the gene pool */
                writegenes(genepool);
            }

            closelog();

            /* Disable interrupts */
            unlock_file(genelock);
        }
        else {
            fprintf(stderr,
                    "Cannot lock gene pool to evaluate '%s'\n",
                    genepool);
        }

        /* Re-enable interrupts */
        uncritical();
    }
}
