// Machine dependencies

// Where the scorefile should live
#define SCOREFILE "/usr/games/rogueroll"

// Variables for chekcing to make sure the system isn't too loaded
// for people to play

// Max number of users for this game
#define MAXUSERS 8

// 10 * max 15 minute load average
#undef MAXLOAD 40

#ifdef MAXUSERS|MAXLOAD
// Number of minutes between load checks.
// If not defined, checks are only on startup
#define	CHECKTIME 10
#endif

#ifdef MAXLOAD
// Defined if rogue should provide loadav()
#define	LOADAV

#ifdef LOADAV
// Where the system namelist lives
#define	NAMELIST "/vmunix"
#endif

#endif

#ifdef MAXUSERS
// Defined if rogue should provide ucount()
#define UCOUNT

#ifdef UCOUNT
// Where utmp file lives
#define UTMP "/etc/UTMP"
#endif

#endif

// BRL Addition:
//     If RNOTES is defined, then the filename it is defined as
//     is printed on the terminal whenever rogue is invoked.
//     Typically this would be used to describe changes or
//     to warn of bugs, etc.
#define	RNOTES "/usr/games/roguenotes"
