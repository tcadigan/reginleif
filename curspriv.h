/*******************************************************************************
 * curspriv.h
 * 
 * Header file for definitions and declarations for the PCcurses package. These
 * definitions should not be gnerally accessible to programmers.
 *******************************************************************************
 * This version of curses is based on ncurses, a curses version originally
 * written by Pavel Curtis at Cornell University. I have made substantial
 * changes to make it run on IBM PC's, and therefore consider myself free to
 * make it public domain.
 *******************************************************************************
 * 1.4:  ERR/OK redefined in curses.h. Use of short whever possible. Portability
 *       improvements:                                                    900114
 * 1.3:  All modules lint-checked with MSC '-W3' and Turbo 'C' '-w -w-pro'
 *       switches:                                                        881005
 * 1.2:  Support (by #ifdef UCMASM) for uppercase-only assembly routine names.
 *       If UCMASM is defined, all assembler names are #defined as upper case.
 *       Not needed if you do "MASM /MX". Also missing declaration of
 *       cursesscroll(). Fixes thanks to N.D. Pentcheff:                  881002
 * 1.1:  Add _chadd() for raw output routines:                            880306
 * 1.0:  Release:                                                         870515
 *******************************************************************************
 */

#define CURSES_RCS_ID "@(#)PCcurses     v.1.4  - 900114"

/* Window properties */
#define _SUBWIN 1 /* Window is a subwindow */
#define _ENDLINE 2 /* Last winline is last screen line */
#define _FULLWIN 4 /* Window fills screen */
#define _SCROLLWIN 8 /* Window lwr rgt is screen lwr rgt */

/* Miscellaneous */
#define _INBUFSIZ 200 /* Size of terminal input buffer */
#define _NO_CHANGE -1 /* Flags line edge unchanged */

#define _BREAKCHAR 0x03 /* ^C character */
#define _DCCHAR 0x08 /* Delete Char character (BS) */
#define _DLCHAR 0x1b /* Delete Line character (ESC) */
#define _GOCHAR 0x11 /* ^Q character */
#define _PRINTCHAR 0x10 /* ^P character */
#define _STOP_CHAR 0x13 /* ^S character */
#define NUNGETCH 10 /* Maximum number characters to ungetch() */

/* Character mask definitions */
#define CHR_MSK ((int)0x00FF) /* ASCIIZ character mask */
#define ATR_MSK ((int)0xFF00) /* Attribute mask */
#define ATR_NRM ((int)0x0000) /* No special attributes */

/* Type declarations */
typedef struct {
    WINDOW *tmpwin; /* Window used for updates */
    short cursrow; /* Position of physical cursor */
    short curscol;
    bool autocr; /* If lf -> crlf */
    bool cbreak; /* If terminal unbuffered */
    bool echo; /* If terminal echo */
    bool raw; /* If terminal raw mode */
    bool refrbrk; /* If premature refresh brk allowed */
    bool orgcbr; /* Original MSDOS ^-BREAK setting */
} cursv;

/* External variables */
extern cursv _cursvar; /* Curses variables */

/* 'C' standard library function declarations */
#ifndef __STDIO_H
extern char *calloc();
extern char *malloc();
extern void free();
extern int sprintf();
extern int sscanf();
#endif

/* Curses internal functions, not to be used by programmers */

/* 
 * #define UCMASM if your version of MASM does not support the '/MX' switch, or
 * if you use another assembler
 */
#ifdef UCMASM
#define _cursescattr _CURSESCATTR
#define _cursescmode _CURSESCMODE
#define _cursescursor _CURSESCURSOR
#define _cursesgcb _CURSESGCB
#define _cursesgcmode _CURSESGCMODE
#define _cursesgcold _CURSESGCOLS
#define _curseskey _CURSESKEY
#define _cursesscroll _CURSESSCROLL
#define _curseskeytst _CURSESKEYTST
#define _cursesputc _CURSESPUTC
#define _cursessch _CURSESSCB
#endif

extern int _chadd();
extern void _cursescattr();
extern void _cursescmode();
extern void _cursescursor();
extern int _cursesgcb();
extern int _cursesgcmode();
extern int _cursesgcols();
extern int _curseskey();
extern bool _curseskeytst();
extern void _cursesscroll();
extern bool _cursespendch();
extern void _cursesputc();
extern void _cursesscb();
