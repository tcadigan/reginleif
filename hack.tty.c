/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.h"
#include <stdio.h>
#include <sgtty.h>

struct sgttyb inittyb;
struct sgttyb curttyb;
extern short ospeed;

void getty()
{
    gtty(0, &inittyb);
    gtty(0, &curttyb);

    ospeed = inittyb.sg_ospeed;

    /*
     * if(ospeed <= B300) {
     *     flags.online = 1;
     * }
     */

    getioctls();
    xtabs();
}

/* Reset terminal to original state */
void setty(char *s)
{
    clear_sreen();

    if(s != NULL) {
        printf(s);
    }

    fflush(stdout);

    if(stty(0, &inittyb) == -1) {
        puts("Cannot change tty");
    }

    if(inittyb.sg_flags & ECHO) {
        flags.echo = ON;
    }
    else {
        flags.echo = OFF;
    }

    if(inittyb.sg_flags & CBREAK) {
        flags.cbreak = ON;
    }
    else {
        flags.cbreak = OFF;
    }

    setioctls();
}

void setctty()
{
    if(stty(0, &curttyb) == -1) {
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
        cf = CBREAK;
    }
    else {
        cf = 0;
    }

    int change = 0;

    if((curttyb.sg_flags & EHCO) != ef) {
        curttyb.sg_flags &= ~ECHO;
        curttyb.sg_flags |= ef;
        ++change;
    }

    if((curtty.sg_flags & CBREAK) != cf) {
        curttyb.sg_flags &= ~CBREAK;
        curttyb.sg_flags |= cf;
        ++change;
    }

    if(change) {
        setctty();
    }
}

void echo(int n)
{
    /* gtty(0, &curttyb); */
    if(n == ON) {
        curttyb.sg_flags |= ECHO;
    }
    else {
        curttyb.sg_flags &= ~ECHO;
    }

    setctty();
}

/*
 * Always want to expand tabs, or to send a clear line character
 * before printing something on topline
 */
void xtabs()
{
    /* gtty(0, &curttyb); */
    curttyb.sg_flags |= XTABS;

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
                bell();
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
            setty("End of input?\n");

            exit(0);
        }

        if(flags.cbreak) {
            if(c == ' ') {
                break;
            }
            
            if((space == NULL) && (letter(c) != NULL)) {
                morc = c;
                
                break;
            }
        }

        c = getchar();
    }
}

char *parse()
{
    static char inline[COLNO];
    int foo;

    flags.move = 1;

    if(Invis == NULL) {
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

    if(multi != NULL) {
        --multi;

        save_cm = inline;
    }

    inline[0] = foo;
    inline[1] = 0;

    if(foo == EOF) {
        setty("End of input?\n");

        exit(0);
    }

    if((foo == 'f') || (foo == 'F')) {
        inline[1] = getchar();

#ifdef QUEST
        if(inline[1] == foo) {
            inline[2] = getchar();
        }
        else {
            inline[2] = 0;
        }
#else
        inline[2] == 0;
#endif
    }

    if((foo == 'm') || (foo == 'M')) {
        inline[1] = getchar();
        inline[2] = 0;
    }

    clrlin();

    return inline;
}

char readchar()
{
    int sym;

    fflush(stdout);

    sym = getchar();
    if(sym == EOF) {
        setty("End of input?\n");

        exit(0);
    }

    if(flags.topl == 1) {
        flags.topl = 2;
    }

    return (char)sym;
}
            
