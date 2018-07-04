/*
 * term.c: Handles the termcap info
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1990-1993
 *
 * See the COPYRIGHT file.
 */

#include "gb.h"
#include "str.h"
#include "types.h"
#include "vars.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#ifdef SYSV
#include <termio.h>

#else

#include <sgtty.h>
#endif

#define term_move(c, r) (term_put_termstring(tgoto(CM, (c), (r))))

extern int cursor_display;

int num_rows = -1;
int output_row;
int last_output_row = 0;
int num_columns = -1;
show ospeed;

/* The termcap variables */
static char *CM;
static char *UP;
static char *BC;
static char *CE;
static char *CR;
static char *NL;
static char *AL;
static char *DL;
static char *CS;
static char *DC;
/* static char *IC; */
/* static char *IM; */
/* static char *EI; */
static char *SO;
static char *SE;
static char *SF;
static char *ND;
static char *LE;
/* static char *BL; */
static char *US; /* Underline on */
static char *UE; /* Underline off */
static char *MD; /* Bold/bright mode on */
static char *ME; /* End all most */
static int SG;
static int boldface = false;
static int inverse = false;
static int underline = false;

/* Terminal structures saved ones and ones client plays with */
#ifdef TERMIO
static struct termio save_term;

#else

static struct sgttyb save_term;
static struct ltchars oldltchars;
static struct ltchars save_ltchars;
static struct tchars oldtchars;
static struct tchars save_tchars;
#endif

extern char *getenv(const char *);
extern char *strcat(char *, const char *);
extern char *tgetstr(char *, char **);
extern char *tgoto(char *, int, int);
/* extern into ioctl(int, int, ...); */
extern int printf(const char *, ...);
extern int tgetent(char *, char *);
extern int tgetflag(char *);
extern int tgetnum(char *);
extern int tputs(char *, int, int(*)(char));
extern int write(int, const void *, unsigned int);

int (*term_clear_to_eol)();
int (*term_cursor_left)();
int (*term_cursor_right)();
int (*term_delete)();
int (*term_scroll)();
void get_screen_size(void);
int term_ALDL_scroll(int l1, int l2, int n);
int term_BS_cursor_left(void);
int term_CE_clear_to_eol(void);
int term_CS_scroll(int l1, int l2, int n);
int term_DC_delete(void);
int term_LE_cursor_left(void);
int term_ND_cursor_right(void);
int term_SPACE_clear_to_eol(int x, int y);
void term_clear(int l1, int l2);
void term_move_cursor(int x, int y);
int term_null(void *);
int term_param_ALDL_scroll(int l1, int l2, int n);
void term_put_termstring(char *c);
int term_putchar(char c);
void term_boldface_off(void);
void term_boldface_on(void);
void term_standout_off(void);
void term_standout_on(void);
void term_underline_off(void);
void term_underline_on(void);

#if 0
int (*term_insert)(char *c);
int term_IC_insert(char *c);
int term_IMEI_insert(char *c);
#endif

/* Local test function, call from gb.c test_client() */
void term_test(void)
{
    char *p;
    char buf[NORMSIZ];
    char *q;
    char *r;

    *buf = '\0';

    for (q = buf, p = MD; *p; ++p) {
        r = dislay_char(*p, 0);
        strcat(q, r);
        q += strlen(r);
    }

    *q = '\0';
    msg("boldon: %s", buf);
    *buf = '\0';

    for (q = buf, p = ME; *p; ++p) {
        r = display_char(*p, 0);
        strcat(q, r);
        q += strlen(r);
    }

    *q = '\0';
    msg("boldoff: %s", buf);
    msg("%sbold%s", MD, ME);
}

/* Initialize term variables for future use */
void get_termcap(void)
{
    static char tinfo[NORMSIZ];
    static char terminfo[NORMSIZ];
    char *term_name;
    char *ptr;
    int state = 0;

    term_name = getenv("TERM");

    if(term_name == NULL) {
        printf("%s: No TERM variable set!\n", progname);

        exit(1);
    }

    state = tgetent(tinfo, term_name);

    if(state == 0) {
        printf("%s: No termcap entry for %s.\n", progname, term_name);

        exit(1);
    }
    else if(state < 0) {
        printf("%s: Could not find the terminfo database.\n", progname);

        exit(1);
    }

    ptr = terminfo;
    get_screen_size();

    CM = tgetstr("cm", &ptr);

    if(CM == NULL) {
        printf("%s: can't run on this terminal! NO cursor movement ability.\n", progname);

        exit(1);
    }

    CR = tgetstr("cr", &ptr);

    if(CR == NULL) {
        CR = "\r";
    }

    NL = tgetstr("nl", &ptr);

    if(NL == NULL) {
        NL = "\n";
    }

    CE = tgetstr("ce", &ptr);

    if(CE) {
        term_clear_to_eol = term_CE_clear_to_eol;
    }
    else {
        term_clear_to_eol = term_SPACE_clear_to_eol;
    }

    ND = tgetstr("nd", &ptr);

    if(ND) {
        term_cursor_right = term_ND_cursor_right;
    }
    else {
        term_cursor_right = term_null;
    }

    LE = tgetstr("le", &ptr);

    if(LE) {
        term_cursor_left = term_LE_cursor_left;
    }
    else if(tgetflag("bs")) {
        term_cursor_left = term_BS_cursor_left;
    }
    else {
        term_cursor_left = term_null;
    }

    SF = tgetstr("sf", &ptr);

    if(SF == NULL) {
        SF = NULL;
    }

    CS = tgetstr("cs", &ptr);

    if(CS) {
        term_scroll = term_CS_scroll;
    }
    else {
        AL = tgetstr("AL", &ptr);
        DL = tgetstr("DL", &ptr);

        if(AL && DL) {
            term_scroll = term_param_ALLD_scroll;
        }
        else {
            AL = tgetstr("al", &ptr);
            DL = tgetstr("dl", &ptr);

            if(AL && DL) {
                term_scroll = term_ALDL_scroll;
            }
            else {
                term_scroll = term_null;

                printf("%s: requires the ability to set a scroll region (termcap ability: cs) OR the ability to add and delete lines (termcap ability: AL and DL, or al and dl).", progname);

                exit(1);
            }
        }
    }

#if 0
    IC = tgetstr("ic", &ptr);

    if(IC) {
        term_insert = term_IC_insert;
    }
    else {
        IM = tgetstr("im", &ptr);

        if(IM) {
            EI = tgetstr("ei", &ptr);

            if(IM && EI) {
                term_insert = term_IMEI_insert;
            }
            else {
                term_insert = term_null;
            }
        }
        else {
            term_insert = term_null;
        }
    }
#endif

    DC = tgetstr("dc", &ptr);

    if(DC) {
        term_delete = term_DC_delete;
    }
    else {
        term_delete = term_null;
    }

    BC = tgetstr("bc", &ptr);
    UP = tgetstr("up", &ptr);
    SO = tgetstr("so", &ptr);
    SE = tgetstr("se", &ptr);

    if((SO == NULL) || (SE == NULL)) {
        SO = "";
        SE = "";
    }

    US = tgetstr("us", &ptr);
    UE = tgetstr("ue", &ptr);

    if((US == NULL) || (UE == NULL)) {
        US = "";
        UE = "";
    }

    MD = tgetstr("md", &ptr);
    ME = tgetstr("me", &ptr);

    if((MD == NULL) || (ME == NULL)) {
        MD = "";
        ME = "";
    }

    SG = tgetflag("sg");
}

int term_null(void *n)
{
    return 1;
}

int term_CE_clear_to_eol(void)
{
    term_put_termstring(CE);

    return 0;
}

int term_SPACE_clear_to_eol(int x, int y)
{
    char c = ' ';
    int i = 0;
    int cnt = num_columns - x;

    while (i < cnt) {
        term_putchar(c);
        ++i;
    }

    term_move_cursor(x, y);

    return 0;
}

int term_CS_scroll(int l1, int l2, int n)
{
    int i;
    char *thing;
    static int oldl1 = -1;
    static int oldl2 = -1;

    if(SF) {
        thing = SF;
    }
    else {
        thing = "\n";
    }

    if((oldl1 != l1) || (oldl2 != l2)) {
        term_put_term_string(tgoto(CS, l2, l1));
        oldl1 = l1;
        oldl2 = l2;
    }

    term_move_cursor(0, l2);

    for (i = 0; i < n; ++i) {
        term_put_termstring(thing);
    }

    return 0;
}

int term_ALDL_scroll(int l1, int l2, int n)
{
    int i;

    term_move_cursor(0, l1);

    for (i = 0; i < n; ++i) {
        term_put_termstring(DL);
    }

    term_move_cursor(0, l2 - n + 1);

    for (i = 0; i < n; ++i) {
        term_put_termstring(AL);
    }

    return 0;
}

int term_param_ALDL_scroll(int l1, int l2, int n)
{
    term_move_cursor(0, l1);
    term_put_termstring(tgoto(DL, n, n));
    term_move_cursor(0, l2 - n + 1);
    term_put_termstring(tgoto(AL, n, n));

    return 0;
}

#if 0
int term_IC_insert(char *c)
{
    term_put_termstring(IC);
    term_putchar(*c);

    return 0;
}

int term_IMEI_insert(char *c)
{
    term_put_termstring(IM);
    term_putchar(*c);
    term_put_termstring(EI);

    return 0;
}
#endif

int term_DC_delete(void)
{
    term_put_termstring(DC);

    return 0;
}

int term_ND_cursor_right(void)
{
    term_put_termstring(ND);

    return 0;
}

int term_LE_cursor_left(void)
{
    term_put_termstring(LE);

    return 0;
}

int term_BS_cursor_left(void)
{
    char c = '\010';

    term_putchar(c);

    return 0;
}

int term_putchar(char c)
{
    printf("%c", c);

    /* write(1, &c, 1); */

    return c;
}

void term_puts(char *str, int len)
{
    int i;

    for (i = 0; *str && (i < len); ++str, ++i) {
        /*
         * Allow for escape character below -mfw
         * if((*str < 32) && (*str != '\t')) {
         */
        if((*str < 32) && (*str != '\t') && (*str != 27)) {
            term_standout_on();
            term_putchar(*str + 64);
            term_standout_off();
        }
        else {
            term_putchar(*str);
        }
    }
}

void term_normal_mode(void)
{
    if(inverse) {
        term_standout_off();
    }

    if(underline) {
        term_underline_off();
    }

    if(boldface) {
        term_boldface_off();
    }
}

void term_toggle_standout(void)
{
    if(inverse) {
        term_standout_off();
    }
    else {
        term_standout_on();
    }
}

void term_standout_on(void)
{
    term_put_termstring(SO);
    inverse = true;
}

void term_standout_off(void)
{
    term_put_termstring(SE);
    inverse = false;
}

int term_standout_status(void)
{
    return inverse;
}

void term_toggle_underline(void)
{
    if(underline) {
        term_underline_off();
    }
    else {
        term_underline_on();
    }
}

void term_underline_on(void)
{
    term_put_termstring(US);
    underline = true;
}

void term_underline_off(void)
{
    term_put_termstring(UE);
    underline = false;
}

void term_toggle_boldface(void)
{
    if(boldface) {
        term_boldface_off();
    }
    else {
        term_boldface_on();
    }
}

void term_boldface_on(void)
{
    term_put_termstring(MD);
    boldface = true;
}

void term_boldface_off(void)
{
    term_put_termstring(ME);

    if(inverse) {
        term_standout_on();
    }

    if(underline) {
        term_underline_on();
    }

    boldface = false;
}

void term_put_termstring(char *c)
{
    /* Bug somewhere */
    if(*c == '\0') {
        return;
    }

    tputs(c, 1, term_putchar);
}

void term_move_cursor(int x, int y)
{
    cursor_display = false;
    term_move(x, y);
}

void term_clear_screen(void)
{
    if(GET_BIT(options, SCROLL_CLR)) {
        term_scroll(0, output_row, output_row + 1);
    }
    else {
        term_clear(0, output_row + 1);
    }

    last_output_row = 0;
}

void term_clear(int l1, int l2)
{
    int i = l1;

    while (i < l2) {
        term_move_cursor(0, i);
        term_clear_to_eol();
        ++i;
    }
}

void term_beep(int n)
{
    while (n) {
        write(1, "\007", 1);
        --n;
    }
}

void get_screen_size(void)
{
#ifdef TIOCGWINSZ
    struct winsize wsize;

    if(ioctl(0, TIOCGWINSZ, &wsize) >= 0) {
        if(wsize.ws_row != 0) {
            num_rows = wsize.ws_row;
        }

        if(wsize.ws_col != 0) {
            num_columns = wsize.ws_col;
        }
    }
    else {
        num_columns = tgetnum("co");
        num_rows = tgetnum("li");
    }

#else

    num_columns = tgetnum("co");
    num_rows = tgetnum("li");
#endif /* TIOCGWINSZ */

    /*
     * Let the variables "LINES and "COLUMNS" override the termcap
     * entry. Technically, this is a terminfo-ism, but I think the
     * vast majority of user will find it pretty handy.
     */
    if(getenv("COLUMNS") != NULL) {
        num_columns = atoi(getenv("COLUMNS"));
    }

    if(getenv("LINES") != NULL) {
        num_rows = atoi(getenv("LINES"));
    }

    if(num_columns <= 0) {
        num_columns = 80;
    }

    if(num_rows <= 0) {
        num_rows = 24;
    }

    /* Very minimum size allowed */
    if(num_columns < 40) {
        num_columns = 40;
    }

    if(num_rows < 12) {
        num_rows = 12;
    }
}

void term_mode_on(void)
{
#ifdef TERMIO
    ioctl(0, TCSETAW, &save_term);

#else

    ioctl(0, TIOCSETP, &save_term);
    ioctl(0, TIOCSLTC, &save_ltchars);
    ioctl(0, TIOCSETC, *save_tchars);
#endif
}

void term_mode_off(void)
{
#ifdef TERMIO
    struct termio s;

    ioctl(0, TCGETA, &s);
    save_term = s;
    ospeed = s.c_cflag & CBAUD;
    s.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL);
    s.c_oflag |= (OPOST | ONLCR | TAB3);
    s.c_oflag &= ~(OCRNL | ONOCR | ONLRET);
    s.c_cc[VMIN] = 1;
    s.c_cc[VTIME] = 0;
    ioctl(0, TCSETAW, &s);

#else

    struct sgttyb s;
    char k_word;
    char k_refresh;
    char k_literal;
    char k_flush;

    ioctl(0, TIOCGETP, &s);
    save_term = s;
    ospeed = s.sg_ospeed;
    s.sg_flags |= (CBREAK | XTABS);
    s.sg_flags &= ~ECHO;
    /* s.sg_flags |= CBREAK; */
    /* s.sg_flags &= ~(ECHO | XTABS); */

    ioctl(0, TIOCSETP, &s);

    if(ioctl(0, TIOCGLTC, *oldltchars) != -1) {
        save_ltchars = oldltchars;
        k_word = oldltchars.t_werasc;
        oldltchars.t_werasc = -1;
        k_refresh = oldltchars.t_rprntc;
        oldltchars.t_rprntc = -1;
        k_literal = oldltchars.t_lnextc;
        oldltchars.t_lnextc = -1;
        k_flush = oldltchars.t_flushc;
        oldltchars.t_flushc = -1;
        ioctl(0, TIOCSLTC, &oldltchars);
    }

    if(ioctl(0, TIOCGETC, &oldltchars) != -1) {
        save_tchars = oldtchars;
    }
#endif
}
