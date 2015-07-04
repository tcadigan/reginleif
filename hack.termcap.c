/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.termcap.h"

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <term.h>

#include "alloc.h"
#include "config.h"
#include "hack.main.h"
#include "hack.topl.h"
#include "hack.tty.h"

/* Terminal baudrate; used by tputs */
short ospeed;
char tbuf[512];
char *HO;
char *CL;
char *CE;
char *UP;
char *CM;
char *ND;
char *XD;
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
    getty();

    tptr = (char *)alloc(1024);

    tbufptr = tbuf;
    tmp = getenv("TERM");

    if(tmp == NULL) {
        hack_error("Can't get TERM.");
    }

    if(tgetent(tptr, tmp) < 1) {
        hack_error("Unknown terminal type: %s.", tmp);
    }

    pc = tgetstr("pc", &tbufptr);
    if(pc != NULL) {
        PC = *pc;
    }

    BC = tgetstr("bc", &tbufptr);
    if(BC == NULL) {
        if(tgetflag("bs") == 0) {
            hack_error("Terminal must backspace.");
        }

        BC = tbufptr;
        tbufptr += 2;
        *BC = '\b';
    }

    HO = tgetstr("ho", &tbufptr);

    if((tgetnum("co") < COLNO) || (tgetnum("li") < (ROWNO + 2))) {
        hack_error("Screen must be at least %d by %d!", ROWNO + 2, COLNO);
    }

    CL = tgetstr("cl", &tbufptr);
    if(CL == NULL) {
        hack_error("Hack needs CL, CE, UP, ND, and no OS.");
    }

    CE = tgetstr("ce", &tbufptr);
    if(CE == NULL) {
        hack_error("Hack needs CL, CE, UP, ND, and no OS.");
    }

    ND = tgetstr("nd", &tbufptr);
    if(ND == NULL) {
        hack_error("Hack needs CL, CE, UP, ND, and no OS.");
    }

    UP = tgetstr("up", &tbufptr);
    if(UP == NULL) {
        hack_error("Hack needs CL, CE, UP, ND, and no OS.");
    }

    if(tgetflag("os") != 0) {
        hack_error("Hack needs CL, CE, UP, ND, and not OS.");
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
        hack_error("TERMCAP entry too big...\n");
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
            xputs(ND);
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
        hack_error("Tries to cmov from %d %d to %d %d\n", curx, cury, x, y);
    }

    xputs(tgoto(CM, x - 1, y - 1));
    cury = y;
    curx = x;
}

int xputc(int c)
{
    fputc(c, stdout);

    return 0;
}

void xputs(char *s)
{
    tputs(s, 1, xputc);
}

void cl_end()
{
    xputs(CE);
}

void hack_clear_screen()
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

void hack_bell()
{
    putsym('\007');
}
