/*******************************************************************************
 * curses.h
 *
 * Header file for definitions and declarations for the PCcurses package. This
 * should be #include'd in all user programs.
 *******************************************************************************
 * This version of curses is based on ncurses, a curses version originally
 * written by Pavel Curtis at Cornell University. I have made substantial
 * changes to make it run on IBM PC's, and therefore consider myself free to
 * make it public domain.
 *     Bjorn Larrsson (bl@infovox.se)
 *******************************************************************************
 * 1.4:  Window origin mod in overlay() and overwrite(), on public (and very
 *       reasonable) request. Swapped #define'd values of OK and ERR; OK now 1,
 *       and ERR is 0/NULL. Conforms better to UNIX versions. borderchars[]
 *       removed from WINDOW struct since the border() functions were
 *       re-defined. Use of short wherever possible. Portability improvements,
 *       mispelled name of [w]setscrreg():                                900114
 * 1.3:  All modules lint-checked with MSC '-W3' and Turbo 'C' '-w -wpro'
 *       switches. Support for border(), wborder() functions:             881005
 * 1.2:  Rcsid[] string in all modules, for maintenance:                  881002
 * 1.1:  'Raw' output routines, revision info in curses.h:                880306
 * 1.0:  Release:                                                         870515
 *******************************************************************************
 */

/* General definitions */

#ifndef bool
#define bool char /* Boolean type */
#endif

#ifndef TRUE
#define TRUE 1 /* Booleans */
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define ERR 0 /* General error flag */
#define OK 1 /* General OK flag */

/* Functions defined as macros */

/* 
 * Using macros allows you to use #undef getch/ungetch in your programs to use
 * MSC/TRC getch() and ungetch() routines()
 */
#define getch() wgetch(stdscr)
#define ungetch(c) wungetch(c)
#define getyx(win, y, x) (y = (win)->_cury, x = (win)->_curx)

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

/*
 * Video attribute definitions. Colour definitions for CGA/EGC should also go
 * here...
 */
#define A_ALTCHARSET 0
#define A_BLINK 0x100
#define A_BLANK 0x200
#define A_BOLD 0x400
#define A_DIM 0
#define A_PROTECT 0
#define A_REVERSE 0x800
#define A_STANDOUT 0x1000
#define A_UNDERLINE 0x2000

/* Function and keypad definitions. Many are just for compatibility */
#define KEY_BREAK 0x101 /* Not on PC */
#define KEY_DOWN 0x102 /* The four arrow keys */
#define KEY_UP 0x103
#define KEY_LEFT 0x104
#define KEY_RIGHT 0x105
#define KEY_HOME 0x106 /* Home key */
#define KEY_BACKSPACE 0x107 /* Not on PC */
#define KEY_F0 0x108 /* Function keys. Space for 64 keys is reserved */
#define KEY_F(n) (KEY_F0 + (n))
#define KEY_DP 0x148 /* Not on PC */
#define KEY_IL 0x149 /* Insert line */
#define KEY_DC 0x14a /* Delete character */
#define KEY_IC 0x14b /* Insert character or enter insert mode */
#define KEY_EIC 0x14c /* Exit insert character mode */
#define KEY_CLEAR 0x14d /* Clear screen */
#define KEY_EOS 0x14e /* Clear to end of screen */
#define KEY_EOL 0x14f /* Clear to end of line */
#define KEY_SF 0x150 /* Scroll 1 line forward */
#define KEY_SR 0x151 /* Scroll 1 line backwards (reverse) */
#define KEY_NPAGE 0x152 /* Next page */
#define KEY_PPAGE 0x153 /* Previous page */
#define KEY_STAB 0x154 /* Set tab */
#define KEY_CTAB 0x155 /* Clear tab */
#define KEY_CATAB 0x156 /* Clear all tabs */
#define KEY_ENTER 0x157 /* Enter or send (unreliable) */
#define KEY_SRESET 0x158 /* Soft (partial) reset (unreliable) */
#define KEY_RESET 0x159 /* Reset or hard reset (unreliable) */
#define KEY_PRINT 0x15a /* Print or copy */
#define KEY_LL 0x15b /* Home down or bottom (lower left) */
#define KEY_ABORT 0x15c /* Abort/terminate key (any) */
#define KEY_SHELP 0x15d /* Short help */
#define KEY_LHELP 0x15e /* Long help */

/* Type declarations */
typedef struct {
    short _cury; /* Current pseudo-cursor */
    short _curx;
    short _maxy; /* Max coordinates */
    short _maxx;
    short _begy; /* Origin on screen */
    short _begx;
    short _flags; /* Window properties */
    short _attrs; /* Attributes of written characters */
    short _tabsize; /* Tab character size */
    bool _clear; /* Causes clear at next refresh */
    bool _leave; /* Leaves cursor as it happens */
    bool _scroll; /* Allows window scrolling */
    bool _nodelay; /* Input character wait flag */
    bool _keypad; /* Flags keypad key mode active */
    short **_line; /* Point to line pointer array */
    short *_minchng; /* First changed character in line */
    short *_maxchng; /* Last changed character in line */
    short _regtop; /* Top of scrolling region */
    short _regbottom; /* Bottom of scrolling region */
} WINDOW;

/* External variables */
extern int LINES; /* Terminal height */
extern int COLS; /* Terminal width */
extern WINDOW *curscr; /* The current screen image */
extern WINDOW *stdscr; /* The default screen window */

/* PCcurses function declarations */
extern int addch(); /* Put character in stdscr */
extern int addrawch(); /* Put character in stdscr, raw */
extern int addstr(); /* Put string in stdscr */
extern void attrset(); /* Set stdscr char attributes */
extern void attroff(); /* Clear attribute(s) stdscr */
extern void attron(); /* Add attribute(s) stdscr */
extern int baudrate(); /* Compatibility dummy */
extern void beep(); /* Sound bell */
extern void border(); /* Set non-std box characters */
extern void box(); /* Draw a box around a window */
extern void cbreak(); /* Set terminal cbreak mode */
extern void clear(); /* Clear stdscr */
extern void clearok(); /* Marks a window for screen clear */
extern int clrtobot(); /* Clear end of stdscr */
extern int clrtoeol(); /* Clear end of line in stdscr */
extern void crmode(); /* Set terminal cbreak mode */
extern void cursoff(); /* Turns off hardware cursor */
extern void curson(); /* Turns on hardware cursor */
extern void def_prog_mode(); /* Save TTY modes */
extern void def_shell_mode(); /* Compatibility dummy */
extern int delch(); /* Delete a character in stdscr */
extern int deleteln(); /* Delete a line in stdscr */
extern void delwin(); /* Delete a window or a subwindow */
extern void doupdate(); /* Update a physical screen */
extern void echo(); /* Set terminal echo mode */
extern int endwin(); /* Cleanup and finalization */
extern void erase(); /* Erase stdscr */
extern int erasechar(); /* Return character kill character */
extern int fixterm(); /* Compatibility dummy */
extern void flash(); /* Flash terminal screen */
extern void flushinp(); /* Kill pending keyboard input */
extern int getstr(); /* Get string to stdscr and buffer */
extern int gettmode(); /* compatibility dummy */
extern void idlok(); /* Use ins/del line (dummy) */
extern int initscr(); /* Curses initialization */
extern int inch(); /* Get character at stdscr */
extern int insch(); /* Insert character in stdscr */
extern int insrawch(); /* Insert character in stdscr, raw */
extern int insertln(); /* Insert new line in stdscr */
extern void keypad(); /* Marks a window for keypad usage */
extern int killchar(); /* Return line kill character */
extern char *longname(); /* Terminal description */
extern void leaveok(); /* Marks window for cursor 'leave' */
extern void meta(); /* Marks window for meta (dummy) */
extern int move(); /* Move cursor in stdscr */
extern int mvaddch(); /* Move & put character in stdscr */
extern int mvaddrawch(); /* Move & put character in stdscr, raw */
extern int mvaddstr(); /* Move & put string in stdscr */
extern int mvclrtobot(); /* Move & put end of stdscr */
extern int mvclrtoeol(); /* Move & put line end in stdscr */
extern int mvcur(); /* Move terminal cursor */
extern int mvdelch(); /* Move & delete a character in stdscr */
extern int mvdeleteln(); /* Move & delete a line in stdscr */
extern int mvgetch(); /* Move & get character to stdscr */
extern int mvgetstr(); /* Move & get string to stdscr */
extern int mvinch(); /* Move & get character at stdscr cursor */
extern int mvinsch(); /* Move & insert character in stdscr */
extern int mvinsrawch(); /* Move & insert raw character in stdscr */
extern int mvinsertln(); /* Move & insert new line in stdscr */
extern int mvprintw(); /* Move & print string in stdscr */
extern int mvscanw(); /* Move & get values via stdscr */
extern int mvwaddch(); /* Move & put character in a window */
extern int mvaddrawch(); /* Move & put character in a window, raw */
extern int mvwaddstr(); /* Move & put string in a window */
extern int mvwclrtobot(); /* Move & clear end of a window */
extern int mvwclrtoeol(); /* Move & clear line end in a window */
extern int mvwdelch(); /* Move & delete a character in a window */
extern int mvwdeleteln(); /* Move & delete a line in a window */
extern int mvwgetch(); /* Move & get character to a window */
extern int mvwgetstr(); /* Move & get string to a window */
extern int mvwinch(); /* Move & get character at window cursor */
extern int mvwinsch(); /* Move & insert character in a window */
extern int mvwinsrawch(); /* Move & insert raw character in window */
extern int mvwinsertln(); /* Move & insert new line in window */
extern int mvwin(); /* Move window */
extern int mvwprintw(); /* Move & print string in a window */
extern int mvwscanw(); /* Move & get values via a window */
extern WINDOW *newwin(); /* Create a window */
extern void nl(); /* Set terminal cr-crlf map mode */
extern void nocbreak(); /* Unset terminal cbreak mode */
extern void nocrmode(); /* Unset terminal cbreak mode */
extern void nodelay(); /* Marks window for no input wait */
extern void noecho(); /* Unset terminal echo mode */
extern void nonl(); /* Unsert terminal cr-crlf map mode */
extern void noraw(); /* Unset raw terminal mode */
extern void overlay(); /* Overlay one window on another */
extern void overwrite(); /* Overwrite one window on another */
extern int printw(); /* Print string in stdscr */
extern void raw(); /* Set raw terminal mode */
extern void refrbrk(); /* Set screen refresh break mode */
extern void refresh(); /* Refresh stdscr */
extern int resetterm(); /* Compatibility dummy */
extern int resetty(); /* Restore termianl I/O modes */
extern void reset_prog_mode(); /* Restore termianl I/O nodes */
extern void reset_shell_mode(); /* Set termianl to default modes */
extern int saveoldterm(); /* Compatibility dummy */
extern int saveterm(); /* Compatibility dummy */
extern int savetty(); /* Save terminal I/O modes */
extern int scanw(); /* Get values via stdscr */
extern void scroll(); /* Scroll region in window */
extern void scrollok(); /* Marks a window to allow scroll */
extern int setscrreg(); /* Define stdscr's scroll region */
extern int setterm(); /* Compatibility dummy */
extern int setupterm(); /* Set up a terminal (no-op) */
extern void standend(); /* Start normal characters in stdscr */
extern void standout(); /* Start standout characters in stdscr */
extern WINDOW *subwin(); /* Create a sub-window */
extern int tabsize(); /* Set/get tabsize of stdscr */
extern void touchwin(); /* Mark a window as modified */
extern char *unctrl(); /* Char-to-string converter */
extern int waddch(); /* Put character in a window */
extern int waddrach(); /* Put a character in a window, raw */
extern int waddstr(); /* Put string in a window */
extern void wattroff(); /* Clear attribute(s) in window */
extern void wattron(); /* Add attribute(s) in window */
extern void wattrset(); /* Set window character attributes */
extern void wborder(); /* Set non-std box characters */
extern int wbox(); /* Draw a box inside a window */
extern void wclear(); /* Clear a window */
extern int wclrtobot(); /* Clear end of a window */
extern int wclrtoeol(); /* Clear end of line in a window */
extern int wdelch(); /* Delete a character in a window */
extern int wdeleteln(); /* Delete a line in a window */
extern void werase(); /* Erase a window */
extern int wgetch(); /* Get character to a window */
extern int wgetstr(); /* Get string to window and buffer */
extern int winch(); /* Get character at window cursor */
extern int winsch(); /* Insert character in a window */
extern int winsrawch(); /* Insert raw character in a window */
extern int winsertln(); /* Insert new line in a window */
extern int wmove(); /* Move cursor in a window */
extern void wnoutrefresh(); /* Create screen image, w/o display */
extern int wprintw(); /* Print string in a window */
extern void wrefresh(); /* Refresh screen */
extern int wscanw(); /* Get values via a window */
extern int wsetscrreg(); /* Define a window's scroll region */
extern void wstandend(); /* Start normal characters in window */
extern void wstandout(); /* Start standout characters in window */
extern int wtabsize(); /* Set/get tabsize of a window */
extern int wungetch(); /* Character push-back */
