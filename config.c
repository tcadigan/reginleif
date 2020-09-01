/*
 * config.c
 * Larn is copyrighted 1986 by Noah Morgan.
 *
 * This defines the installation dependent variables.  Some string are modified
 * later. ANSI C would allow compile time string concatenation, we must do
 * runtime concatenation, in main.
 */

#include "header.h"
#include "pathnames.h"

/*
 * All these strings will be appended to in main() to be complete filenames
 */

/* The game save filename */
char savefilename[1024];

/* The logging file */
char logfile[] = _PATH_LOG;

/* The help text file */
char helpfile[] = _PATH_HELP;

/* The score file */
char scorefile[] = _PATH_SCORE;

/* The maze data file */
char larnlevels[] = _PATH_LEVELS;

/* The fortune data file */
char fortfile[] = _PATH_FORTS;

/* The .larnopts filename */
char optsfile[1024] = "/.larnopts";

/* The player id data file name */
char playerids[] = _PATH_PLAYERIDS;

/* The diagnostic filename */
char diagfile[] = "Diagfile";

/* The checkpoint filename */
char ckpfile[] = "Larn12.0.ckp";

/* The wizard's password <= 32 */
char *password = "pvnert(x)";

/* The process name */
char psname[PSNAMESIZE] = "larn";

#define WIZID 1

/* The user id of the only person who can be wizard */
int wisid = 0;
