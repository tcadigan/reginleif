/*
 * io.c
 * Larn is copyrighted 1986 by Noah Morgan.
 *
 * Below are the functions in this file:
 *
 * setupvt100()    Subroutine to set up terminal in correct mode for game
 * clearvt100()    Subroutine to clean up terminal when the game is over
 * scbr()          Function to set cbreak -echo for the terminal
 * sncbr()         Function to set -cbreak echo for the terminal
 * newgame()       Subroutine to save the initial time and seed rnd()
 *
 * FILE OUTPUT ROUTINES
 * 
 * lprintf(format, args...)    printf to the output buffer
 * lprint(integer)             Send binary integer to output buffer
 * lwrite(buf, len)            Write a buffer to the output buffer
 * lprcat(str)                 Send string to output buffer
 *
 * FILE OUTPUT MACROS (in header.h)
 * 
 * lprc(character)    Put the character into the output buffer
 *
 * FILE INPUT ROUTINES
 *
 * long lgetc()               Read one character from input buffer
 * long lrint()               Read one integer from input buffer
 * lrfill(address, number)    Put input bytes into a buffer
 * char *lgetw()              Get a whitespace ended word from input
 * char *lgetl()              Get a \n or EOF ended line from input
 *
 * FILE OPEN/CLOSE ROUTINES
 *
 * lcreat(filename)     Create a new file for write
 * lopen(filename)      Open a file for read
 * lappend(filename)    Open for append to an existing file
 * lrclose()            Close the input file
 * lwclose()            Close output file
 * lflush()             Flush the output buffer
 *
 * OTHER ROUTINES
 *
 *  cursor(x, y)              position cursor at [x, y]
 *  cursors()                 position cursor at [1, 24] (saves memory)
 *  cl_line(x, y)             Clear line at [1, y] and leave cursor at [x, y]
 *  cl_up(x, y)               Clear screen from [x, 1] to current line
 *  cl_dn(x, y)               Clear screen from [1, y] to end of display
 *  standout(str)             Print the string in standout mode
 *  set_score_output()        Called when output should be literally printed
 ** flush_buf()               Flush buffer with decoded output
 ** init_term()               Terminal initialization -- setup termcap info
 ** char *tmcapcnv(sd, ss)    Routine to convert VT100 \33's to termcap format
 *
 * Note: ** entries are available only in termcap mode.
 */

#include "io.h"

#include "header.h"
#include "scores.h"

#include <curses.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* Size of the lgetw() and lgetl() buffer */
#define LINBUFSIZE 128

/* Output file numbers */
FILE *lfd;

/* Input file numbers */
FILE *fd;

/* Input buffering pointers */
static int ipoint = MAXIBUF;
static int iepoint = MAXIBUF;

/* Get line (word) buffer */
static char *lgetwbuf;

/*
 * setupvt100()
 *
 * Subroutine to set up terminal in correct mode for game.
 * Attributes off, clear screen, set scrolling region, set tty mode
 */
void setupvt100()
{
    lgetwbuf = (char *)malloc(LINBUFSIZE * sizeof(char));
    
    clear();
    setscroll();
    scbr();
    /* system("stty cbreak -echo"); */
}

/*
 * clearvt100()
 *
 * Subroutine to clean up terminal when the game is over.
 * Attributes off, clear screen, unset scrolling region, restore tty mode
 */
void clearvt100()
{
    free(lgetwbuf);
    
    resetscroll();
    clear();
    sncbr();
    /* system("stty -cbreak echo"); */
}

/*
 * scbr()
 *
 * Function to set cbreak -echo for the terminal
 *
 * like: system("stty cbreak -echo")
 */
void scbr()
{
    cbreak();
}

/*
 * sncbr()
 *
 * Function to set -cbreak echo for the terminal
 *
 * like: system("stty -cbreak echo")
 */
void sncbr()
{
    nocbreak();
}

/*
 * newgame()
 *
 * Subroutine to save the initial time and seed rnd()
 */
void newgame()
{
    initscr();
    
    time(&initialtime);
    srand(initialtime);

    /* Open buffering for output to terminal */
    lfd = stdout;
}

/*
 * lprintf(char *format, ??? args...)
 *
 * printf to the output buffer
 *
 * Enter with the format string in "format", as per printf() usage and
 * any needed arguments following it.
 *
 * Note: lprintf() only supports %s, %c and %d, with width modifier
 *       and left or right justification
 *
 * No correct checking for output buffer overflow is done, but flushes
 * are done beforehand if needed
 *
 * Returns nothing of value.
 */
void lprintf(char *format, ...)
{
    va_list args;
    
    va_start(args, format);
    vfprintf(lfd, format, args);
    va_end(args);
}

/*
 * lwrite(char *buf, int len)
 *
 * Write a buffer to the output buffer
 *
 * Enter with the address and number of bytes to write out.
 *
 * Returns nothing of value.
 */
void lwrite(char *buf, int len)
{
#ifdef EXTRA
    c[BYTESOUT] += len;
#endif

    fprintf(lfd, "%s", buf);
}

/*
 * long lgetc()
 *
 * Read one character from input buffer
 *
 * Returns 0 if EOF, otherwise the character
 */
long lgetc()
{
    int i;

    i = fread(inbuffer, sizeof(char), 1, fd);

    if(i <= 0) {
	if(i != 0) {
	    if(write(1, "error reading from input file\n", 30) == -1) {
		exit(1);
	    }
	    
	    ipoint = 0;
	    iepoint = ipoint;

	    return 0;
	}
    }

    return *inbuffer;
}

/*
 * long lrint()
 *
 * Read one integer from input buffer
 *
 *  +---------+---------+---------+---------+
 *  |  high   |         |         |  low    |
 *  |  order  |         |         |  order  |
 *  |  byte   |         |         |  byte   |
 *  +---------+---------+---------+---------+
 * 31  ---  24 23 --- 16 15 ---  8 7  ---   0
 *
 * The save order is low order first, to high order (4 bytes total).
 *
 * Returns the int read
 */
int larnint()
{
    int i;

    i = (255 & lgetc());
    i |= ((255 & lgetc()) << 8);
    i |= ((255 & lgetc()) << 16);
    i |= ((255 & lgetc()) << 24);

    return i;
}

/*
 * lrfill(char *address, int number)
 *
 * Put input bytes into a buffer
 *
 * Returns pointer to a buffer that contains word. If EOF, returns a NULL
 */
void lrfill(char *adr, int num)
{
    if(fread(adr, sizeof(char), num, fd) == -1) {
	exit(1);
    }
}

/*
 * char *lgetw()
 *
 * Get a whitespace ended word from input
 *
 * Returns pointer to a buffer that contains word. if EOF, returns a NULL.
 */
char *lgetw()
{
    char *lgp;
    char cc;

    int n = LINBUFSIZE;
    int quote = 0;

    lgp = lgetwbuf;

    cc = lgetc();

    /* Eat whitespace */
    while((cc <= 32) && (cc > EOF)) {
	cc = lgetc();
    }

    while(1) {
	/* EOF */
	if((cc == EOF) && (lgp == lgetwbuf)) {
	    return NULL;
	}

	if((n <= 1) || ((cc <= 32) && (quote == 0))) {
	    *lgp = EOF;

	    return lgetwbuf;
	}

	if(cc != '"') {
	    *lgp++ = cc;
	}
	else {
	    quote ^= 1;
	}

	--n;
	cc = lgetc();
    }
}

/*
 * char *lgetl()
 *
 * Function to read in a line ended by newline or EOF
 *
 * Returns pointer to a buffer that contains the line. if EOF, returns NULL
 */
char *lgetl()
{
    int amount;
    size_t max = LINBUFSIZE;

    amount = getline(&lgetwbuf, &max, fd);

    if(amount < 0) {
	return NULL;
    }
    else {
	return lgetwbuf;
    }
}

/*
 * lcreat(char *filename)
 *
 * Create a new file for write
 *
 * lcreat((char *)0); means to the terminal
 * Retrurns -1 if error, otherwise the file descriptor opened.
 */
FILE *lcreat(char *str)
{
    if(str == NULL) {
	lfd = stdout;
	
	return lfd;
    }

    lfd = fopen(str, "w");

    if(lfd < 0) {
	lfd = stdout;
	lprintf("error creating file <%s>\n", str);
	lflush();

	return NULL;
    }

    return lfd;
}

/*
 * lopen(char *filename)
 *
 * Open a file for read
 *
 * lopen(0) means from the terminal
 * Returns -1 if error, otherwise the file descriptor opened.
 */
FILE *lopen(char *str)
{
    if(str == NULL) {
	fd = STDIN_FILENO;
	
	return fd;
    }

    lfd = fopen(str, "r");

    if(fd < 0) {
	lwclose();
	lfd = stdout;
	lpnt = lpbuf;

	return NULL;
    }

    return fd;
}

/*
 * lappend(char *filename)
 *
 * Open for append to an existing file
 *
 * lappend(0) means to the terminal
 * Returns -1 if error, otherwise the file descriptor opened.
 */
FILE *lappend(char *str)
{
    if(str == NULL) {
	lfd = stdout;

	return lfd;
    }

    lfd = fopen(str, "a");

    if(lfd < 0) {
	lfd = stdout;

	return NULL;
    }

    return lfd;
}

/*
 * lrclose()
 *
 * Close the input file
 *
 * Returns nothing of value
 */
void lrclose()
{
    if(fd > 0) {
	fclose(fd);
    }
}

/*
 * lwclose()
 *
 * Close output file flushing if needed
 *
 * Returns nothing of value
 */
void lwclose()
{
    lflush();

    if((lfd != stdout) && (lfd != stdin) && (lfd != stderr)) {
	fclose(lfd);
    }
}

/*
 * lprcat(string)
 *
 * Append a string to the output buffer avoids calls to lprintf (time consuming)
 */
void lprcat(char *str)
{
    strcat(lpnt, str);
}

/*
 * cursor(x, y)
 *
 * Put the cursor at specified coordinates starting at [1, 1] (termcap)
 */
void cursor(int x, int y)
{
    move(y, x);
}

/*
 * Routine to position cursor at beginning of 24th line
 */
void cursors()
{
    cursor(1, 24);
}

/*
 * cl_line(x, y)
 *
 * Clear the whole line indicated by 'y' and leave cursor at [x, y]
 */
void cl_line(int x, int y)
{
    cursor(1, y);
    clrtoeol();
    cursor(x, y);
}

/*
 * cl_up(x, y)
 *
 * Clear screen from [x, 1] to current position. Leave cursor at [x, y]
 */
void cl_up(int x, int y)
{    
    int i;

    for(i = 1; i <= y; ++i) {
	cursor(1, i);
	clrtoeol();
    }

    cursor(x, y);
}

/*
 * cl_dn(x, y)
 *
 * Clear screen from [1, y] to end of display. Leave cursor at [x, y]
 */
void cl_dn(int x, int y)
{
    int i;

    for(i = y; i < LINES; ++i) {
	cursor(1, y);
	clrtoeol();
    }

    cursor(x, y);
}

/*
 * standout(str)
 *
 * Print the argument string in inverse video (standout mode).
 */
void larnstandout(char *str)
{
    standout();
    sprintf(lpnt, "%s", str);
    standend();
}

/*
 * set_score_output()
 *
 * Called when output should be literally printed.
 */
void set_score_output()
{
    scrollok(stdscr, FALSE);
}

/*
 * lflush()
 *
 * Flush the output buffer
 *
 * Returns nothing of value.
 *
 * For termcap version: Flush output in output buffer according to
 *                      output status as indicated by `enable_scroll'
 */
void lflush()
{
    fflush(lfd);
}
