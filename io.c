/*
 * io.c
 * Larn is copyrighted 1986 by Noah Morgan.
 *
 * Below are the functions in this file:
 *
 * setupvt100()    Subroutine to set up terminal in correct mode for game
 * clearvt100()    Subroutine to clean up terminal when the game is over
 * getchar()       Routine to read in one character from the terminal
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
 ** putchar(ch)               Print one character in decoded output buffer
 ** flush_buf()               Flush buffer with decoded output
 ** init_term()               Terminal initialization -- setup termcap info
 ** char *tmcapcnv(sd, ss)    Routine to convert VT100 \33's to termcap format
 *  beep()                    Routine to emit a beep if enabled (see no-beep in .larnopts)
 *
 * Note: ** entries are available only in termcap mode.
 */

#include "header.h"

#include <curses.h>
#include <stdarg.h>

/* System III or System V */
#ifdef SYSV
#include <termio.h>

#define sgttyb termio
#define stty(_a, _b) ioctl(_a, TCSETA, _b)
#define gtty(_a, _b) ioctl(_a, TCGETA, _b)

static int rawflg = 0;
static char saveeof;
static char saveeol;

#define doraw(_a)						\
    if(!rawflg) {						\
	++rawflg;						\
	saveeof = _a.c_cc[VMIN];				\
	saveeol = _a.c_cc[VTIME];				\
    }								\
    _a.c_cc[VMIN] = 1;						\
    _a.c_cc[VTIME] = 1;						\
    _a.c_lflag &= ~(ICANON | EHCO | ECHOE | ECHOK | ECHONL)

#define unraw(_a)						\
    _a.c_cc[VMIN] = saveeof;					\
    _a.c_cc[VTIME] = saveeol;					\
    _a.c_lflg |= (ICANON | ECHO | ECHOE | ECHOK | ECHONL)

#else

#ifndef BSD

/* V7 has no CBREAK */
#define CBREAK RAW

#endif

#define doraw(_a)				\
    _a.sg_flags |= CBREAK;			\
    _a.sg_flags &= ~ECHO

#define unraw(_a)				\
    _a.sg_flag &= ~ECHO;			\
    _a.sg_flags |= ECHO

#include <sgtty.h>

#endif

/* If we have varargs */
#ifndef NOVARARGS

#include <varargs.h>

/* If we don't have varargs */
#else

typedef char *va_list;

#define va_dcl int va_alist;
#define va_start(plist) plist = (char *)&va_alist
#define va_end(plist)
#define va_arg(plist, mode) ((mode *)(plist += sizeof(mode)))[-1]

#endif

/* Size of the lgetw() and lgetl() buffer */
#define LINBUFSIZE 128

/* Output file numbers */
int lfd;

/* Input file numbers */
int fd;

/* Storage for the tty modes */
static struct sgttyb ttx;

/* Input buffering pointers */
static int ipoint = MAXIBUF;
static int iepoint = MAXIBUF;

/* Get line (word) buffer */
static char lgetwbuf[LINBUFSIZE];

/*
 * setupvt100()
 *
 * Subroutine to set up terminal in correct mode for game.
 * Attributes off, clear screen, set scrolling region, set tty mode
 */
void setupvt100()
{
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
    resetscroll();
    clear();
    sncbr();
    /* system("stty -cbreak echo"); */
}

/*
 * getchar()
 * 
 * Routine to read in one character from the terminal
 */
char getchar()
{
    char byt;

#ifdef EXTRA
    ++c[BYTESIN];
#endif

    /* Be sure output buffer is flushed */
    lflush();

    /* Get byte from terminal */
    read(STDIN_FILENO, &byt, 1);

    return byt;
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
    raw();
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
    noraw();
}

/*
 * newgame()
 *
 * Subroutine to save the initial time and seed rnd()
 */
void newgame()
{
    long *p;
    long *pe;

    p = c;
    pe = c + 100;

    while(p < pe) {
	*p++ = 0;
    }

    time(&initialtime);
    srand(initialtime);
    
    /* Open buffering for output to terminal */
    lcreat((char *)0);
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
#ifdef lint

void lprintf(char *str)
{
    char *str2;
    
    str2 = str;
    
    /* To make lint happy */
    str = str2;
}

void sprintf(char *str)
{
    char *str2;
    
    str2 = str;
    
    /* To make lint happy */
    str = str2;
}

#else

void lprintf(char *format, ...)
{
    va_list args;
    
    va_start(args, format);
    vsprintf(outb, format, args);
    va_end();
}

#endif

/*
 * lprintf(long integer)
 *
 * Send binary integer to output buffer
 *
 *  +---------+---------+---------+---------+
 *  |  high   |         |         |  low    |
 *  |  order  |         |         |  order  |
 *  |  byte   |         |         |  byte   |
 *  +---------+---------+---------+---------+
 * 31  ---  24 23 --- 16 15 ---  8 7  ---   0
 *
 * The save order is low order first, to high order (4 bytes total)
 * and is written to be system independent.
 *
 * No checking for output buffer overflow is done, but flushes if needed!
 *
 * Returns nothing of value.
 */
void lprintf(long x)
{
    lprintf("%ld", x);
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
    char *str;
    int num2;

    /* Don't copy data if you can just write it */
    if(len > 399) {
#ifdef EXTRA
	c[BYTESOUT] += len;
#endif

#ifndef VT100
	for(str = buf; len > 0; --len) {
	    lprc(*str++);
	}	
#else
	lflush();
	write(lfd, buf, len);
#endif
    }
    else {
	while(len) {
	    /* If buffer is full flush it */
	    if(lpnt >= lpend) {
		lflush();
	    }

	    /* # bytes left in output buffer */
	    num2 = lpbug + BUFBIG - lpnt;

	    if(num2 > len) {
		num2 = len;
	    }

	    str = lpnt;
	    len -= num2;

	    /* Copy in the bytes */
	    while(num2--) {
		*str++ = *buf++;
	    }

	    lpnt = str;
	}
    }
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

    if(ipoint != iepoint) {
	return inbuffer[ipoint++];
    }

    if(iepoint != MAXIBUF) {
	return 0;
    }

    i = read(fd, inbuffer, MAXIBUF);

    if(i <= 0) {
	if(i != 0) {
	    write(1, "error reading from input file\n", 30);
	    ipoint = 0;
	    iepoint = ipoint;

	    return 0;
	}
    }

    ipoint = 1;
    iepoint = i;

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
long lrint()
{
    unsigned long i;

    i = (255 & lgetc);
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
char *lgetw()
{
    char *lgp;
    char cc;

    int n = LINBUFSIZE;
    int quote = 0;

    lgp = lgetwbuf;

    cc = lgetc();

    /* Eat whitespace */
    while((cc <= 32) && (cc > NULL)) {
	cc = lgetc();
    }

    while(1) {
	/* EOF */
	if((cc == NULL) && (lgp == lgetwbuf)) {
	    return NULL;
	}

	if((n <= 1) || ((c <= 32) && (quote == 0))) {
	    *lgp = NULL;

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
    int i = LINEBUFSIZE;
    int ch;
    char *str = lgetwbuf;

    while(1) {
	ch = lgetc();
	
	if((*str++ = ch) == NULL) {
	    /* EOF */
	    if(str == (lgetwbuf + 1)) {
		return NULL;
	    }

	    /* Line ended by EOF */
	    *str = NULL;

	    return lgetwbuf;
	}

	/* Line ended by \n */
	if((ch == '\n') || (i <= 1)) {
	    *str = NULL;

	    return lgetwbuf;
	}
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
int lcreat(char *str)
{
    lpnt = lpbuf;
    lpend = lpbuf + BUFBIG;

    if(str == NULL) {
	lfd = 1;
	
	return lfd;
    }

    lfd = creat(str, 0644);

    if(lfd < 0) {
	lfd = 1;
	lprintf("error creating file <%s>\n", str);
	lflush();

	return -1;
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
int lopen(char *str)
{
    iepoint = MAXIBUF;
    ipoint = iepoint;

    if(str == NULL) {
	fd = STDIN_FILNO;
	
	return fd;
    }

    fd = open(str, 0);

    if(fd < 0) {
	lwclose();
	lfd = 1;
	lpnt = lpbuf;

	return -1;
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
int lappend(char *str)
{
    lpnt = lpbuf;
    lpend = lpbuf + BUFBIG;

    if(str == NULL) {
	lfd = 1;

	return lfd;
    }

    lfd = open(str, 2);

    if(lfd < 0) {
	lfd = 1;

	return -1;
    }

    /* Seek to end of file */
    lseek(lfd, 0, 2);

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
	close(fd);
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

    if(lfd > 2) {
	close(lfd);
    }
}

/*
 * lprcat(string)
 *
 * Append a string to the output buffer avoids calls to lprintf (time consuming)
 */
void lprcat(char *str)
{
    char *str2;

    if(lpnt >= lpend) {
	lflush();
    }

    str2 = lpnt;

    *str2++ = *str++;

    while(*str2) {
	*str2++ = *str++;
    }

    lpnt = str2 - 1;
}

#ifdef VT100
/*
 * cursor(x, y)
 *
 * Subroutine to set the cursor position
 *
 * x and y are the cursor coordinates, and lpbuff is the output buffer
 * where escape sequence will be placed
 */
static char *y_num[] = {
        "\33[",   "\33[",  "\33[2",  "\33[3",  "\33[4",
       "\33[5",  "\33[6",  "\33[7",  "\33[8",  "\33[9",
      "\33[10", "\33[11", "\33[12", "\33[13", "\33[14",
      "\33[15", "\33[16", "\33[17", "\33[18", "\33[19",
      "\33[20", "\33[21", "\33[22", "\33[23", "\33[24"
};

static char *x_num[] = {
       "H",    "H",  ";2H",  ";3H",  ";4H",  ";5H",  ";6H",  ";7H",  ";8H",  ";9H",
    ";10H", ";11H", ";12H", ";13H", ";14H", ";15H", ";16H", ";17H", ";18H", ";19H",
    ";20H", ";21H", ";22H", ";23H", ";24H", ";25H", ";26H", ";27H", ";28H", ";29H",
    ";30H", ";31H", ";32H", ";33H", ";34H", ";35H", ";36H", ";37H", ";38H", ";39H",
    ";40H", ";41H", ";42H", ";43H", ";44H", ";45H", ";46H", ";47H", ";48H", ";49H",
    ";50H", ";51H", ";52H", ";53H", ";54H", ";55H", ";56H", ";57H", ";58H", ";59H",
    ";60H", ";61H", ";62H", ";63H", ";64H", ";65H", ";66H", ";67H", ";68H", ";69H",
    ";70H", ";71H", ";72H", ";73H", ";74H", ";75H", ";76H", ";77H", ";78H", ";79H",
    ";80H"
};

void cursor(int x, int y)
{
    char *p;

    if(lpnt >= lpend) {
	lflush();
    }

    /* Get the string to print */
    p = y_num[y];

    /* Print the string */
    while(*p) {
	*lpnt++ = *p++;
    }

    /* Get the string to print */
    p = x_num[x];

    /* Print the string */
    while(*p) {
	*lpnt++ = *p++;
    }
}

#else

/*
 * cursor(x, y)
 *
 * Put the cursor at specified coordinates starting at [1, 1] (termcap)
 */
void cursors()
{
    cursor(1, 24);
}

#ifndef

/*
 * WARNING: Ringing the bell is control code 7. Don't use in defines.
 *          Don't change the order of these defines.
 *          Also used in helpfiles. Codes used in helpfiles should be
 *          \E[1 to \E[7 with obvious meanings
 */

static char cap[256];

/* Termcap capabilites */
char *CM;
char *CE;
char *CD;
char *CL;
char *SO;
char *SE;
char *AL;
char *DL;

/* Translated output buffer */
static char *outbuf = 0;

int putchar();

/*
 * init_term()
 * 
 * Terminal initialization -- setup termcap info
 */
void init_term()
{
    char termbuf[1024];
    char *capptr = cap + 10;
    char *term;

    term = getenv("TERM");
    
    switch(tgetent(termbuf, term)) {
    case -1:
	write(2, "Cannot open termcap file.\n", 26);

	exit();
    case 0:
	write(2,  "Cannot find entry of ", 21);
	write(2, term, strlen(term));
	write(2, " in termcap\n", 12);

	exit();
    }

    /* Cursor motion */
    CM = tgetstr("cm", &capptr);

    /* Clear to eoln */
    CE = tgetstr("ce", &capptr);

    /* Clear screen */
    CL = tgetstr("cl", &capptr);

    /* OPTIONAL */

    /* Insert line */
    AL = tgetstr("al", &capptr);

    /* Delete line */
    DL = tgetstr("dl", &capptr);

    /* Begin standout mode */
    SO = tgetstr("so", &capptr);

    /* End standout mode */
    SE = tgetstr("se", &capptr);

    /* Clear to end of display */
    CD = tgetstr("cd", &capptr);

    /* Can't find cursor motion entry */
    if(!CM) {
	write(2, "Sorry, for a ", 13);
	write(2, term, strlen(term));
	write(2, ", I can't find the cursor motion entry in termcap\n", 50);

	exit();
    }

    /* Can't find clear to end of line entry */
    if(!CE) {
	write(2, "Sorry, for a ", 13);
	write(2, term, strlen(term));
	write(2, ", I can't find the clear to end of line entry in termcap\n", 57);

	exit();
    }

    /* Can't find the clear entire screen entry */
    if(!CL) {
	write(2, "Sorry, for a ",13);
	write(2, term, strlen(term));
	write(2, ", I can't find the clear entire screen entry in termcap\n", 56);

	exit();
    }

    /* Get memory for decoded output buffer */
    outbuf = malloc(BUFBIG + 16);

    if(outbuf == 0) {
	write(2, "Error malloc'ing memory for decoded output buffer\n", 50);

	/* malloc() failure */
	died(-285);
    }
}

#endif

/*
 * cl_line(x, y)
 *
 * Clear the whole line indicated by 'y' and leave cursor at [x, y]
 */
void cl_line(int x, int y)
{
#ifdef VT100
    cursor(x, y);
    lprcat("\33[2k");
    
#else
    cursor(1, y);
    *lpnt++ = CL_LINE;
    cursor(x, y);
#endif
}

/*
 * cl_up(x, y)
 *
 * Clear screen from [x, 1] to current position. Leave cursor at [x, y]
 */
void cl_up(int x, int y)
{
#ifdef VT100
    cursor(x, y);
    lprcat("\33[1J\33[2K");

#else
    int i;

    cursor(1, 1);

    for(i = 1; i <= y; ++i) {
	*lpnt++ = CL_LINE;
	*lpnt++ = '\n';
    }

    cursor(x, y);
#endif
}

/*
 * cl_dn(x, y)
 *
 * Clear screen from [1, y] to end of display. Leave cursor at [x, y]
 */
void cl_dn(int x, int y)
{
#ifdef VT100
    cursor(x, y);
    lprcat("\33[J\33[2K");

#else
    int i;

    cursor(1, y);

    if(!CD) {
	*lpnt++ = CL_LINE;

	for(i = y; i <= 24; ++i) {
	    *lpnt++ = CL_LINE;

	    if(i != 24) {
		*lpnt++ = '\n';
	    }
	}

	cursor(x, y);
    }
    else {
	*lpnt++ = CL_DOWN;
    }

    cursor(x, y);
#endif
}

/*
 * standout(str)
 *
 * Print the argument string in inverse video (standout mode).
 */
void standout(char *str)
{
#ifdef VT100
    setbold();

    while(*str) {
	*lpnt++ = *str++;
    }

    resetbold();

#else
    *lpnt++ = ST_START;

    while(*str) {
	*lpnt++ = *str++;
    }

    *lpnt++ = ST_END;
#endif
}

/*
 * set_score_output()
 *
 * Called when output should be literally printed.
 */
void set_score_output()
{
    enable_scroll = -1;
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
#ifndef VT100
/* Line # for wraparound instead of scrolling if no DL */
static int srcline = 18;

void lflush()
{
    int lpoint;
    char *str;
    static int curx = 0;
    static int cury = 0;

    lpoint = lpnt - lpbuf;

    if(lpoint > 0) {
#ifdef EXTRA
	c[BYTESOUT] += lpoint;
#endif

	if(enable_scroll <= -1) {
	    flush_buf();

	    if(write(lfd, lpbuf, lpoint) != lpoint) {
		write(2, "error writing to output file\n", 29);
	    }

	    /* Point back to beginning of buffer */
	    lpnt = lpbuf;

	    return;
	}

	for(str = lpbuf; str < lpnt; ++str) {
	    if(*str >= 32) {
		putchar(*str);
		++curxx;
	    }
	    else {
		switch(*str) {
		case CLEAR:
		    tputs(CL, 0, putchar);
		    cury = 0;
		    curx = cury;

		    break;
		case CL_LINE:
		    tputs(CE, 0, putchar);

		    break;
		case CL_DOWN:
		    tputs(CD, 0, putchar);

		    break;
		case ST_START:
		    tputs(SO, 0, putchar);

		    break;
		case ST_END:
		    tputs(SE, 0, putchar);

		    break;
		case CURSOR:
		    curx = *++str - 1;
		    cury = *str++ - 1;
		    tputs(tgoto(CM, curx, cury), 0, putchar);

		    break;
		case '\n':
		    if((cury == 23) && enable_scroll) {
			/* Wraparound or scroll? */
			if(!DL || !AL) {
			    if(++scrline > 23) {
				scrline = 19;
			    }

			    if(++scrline > 23) {
				scrline = 19;
			    }

			    tputs(tgoto(CM, 0, scrline), 0, putchar);

			    tputs(CE, 0, putchar);

			    if(--scrline < 19) {
				scrline = 23;
			    }

			    tputs(tgoto(CM, 0, scrline), 0, putchar);
			    tputs(CE, 0, putchar);
			}
			else {
			    tputs(tgoto(CM, 0, 19), 0, putchar);
			    tputs(DL, 0, putchar);
			    tputs(tgoto(CM, 0, 23), 0, putchar);
			    /* tputs(AL, 0, putchar); */
			}
		    }
		    else {
			putchar('\n');
			++cury;
		    }

		    curx = 0;
		    
		    break;
		default:
		    putchar(*str);
		    ++curx;
		}
	    }
	}
    }

    lpnt = lpbuf;

    /* Flush real output buffer now */
    flush_buf();
}

#else

/*
 * lflush()
 *
 * Flush the output buffer
 *
 * Returns nothing of value.
 */
void lflush()
{
    int lpoint;

    lpoint = lpnt - lpbuf;

    if(lpoint > 0) {
#ifdef EXTRA
	c[BYTESOUT] += lpoint;
#endif
	if(write(lfd, lpbuf, lpoint) != lpoint) {
	    write(2, "error writing output file\n", 29);
	}
    }
    
    /* Point back to beginning of buffer */
    lpnt = lpbuf;
}

#endif

#ifndef VT100
static int index = 0;

/*
 * putchar()
 *
 * Print one character in decoded output buffer.
 */
void putchar(int c)
{
    outbuf[index] = c;
    ++index;

    if(index >= BUFBIG) {
	flush_buf();
    }
}

/*
 * flush_buf()
 *
 * Flush buffer with decoded output.
 */
void flush_buf()
{
    if(index) {
	write(lfd, outbuf, index);
    }

    index = 0;
}

/*
 * char *tmcapcnv(sd, ss)
 *
 * Routine to convert VT100 escapes to termcap format
 *
 * Processes only the \33[#m sequence (converts . files for termcap use)
 */
char *tmcapcnv(char *sd, char *ss)
{
    /* 
     * 0 = normal
     * 1 = \33
     * 2 = [
     * 3 = #
     */
    int tmstate = 0;

    /* The # in \33[#m */
    char tmdigit = 0;

    while(*ss) {
	switch(tmstate) {
	case 0:
	    if(*ss == '\33') {
		++tmstate;

		break;
	    }

	    *sd++ = *ss;
	    tmstate = 0;

	    break;
	case 1:
	    if(*ss != '[') {
		*sd++ = *ss;
		tmstate = 0;

		break;
	    }

	    ++tmstate;

	    break;
	case 2:
	    if(isdigit(*ss)) {
		tmdigit = *ss - '0';
		++tmstate;

		break;
	    }

	    if(*ss == 'm') {
		*sd++ = ST_END;
		tmstate = 0;

		break;
	    }

	    *sd++ = *ss;
	    tmstate = 0;

	    break;
	case 3:
	    if(*ss == 'm') {
		if(tmdigit) {
		    *sd++ = ST_START;
		}
		else {
		    *sd++ = ST_END;
		}

		tmstate = 0;

		break;
	    }
	    
	default:
	    *sd++ = *ss;
	    tmstate = 0;

	    break;
	}

	++ss;
    }

    /* NULL terminator */
    *sd = 0;

    return sd;
}

#endif

/*
 * beep()
 *
 * Routine to emit a beep if enabled (see no-beep in .larnopts)
 */
void beep()
{
    if(!nobeep) {
	*lnpt++ = '\7';
    }
}
