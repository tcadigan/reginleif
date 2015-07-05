/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.tty.h"

#include <stdio.h>
#include <stdlib.h>
#include <sgtty.h>
#include <termios.h>

#include "hack.h"
#include "hack.mkobj.h"
#include "hack.termcap.h"
#include "hack.topl.h"

struct termios inittyb;
struct termios curttyb;
extern speed_t ospeed;

void getty()
{
    tcgetattr(0, &inittyb);
    tcgetattr(0, &curttyb);

    ospeed = cfgetospeed(&inittyb);
    
    if(ospeed <= B300) {
        flags.oneline = 1;
    }

    /* getioctls(); */
    xtabs();
}

/* Reset terminal to original state */
void settty(char *s)
{
    hack_clear_screen();

    if(s != NULL) {
        printf("%s", s);
    }

    fflush(stdout);

    if(tcsetattr(0, TCSANOW, &inittyb) == -1) {
        puts("Cannot change tty");
    }

    if(inittyb.c_lflag & ECHO) {
        flags.echo = ON;
    }
    else {
        flags.echo = OFF;
    }

    if(inittyb.c_lflag & ICANON) {
        flags.cbreak = ON;
    }
    else {
        flags.cbreak = OFF;
    }

    /* setioctls(); */
}

void setctty()
{
    if(tcsetattr(0, TCSANOW, &curttyb) == -1) {
        puts("Cannot change tty");
    }
}

void setftty()
{
    int ef;

    if(flags.echo == ON) {
        ef = ECHO;
    }
    else {
        ef = 0;
    }

    int cf;
    if(flags.cbreak == ON) {
        cf = ICANON;
    }
    else {
        cf = 0;
    }

    int change = 0;

    if((curttyb.c_lflag & ECHO) != ef) {
        curttyb.c_lflag &= ~ECHO;
        curttyb.c_lflag |= ef;
        ++change;
    }

    if((curttyb.c_lflag & ICANON) != cf) {
        curttyb.c_lflag &= ~ICANON;
        curttyb.c_lflag |= cf;
        ++change;
    }

    if(change) {
        setctty();
    }
}

/*
 * Always want to expand tabs, or to send a clear line character
 * before printing something on topline
 */
void xtabs()
{
    
    /* gtty(0, &curttyb); */
    curttyb.c_oflag |= XTABS;

    setctty();
}

#ifdef LONG_CMD
void cbreak(int n)
{
    /* gtty(0, &curttyb); */
    if(n == ON) {
        curttyb.sg_flags |= CBREAK;
    }
    else {
        curttyb.sg_flags &= ~CBREAK;
    }

    setctty();
}
#endif

void getlin(char *bufp)
{
    char *obufp = bufp;
    int c;

    /* nonempty, no --More-- required */
    flags.topl = 2;

    while(1) {
        fflush(stdout);

        c = getchar();
        if(c == EOF) {
            *bufp = 0;
            
            return;
        }

        if(c == '\b') {
            if(bufp != obufp) {
                --bufp;
                /* putsym converts \b */
                putstr("\b \b");
            }
            else {
                hack_bell();
            }
        }
        else if(c == '\n') {
            *bufp = 0;
       
            return;
        }
        else {
            *bufp = c;
            bufp[1] = 0;
            putstr(bufp);

            if(((bufp - obufp) < (BUFSZ - 1)) && ((bufp - obufp) < COLNO)) {
                ++bufp;
            }
        }
    }
}

void getret()
{
    xgetret(TRUE);
}

void cgetret()
{
    xgetret(FALSE);
}

/* spaceflag: TRUE if space (return) required */
void xgetret(boolean spaceflag)
{
    if(flags.cbreak) {
        printf("\nHit %s to continue: ", "space");
    }
    else {
        printf("\nHit %s to continue: ", "return");
    }

    xwaitforspace(spaceflag);
}

/* Tell the outside world what char he used */
char morc;

void xwaitforspace(boolean spaceflag)
{
    int c;
    
    fflush(stdout);
    morc = 0;

    c = getchar();
    while(c != '\n') {
        if(c == EOF) {
            settty("End of input?\n");

            exit(0);
        }

        if(flags.cbreak) {
            if(c == ' ') {
                break;
            }
            
            if((spaceflag == 0) && (letter(c) != 0)) {
                morc = c;
                
                break;
            }
        }

        c = getchar();
    }
}

unsigned char *parse()
{
    static unsigned char in_line[COLNO];
    int foo;

    flags.move = 1;

    if(Invis == 0) {
        curs(u.ux, u.uy + 2);
    }
    else {
        home();
    }

    fflush(stdout);

    foo = getchar();
    while((foo >= '0') && (foo <= '9')) {
        multi += (((10 * multi) + foo) - '0');

        foo = getchar();
    }

    if(multi != 0) {
        --multi;

        save_cm = in_line;
    }

    in_line[0] = foo;
    in_line[1] = 0;

    if(foo == EOF) {
        settty("End of input?\n");

        exit(0);
    }

    if((foo == 'f') || (foo == 'F')) {
        in_line[1] = getchar();

#ifdef QUEST
        if(in_line[1] == foo) {
            in_line[2] = getchar();
        }
        else {
            in_line[2] = 0;
        }
#else
        in_line[2] = 0;
#endif
    }

    if((foo == 'm') || (foo == 'M')) {
        in_line[1] = getchar();
        in_line[2] = 0;
    }

    clrlin();

    return in_line;
}

char readchar()
{
    int sym;

    fflush(stdout);

    sym = getchar();
    if(sym == EOF) {
        settty("End of input?\n");

        exit(0);
    }

    if(flags.topl == 1) {
        flags.topl = 2;
    }

    return (char)sym;
}
