/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include <stdio.h>

/* For ROWNO and COLNO */
#include "config.h"

/* Terminal baudrate; used by tputs */
short ospeed;
char tbuf[512];
char *HO;
char *CL;
char *CE;
char *UP;
char *CM;
char *ND;
char *BC;
char *SO;
char *SE;
char PC = '\0';

void startup()
{
    char *tmp;
    char *tptr;
    char *tbufptr;
    char *pc;

    /* Sets ospeed */
    gettty();

    tptr = (char *)alloc(1024);

    tbufptr = tbuf;
    tmp = getenv("TERM");

    if(tmp == NULL) {
        error("Can't get TERM.");
    }

    if(tgetent(tptr, tmp) < 1) {
        error("Unknown terminal type: %s.", tmp);
    }

    pc = tgetstr("pc", &tbufptr);
    if(pc != NULL) {
        PC = *pc;
    }

    BC = tgetstr("bc", &tbufptr);
    if(BC == NULL) {
        if(tgetflag("bs") == NULL) {
            error("Terminal must backspace.");
        }

        BC = tbufptr;
        tbufptr += 2;
        *BC = '\b';
    }

    H0 = tgetstr("ho", &tbufptr);

    if((tgetnum("co") < COLNO) || (tgetnum("li") < (ROWNO + 2))) {
        error("Screen must be at least %d by %d!", ROWNO + 2, COLNO);
    }

    CL = tgetstr("cl", &tbufptr);
    if(CL == NULL) {
        error("Hack needs CL, CE, UP, ND, and no OS.");
    }

    CE = tgetstr("ce", &tbufptr);
    if(CE == NULL) {
        error("Hack needs CL, CE, UP, ND, and no OS.");
    }

    ND = tgetstr("nd", &tbufptr);
    if(ND == NULL) {
        error("Hack needs CL, CE, UP, ND, and no OS.");
    }

    UP = tgetstr("up", &tbufptr);
    if(UP == NULL) {
        error("Hack needs CL, CE, UP, ND, and no OS.");
    }

    if(tgetflag("os") != NULL) {
        error("Hack needs CL, CE, UP, ND, and not OS.");
    }

    CM = tgetstr("cm", &tbufptr);
    if(CM == NULL) {
        printf("Use of hack on terminals with CM is suspect...\n");
    }

    XD = tgetstr("xd", &tbufptr);
    SO = tgetstr("so", &tbufptr);
    SE = tgetstr("se", &tbufptr);

    if((SO == NULL) || (SE == NULL)) {
        SE = 0;
        SO = SE;
    }

    if((tbufptr - tbuf) > sizeof(tbuf)) {
        error("TERMCAP entry too big...\n");
    }

    free(tptr);
}

/* Cursor movements */
extern xchar curx;
extern xchar cury;

/*
 * Not xchar: perhaps xchar is unsigned
 * and curx - x would be unsigned as well
 */
void curs(int x, int y)
{
    if((y == cury) && (x == curx)) {
        return;
    }

    if((abs(cury - y) <= 3) && (abs(curx - x) <= 3)) {
        nocmov(x, y);
    }
    else if (((x <= 3) && (abs(cury - y) <= 3))
             || ((CM == NULL) && (x < abs(curx - x)))) {
        putchar('\r');
        curx = 1;
        nocmov(x, y);
    }
    else if(CM == NULL) {
        nocmov(x, y);
    }
    else {
        cmov(x, y);
    }
}

void nocmov(int x, int y)
{
    if(curx < x) {
        /* Go to the right. */
        while(curx < x) {
            xpots(ND);
            ++curx;
        }
    }
    else if(curx > x) {
        /* Go to the left. */
        xputs(BC);
        --curx;
    }

    if(cury > y) {
        if(UP != NULL) {
            /* Go up. */
            while(cury > y) {
                xputs(UP);
                --cury;
            }
        }
        else {
            cmov(x, y);
        }

        if(XD != NULL) {
            while(cury < y) {
                xputs(XD);
                ++cury;
            }
        }
        else {
            cmov(x, y);
        }
    }
}

void cmov(int x, int y)
{
    if(CM == NULL) {
        error("Tries to cmov from %d %d to %d %d\n", curx, cury, x, y);
    }

    xputs(tgoto(CM, x - 1, y - 1));
    cury = y;
    curx = x;
}

void xputc(char c)
{
    fputc(c, stdout);
}

void xputs(char *s)
{
    tputs(s, 1, xputc);
}

void cl_end()
{
    xputs(CE);
}

void clear_screen()
{
    xputs(CL);
    cury = 1;
    curx = 1;
}

void home()
{
    if(HO != NULL) {
        xputs(HO);
    }
    else {
        xputs(tgoto(CM, 0, 0));
    }

    cury = 1;
    curx = 1;
}

void standoutbeg()
{
    if(SO != NULL) {
        xputs(SO);
    }
}

void standoutend()
{
    if(SE != NULL) {
        xputs(SE);
    }
}

void backsp()
{
    xputs(BC);
    --curx;
}

void bell()
{
    putsym('\007');
}

void delay_output()
{
    /* Delay 40 ms - could also use a 'nap'-system call */
    tputs("40", 1, xputc);
}
