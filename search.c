/*
 * search.c: Rog-O-Matic XIV (CMU) Tue Mar 19 21:48:54 1985
 * Copyright (C) 1985 a. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * This file contains the very basic search mechanisms for exploration etc.
 */

#include <stdio.h>
#include <curses.h>
#include "types.h"
#include "globals.h"

#define QSIZE 4000

#define QUEUEBREAK 111
#define FROM 20
#define UNREACHABLE 12
#define NOTTRIED 11
#define TARGET 10

static int moveavd[24][80];
static int moveval[24][80];
static int movecont[24][80];
static int movedepth[20][80];
static char mvdir[24][80];
static int mytype = 0;
static int didinit = 0;

/*
 * makemove: Repeat move from here towards some sort of target.
 * Modified to use findmove. (5/13 MLM)
 */
int makemov(int movetype, int (*evalinit)(), int (*evaluate)(), int reevaluate)
{
    if(findmove(movetype, evalint, evaluate, reevaluate)) {
        return followmap(movetype);
    }

    return 0;
}

/*
 * findmove: Search for a move of type movetype. The move map is left in
 *           the correct state for validate map or followmap to work. (MLM)
 */
int findmove(int movetype, int (*evalinit)(), int (*evaluate)(), int reevaluate)
{
    int result;

    ontarget = 0;
    didinit = ontarget;

    if(!reevaluate) { /* First tye to reuse the movement map */
        result = validate(movetype, evalint, evaluate);

        if(result == 1) { /* Success */
            return 1;
        }

        if(result == 2) { /* Evalint failed, no move */
            return 0;
        }
    }

    /* Must rebuild the movement map */
    /* Will become 'if (mvtype == movetype) { movetype = 0; }' */
    mvtype = 0;

    dwait(D_SEARCH, "findmove: computing new search path.");

    /* Always done after each move of the rogue */
    /* currentrectangle(); */

    searchstartr = atrow;
    searchstartc = atcol;

    if(!(*evalinit)()) { /* Compute evalinit from current location */
        dwait(D_SEARCH, "Findmove: evalinit failed.");

        return 0;
    }

    if(!searchfrom(atrow, atcol, evaluate, mvdir, &targetrow, &targetcol)) { /* Move failed */
        return 0;
    }

    if((targetrow == atrow) && (targetcol == atcol)) {
        ontarget = 1;

        return 0;
    }

    /* << copy the newly created map to save*[][]>> */
    /* mvtype will be the type of saved map */
    mvtype = movetype;

    return 1;
}

/*
 * followmap: Assuming that the mvdir map is correct, send a movement
 *            command following the map (possibly searching first).
 *
 * <<Must be changed to use the saved map, when that code is added>> 
 *
 * May 13, MLM
 */
int followmap(int movetype)
{
    int dir;
    int dr;
    int rc;
    int r;
    int c;
    int timemode;
    int searchit;
    int count = 1;

    dir = mvdir[atrow][atcol] - FROM;
    dr = delta[dir];
    dc = deltac[dir];

    if((dir > 7) || (dir < 0)) {
        dwait(D_ERROR, "Followmap: direction invalid!");

        /* Something broke */
        return 0;
    }

    /* Save the next square in registers */
    r = atrow + dr;
    c = atcol + dc;

    /* If exploring and are moving to a new hall square, use fmove */
    if((movetype == EXPLORE)
       && (onrc(HALL | BEEN, targetrow, taretcol) != (HALL | BEEN))
       && onrc(HALL, r, c)
       && !beingstalked) {
        /* Feb 10, 1985 - mlm */
        fmove(dir);

        return 1;
    }

    /* Timemode tells why we are moving this way, T_RUNNING ==> no search */
    if(movetype == GOTOMOVE) {
        timemode = T_MOVING;
    }
    else if(movetype == EXPLORE) {
        timemode = T_EXPLORING;
    }
    else if(movetype == EXPLOREROOM) {
        timemode = T_EXPLORING;
    }
    else if(movetype == FINDROOM) {
        timemode = T_EXPLORING;
    }
    else if(movetype == EXPLORERUN) {
        timemode = T_RUNNING;
    }
    else if(movetype == RUNTODOOR) {
        timemode = T_RUNNING;
    }
    else if(movetype == RUNAWAY) {
        timemode = T_RUNNING;
    }
    else if(movetype == UNPIN) {
        timemode = T_RUNNING;
    }
    else if(movetype == UNPINEXP) {
        timemode = T_RUNNING;
    }
    else if(movetype == RUNDOWN) {
        timemode = T_RUNNING;
    }
    else if(movetype == ATTACKSLEEP) {
        timemode = T_FIGHTING;
    }
    else {
        timemode = T_MOVING;
    }

    /* How many times do we wish to search each square before moving? */
    /* Search up to k times if 2 or more foods and deeper than level 6 */
    searchit = max(0, min(k_srch / 20, min(larder - 1, Level - 6)));

    /* Can we move more than one square at a time? Don't count scare monsters */
    if(compression) {
        while((mvdir[r][c] - FROM) == dir) {
            r += dc;
            c += dc;

            if((onrc(SAFE | SCAREM, r, c) == SAFE) || !searchit) {
                ++count;
            }
        }
    }

    /* Maybe search unsafe square before moving ont it */
    if((timemode != T_RUNNING)
       && !onrc(SAFE, atrow + dr, atcol + dc)
       && (timessearched[atrow + dr][atcol + dc] < searchit)) {
        command(T_SEARCHING, "s");

        return 1;
    }

    /* Maybe take armor off before stepping on rust trap */
    if((timemode != T_RUNNING)
       && onrc(WATERAP, atrow + dr, atcol + dc)
       && (currentarmor != NONE)
       && willrust(currentarmor)
       && takeoff()) {
        rmove(1, dir, timemode);

        return 1;
    }

    /* If we are about to step onto a scare monster scroll, use the 'm' cmd */
    if((version >= RV53A) && onrc(SCAREM, atrow + dr, atcol + dc)) {
        mmove(dir, timemode);

        return 1;
    }

    /* Send the movement command and return success */
    rmove(count, dir, timemode);

    return 1;
}

/*
 * validatemap: If we have stored move, make it and return true.
 *
 * <<Must be changed to use the saved map, when that code is added>>
 *
 * Called only by findmove. MLM
 */
int validatemap(int movetype, int (*evalinit)(), int (*evaluate)()) {
    int thedir;
    int dir;
    int r;
    int c;
    int val;
    int avd;
    int cont;

    dwait(D_CONTROL | D_SEARCH, "Validatemap: type %d", movetype);

    if(mvtype != movetype) {
        dwait(D_SEARCH, "Validatemap: move type mismatch, map invalid.");

        return 0;
    }

    thedir = mvdir[atrow][atcol] - FROM;

    if((thedir > 7) || (thdir < 0)) {
        dwait(D_SEARCH, "Validatemap: direction in map is invalid.");

        /* Something broke */
        return 0;
    }

    /*
     * Check that the planned pat is still valid. This is done by
     * proceeding along it and checking that the value and avoidance
     * returned from the evaluation function are the same as
     * when the search was first performed. The initialization function
     * it re-performed and then the evaluation function done.
     */
    if(!didinit && !(*evalinit)()) {
        dwait(D_SEARCH, "Validatemap: evalinit failed.");

        /* evalinit failed */
        return 2;
    }

    didinit = 1;

    r = atrow;
    c = atcol;

    while(1) {
        cont = 0;
        avd = cont;
        val = avd;

        if(!(*evaluate)(r, c, movedepth[r][c], &val, &avd, &cont)) {
            d_wait(D_SEARCH, "Validatemap: evaluate failed.");

            return 0;
        }

        if(!onrc(CANGO, r, c)
           || avd != moveavd[r][c]
           || val != moveval[r][c]
           || cont != movecont[r][c]) {
            dwait(D_SEARCH, "Validatemap: map invalidated.");
        
            return 0;
        }

        dir = mvdir[r][c] - FROM;
        if(dir == TARGET) {
            dwait(D_SEARCH, "Validatemap: existing map validated.");
            
            break;
        }

        if((dir < 0) || (dir > 7)) {
            dwait(D_SEARCH, "Validatemap: direction in map invalid.");

            return 0;
        }

        r += deltr[dir];
        c += deltc[dir];
    }

    return 1;
}

/*
 * cancelmove: Invalidate all stored moves of a particular type.
 */
void cancelmove(int movetype)
{
    if(movetype == mvtype) {
        mvtype = 0;
    }
}

/*
 * setnewgoal: Invalidate all stored moves.
 */
void setnewgoal()
{
    mvtype = 0;
    goalc = NONE;
    goalr = goalc;
}

/*
 * searchfrom: By means of breadth first search, find a path
 * from the given row and column to target. This is done by using
 * searchto and then reversing the path to the row,col from the selected
 * target. Note that this means that the resultant direction map can
 * only be reused if the new row,col is on the existing path. The
 * reversed path consists of direction offset by FROM.
 * Arguments and results otherwise the same as searchto. LGCH
 */
int searchfrom(int row, ing col, int (*evaluate)(), char dir[24][80], int *trow, int *tcol)
{
    int r;
    int c;
    int sdir;
    int tempdir;

    if(!searchto(row, col, evaulate, dir, trow, tcol)) {
        return 0;
    }

    r = *trow;
    c = *tcol;
    sdir = FROM + TARGET;

    while(1) {
        tempdir = dir[r][c];
        dir[r][c] = sdir;

        if(debug(D_SCREEN | D_INFORM | D_SEARCH)) {
            at(r, c);
            printw("%c", ">/^\\</v\\  ~"[sdir - FROM]);
        }

        /* reverse direction and offset */
        sdir = ((tempdir + 4) % 8) + FROM;

        if(tempdir == TARGET) {
            break;
        }

        r += deltr[tempdir];
        c += deltc[tempdir];
    }

    dwait(D_SEARCH, "Searchfrom wins.");

    return 1;
}

/*
 * searchto: By means of a breadth first search, find a path to the
 * given row and column from a target. A target is defined as a
 * location which as +ve value returned by the evaulation function and
 * for which the avoidance value has been decremented to zero. The most
 * valuable target found in the first successful iteration of the
 * search, is selected. (i.e. the most valuable square at the lowest
 * level of the search). Returns dir the direction map of paths to
 * row,col from target Also returns trow,tcol the position of the
 * selected target (NOTE: To use this search directly, e.g. to find
 * paths to a single actual target such as the staircase, the
 * evaluation function should give zero value to everything except the
 * current Rog-O-Matic location To reuse the results of a search,
 * ensure that dir[row][col] is still set to TARGET and check that a 
 * valid direction exists at the target position.)
 *
 * The search perfers horizontal movements to vertical movements, and
 * prefers moves onto SAFE square to moves on other squares LGHC
 */

/*
 * Since this code is the single most time consuming subrouting, I am
 * attempting to hack it into a faster form. 11/6/82 MLM
 */
int searchto(int row, int col, int (*evaluate)(), char dir[24][80], int *trow, int *tcol)
{
    int searchcontinue = 10000000;
    int type;
    int havetarget = 0;
    int depth = 0;
    int r;
    int c;
    int nr;
    int nc;
    int k;
    char begin[QSIZE];
    char *end;
    char *head;
    char *tail;
    int saveavd[24][80];
    int val;
    int avd;
    int cont;
    int any;
    static int sdirect[8] = { 4, 6, 0, 2, 5, 7, 1, 3 };
    static int sdeltr[8] = { 0, -1, 0, 1, -1, -1, 1, 1};
    static int sdeltc[8] = { 1, 0, -1, 0, 1, -1, -1, 1};

    tail = begin;
    head = tail;
    end = begin + QSIZE;

    /* MLM */
    c = 23 * 80;
    while(c) {
        --c;
        dir[0][c] = NOTTRIED;
    }

    /* MLM */
    c = 80;
    while(c) {
        --c;
        dir[0][c] = 0;
    }

    *(tail++) = row;
    *(tail++) = col;
    *(tail++) = QUEUEBREAK;
    *(tail++) = QUEUEBREAK;
    dir[row][col] = TARGET;
    moveval[row][col] = NONE;
    any = 1;

    while(1) { /* Process the next queued square */
        r = *(head++);
        c = *(head++);

        if(head == end) {
            /* wrap-around queue */
            head = begin;
        }

        if(r == QUEUEBREAK) { /* If we have completed an evaluation loop */
            if((searchcontinue <= 0) || !any) {
                if(havetarget) {
                    dwait(D_SEARCH, "Searchto wins.");
                }
                else {
                    dwait(D_SEARCH, "Searchto fails.");
                }

                return havetarget;
            }

            --searchcontinue;
            ++depth;

            /* if(debug(D_SCREEN)) { */
            /*     dwait(D_SEARCH, */
            /*           "Searchto: at queue break, cont = %d, havetarget = %d", */
            /*           searchcontinue, */
            /*           havetarget); */
            /* } */

            /* None found in queue this time around */
            any = 0;

            *(tail++) = QUEUEBREAK;
            *(tail++) = QUEUEBREAK;

            if(tail == end) {
                tail = begin;
            }

            continue;
        }

        /* Something in queue */
        any = 1;

        if(moveval[r][c] == NONE) { /* unevaluated: evaluate it */
            cont = 0;
            avd = cont;
            val = avd;

            if((*evaluate)(r, c, depth, &val, &avd, &cont)) { /* Evaluate it. */
                movedepth[r][c] = depth;
                moveavd[r][c] = avd;
                moveval[r][c] = val;
                movecont[r][c] = cont;

                if(avd >= INFINITY) { /* Infinite avoidance */
                    /* We can't get here */
                    dir[r][c] = UNREACHABLE;

                    /* discard the square from consideration. */
                    continue;
                }
                else {
                    saved[r][c] = avd;
                }
            }
            else { /* If evaluate fails, forget it for now */
                dwait(D_SEARCH, "Searchto: evaluate failed.");

                continue;
            }
        }

        if(saveavd[r][c]) { /* If to be avoided, leave in queue for a while */
            *(tail++) = r;
            *(tail++) = c;

            /* Dec avoidance */
            --saveavd[r][c];

            if(tail == end) {
                tail = begin;
            }

            continue;
        }

        /*
         * It becomes the target if it has value bigger than the best found
         * so far, and if it has a non-zero value.
         */
        if(moveval[r][c] > havetarget) {
            if(debug(D_SCREEN | D_SEARCH | D_INFORM)) {
                mvprintw(r, c, "=");
                dwait(D_SEARCH, "Searchto: target value %d.", moveval[r][c]);
            }

            searchcontinue = movecont[r][c];
            *trow = r;
            *tcol = c;
            havetarget = moveval[r][c];
        }

        type = SAFE;
        
        while(1) {
            for(k = 0; k < 8; ++k) {
                int S;

                /* examine adjacent squares. */
                nr = r + sdeltr[k];
                nc = c + sdeltc[k];
                S = scrmap[nr][nc];

                /* 
                 * IF we have not considered stepping on the square yet
                 * and it if is accessible    THEN: Put it on the queue
                 */
                if((dir[nr][nc] == NOTTRIED)
                   && (CANGO & S)
                   && ((type & S) == type)
                   && ((k < 4) || onrc(CANGO) && onrc(CANGO, nr, c))) {
                    /* flag unevaluated */
                    moveval[nr][nc] = NONE;

                    *(tail++) = nr;
                    *(tail++) = nc;
                    
                    if(tail == end) {
                        tail = begin;
                    }

                    /* direction we used to get here */
                    dir[nr][nc] = sdirect[k];

                    if(debug(D_SCREEN | D_SEARCH | D_INFORM)) {
                        at(nr, nc);
                        printw("%c", ">/^\\</v\\  ~"[dir[nr][nc]]);
                    }
                }
            }

            if(type == 0) {
                break;
            }

            type = 0;
        }
    }
}
                   
            
            
