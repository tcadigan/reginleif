/*
 * monsters.c: Rog-O-Matic XIV (CMU) Tue Mar 19 21:39:44 1985 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * This file contains all of the monster specific functions.
 */
#include "monsters.h"

#include <ctype.h>
#include <curses.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "globals.h"
#include "ltm.h"
#include "types.h"
#include "utility.h"

#define ADJACENT(m) \
    (max(abs(mlist[m].mrow - atrow), abs(mlist[m].mcol - atcol)) == 1)

/*
 * monname: Return a monster name given letter '@ABC..Z'
 */
char *monname(char m)
{
    return monhist[monindex[m - 'A' + 1]].m_name;
}

/*
 * addmonster: Add a monster to this level. Remove any monsters on the
 * list which are in the same square.
 */
void addmonster(char ch, int r, int c, int quiescence)
{
    char *monster = monname(ch);

    if((r > 1) || (c > 3)) {
        if(isholder(monster)) {
            quiescence = AWAKE;
        }

        deletemonster(r, c);
        mlist[mlistlen].chr = ch;
        mlist[mlistlen].mrow = r;
        mlist[mlistlen].mcol = c;
        mlist[mlistlen].q = quiescence;

        if(++mlistlen >= MAXMONST) {
            dwait(D_FATAL, "Too many monsters");
        }

        setrc(MONSTER, r, c);
        lyinginwait = 0;
        new_arch = 1;

        /* If we can see it, it is not really invisible */
        if(stlmatch(monster, "invisible") || streq(monster, "phantom")) {
            beingstalked = 0;
        }
    }
}

/*
 * deletemonster: Remove a monster from the list at location (row, col).
 */
void deletemonster(int r, int c)
{
    int i;
    
    new_arch = 1;
    unsetrc(MONSTER, r, c);

    for(i = 0; i < mlistlen; ++i) {
        if((mlist[i].mcol == c) && (mlist[i].mrow == r)) {
            --mlistlen;
            mlist[i] = mlist[mlistlen];
            --i;
        }
    }
}

/*
 * dumpmonster: (debugging) Dump the list of monsters on this level.
 */
void dumpmonster()
{
    int i;

    move(1, 0);
    refresh();
    for(i = 0; i < mlistlen; ++i) {
        if(mlist[i].q == AWAKE) {
            printw("%s at %d,%d(%c) \n",
                   "alert",
                   mlist[i].mrow,
                   mlist[i].mcol,
                   mlist[i].chr);
        }
        else if(mlist[i].q == ASLEEP) {
            printw("%s at %d,%d(%c) \n",
                   "sleeping",
                   mlist[i].mrow,
                   mlist[i].mcol,
                   mlist[i].chr);
        }
        else if(mlist[i].q == HELD) {
            printf("%s at %d,%d(%c) \n",
                   "held",
                   mlist[i].mrow,
                   mlist[i].mcol,
                   mlist[i].chr);
        }
        else {
            printf("%s at %d,%d(%c) \n",
                   "unknown",
                   mlist[i].mrow,
                   mlist[i].mcol,
                   mlist[i].chr);
        }
    }

    printw("You are at %d,%d.", atrow, atcol);
    move(row, col);
    refresh();
}

/*
 * sleepmonster: Turn all unknown monsters into sleeping monsters.
 * This routine is called after we have executed a command, so if
 * the value of ASLEEP is not overridden by the monster's movement,
 * it sat still for a turn and must be asleep.
 */
void sleepmonster()
{
    int m;

    for(m = 0; m < mlistlen; ++m) {
        if((mlist[m].q == 0) && !ADJACENT(m)) {
            dwait(D_MONSTER,
                  "Found a sleeping %s at %d,%d",
                  monname(mlist[m].chr),
                  mlist[m].mrow,
                  mlist[m].mcol);

            mlist[m].q = ASLEEP;
        }
    }
}

/*
 * holdmonsters: Mark all close monsters as being held.
 */
void holdmonsters()
{
    int m;

    for(m = 0; m < mlistlen; ++m) {
        if((mlist[m].q == 0)
           && (max(abs(mlist[m].mrow - atrow), abs(mlist[m].mcol - atcol)) < 3)) {
            dwait(D_MONSTER, 
                  "Holding %s at %d,%d",
                  monname(mlist[m].chr),
                  mlist[m].mrow,
                  mlist[m].mcol);

            mlist[m].q = HELD;
        }
    }
}

/*
 * wakemonster: Turn monsters into waking monsters
 *
 * dir = 0-7 means wake up adjacent plus monster in that dir
 * dir = 8   means wake up only adjacent monster
 * dir = ALL means wake up all monsters
 * dir = -m  means wake up all adjacent monsters of type m.
 */
void wakemonster(int dir)
{
    int m;

    for(m = 0; m < mlistlen; ++m) {
        if((mlist[m].q != AWAKE)
           && ((dir == ALL)
               || ((dir < 0) && ADJACENT(m) && (mlist[m].chr == -dir + 'A' - 1))
               || ((dir >= 0) 
                   && (dir < 8)
                   && (mlist[m].mrow == atdrow(dir))
                   && (mlist[m].mcol == atdcol(dir))))) {
            dwait(D_MONSTER,
                  "Waking up %s at %d,%d",
                  monname(mlist[m].chr),
                  mlist[m].mrow,
                  mlist[m].mcol);

            mlist[m].q = AWAKE;
            setrc(EVERCLR, mlist[m].mrow, mlist[m].mcol);
        }
    }
}

/*
 * seemonster: Return true if a particular monster is on the monster list
 */
int seemonster(char *monster)
{
    int m;

    for(m = 0; m < mlistlen; ++m) {
        if(streq(monname(mlist[m].chr), monster)) {
            return 1;
        }
    }

    return 0;
}

/*
 * seeawakemonster: Returns true if there is a particular awak
 * monster on the monster list. (DR UTexas 26 Jan 84)
 */
int seeawakemonster(char *monster)
{
    int m;

    for(m = 0; m < mlistlen; ++m) {
        if(streq(monname(mlist[m].chr), monster) && (mlist[m].q == AWAKE)) {
            return 1;
        }
    }

    return 0;
}

/*
 * monsternum: Given a string e.g. "umber hulk", return the monster
 *             number from 0 to 26, e.g. "umber hulk" ==> 21. Zero
 *             is used for unknown monsters (e.g. "it").
 */
int monsternum(char *monster)
{
    int m;
    int mh;

    mh = findmonster(monster);
    if(mh != NONE) {
        for(m = 0; m <= 26; ++m) {
            if(monindex[m] == mh) {
                return m;
            }
        }
    }

    return 0;
}

/*
 * newmonsterlevel: Starting a new level. Set the initial sleep status of
 * each monster.
 */
void newmonsterlevel()
{
    int m;
    char *monster;

    for(m = 0; m < mlistlen; ++m) {
        monster = monname(mlist[m].chr);

        if(streq(monster, "floating eye")
           || streq(monster, "leprechaun")
           || streq(monster, "nymph")
           || streq(monster, "ice monster")) {
            mlist[m].q = ASLEEP;
        }
        else {
            mlist[m].q = 0;
        }
    }
}

/*
 * isholder: Return true if the monster can hold us.
 */
int isholder(char *monster)
{
    if(streq(monster, "venus flytrap") || streq(monster, "violet fungi")) {
        return 1;
    }
    else {
        return 0;
    }
}
