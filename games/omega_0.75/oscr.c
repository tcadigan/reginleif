/*
 * Omega (c) 1987-1989 by Laurence Raphael Brothers
 *
 * oscr.c
 *
 * Functions that use curses routines directly plus a few file i/o
 * stuff also some in ofile.c
 */

#include "oscr.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "oaux1.h"
#include "ofile.h"
#include "ogen1.h"
#include "oglob.h"
#include "oinv.h"
#include "omon.h"
#include "ospell.h"
#include "outil.h"

#ifdef EXCESSIVE_REDRAW
#define wclear werase
#endif

/* Note these variables are not exported to other files */
WINDOW *Levelw;
WINDOW *Dataw;
WINDOW *Flagw;
WINDOW *Timew;
WINDOW *Menuw;
WINDOW *Locw;
WINDOW *Morew;
WINDOW *Phasew;
WINDOW *Comwin;
WINDOW *Rightside;
WINDOW *Msg1w;
WINDOW *Msg2w;
WINDOW *Msg3w;
WINDOW *Msgw;
WINDOW *Packw;
WINDOW *Hideline[MAXITEMS];
WINDOW *Showline[MAXITEMS];

void phaseprint()
{
    wclear(Phasew);
    wprintw(Phasew, "Moon's Phase:\'");

    switch(Phase / 2) {
    case 0:
        wprintw(Phasew, "NEW");

        break;
    case 1:
    case 11:
        wprintw(Phasew, "CRESCENT");

        break;
    case 2:
    case 10:
        wprintw(Phasew, "1/4");

        break;
    case 3:
    case 9:
        wprintw(Phasew, "HALF");

        break;
    case 4:
    case 8:
        wprintw(Phasew, "3/4");

        break;
    case 5:
    case 7:
        wprintw(Phasew, "GIBBOUS");

        break;
    case 6:
        wprintw(Phasew, "FULL");

        break;
    }
}

void show_screen()
{
    int i;
    int j;
    int top;
    int bottom;
    
    wclear(Levelw);
    top = ScreenOffset;
    bottom = ScreenOffset + ScreenLength;
    top = max(0, top);
    bottom = min(bottom, LENGTH - 1);

    if(Current_Environment != E_COUNTRYSIDE) {
        for(j = top; j <= bottom; ++j) {
            for(i = 0; i < WIDTH; ++i) {
                wmove(Levelw, screenmod(j), i);

                if(loc_statusp(i, j, SEEN)) {
                    waddch(Levelw, getspot(i, j, FALSE));
                }
                else {
                    waddch(Levelw, ' ');
                }
            }
        }
    }
    else {
        for(j = top; j <= bottom; ++j) {
            for(i = 0; i < WIDTH; ++i) {
                wmove(Levelw, screenmod(j), i);

                if(Country[i][j].explored) {
                    waddch(Levelw, Country[i][j].current_terrain_type);
                }
                else {
                    waddch(Levelw, ' ');
                }
            }
        }
    }

    wrefresh(Levelw);
}

char mgetc()
{
    return wgetch(Msgw);
}

/* Case insensitive mgetc -- sends uppercase to lowercase */
char mcigetc()
{
    char c = wgetch(Msgw);

    if((c >= 'A') && (c <= 'Z')) {
        return (c + ('a' - 'A'));
    }
    else {
        return c;
    }
}

char menugetc()
{
    return wgetch(Menuw);
}

char lgetc()
{
    return wgetch(Levelw);
}

char ynq()
{
    char p = ' ';

    while((p != 'n') && (p != 'y') && (p != 'q') && (p != ESCAPE)) {
        p = wgetch(Msgw);
    }

    switch(p) {
    case 'y':
        wprintw(Msgw, "yes. ");

        break;
    case 'n':
        wprintw(Msgw, "no. ");

        break;
    case ESCAPE:
    case 'q':
        wprintw(Msgw, "quit. ");

        break;
    }

    wrefresh(Msgw);

    return p;
}

char ynq1()
{
    char p = ' ';

    while((p != 'n') && (p != 'y') && (p != 'q') && (p != ESCAPE)) {
        p = wgetch(Msg1w);
    }

    switch(p) {
    case 'y':
        wprintw(Msg1w, "yes. ");

        break;
    case 'n':
        wprintw(Msg1w, "no. ");

        break;
    case ESCAPE:
    case 'q':
        wprintw(Msg1w, "quit. ");

        break;
    }

    wrefresh(Msg1w);

    return p;
}

char ynq2()
{
    char p = ' ';

    while((p != 'n') && (p != 'y') && (p != 'q') && (p != ESCAPE)) {
        p = wgetch(Msg2w);
    }

    switch(p) {
    case 'y':
        wprintw(Msg2w, "yes. ");

        break;
    case 'n':
        wprintw(Msg2w, "no. ");

        break;
    case ESCAPE:
    case 'q':
        wprintw(Msg2w, "quit. ");

        break;
    }

    wrefresh(Msg2w);

    return p;
}

/* Puts up a morewait to allow reading if anything in top two lines */
void checkclear()
{
    int x1;
    int y;
    int x2;

    getyx(Msg1w, x1, y);
    getyx(Msg2w, x2, y);

    y = 0;
    x1 += y;
    
    if((x1 != 0) || (x2 != 0)) {
        morewait();
        wclear(Msg1w);
        wclear(Msg2w);
        wrefresh(Msg1w);
        wrefresh(Msg2w);
    }
}

/* For external call */
void clearmsg()
{
    wclear(Msg1w);
    wclear(Msg2w);
    wclear(Msg3w);
    Msgw = Msg1w;
    wrefresh(Msg1w);
    wrefresh(Msg2w);
    wrefresh(Msg3w);
}

void clearmsg3()
{
    wclear(Msg3w);
    wrefresh(Msg3w);
}

void clearmsg1()
{
    wclear(Msg1w);
    wclear(Msg2w);
    Msgw = Msg1w;
    wrefresh(Msg1w);
    wrefresh(Msg2w);
}

void erase_level()
{
    wclear(Levelw);
}

/* Direct printe to first msg line */
void print1(char *s)
{
    if(!gamestatusp(SUPPRESS_PRINTING)) {
        buffercycle(s);
        wclear(Msg1w);
        wprintw(Msg1w, "%s", s);
        wrefresh(Msg1w);
    }
}

/* For run-on messages -- print1 clears first... */
void nprint1(char *s)
{
    if(!gamestatusp(SUPPRESS_PRINTING)) {
        buffercycle(s);
        wprintw(Msg1w, "%s", s);
        wrefresh(Msg1w);
    }
}

/* Direct print to second msg line */
void print2(char *s)
{
    if(!gamestatusp(SUPPRESS_PRINTING)) {
        buffercycle(s);
        wclear(Msg2w);
        wprintw(Msg2w, "%s", s);
        wrefresh(Msg2w);
    }
}

/* For run-on messages -- print2 clears first... */
void nprint2(char *s)
{
    if(!gamestatusp(SUPPRESS_PRINTING)) {
        buffercycle(s);
        wprintw(Msg2w, "%s", s);
        wrefresh(Msg2w);
    }
}

/*
 * msg line 3 is not part of the region that mprint or printm can
 * reach. Typical use of print3 is for "you can't do that" type error
 * messages
 */
void print3(char *s)
{
    if(!gamestatusp(SUPPRESS_PRINTING)) {
        buffercycle(s);
        wclear(Msg3w);
        wprintw(Msg3w, "%s", s);
        wrefresh(Msg3w);
    }
}

/* For run-on messages -- print3 clears first... */
void nprint3(char *s)
{
    if(!gamestatusp(SUPPRESS_PRINTING)) {
        buffercycle(s);
        wprintw(Msg3w, "%s", s);
        wrefresh(Msg3w);
    }
}

/* 
 * Prints where ever cursor is in window, but check to see if it
 * should morewait and clear window
 */
void mprint(char *s)
{
    int x;
    int y;

    if(!gamestatusp(SUPPRESS_PRINTING)) {
        buffercycle(s);
        getyx(Msgw, y, x);
        y = 0;
        x += y;

        if((x + strlen(s)) >= WIDTH) {
            if(Msgw == Msg1w) {
                wclear(Msg2w);
                Msgw = Msg2w;
            }
            else {
                morewait();
                wclear(Msg1w);
                wclear(Msg2w);
                wrefresh(Msg2w);
                Msgw = Msg1w;
            }
        }

        wprintw(Msgw, "%s", s);
        waddch(Msgw, ' ');
        wrefresh(Msgw);
    }
}

void title()
{
    showmotd();

    /* Pseudo random seed */
    Seed = (int)time((long *)NULL);
    clear();
    refresh();
    showscores();
}

/*
 * Blanks out ith line of Menuw or Levelw. This is a serious kludge,
 * but nothing else seems to work the way I want it to!
 */
void hide_line(int i)
{
    touchwin(Hideline[i]);
    wrefresh(Hideline[i]);
}

/* Iniitalize, screen, windows */
void initgraf()
{
    int i;

    initscr();

    if(LINES < 24) {
        printf("Minimum screen size: 24 lines.");

        exit(0);
    }
    else {
        ScreenLength = LINES - 6;
    }

    Rightside = newwin(ScreenLength, 15, 2, 65);
    Msg1w = newwin(1, 80, 0, 0);
    Msg2w = newwin(1, 80, 1, 0);
    Msg3w = newwin(1, 80, 2, 0);
    Msgw = Msg1w;
    Morew = newwin(1, 15, 3, 65);
    Locw = newwin(1, 80, ScreenLength + 3, 0);
    Levelw = newwin(ScreenLength, 64, 3, 0);

    for(i = 0; i < MAXITEMS; ++i) {
        Hideline[i] = newwin(1, 64, i + 3, 0);
        Showline[i] = newwin(1, 64, i + 3, 0);
        wclear(Hideline[i]);
        wclear(Showline[i]);
    }

    Menuw = newwin(ScreenLength, 64, 3, 0);
    Packw = newwin(ScreenLength, 64, 3, 0);
    Dataw = newwin(2, 80, ScreenLength + 4, 0);
    Timew = newwin(2, 15, 4, 65);
    Phasew = newwin(2, 15, 6, 65);
    Flagw = newwin(4, 15, 9, 65);
    Comwin = newwin(8, 15, 14, 65);
    noecho();
    crmode();
    clear();
    title();
    clear();
    refresh();
}

void drawplayer()
{
    static int lastx = -1;
    static int lasty = -1;

    if(Current_Environment == E_COUNTRYSIDE) {
        wmove(Levelw, screenmod(lasty), lastx);
        waddch(Levelw, Country[lastx][lasty].current_terrain_type);
        wmove(Levelw, screenmod(Player.y), Player.x);
        waddch(Levelw, PLAYER);
    }
    else {
        if(inbounds(lastx, lasty)) {
            if(Player.status[BLINDED] > 0) {
                plotspot(lastx, lasty, FALSE);
            }
            else {
                plotspot(lastx, lasty, TRUE);
            }
        }

        wmove(Levelw, screenmod(Player.y), Player.x);

        if(!Player.status[INVISIBLE] || Player.status[TRUESIGHT]) {
            waddch(Levelw, PLAYER);
        }
    }

    lastx = Player.x;
    lasty = Player.y;
}

int litroom(int x, int y)
{
    if(Level->site[x][y].roomnumber < ROOMBASE) {
        return FALSE;
    }
    else {
        if(loc_statusp(x, y, LIT) || Player.status[ILLUMINATION]) {
            return 1;
        }
        else {
            return 0;
        }
    }
}

void drawvision(int x, int y)
{
    static int oldx = -1;
    static int oldy = -1;
    int i;
    int j;

    if(Current_Environment != E_COUNTRYSIDE) {
        if(Player.status[BLINDED]) {
            drawspot(oldx, oldy);
            drawspot(x, y);
            drawplayer();
        }
        else {
            if(Player.status[ILLUMINATION] > 0) {
                for(i = -2; i < 3; ++i) {
                    for(j = -2; j < 3; ++j) {
                        if(inbounds(x + i, y + j)) {
                            if(view_los_p(x + i, y + j, Player.x, Player.y)) {
                                dodrawspot(x + i, y + j);
                            }
                        }
                    }
                }
            }
            else {
                for(i = -1; i < 2; ++i) {
                    for(j = -1; j < 2; ++j) {
                        if(inbounds(x + i, y + j)) {
                            dodrawspot(x + i, y + j);
                        }
                    }
                }
            }

            drawplayer();

            /* Erase all monsters */
            drawmonsters(FALSE);

            /* Draw those now visible */
            drawmonsters(TRUE);
        }

        if(!gamestatusp(FAST_MOVE) || !optionp(JUMPMOVE)) {
            showcursor(Player.x, Player.y);
        }

        oldx = x;
        oldy = y;
    }
    else {
        for(i = -1; i < 2; ++i) {
            for(j = -1; j < 2; ++j) {
                if(inbounds(x + i, y + j)) {
                    wmove(Levelw, screenmod(y + j), x + i);
                    Country[x + i][y + j].explored = TRUE;
                    waddch(Levelw, Country[x + i][y + j].current_terrain_type);
                }
            }
        }
 
        drawplayer();
        showcursor(Player.x, Player.y);
    }
}

void showcursor(int x, int y)
{
    wmove(Levelw, screenmod(y), x);
    wrefresh(Levelw);
}

void levelrefresh()
{
    wrefresh(Levelw);
}

/* Draws a particular spot under if in line-of-sight */
void drawspot(int x, int y)
{
    char c;

    if(inbounds(x, y)) {
        c = getspot(x, y, FALSE);

        if(c != Level->site[x][y].showchar) {
            if(view_los_p(Player.x, Player.y, x, y)) {
                lset(x, y, SEEN);
                Level->site[x][y].showchar = c;
                putspot(x, y, c);
            }
        }
    }
}

/* Draws a parcticular spot regardless of line-of-sight */
void dodrawspot(int x, int y)
{
    char c;

    if(inbounds(x, y)) {
        c = getspot(x, y, FALSE);

        if(c != Level->site[x][y].showchar) {
            lset(x, y, SEEN);
            Level->site[x][y].showchar = c;
            putspot(x, y, c);
        }
    }
}

/* Write a blank to a spot if it is floor */
void blankoutspot(int i, int j)
{
    if(inbounds(i, j)) {
        lreset(i, j, LIT);

        if(Level->site[i][j].locchar == FLOOR) {
            Level->site[i][j].showchar = ' ';
            putspot(i, j, ' ');
        }
    }
}

/* Blank out a spot regardless */
void blotspot(int i, int j)
{
    if(inbounds(i, j)) {
        lreset(i, j, SEEN);
        Level->site[i][j].showchar = SPACE;

        if(!offscreen(j)) {
            wmove(Levelw, screenmod(j), i);
            wdelch(Levelw);
            winsch(Levelw, SPACE);
        }
    }
}

/* For displaying activity specifically at some point */
void plotspot(int x, int y, int showmonster)
{
    if(loc_statusp(x, y, SEEN)) {
        putspot(x, y, getspot(x, y, showmonster));
    }
    else {
        putspot(x, y, ' ');
    }
}

/* Puts c at x,y on screen. No fuss, no bother. */
void putspot(int x, int y, char c)
{
    if(!offscreen(y)) {
        wmove(Levelw, screenmod(y), x);
        waddch(Levelw, c);
    }
}

/* Regardless of line-of-sight, etc. draw a monster */
void plotmon(struct monster *m)
{
    if(!offscreen(m->y)) {
        wmove(Levelw, screenmod(m->y), m->x);
        waddch(Levelw, m->monchar);
    }
}

/* If display, displays monsters, otherwise erases them */
void drawmonsters(int display)
{
    pml ml;

    for(ml = Level->mlist; ml != NULL; ml = ml->next) {
        if(ml->m->hp > 0) {
            if(display) {
                if(view_los_p(Player.x, Player.y, ml->m->x, ml->m->y)) {
                    if(Player.status[TRUESIGHT] || !m_statusp(ml->m, M_INVISIBLE)) {
                        if((ml->m->level > 5)
                           && (ml->m->monchar != '@')
                           && (ml->m->monchar != '|')) {
                            wstandout(Levelw);
                        }

                        putspot(ml->m->x, ml->m->y, ml->m->monchar);
                        wstandend(Levelw);
                    }
                }
            }
            else {
                erase_monster(ml->m);
            }
        }
    }
}

/* Replace monster with what would be displayed if monster weren't there */
void erase_monster(struct monster *m)
{
    if(loc_statusp(m->x, m->y, SEEN)) {
        putspot(m->x, m->y, getspot(m->x, m->y, FALSE));
    }
    else {
        blotspot(m->x, m->y);
    }
}

/* Find apt char to display at some location */
char getspot(int x, int y, int showmonster)
{
    if(loc_statusp(x, y, SECRET)) {
        return WALL;
    }
    else {
        switch(Level->site[x][y].locchar) {
        case WATER:
            if(Level->site[x][y].creature == NULL) {
                return WATER;
            }
            else if(m_statusp(Level->site[x][y].creature, SWIMMING)) {
                return WATER;
            }
            else if(showmonster) {
                return Level->site[x][y].creature->monchar;
            }
            else {
                return WATER;
            }

            break;
        case CLOSED_DOOR:
        case LAVA:
        case FIRE:
        case ABYSS: /* These sites never show anything but their location chars */
            return Level->site[x][y].locchar;

            break;
        case RUBBLE:
        case HEDGE: /* Rubble and hedge don't show items on their location */
            if(showmonster && (Level->site[x][y].creature != NULL)) {
                if(m_statusp(Level->site[x][y].creature, M_INVISIBLE)
                   && !Player.status[TRUESIGHT]) {
                    return getspot(x, y, FALSE);
                }
                else {
                    return Level->site[x][y].creature->monchar;
                }
            }
            else {
                return Level->site[x][y].locchar;
            }

            break;
        default:
            /*
             * Everywhere else, first try to show monster, next show
             * items, next show location char
             */
            if(showmonster && (Level->site[x][y].creature != NULL)) {
                if(m_statusp(Level->site[x][y].creature, M_INVISIBLE)
                   && !Player.status[TRUESIGHT]) {
                    return getspot(x, y, FALSE);
                }
                else {
                    return Level->site[x][y].creature->monchar;
                }
            }
            else if(Level->site[x][y].things != NULL) {
                if(Level->site[x][y].things->next != NULL) {
                    return PILE;
                }
                else {
                    return Level->site[x][y].things->thing->objchar;
                }
            }
            else {
                return Level->site[x][y].locchar;
            }

            break;
        }
    }
}

void commanderror()
{
    wclear(Msg2w);
    wprintw(Msg3w, "%c : unknown command", Cmd);
    wrefresh(Msg3w);
}

void timeprint()
{
    wclear(Timew);
    wprintw(Timew, "%d:%d", showhour(), showminute());

    if(showminute() == 0) {
        waddch(Timew, '0');
    }

    if(hour() > 11) {
        wprintw(Timew, " PM\n");
    }
    else {
        wprintw(Timew, " AM\n");
    }
    
    wprintw(Timew, "%s", month());
    wprintw(Timew, " the %d", day());
    wprintw(Timew, "%s", ordinal(day()));
    wrefresh(Timew);
}

void comwinprint()
{
    wclear(Comwin);
    wprintw(Comwin, "Hit: %d  \n", Player.hit);
    wprintw(Comwin, "Dmg: %d  \n", Player.dmg);
    wprintw(Comwin, "Def: %d  \n", Player.defense);
    wprintw(Comwin, "Arm: %d  \n", Player.absorption);
    wprintw(Comwin, "Spd: %.2f  \n", 5.0 / Player.speed);
    wrefresh(Comwin);
}

void dataprint()
{
    wclear(Dataw);

    wprintw(Dataw,
            "HP:%d/%d MANA:%ld/%ld AU:%ld LEVEL:%d/%ld CARRY:%d/%d \n",
            Player.hp,
            Player.maxhp,
            Player.mana,
            Player.maxmana,
            Player.cash,
            Player.level,
            Player.xp,
            Player.itemweight,
            Player.maxweight);

    wprintw(Dataw,
            "STR:%d/%d CON:%d/%d DEX:%d/%d AGI:%d/%d INT:%d/%d POW:%d/%d   ",
            Player.str,
            Player.maxstr,
            Player.con,
            Player.maxcon,
            Player.dex,
            Player.maxdex,
            Player.agi,
            Player.maxagi,
            Player.iq,
            Player.maxiq,
            Player.pow,
            Player.maxpow);

    wrefresh(Dataw);
    wrefresh(Dataw);
}

/* Redraw everything currently displayed */
void redraw()
{
    touchwin(curscr);
    wrefresh(curscr);
}

/* Redraw each permanent window */
void xredraw()
{
    wclear(Rightside);
    wrefresh(Rightside);
    touchwin(Msgw);
    touchwin(Msg3w);
    touchwin(Levelw);
    touchwin(Timew);
    touchwin(Flagw);
    touchwin(Dataw);
    touchwin(Locw);
    touchwin(Morew);
    touchwin(Phasew);
    touchwin(Comwin);
    wrefresh(Msgw);
    wrefresh(Msg3w);
    wrefresh(Levelw);
    wrefresh(Timew);
    wrefresh(Flagw);
    wrefresh(Dataw);
    wrefresh(Locw);
    wrefresh(Morew);
    wrefresh(Phasew);
    wrefresh(Comwin);
}

void menuaddch(char c)
{
    waddch(Menuw, c);
    wrefresh(Menuw);
}

void morewait()
{
    int display = TRUE;
    char c;

    wclear(Morew);

    if(display) {
        wprintw(Morew, "***  MORE  ***");
    }
    else {
        wprintw(Morew, "+++  MORE  +++");
    }

    display = !display;
    wrefresh(Morew);
    c = wgetch(Msgw);

    while((c != SPACE) && (c != RETURN)) {
        wclear(Morew);

        if(display) {
            wprintw(Morew, "***  MORE  ***");
        }
        else {
            wprintw(Morew, "+++  MORE +++");
        }

        display = !display;
        wrefresh(Morew);
        c = wgetch(Msgw);
    }

    wclear(Morew);
    wrefresh(Morew);
}

int stillonblock()
{
    int display = TRUE;
    char c;

    wclear(Morew);

    if(display) {
        wprintw(Morew, "<<<STAY?>>>");
    }
    else {
        wprintw(Morew, ">>>STAY?<<<");
    }

    display = !display;
    wrefresh(Morew);
    c = wgetch(Msgw);

    while((c != SPACE) && (c != ESCAPE)) {
        wclear(Morew);

        if(display) {
            wprintw(Morew, "<<<STAY?>>>");
        }
        else {
            wprintw(Morew, ">>>STAY?<<<");
        }

        display = !display;
        wrefresh(Morew);
        c = wgetch(Msgw);
    }

    wclear(Morew);
    wrefresh(Morew);

    if(c == SPACE) {
        return 1;
    }
    else {
        return 0;
    }
}

void menuclear()
{
    wclear(Menuw);
    wrefresh(Menuw);
}

void menuspellprint(int i)
{
    int x;
    int y;
    
    getyx(Menuw, y, x);
    x = 0;
    y += x;

    if(y >= (ScreenLength - 2)) {
        morewait();
        wclear(Menuw);
    }

    wprintw(Menuw, "%s", spellid(i));
    wprintw(Menuw, "(%d)\n", Spells[i].powerdrain);
    wrefresh(Menuw);
}

void menuprint(char *s)
{
    int x;
    int y;

    getyx(Menuw, y, x);
    x = 0;
    y += x;
    
    if(y >= (ScreenLength - 2)) {
        morewait();
        wclear(Menuw);
    }

    wprintw(Menuw, "%s", s);
    wrefresh(Menuw);
}

void endgraf()
{
    clear();
    refresh();
    endwin();
}

void plotchar(char pyx, int x, int y)
{
    if(!offscreen(y)) {
        wmove(Levelw, screenmod(y), x);
        waddch(Levelw, pyx);
        wrefresh(Levelw);
    }
}

void draw_explosion(char pyx, int x, int y)
{
    int i;
    int j;

    for(j = 0; j < 3; ++j) {
        for(i = 0; i < 9; ++i) {
            plotchar(pyx, x + Dirs[0][i], y + Dirs[1][i]);
        }

        for(i = 0; i < 9; ++i) {
            plotchar(' ', x + Dirs[0][i], y + Dirs[1][i]);
        }
    }

    for(i = 0; i < 9; ++i) {
        plotspot(x + Dirs[0][i], y + Dirs[1][i], TRUE);
    }

    wrefresh(Levelw);
}

char *msgscanstring()
{
    static char instring[80];
    static char byte = 'x';
    int i = 0;

    instring[0] = 0;
    byte = mgetc();

    while(byte != '\n') {
        /* ^h or delete */
        if((byte == 8) || (byte == 127)) {
            instring[i] = 0;

            if(i > 0) {
                --i;
                dobackspace();
            }
        }
        else {
            instring[i] = byte;
            waddch(Msgw, byte);
            wrefresh(Msgw);
            ++i;
            instring[i] = 0;
        }

        byte = mgetc();
    }

    return instring;
}

void locprint(char *s)
{
    wclear(Locw);
    wprintw(Locw, "%s", s);
    wrefresh(Locw);
}

/* Draw everything whether visible or not */
void drawscreen()
{
    int i;
    int j;

    if(Current_Environment == E_COUNTRYSIDE) {
        for(i = 0; i < WIDTH; ++i) {
            for(j = 0; j < LENGTH; ++j) {
                putspot(i, j, Country[i][j].current_terrain_type);
            }
        }
    }
    else {
        for(i = 0; i < WIDTH; ++i) {
            for(j = 0; j < LENGTH; ++j) {
                putspot(i, j, Level->site[i][j].locchar);
            }
        }
    }

    wrefresh(Levelw);
    morewait();
    xredraw();
}

/* Selects a number up to range */
int getnumber(int range)
{
    int done = FALSE;
    int value = 1;
    char atom;

    if(range == 1) {
        return 1;
    }
    else {
        while(!done) {
            clearmsg1();
            wprintw(Msg1w, "How many? Change with < or >, ESCAPE to select:");
            mnumprint(value);
            atom = mgetc();

            while((atom != '<') && (atom != '>') && (atom != ESCAPE)) {
                atom = mgetc();
            }

            if((atom == '>') && (value < range)) {
                ++value;
            }
            else if((atom == '<') && (value > 1)) {
                --value;
            }
            else if(atom == ESCAPE) {
                done = TRUE;
            }
        }        
    }
    
    return value;
}

/* Reads a positive number up to 999999 */
int parsenum()
{
    int number[8];
    int place = -1;
    int i;
    int x;
    int y;
    int num = 0;
    int mult = 1;
    char byte = ' ';

    while((byte != ESCAPE) && (byte != '\n')) {
        byte = mgetc();

        if((byte == BACKSPACE) || (byte == DELETE)) {
            if(place  > -1) {
                number[place] = 0;
                --place;
                getyx(Msgw, y, x);
                wmove(Msgw, y, x - 1);
                waddch(Msgw, ' ');
                wmove(Msgw, y, x - 1);
                wrefresh(Msgw);
            }
        }
        else if((byte <= '9') && (byte >= '0') && (place < 7)) {
            ++place;
            number[place] = byte;
            waddch(Msgw, byte);
            wrefresh(Msgw);
        }
    }

    waddch(Msgw, ' ');

    if(byte == ESCAPE) {
        return ABORT;
    }
    else {
        for(i = place; i >= 0; --i) {
            num += (mult * (number[i] - '0'));
            mult *= 10;
        }

        return num;
    }
}

void maddch(char c)
{
    waddch(Msgw, c);
    wrefresh(Msgw);
}

void display_death(char *source)
{
    clear();
    printw("\n\n\n\n");
    printw("Reuiescat In Pace, ");
    printw("%s", Player.name);
    printw(" (%d points)", calc_points());
    strcpy(Str4, "Killed by ");
    strcat(Str4, source);
    printw("\n");
    printw("%s", Str4);
    printw(".");
    printw("\n\n\n\n\nHit any key to continue.");
    refresh();
    wgetch(stdscr);
    clear();
    refresh();
    extendlog(Str4, DEAD);
}

void display_win()
{
    clear();
    printw("\n\n\n\n");
    printw("%s", Player.name);

    if(Player.status[ADEPT]) {
        printw(" is a total master of omega with %d points!", FixedPoints);
        strcpy(Str4, "A total master of omega");
    }
    else {
        strcpy(Str4, "retired winner");
        printw(" triumphed in omega with %d points!", calc_points());
    }

    printw("\n\n\n\n\nHit any key to continue.");
    refresh();
    wgetch(stdscr);
    clear();
    refresh();

    if(Player.status[ADEPT]) {
        extendlog(Str4, BIGWIN);
    }
    else {
        extendlog(Str4, WIN);
    }
}

void display_quit()
{
    clear();
    printw("\n\n\n\n");
    printw("%s", Player.name);
    strcpy(Str4, "A quitter.");
    printw(" wimped out with %d points!", calc_points());
    printw("\n\n\n\n\nHit any key to continue.");
    refresh();
    wgetch(stdscr);
    clear();
    refresh();
    extendlog(Str4, QUIT);
}

void display_bigwin()
{
    clear();
    printw("\n\n\n\n");
    printw("%s", Player.name);
    strcpy(Str4, "retired, an Adept of Omega.");
    printw(" retired, an Adept of Omega with %d points!", FixedPoints);
    printw("\n\n\n\n\nHit any key to continue.");
    refresh();
    wgetch(stdscr);
    clear();
    refresh();
    extendlog(Str4, BIGWIN);
}

void mnumprint(int n)
{
    char numstr[20];
    sprintf(numstr, "%d", n);
    buffercycle(numstr);
    wprintw(Msgw, "%d", n);
    wrefresh(Msgw);
}

void menunumprint(int n)
{
    int x;
    int y;

    getyx(Menuw, y, x);
    x = 0;
    y += x;

    if(y >= (ScreenLength - 2)) {
        morewait();
        wclear(Menuw);
    }

    wprintw(Menuw, "%d", n);
    wrefresh(Menuw);
}

void dobackspace()
{
    int x;
    int y;

    getyx(Msgw, y, x);

    if(x > 0) {
        waddch(Msgw, ' ');
        wmove(Msgw, y, x - 1);
        waddch(Msgw, ' ');
        wmove(Msgw, y, x - 1);
    }

    wrefresh(Msgw);
}

void showflags()
{
    phaseprint();
    wclear(Flagw);

    if(Player.food < 0) {
        wprintw(Flagw, "Starving\n");
    }
    else if(Player.food <= 3) {
        wprintw(Flagw, "Weak\n");
    }
    else if(Player.food <= 10) {
        wprintw(Flagw, "Ravenous\n");
    }
    else if(Player.food <= 20) {
        wprintw(Flagw, "Hungry\n");
    }
    else if(Player.food <= 30) {
        wprintw(Flagw, "A mite peckish\n");
    }
    else if(Player.food <= 36) {
        wprintw(Flagw, "Content\n");
    }
    else if(Player.food <= 44) {
        wprintw(Flagw, "Satiated\n");
    }
    else {
        wprintw(Flagw, "Bloated\n");
    }

    if(Player.status[POISONED] > 0) {
        wprintw(Flagw, "Poisoned\n");
    }
    else {
        wprintw(Flagw, "Vigorous\n");
    }

    if(Player.status[DISEASED] > 0) {
        wprintw(Flagw, "Diseased\n");
    }
    else {
        wprintw(Flagw, "Healthy\n");
    }

    if(gamestatusp(MOUNTED)) {
        wprintw(Flagw, "Mounted\n");
    }
    else if(Player.status[LEVITATING]) {
        wprintw(Flagw, "Levitating\n");
    }
    else {
        wprintw(Flagw, "afoot\n");
    }

    wrefresh(Flagw);
}

void drawomega()
{
    int i;

    for(i = 0; i < 7; ++i) {
        clear();
        printw("\n\n\n");
        printw("\n                                    *****");
        printw("\n                               ******   ******");
        printw("\n                             ***             ***");
        printw("\n                           ***                 ***");
        printw("\n                          **                     **");
        printw("\n                         ***                     ***");
        printw("\n                         **                       **");
        printw("\n                         **                       **");
        printw("\n                         ***                     ***");
        printw("\n                          ***                   ***");
        printw("\n                            **                 **");
        printw("\n                       *   ***                ***   *");
        printw("\n                        ****                    ****");
        refresh();
        clear();
        printw("\n\n\n");
        printw("\n                                    +++++");
        printw("\n                               ++++++   ++++++");
        printw("\n                             +++             +++");
        printw("\n                           +++                 +++");
        printw("\n                          ++                     ++");
        printw("\n                         +++                     +++");
        printw("\n                         ++                       ++");
        printw("\n                         ++                       ++");
        printw("\n                         +++                     +++");
        printw("\n                          +++                   +++");
        printw("\n                            ++                 ++");
        printw("\n                       +   +++                +++   +");
        printw("\n                        ++++                    ++++");
        refresh();
        clear();
        printw("\n\n\n");
        printw("\n                                    .....");
        printw("\n                               ......   ......");
        printw("\n                             ...             ...");
        printw("\n                           ...                 ...");
        printw("\n                          ..                     ..");
        printw("\n                         ...                     ...");
        printw("\n                         ..                       ..");
        printw("\n                         ..                       ..");
        printw("\n                         ...                     ...");
        printw("\n                          ...                   ...");
        printw("\n                            ..                 ..");
        printw("\n                       .   ...                ...   .");
        printw("\n                        ....                    ....");
    }
}

/*
 * y is an absolute coordinate. ScreenOffset is the upper left had
 * corner of the current screen in absolute coordinates
 */
void screencheck(int y)
{
    if(((y - ScreenOffset) < (ScreenLength / 8))
       || ((y - ScreenOffset) > ((7 * ScreenLength) / 8))) {
        ScreenOffset = y - (ScreenLength / 2);
        show_screen();
    }
}

void spreadroomlight(int x, int y, int roomno)
{
    lightspot(x, y);

    if(inbounds(x + 1, y)) {
        if(!loc_statusp(x + 1, y, LIT)
           && (Level->site[x + 1][y].roomnumber == roomno)) {
            spreadroomlight(x + 1, y, roomno);
        }
        else {
            lightspot(x + 1, y);
        }
    }

    if(inbounds(x, y + 1)) {
        if(!loc_statusp(x, y + 1, LIT)
           && (Level->site[x][y + 1].roomnumber == roomno)) {
            spreadroomlight(x, y + 1, roomno);
        }
        else {
            lightspot(x, y + 1);
        }
    }

    if(inbounds(x - 1, y)) {
        if(!loc_statusp(x - 1, y, LIT)
           && (Level->site[x - 1][y].roomnumber == roomno)) {
            spreadroomlight(x - 1, y, roomno);
        }
        else {
            lightspot(x - 1, y);
        }
    }

    if(inbounds(x, y - 1)) {
        if(!loc_statusp(x, y - 1, LIT)
           && (Level->site[x][y - 1].roomnumber == roomno)) {
            spreadroomlight(x, y - 1, roomno);
        }
        else {
            lightspot(x, y - 1);
        }
    }
}

/* Illuminate one spot at x,y */
void lightspot(int x, int y)
{
    char c;
    lset(x, y, LIT);
    lset(x, y, SEEN);
    c = getspot(x, y, FALSE);
    Level->site[x][y].showchar = c;
    putspot(x, y, c);
}

void spreadroomdark(int x, int y, int roomno)
{
    if(inbounds(x, y)) {
        if(loc_statusp(x, y, LIT) && (Level->site[x][y].roomnumber == roomno)) {
            blankoutspot(x, y);
            spreadroomdark(x + 1, y, roomno);
            spreadroomdark(x, y + 1, roomno);
            spreadroomdark(x - 1, y, roomno);
            spreadroomdark(x, y - 1, roomno);
        }
    }
}

void display_pack()
{
    int i;
    int x;
    int y;

    if(Player.packptr < 1) {
        print3("Pack is empty.");
    }
    else {
        wclear(Packw);
        wprintw(Packw, "Items in Pack:");

        for(i = 0; i < Player.packptr; ++i) {
            wprintw(Packw, "\n");
            getyx(Packw, y, x);
            x = 0;
            y += x;

            if(y >= (ScreenLength - 2)) {
                wrefresh(Packw);
                morewait();
                wclear(Packw);
            }

            waddch(Packw, i + 'A');
            waddch(Packw, ':');
            wprintw(Packw, "%s", itemid(Player.pack[i]));
        }

        wrefresh(Packw);
    }
}

void display_possessions()
{
    int i;

    wclear(Menuw);

    for(i = 0; i < MAXITEMS; ++i) {
        display_inventory_slot(i, FALSE);
    }

    move_slot(1, 0, MAXITEMS);
    wrefresh(Menuw);
}

void display_inventory_slot(int slotnum, int topline)
{
    WINDOW *W;
    char usechar = ')';

    if(Player.possessions[slotnum] != NULL) {
        if(Player.possessions[slotnum]->used) {
            usechar = '>';
        }
    }

    if(topline) {
        W = Msg3w;
    }
    else {
        W = Showline[slotnum];
        hide_line(slotnum);
    }

    wclear(W);

    switch(slotnum) {
    case O_UP_IN_AIR:
        wprintw(W, "-- Object \'up in air\':");

        break;
    case O_READY_HAND:
        wprintw(W, "-- a%c ready hand: ", usechar);

        break;
    case O_WEAPON_HAND:
        wprintw(W, "-- b%c weapon hand: ", usechar);

        break;
    case O_LEFT_SHOULDER:
        wprintw(W, "-- c%c left shoulder: ", usechar);

        break;
    case O_RIGHT_SHOULDER:
        wprintw(W, "-- d%c right shoulder: ", usechar);

        break;
    case O_BELT1:
        wprintw(W, "-- e%c belt: ", usechar);

        break;
    case O_BELT2:
        wprintw(W, "-- f%c belt: ", usechar);

        break;
    case O_BELT3:
        wprintw(W, "-- g%c belt: ", usechar);

        break;
    case O_SHIELD:
        wprintw(W, "-- h%c shield: ", usechar);

        break;
    case O_ARMOR:
        wprintw(W, "-- i%c armor: ", usechar);

        break;
    case O_BOOTS:
        wprintw(W, "-- j%c boots: ", usechar);

        break;
    case O_CLOAK:
        wprintw(W, "-- k%c cloak: ", usechar);

        break;
    case O_RING1:
        wprintw(W, "-- l%c finger: ", usechar);

        break;
    case O_RING2:
        wprintw(W, "-- m%c finger: ", usechar);

        break;
    case O_RING3:
        wprintw(W, "-- n%c finger: ", usechar);

        break;
    case O_RING4:
        wprintw(W, "-- o%c finger: ", usechar);

        break;
    }

    if(Player.possessions[slotnum] == NULL) {
        wprintw(W, "(slot vacant)");
    }
    else {
        wprintw(W, "%s", itemid(Player.possessions[slotnum]));
    }

    wrefresh(W);
}

int move_slot(int oldslot, int newslot, int maxslot)
{
    if((newslot > 0) && (newslot < maxslot)) {
        wmove(Showline[oldslot], 0, 0);
        waddstr(Showline[oldslot], "--");
        wrefresh(Showline[oldslot]);
        wmove(Showline[newslot], 0, 0);
        wstandout(Showline[newslot]);
        waddstr(Showline[newslot], ">>");
        wstandend(Showline[newslot]);
        wrefresh(Showline[newslot]);

        return newslot;
    }
    else {
        return oldslot;
    }
}

void display_option_slot(int slot)
{
    hide_line(slot);
    wclear(Showline[slot]);

    switch(slot) {
    case 1:
        wprintw(Showline[slot], "-- Option BELLICOSE [TF]: ");

        if(optionp(BELLICOSE)) {
            wprintw(Showline[slot], "(now T) ");
        }
        else {
            wprintw(Showline[slot], "(now F) ");
        }

        break;
    case 2:
        wprintw(Showline[slot], "-- Option JUMPMOVE [TF]: ");

        if(optionp(JUMPMOVE)) {
            wprintw(Showline[slot], "(now T) ");
        }
        else {
            wprintw(Showline[slot], "(now F) ");
        }

        break;
    case 3:
        wprintw(Showline[slot], "-- Option RUNSTOP [TF]: ");

        if(optionp(RUNSTOP)) {
            wprintw(Showline[slot], "(now T) ");
        }
        else {
            wprintw(Showline[slot], "(now F) ");
        }

        break;
    case 4:
        wprintw(Showline[slot], "-- Option PICKUP [TF]: ");

        if(optionp(PICKUP)) {
            wprintw(Showline[slot], "(now T) ");
        }
        else {
            wprintw(Showline[slot], "(now F) ");
        }

        break;
    case 5:
        wprintw(Showline[slot], "-- Option CONFIRM [TF]: ");

        if(optionp(CONFIRM)) {
            wprintw(Showline[slot], "(now T) ");
        }
        else {
            wprintw(Showline[slot], "(now F) ");
        }

        break;
    case 6:
        wprintw(Showline[slot], "-- Option TOPINV [TF]: ");

        if(optionp(TOPINV)) {
            wprintw(Showline[slot], "(now T) ");
        }
        else {
            wprintw(Showline[slot], "(now F) ");
        }

        break;
    case 7:
        wprintw(Showline[slot], "-- Option PACKADD [TF]: ");

        if(optionp(PACKADD)) {
            wprintw(Showline[slot], "(now T) ");
        }
        else {
            wprintw(Showline[slot], "(now F) ");
        }

        break;
    case 8:
        wprintw(Showline[slot], "-- Option VERBOSITY [(T)erse, (M)edium, (V)erbose]: (now ");

        if(Verbosity == VERBOSE) {
            wprintw(Showline[slot], "Verbose)");
        }
        else if(Verbosity == MEDIUM) {
            wprintw(Showline[slot], "Medium)");
        }
        else {
            wprintw(Showline[slot], "Terse)");
        }

        break;
    case 9:
        wprintw(Showline[slot], "-- Option SEARCHNUM [0 > x > 10]: (now %d", Searchnum);

        break;
    }

    wrefresh(Showline[slot]);
}

void display_options()
{
    int i;

    menuclear();
    hide_line(0);

    for(i = 1; i <= NUMOPTIONS; ++i) {
        display_option_slot(i);
    }
}

/* Nya ha ha ha ha haaaa... */
void deathprint()
{
    mgetc();
    waddch(Msgw, 'D');
    mgetc();
    waddch(Msgw, 'e');
    mgetc();
    waddch(Msgw, 'a');
    mgetc();
    waddch(Msgw, 't');
    mgetc();
    waddch(Msgw, 'h');
    mgetc();
}

void clear_if_necessary()
{
    int x;
    int y;

    getyx(Msg1w, y, x);
    y = 0;
    x += y;

    if(x != 0) {
        wclear(Msg1w);
        wrefresh(Msg1w);
    }

    getyx(Msg2w, y, x);

    if(x != 0) {
        wclear(Msg2w);
        wrefresh(Msg2w);
    }

    getyx(Msg3w, y, x);

    if(x != 0) {
        wclear(Msg3w);
        wrefresh(Msg3w);
    }
}

void buffercycle(char *s)
{
    int i;

    for(i = 9; i > 0; --i) {
        strcpy(Stringbuffer[i], Stringbuffer[i - 1]);
    }

    strcpy(Stringbuffer[0], s);
}

void bufferprint()
{
    int i = 0;

    clearmsg();
    wprintw(Msg1w, "^p for last message, anything else ot quit.");
    wrefresh(Msg1w);
    wclear(Msg2w);
    wprintw(Msg2w, "%s", Stringbuffer[i]);
    wrefresh(Msg2w);
    ++i;

    if(i > 9) {
        i = 0;
    }

    while(mgetc() == 16) {
        wclear(Msg2w);
        wprintw(Msg2w, "%s", Stringbuffer[i]);
        wrefresh(Msg2w);
        ++i;

        if(i > 9) {
            i = 0;
        }
    }

    clearmsg();
    showcursor(Player.x, Player.y);
}
