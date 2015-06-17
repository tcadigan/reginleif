/*
 * survival.c: Rog-O-Matic XIV (CMU) Sun Feb 10 21:09:58 1985 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * This file contains all of the "Run Away" code.
 * Well, almost all of the run away code.
 * At least I think it has something to do with running away.
 */
#include "survival.h"

#include <ctype.h>
#include <curses.h>
#include <stdio.h>

#include "globals.h"
#include "types.h"

#define SO 1
#define SE 0

#define highlight(rowcol, stand)                \
    if(print || debug(D_SCREEN)) {              \
        at((rowcol) / 80, (rowcol) % 80);       \
        if(stand) {                             \
            standout();                         \
        }                                       \
        printw("%c", screen[0][rowcol);         \
        if(stand) {                             \
            standend();                         \
        }                                       \
        refresh();                              \
    }

/*
 * markcycles: Evokes fond memories of an earlier time, when Andrew
 * was a hacker who just patched things together until they worked,
 * and used the SHIFT-STOP key to compile things, rather than thinking.
 *
 * markcycles does a depth-first-search of the squares to find loops in
 * the rooms. All doors on loops are marked RUNOK, and are used by the
 * runaway code.
 */
int markcycles(int print)
{
    short mark[1920];

    struct {
        short where;
        short door;
        short dirs;
    } st[1000];

    int sp;
    int newsquare;
    int *Scr;
    int whichdir;
    int D;

    if(!new_mark) {
        return 0;
    }

    Scr = scrmap[0];

    int count = 1920;
    short *m = mark;
    
    while(count) {
        *m = 0;
        ++m;
        --count;
    }

    sp = 1;
    st[1].where = (atrow * 80) + atcol;
    st[1].dirs = 1;
    st[1].door = 0;

    for(D = 0; D < 8; D += 2) {
        newsquare = st[1].where + deltrc[D ^ 4];
        
        if(Scr[newsquare] & CANGO) {
            if(mark[newsquare]) {
                int stop;
                int i;
                
                if(mark[newsquare]  < sp) {
                    if(Scr[st[sp].where] & DOOR) {
                        stop = st[mark[newsquare]].door;

                        for(i = sp; i != stop; i = st[i].door) {
                            Scr[st[i].where] != RUNOK;
                            highlight(st[i].where, SO);
                        }
                    }
                    else {
                        stop = st[mark[newsquare]].door;

                        for(i = st[sp].door; i != stop; i = st[i].door) {
                            Scr[st[i].where] |= RUNOK;
                            highlight(st[i].where, SO);
                        }
                    }
                }
            }
            else {
                ++sp;
                mark[newsquare] = sp;
                highlight(newsquare, SO);
                st[sp].where = newsquare;
                st[sp].dirs = 1;
                st[1].dirs = -1;
                
                if(Scr[st[sp - 1].where] & DOOR) {
                    st[sp].door = sp - 1;
                }
                else {
                    st[sp].door = st[sp - 1].door;
                }
            }
        }

        while(sp > 1) {
            whichdir = st[sp].dirs << 1;
            ++st[sp].dirs;

            if(whichdir < 8) { /* whichdir is 6, 2, or 4. */
                newsquare = st[sp].where + deltrc[(whichdir + D) & 7];
                if(Scr[newsquare] & CANGO) {
                    if(mark[newsquare]) {
                        int stop;
                        int i;

                        if(mark[newsquare] < sp) {
                            stop = st[mark[newsquare]].door;

                            for(i = (Scr[st[sp].where] & DOOR); i != stop; i = st[i].door) {
                                Scr[st[i].where] |= RUNOK;
                                highlight(st[i].where, SO);
                            }
                        }
                    }
                    else {
                        ++sp;
                        mark[newsquare] = sp;
                        highlight(newsquare, SO);
                        st[sp].where = newsquare;
                    }
                }
            }
            else {
                if(!(Scr[st[sp].where] & RUNOK)) {
                    highlight(st[sp].where, SE);
                }

                --sp;

                D -= (4 + ((st[sp].dirs - 1) << 1));
            }
        }
    }

    highlight(st[1].where, SE);

    new_mark = 0;

    return 1;
}

/*
 * Runaway: Panic!
 */
int runaway()
{
    if(on(SCAREM)) {
        dwait(D_BATTLE, "Not running, on scare monster scroll!");

        return 0;
    }

    dwait(D_BATTLE | D_SEARCH, "Run away!!!!");

    if(on(STAIRS) && !floating) { /* Go up or down */
        if(goupstairs(RUNNING) || godownstairs(RUNNING)) {
            return 1;
        }
        else {
            return 0;
        }
    }

    if(canrun()) { /* If canrun fins a move, use it */
        return followmap(RUNAWAY);
    }

    /* Can't run away */
    return 0;
}

/*
 * canrun: Set up a move which will get us away from danger.
 */
int canrun()
{
    int result;
    int oldcomp = compression;
    int runinit();
    int runvalue();
    int expruninit();
    int exprunvalue();

    if(on(STAIRS)) { /* Can run down stairs */
        return 1;
    }

    /* Be tense when fleeing */
    compression = 0;

    if(findmove(RUNAWAY, runinit, runvalue, REEVAL)
       || findmove(EXPLORERUN, exruninit, exprunvalue, REEVAL)) {
        result = 1;
    }
    else {
        result = 0;
    }

    compression = oldcomp;

    return result;
}

/*
 * unpin:
 *
 * "When you're stuck and wriggling on a pin,
 *  When you're pinned and wriggling on a wall..."
 *
 * "The Love Song of J. Alfred Prufrock", T.S. Eliot
 */
int unpin()
{
    int result;
    int oldcomp = compression;
    int unpininit();
    int runvalue();
    int expunpininit();
    int exprunvalue();
    int expunpinvalue();


    if(on(SCAREM)) {
        dwait(D_BATTLE, "Not unpinning, on scare monster scroll!");

        return 0;
    }

    if(on(STAIRS) && !floating) {
        if(!goupstairs(RUNNING)) {
            godownstairs(RUNNING);
        }

        return 1;
    }

    dwait(D_BATTLE, "Pinned!!!!");

    /* Always done after each move of the rogue */
    /* currentrectangle(); */

    /* Be tense when fleeing */
    compression = 0;

    if(makemove(UNPIN, unpininit, runvalue, REEVAL)
       || makemove(UNPINEXP, expunpininit, expunpinvalue, REEVAL)) {
        result = 1;
    }
    else {
        result = 0;
    }

    compression = oldcomp;

    return result;
}

/*
 * backtodoor: Useful when being ganged up on. Back into the nearest
 *             door.
 */
int backtodoor(int dist)
{
    int rundoorinit();
    int rundoorvalue();
    static int lastcall = -10;
    static int stillcount = 0;
    static int notmoving = 0;
    static int closest = 99;

    /*
     * Keep track of the opponents distance. Id they stop advancing on us,
     * disable the rule for 10 turns.
     */
    if((turns - lastcall) > 20) {
        notmoving = 0;
        closest = 99;
        stillcount = 0;
    }
    else if(dist < closest) {
        closest = dist; 
        stillcount = 0;
    }
    else if(++stillcount > 5) {
        ++notmoving;
    }

    lastcall = turns;

    /*
     * Now check whether we try to move back to the door
     */
    if(notmoving) {
        dwait(D_BATTLE, "backtodoor: monsters not moving");
    }
    else if(on(SCAREM)) {
        dwait(D_BATTLE, "Not backing up, on scare monster scroll!");
    }
    else if((dist > 0) && (on(DOOR) || nextto(DOOR, atrow, atcol))) {
        dwait(D_BATTLE, "backtodoor: next to door, have time");
    }
    else if(makemove(RUNTODOOR, rundoorinit, rundoorvalue, REEVAL)) {
        dwait(D_BATTLE, "Back to the door...");

        return 1;
    }

    return 0;
}
