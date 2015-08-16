/*
 * Person to bother if something does wrong.
 * Recompile files.c and misc2.c if these change
 */
#define WIZARD "Ed Vopata"

/* Wizard password */
#define PASSWD1 "xxxx"

/* God password, for debugging */
#define PASSWD2 "yyyy"

/* Wizard/God user id */
#define UID 283

/*
 * Files used by Moria, set these to valid pathnames.
 * If you changed these, you have to recompile death.c and files.c
 */
#define MORIA_HOU "/usrb/vopata/mm/Moria_hours"
#define MORIA_MOR "/usrb/vopata/mm/Moria_news"
#define MORIA_MAS "/usrb/vopata/mm/Character"
#define MORIA_TOP "/usrb/vopata/mm/Highscores"
#define MORIA_HLP "/usrb/vopata/mm/Helpfile"

/*
 * This set the default user interface, ONE of two must be defined.
 * Use the original key bindings, keypad for movement.
 * If you change this, you need to recompile main.c
 */
/* #define KEY_BINDINGS ORIGINAL */

/* This uses a rogue-like key bindings, vi style movement */
#define KEY_BINDINGS ROGUE_LIKE

/*
 * No system definitions are needed for 4.3BSD or SUN OS */

/*
 * If you are compiling on an ultrix/4.2BSD/Dynix/etc. version of UNIX,
 * define this, not needed for SUNs
 */
/* #define ultrix */

/* If you are compiling on a SYS V version of UNIX, define this */
/* #define SYS_V */

/* If yo uare compiling on a SYS III version of UNIX, define this */
/* #define SYS_III */

/*
 * If the screen does not look right (it will be obvious) then you have a buggy
 * version of curses, and you must define this.
 * This must be dfined for Ultrix systems, and for most 4.2BSD systems it is
 * not needed for Dynix.
 * Only need to recompile io.c
 */
/* #define BUGGY_CURSES */

/*
 * Define this if you machine does not have signed characters:
 * This is needed for IBM PC-RT, 3B1, 3B2, 3B20, and perhaps others.
 */
/* #define NO_SIGNED_CHARS */

/*
 * This will slow down the game a bit, and therefore make it use up a little
 * less cpu time, this enables sleep() code for the resting commands, and for
 * character rerolling.
 * Only need to recompile dungeon.c and create.c
 */
/* #define SLOW */

/*
 * System dependent defines follow, you should not need to change anything below
 */

/* Substitute strchr forindex on USG versions of UNIX */
#ifdef SYS_V
#define index strchr
#endif

#ifdef SYS_III
char *index();
#endif

#if defined(SYS_III) || defined(SYS_V)
#ifndef USG
#define USG
#endif
#endif
