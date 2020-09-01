/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.topl.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "hack.h"
#include "hack.end.h"
#include "hack.mkobj.h"
#include "hack.pri.h"
#include "hack.termcap.h"
#include "hack.tty.h"

/* Leave room for "--More--" */
#define TOPLSZ (COLNO - 8)

char toplines[BUFSZ];

/* Set by pline; used by addtopl */
xchar tlx;
xchar tly;

struct topl {
    struct topl *next_topl;
    char *topl_text;
};

struct topl *old_toplines;
struct topl *last_redone_topl;

/* Max number of old toplines remembered */
#define OTLMAX 20

int doredotopl()
{
    if(last_redone_topl != NULL) {
        last_redone_topl = last_redone_topl->next_topl;
    }

    if(last_redone_topl == NULL) {
        last_redone_topl = old_toplines;
    }

    if(last_redone_topl != NULL) {
        strcpy(toplines, last_redone_topl->topl_text);
    }

    redotoplin();

    return 0;
}

void redotoplin()
{
    home();

    if(index(toplines, '\n') != 0) {
        cl_end();
    }

    putstr(toplines);
    cl_end();
    tlx = curx;
    tly = cury;
    flags.topl = 1;

    if(tly > 1) {
        more();
    }
}

void remember_topl()
{
    struct topl *tl;
    int cnt = OTLMAX;

    if((last_redone_topl != NULL) 
       && (strcmp(toplines, last_redone_topl->topl_text) == 0)) {
        return;
    }

    if((old_toplines != NULL)
       && (strcmp(toplines, old_toplines->topl_text))) {
        return;
    }

    last_redone_topl = 0;
    tl = (struct topl *)alloc((unsigned)((strlen(toplines) + sizeof(struct topl)) + 1));
    tl->next_topl = old_toplines;
    tl->topl_text = (char *)(tl + 1);
    strcpy(tl->topl_text, toplines);
    old_toplines = tl;

    while((cnt != 0) && (tl != NULL)) {
        --cnt;
        tl = tl->next_topl;
    }

    if((tl != NULL) && (tl->next_topl != NULL)) {
        free(tl->next_topl);
        tl->next_topl = 0;
    }
}

void addtopl(char *s)
{
    curs(tlx, tly);

    if((tlx + strlen(s)) > COLNO) {
        putsym('\n');
    }

    putstr(s);
    tlx = curx;
    tly = cury;
    flags.topl = 1;
}

/* spaceflag: TRUE if space required */
void xmore(boolean spaceflag)
{
    if(flags.topl != 0) {
        curs(tlx, tly);
        
        if((tlx + 8) > COLNO) {
            putsym('\n');
            ++tly;
        }
    }

    putstr("--More--");
    xwaitforspace(spaceflag);

    if((flags.topl != 0) && (tly > 1)) {
        home();
        cl_end();
        docorner(1, tly - 1);
    }

    flags.topl = 0;
}

void more()
{
    xmore(TRUE);
}

void cmore()
{
    xmore(FALSE);
}

void clrlin()
{
    if(flags.topl != 0) {
        home();
        cl_end();

        if(tly > 1) {
            docorner(1, tly - 1);
        }

        remember_topl();
    }

    flags.topl = 0;
}

void pline(char *line, ...)
{
    char pbuf[BUFSZ];
    char *bp = pbuf;
    char *tl;
    int n;
    int n0;

    if((line == NULL) || (*line == 0)) {
        return;
    }

    if(index(line, '%') == 0) {
        strcpy(pbuf, line);
    }
    else {
        va_list args;
        va_start(args, line);
        vsprintf(pbuf, line, args);
        va_end(args);
    }

    if((flags.topl == 1) && (strcmp(pbuf, toplines) == 0)) {
        return;
    }

    /* %% */
    nscr();

    /*
     * If there is room on the line, print message on same line.
     * But messages like "You die..." deserve their own line
     */
    n0 = strlen(bp);

    if((flags.topl == 1)
       && (tly == 1)
       && (((n0 + strlen(toplines)) + 3) < TOPLSZ)
       && (strncmp(bp, "You ", 4) != 0)) {
        strcat(toplines, "  ");
        strcat(toplines, bp);
        tlx +=2;
        
        return;
    }

    if(flags.topl == 1) {
        more();
    }

    remember_topl();
    toplines[0] = 0;

    while(n0 != 0) {
        if(n0 >= COLNO) {
            /* Look for appropriate cut point */
            n0 = 0;
            for(n = 0; n < COLNO; ++n) {
                if(bp[n] == ' ') {
                    n0 = n;
                }
            }

            if(n0 == 0) {
                for(n = 0; n < COLNO - 1; ++n) {
                    if(letter(bp[n]) == 0) {
                        n0 = n;
                    }
                }
            }

            if(n0 == 0) {
                n0 = COLNO - 2;
            }
        }

        tl = eos(toplines);
        strncpy(tl, bp, n0);
        tl[n0] = 0;
        bp += n0;

        /* Remove the trailing spaces, but leave one */
        while((n0 > 1) && (tl[n0 - 1] == ' ') && (tl[n0 - 2] == ' ')) {
            --n0;
            tl[n0] = 0;
        }

        n0 = strlen(bp);

        if((n0 != 0) && (tl[0] != 0)) {
            strcat(tl, "\n");
        }
    }

    redotoplin();
}

void putsym(char c)
{
    switch(c) {
    case '\b':
        backsp();
        
        return;
    case '\n':
        curx = 1;
        ++cury;

        if(cury > tly) {
            tly = cury;
        }

        break;
    default:
        ++curx;
        
        if(curx == COLNO) {
            putsym('\n');
        }
    }

    putchar(c);
}

void putstr(char *s)
{
    while(*s != 0) {
        putsym(*s);
        ++s;
    }
}
