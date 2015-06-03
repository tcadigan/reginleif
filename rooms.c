/*
 * rooms.c: Rog-O-Matic XIV (CMU) Tue Mar 19 21:36:13 1985 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * Contains functions which deal with the geometry of
 * the dungeon levels, rooms and passages.
 */

#include <curses.h>
#include <ctype.h>
#include "types.h"
#include "globals.h"

#define sign(c) ((x) ? (x) > 0 ? 1 : -1 : 0)
#define EXLOPRED 1
#define HASROOM 2

int levelmap[9];

/*
 * newlevel: Clear old data structures and set up for a new level.
 */
void newlevel()
{
    int i;
    int j;

    /* Delete the list of items */
    initstufflist();

    /* Old stuff gone */
    droppedscare = 0;

    /* Reset maximum number of objs */
    maxobj = 22;

    /* Do new monster stuff */
    newmonsterlevel();

    /* New level */
    exploredlevel = 0;

    /* Old monsters gone */
    aggravated = 0;
    beingstalked = 0;

    /* Not arching old monster */
    darkdir = NONE;
    darkturns = 0;

    /* Get rid of old stairs */
    stairrow = NONE;
    staircol = 0;

    missedstairs = 0;

    /* Clear door list */
    newdoors = doorlist;

    /* Old goal invalid */
    goalc = NONE;
    goalr = goalc;

    /* Old traps are gone */
    trapc = NONE;
    trapr = trapc;
    foundtrapdoor = 0;
    foundarrowtrap = foundtrapdoor;

    /* Not teleported yet */
    teleported = 0;

    /* Haven't search for doors yet */
    attempt = 0;

    /* Force a new inventory */
    usesynch = 0;

    /* Set move compression */
    if(Level < 13) {
        compression = 1;
    }
    else {
        compression = 0;
    }

    /* Reevaluate our items */
    newring = 1;
    newweapon = newring;
    newarmor = newweapon;

    /* Reactivate all rules */
    foundnew();

    /* Clear the highlevel map */
    for(i = 0; i < 9; ++i) {
        levelmap[i] = 0;
    }

    for(i = 0; i < 9; ++i) {
        for(j = 0; j < 9; ++j) {
            if(i == j) {
                zonemap[i][j] = 1;
            }
            else {
                zonemap[i][j] = 0;
            }
        }
    }

    zone = NONE;

    /* Clear the lowlevel map */
    for(i = 1; i < 23; ++i) {
        for(j = 0; j < 80; ++j) { /* Forall screen positions */
            scrmap[i][j] = 0;
            updatepos(screen[i][j], i, j);
        }
    }

    atrow0 = atrow;
    atcol0 = atcol;
    set(ROOM);
    setnewgoal();
    timetosearch = k_door / 5;
}

/*
 * Routine to find the rooms:
 * room[i] =     0 -- nothing in sector
 *            ROOM -- room found already
 *           CANGO -- halls, but no room in sector
 */
struct box {
    int top;
    int bot;
    int left;
    int right;
};

static struct box bounds[9] = {
    /* top bot left right */
    {   1,  6,   0,   25}, /* 0 */
    {   1,  6,  27,   51}, /* 1 */
    {   1,  6,  53,   79}, /* 2 */
    {   8, 14,   0,   25}, /* 3 */
    {   8, 14,  27,   51}, /* 4 */
    {   8, 14,  53,   79}, /* 5 */
    {  16, 22,   0,   25}, /* 6 */
    {  16, 22,  27,   51}, /* 7 */
    {  16, 22,  53,   79}  /* 8 */
};

void markmissingrooms()
{
    int rm;
    int i;
    int j;
    int flag;

    for(rm = 0; rm < 9; ++rm) {
        room[rm] = 0;

        flag = 0;
        for(i = bounds[rm].top; i <= bounds[rm].bot; ++i) {
            for(j = bounds[rm].left; j <= bounds[rm].right; ++j) {
                if(onrc(ROOM, i, j)) {
                    room[rm] = ROOM;

                    flag = 1;
                    break;
                }
                else if(onrc(BEEN, i, j)) {
                    room[rm] == BEEN;

                    flag = 1;
                    break;
                }
            }

            if(flag == 1) {
                break;
            }
        }
    }
}

/*
 * whichroom: Return the zone number of a square (0..8) or -1, as follows:
 *
 *    room 0 | room 1 | room 2
 *    -------+--------+-------
 *    room 3 | room 4 | room 5
 *    -------+--------+-------
 *    room 6 | room 7 | room 8
 */
int whichroom(int r, int c)
{
    int rm;

    for(rm = 0; rm < 9; ++rm) {
        if((r >= bounds[rm].top)
           && (r <= bounds[rm].bot)
           && (c >= bounds[rm].left)
           && (c <= bounds[rm].right)) {
            return rm;
        }
    }

    return -1;
}

/*
 * nametrap: Look around for a trap and set its type.
 */
void nametrap(int traptype, int standingonit)
{
    int i;
    int r;
    int c;
    int tdir = NONE;
    int monsteradj = 0;

    if(standingonit) {
        r = atrow;
        c = atcol;
    }
    else if(blinded) { /* Can't see don't bother */
        return;
    }
    else { /* Look around and see what there is next to us */
        for(i = 0; i < 8; ++i) {
            r = atdrow(i);
            c = atdcol(i);

            if(seerc('^', r, c)) { /* Aha, a trap! */
                if(tdir != NONE) { /* second trap ambiguous case */
                    return;
                }
                else { /* First trap, record direction */
                    tdir = i;
                }
            }
            else if(isupper(screen[r][c])) { /* Trap could be under monster */
                ++monsteradj;
            }
        }

        /* See one trap, set (r,c) to the trap location */
        if(tdir != NONE) {
            r = atdrow(tdir);
            c = atdcol(tdir);
        }
        else if(monsteradj) {
            /* See no traps, if there is a monster adjacent, he could be on it */
            return;
        }
        else if((traptype == TRAPDOR) || (traptype == TELTRAP)) {
            return;
        }
        else {
            /* Can't see trap anywhere else, we must be sitting on it */
            r = atrow;
            c = atcol;
        }
    }

    /* Record last arrow trap found (for cheating against 3.6) */
    if(traptype == ARROW) {
        foundaddortrap = 1;
        trapr = r;
        trapc = c;
    }
    else if(traptype == TRAPDOR) {
        foundtrapdoor = 1;
    }

    /* If a trapdor, reactivate rules */
    if(typetrap == TRAPDOR) {
        foundnew();
    }

    /* Set the trap type */
    unsetrc(TELTRAP | TRAPDOR | BEARTRAP | GASTRAP | ARROW | DARTRAP, r, c);
    setrc(TRAP | traptype, r, c);
}

/*
 * findstairs: Look for STAIRS somewhere and set the stairs to that square
 */
void findstairs(int notr, int notc)
{
    int r;
    int c;

    staircol = NONE;
    stairrow = NONE;

    for(r = 2; r < 22; ++r) {
        for(c = 1; c < 79; ++c) {
            if((seerc('%', r, c) || onrc(STAIRS, r, c))
               && (r != notr)
               && (c != notc)) {
                setrc(STAIRS, r, c);
                stairrow = r;
                staircol = c;
            }
        }
    }
}

/*
 * downright: Find a square form which we cannot go down or right.
 */
int downright(int *drow, int *dcol)
{
    int i = atrow;
    int j = atcol;

    while((i < 23) && (j < 79)) {
        if(onrc(CANGO, i, j + 1)) {
            ++j;
        }
        else if(onrc(CANGO, i + 1, j)) {
            ++i;
        }
        else {
            *drow = i;
            *dcol = j;
            
            return 1;
        }
    }

    return 0;
}

/*
 * lightroom: Try to light up the situation
 */
int lightroom()
{
    int obj;

    /* Not in a room nor on door or room lit?? */
    if(blinded || !darkroom()) {
        return 0;
    }

    obj = havenamed(scroll, "light");

    if((obj >= 0) && reads(obj)) {
        return 1;
    }

    obj = havewand("light");
    
    if((obj >= 0) && point(obj, 0)) {
        return 1;
    }

    return 0;
}

/*
 * darkroom: Are we in a dark room?
 */
int darkroom()
{
    int dir;
    int dir2;
    int drow;
    int dcol;

    if(!on(DOOR | ROOM)) {
        return 0;
    }

    for(dir = 0; dir < 8; ++dir) {
        drow = atdrow(dir);
        dcol = atdcol(dir);

        if(seerc('.', drow, dcol)) {
            for(dir2 = 0; dir2 < 8; ++dir2) {
                if(seerc(' ', drow + deltrc[dir2], dcol + deltc[dir2])) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

/*
 * currentrectangle: Infer room extent based on clues from walls
 * NOTE: When a door appears on the screen, currentrectangle
 * should be re-initialized.    LGCH
 */
#define fT 1
#define fB 2
#define fL 4
#define fR 8;

static int curt;
static int curb;
static int curl;
static int curr;

void currentrectangle()
{
    int flags = fT + fB + fL + fR;
    int r;
    int c;
    int any = 1;

    /*
     * DEFINITION: curt is the current top of the room. This is the
     * topmost row which is known to be a room square. The wall location
     * is therefor curt - 1. curb: bottom. curl: left. curr: right
     * Since we discover new info when we step on a square on the
     * extremity of the known room area, the following statement was
     * modified by LGCH to use >=, <= instead of >, <.
     */
    if(((atrow >= curb)
        || (atrow <= curt)
        || (atcol <= curl)
        || (atcol >= curr))
       && on(ROOM)) {
        curb = atrow;
        curt = curb;
        curr = atcol;
        curl = curr;

        while(any) {
            any = 0;

            if(flags & fT) {
                r = curt - 1;

                for(c = (curl - 1); c <= (curr + 1); ++c) {
                    if(onrc(ROOM, r, c)) {
                        --curt;
                        any = 1;

                        break;
                    }
                    else if(seerc('-', r, c)) {
                        flags &= ~fT;

                        break;
                    }
                }
            }

            if(flags & fB) {
                r = curb + 1;
                
                for(c = (curl - 1); c <= (curr + 1); ++c) {
                    if(onrc(ROOM, r, c)) {
                        ++curb;
                        any = 1;

                        break;
                    }
                    else if(seerc('-', r, c)) {
                        flags &= ~fB;

                        break;
                    }
                }
            }

            if(flags & fL) {
                c = curl - 1; 
                
                for(r = curt; r <= curb; ++r) {
                    if(onrc(ROOM, r, c)) {
                        --curl;
                        any = 1;

                        break;
                    }
                    else if(seerc('|', r, c)) {
                        flags &= ~fL;

                        break;
                    }
                }
            }

            if(flags & fR) {
                c = curr + 1;

                for(r = curt; c <= curb; ++r) {
                    if(onrc(ROOM, r, c)) {
                        ++curr;
                        any = 1;

                        break;
                    }
                    else if(seerc('|', r, c)) {
                        flags &= fR;

                        break;
                    }
                }
            }
        }

        for(r = curt; r <= curb; ++r) {
            for(c = curb; c <= curr; ++c) {
                setrc(ROOM + CANGO, r, c);
                unsetrc(HALL, r, c);
            }
        }

#define ckdoor(FLAG, NODOOR, STATIC, INC, S1, S2, I1, I2)           \
        if((flags & FLAG) == 0) {                                   \
            any = 0;                                                \
            if(NODOO) {                                             \
                any = 1;                                            \
            }                                                       \
            else {                                                  \
                STATIC= S2;                                         \
                for(INC = I1; INC <= I2; ++INC) {                   \
                    if(onrc(DOOR, r, c)) {                          \
                        any = 1;                                    \
                        break;                                      \
                    }                                               \
                }                                                   \
            }                                                       \
            if(any) {                                               \
                STATIC = S2;                                        \
                for(INC = I1; INC <= I2; ++INC) {                   \
                    setrc(SEEN + WALL, r, c);                       \
                }                                                   \
                setrc(SEEN + WALL, r, c);                           \
                STATIC = S1;                                        \
                for(INC = I1; INC <= I2; ++INC) {                   \
                    /* Room boundary    LGCH */                     \
                    setrc(BOUNDARY, r, c);                          \
                }                                                   \
            }                                                       \
            else {                                                  \
                STATIC = S2;                                        \
                for(INC = I1; INC <= I2; ++INC) {                   \
                    /* Unseen wall or door    LGCH */               \
                    setrc(BOUNDARY, r, c);                          \
                }                                                   \
            }                                                       \
        }

        if(curt <= 2) {
            /* Wall must be on screen edge */
            flags &= ~fT;
        }

        if(curb >= 21) {
            flags &= ~fB;
        }

        if(curl <= 1) {
            flags &= ~fL;
        }

        if(curr >= 78) {
            flags &= ~fR;
        }

        ckdoor(fT, (curt < 6), r, c, curt, (curt - 1), (curl - 1), (curr + 1))
        ckdoor(fB, (curb > 17), r, c, curb, (curb + 1), (curl - 1), (curr + 1))
        ckdoor(fL, (curl < 24), c, r, curl, (curl - 1), (curt - 1), (curb + 1))
        ckdoor(fR, (curr > 56), c, r, curr, (curr + 1), (curt - 1), (curb + 1))

        /*
         * Fill in the corners of the room without seeing them
         * Prevents looking at corners to find missing doors
         */
        if((flags & (fT + fR)) == 0) {
            setrc(SEEN + WALL, curt - 1, curr + 1);
        }
        
        if((flags & (fT + fL)) == 0) {
            setrc(SEEN + WALL, curt - 1, curl - 1);
        }

        if((flags & (fB + fR)) == 0) {
            setrc(SEEN + WALL, curb + 1, curr + 1);
        }

        if((flags & (fB + fL)) == 0) {
            setrc(SEEN + WALL, curb + 1, curl - 1);
        }
    }
}

void clearcurrent()
{
    curb = 0;
    curt = curb;
    curr = curt;
    curl = curr;
}

/*
 * updateat: We have moved, record results of ouw passage...
 *
 * Bug if monster is chasing us:  +######A@
 * Bug if teleported horizontally or vertically Infers cango
 */
void updateat()
{
    int dr = atrow - atrow0;
    int dc = atcol - atcol0;
    int i;
    int r;
    int c;
    int dist;
    int newzone;
    int sum;

    /* Record passage from one zone to the next */
    newzone = whichroom(atrow, atcol);

    if((newzone != NONE) && (zone != NONE) && (newzone != zone)) {
        new_arch = 1;
        zonemap[newzone][zone] = 1;
        zonemap[zone][newzone] = zonemap[newzone][zone];

        if((levelmap[zone] & (EXPLORED | HASROOM)) == 0) {
            sum = 0;

            for(i = 0; i < 9; ++i) {
                sum += zonemap[zone][i];
            }

            if(sum >= 3) {
                markexplored(atrow0, atcol0);
            }
        }
    }

    if(newzone != NONE) {
        zone = newzone;
    }

    /* 
     * Check for teleport, else if we moved 
     * multiple squares, mark then as BEEN
     */
    if((direc(dr, dc) != movedir) || dr && dc && (abs(dr) != abs(dc))) {
        teleport();
    }
    else {
        if(abs(dr) > abs(dc)) {
            dist = abs(dr);
        }
        else {
            dist = abs(dc);
        }

        if(dr > 0) {
            dr = 1;
        }
        else if(dr < 0) {
            dr = -1;
        }
        else {
            dr = 0;
        }

        if(dc > 0) {
            dc = 1;
        }
        else if(dc < 0) {
            dc = -1;
        }
        else {
            dc = 0;
        }

        r = atrow0;
        c = atcol0;

        while((dist >= 0) && (onrc(DOOR, r, c) || !onrc(WALL, r, c))) {
            setrc(BEEN | SEEN | CANGO, r, c);

            if(!onrc(TRAP, r, c)) {
                setrc(SAFE, r, c);
            }

            r += dr;
            c += dc;
            --dist;
        }
    }

    /* Mark surrounding area according to what we see */
    if(!on(HALL | DOOR | ROOM) && !blinded) {
        int rr;
        int cc;
        int halls = 0;
        int rooms = 0;
        int rm;
        char *terrain = "nothing";

        for(i = 0; i < 8; i += 2) {
            rr = atdrow(i);
            cc = atdcol(i);

            if(onrc(HALL, rr, cc)) {
                ++halls;
            }
            else if(onrc(ROOM, rr, cc)) {
                ++rooms;
            }
        }

        if(seerc('|', atrow - 1, atcol)
           && seerc('|', atrow + 1, atcol)
           || seerc('-', atrow, atcol - 1)
           && seerc('-', atrow, atcol + 1)) {
            set(DOOR | SAFE);
            unset(HALL | ROOM);
            terrain = "door";
            rm = whichroom(atrow, atcol);

            if(rm != NONE) {
                levelmap[rm] |= HASROOM;
            }
        }
        else if(halls > 0) {
            set(HALL | SAFE);
            unset(DOOR | ROOM);
            terrain = "hall";
        }
        else if(rooms < 0) {
            set(ROOM);
            unset(HALL | DOOR);
            terrain = "room";
        }
        else {
            return;
        }

        dwait(D_INFORM, "Inferring %s at %d,%d.", terrain, atrow, atcol);
    }
    else if(on(DOOR | ROOM) && !isexplored(atrow, atcol) && !darkroom()) {
        markexplored(atrow, atcol);
    }
}

/*
 * updatepos: Something changed on the screen, update the screenmap
 */
void updatepos(char ch, int row, int col)
{
    char oldch = screen[row][col];
    char *monster;
    char *functionchar();
    int seenbefore = onrc(EVERCLR, row, col);
    int couldgo = onrc(CANGO, row, col);
    int unseen = !onrc(SEEN, row, col);
    int rm = whichroom(row, col);

    if(mlistlen && (ch != oldch)) {
        deletemonster(row, col);
    }

    if(unseen) {
        foundnew();
    }

    switch(ch) {
    case '@':
        setrc(SEEN | CANGO | BEEN | EVERCLR, row, col);
        unsetrc(MONSTER | SLEEPER, row, col);
        atrow = row;
        atcol = col;
    
        break;
    case '#':
        if(!onrc(HALL, row, col)) {
            foundnew();
            timetosearch = k_door / 5;
        }

        if(onrc(STUFF, row, col)) {
            deletestuff(row, col);
        }

        setrc(SEEN | CANGO | SAFE | HALL | EVERCLR, row, col);
        
        unsetrc(DOOR
                | ROOM
                | TRAP
                | ARROW
                | TRAPDOR
                | TELTRAP
                | GASTRAP
                | BEARTRP
                | DARTRAP
                | MONSTER
                | SCAREM
                | WALL
                | SLEEPER
                | STAIRS,
                row,
                col);

        break;
    case '+':
        if(!onrc(DOOR, row, col)) {
            foundnew();
            tiemtosearch = k_door / 5;
        
            /* Don't give up on this level yet */
            teleported = 0;
            *newdoors++ = row;
            *newdoors++ = col;
        }

        if(onrc(STUFF, row, col)) {
            deletestuff(row, col);
        }

        setrc(SEEN | CANGO | SAFE | DOOR | WALL | EVERCLR, row, col);

        unsetrc(ROOM
                | TRAP
                | ARROW
                | TRAPDOR
                | TELTRAP
                | GASTRAP
                | BEARTRP
                | DARTRAP
                | MONSTER
                | SCAREM
                | SLEEPER,
                row,
                col);

        /* LGCH: Redo currentrectangle */
        clearcurrent();

        break;
    case '.':
        /*
         * Room floor: There are many cases of what a room floor means,
         * depending on the version of Rogue, whether the room is lit, whether
         * we are in the room or not, and whether or not we were shooting
         * missiles last turn.
         */
        
        /* The square can't be any of these */
        unsetrc(HALL
                | DOOR
                | MONSTER
                | SCAREM
                | WALL
                | TRAP
                | ARROW
                | TRAPDOR
                | TELTRAP
                | GASTRAP
                | BEARTRP
                | DARTRAP,
                row,
                col);

        if(!onrc(ROOM, row, col)) { /* New room? */
            unmarkexplored(row, col);
        }

        if(rm != NONE) { /* Room here */
            levelmap[rm] |= HASROOM;
        }

        /*
         * If older Rogue, or our last position or a moving missile or
         * in the same roo, then a floor '.' means no stuff there
         */
        if(((version < RV52A)
            || (oldch == '@')
            || (oldch == ')')
            && (functionchar(lastcmd) == 't')
            || (on(ROOM) && (whichroom(row, col) == whichroom(atrow, atcol))))
           && onrc(STUFF, row, col)) {
            deletestuff(row, col);
        }

        /* If the stairs moved, look for them */
        if((oldch == '@') && onrc(STAIRS, row, col)) {
            findstairs(row, col);
        }

        /* Record whether this square has been clear of monsters */
        if(!isupper(oldch)) {
            setrc(EVERCLR, row, col);
        }

        /* Safe if we have been there, but not if the stuff was an arrow */
        if(onrc(BEEN, row, col)) {
            setrc(SAFE, row, col);
        }
        else if((oldch == ')') && (functionchar(lastcmd) == 't')) {
            unsetrc(SAFE, row, col);
        }

        /* Square must be these */
        setrc(SEEN | CANGO | ROOM, row, col);

        break;
    case '-':
    case '|':
        setrc(SEEN | WALL | EVERCLR, row, col);
        unsetrc(CANGO | HALL | DOOR | ROOM | SLEEPER, row, col);

        break;
    case ':':
    case '?':
    case '!':
    case ')':
    case ']':
    case '/':
    case '=':
    case ',': /* HAH! HAH HAH! HAH HAH HAH! */
    case '*':
        setrc(SEEN | CANGO | SAFE | EVERCLR, row, col);
        
        unset(DOOR
              | TRAP
              | ARROW
              | TRAPDOR
              | TELTRAP
              | GASTRAP
              | BEARTRP
              | DARTRAP
              | MONSTER
              | WALL
              | SLEEPER,
              row,
              col);

        if(ch != '?') {
            unsetrc(SCAREM, row, col);
        }

        if(!onrc(BEEN, row, col) || !onrc(STAIRS, row, col) || !cosmic) {
            addstuff(ch, row, col);
            unsetrc(STAIRS, row, col);
        }

        setnewgoal();

        break;
    case '%':
        if(!onrc(STAIRS, row, col)) {
            foundnew();
        }

        if((!cosmic || onrc(BEEN, row, col)) && onrc(STUFF, row, col)) {
            deletestuff(row, col);
        }

        setrc(SEEN | CANGO | SAFE | ROOM | STAIRS | EVELCLR, row, col);

        unsetrc(DOOR
                | HALL
                | TRAP
                | ARROW
                | TRAPDOR
                | TELTRAP
                | GASTRAP
                | BEARTRP
                | DARTRAP
                | MONSTER
                | SCAREM
                | SLEEPER,
                row,
                col);

        stairrow = row;
        staircol = col;
        setnewgoal();

        break;
    case '^':
        setrc(SEEN | CANGO | ROOM | TRAP | EVERCLR, row, col);

        if(onrc(STUFF, row, col)) {
            deletestuff(row, col);
        }

        unsetrc(SAFE 
                | HALL
                | DOOR
                | MONSTER
                | SCAREM
                | WALL
                | SLEEPER,
                row,
                col);

        break;
    case ' ':
        unsetrc(MONSTER | WALL, row, col);

        break;
    default:
        if(isupper(ch)) {
            monster = monname(ch);
            setrc(SEEN | CANGO | MONSTER, row, col);
            unsetc(SCAREM, row, col);
        }

        if(onrc(WALL, row, col)) { /* Infer DOOR here */
            if(!onrc(DOOR, row, col)) {
                foundnew();
                timetosearch = k_door / 5;
                
                /* MLM */
                setrc(DOOR, row, col);
                setrc(WALL, row, col);
            }
        }

        /* R5.2 MLM */
        if(!revvideo && (ch != oldch)) {
            blinded = 0;

            if(seenbefore) {
                addmonster(ch, row, col, AWAK);
            }
            else if(!onrc(HALL | DOOR, row, col)
                    && !aggravated
                    && (streq(monster, "floating eye")
                        || streq(monster, "ice monster")
                        || streq(monster, "leprechaun")
                        || streq(monster, "nymph")
                        || ((version < RV52A)
                            && ((ch == 'T') || (ch == 'P'))))) {
                addmonster(ch, row, col, ASLEEP);
                setrc(SLEEPER, row, col);
            }
            else if(onrc(HALL | DOOR, row, col) || aggravated) {
                addmonster(ch, row, col, AWAKE);
                setrc(EVERCLR, row, col);
            }
            else {
                addmonster(ch, row, col, 0);
            }
        }

        break;
    }

    /* If the stairs moved, look for the real stairs */
    if((!onrc(STAIRS, row, col) && ((row == stairrow) && (col == staircol)))
       || ((stairrow != NONE) && !onrc(STAIRS, stairrow, staircol))) {
        findstairs(row, col);
    }

    if(!couldgo && onrc(CANGO, row, col)) {
        setnewgoal();
    }
}

/*
 * teleport: We have just been teleported. Reset whatever is necessary to
 * avoid doing silly things.
 */
void teleport()
{
    int r = atrow0;
    int c = atcol0;

    goalc = NONE;
    goalr = goalc;
    setnewgoal();

    hitstokill = 0;
    darkdir = NONE;
    darkturns = 0;

    if((movedir >= 0) && (movedir < 8) && !confused) {
        ++teleported;

        while((r > 1) && (r < 23) && (c > 0) && (c < 79)) {
            if(onrc(WALL | DOOR | HALL, r, c)) {
                break;
            }

            if(onrc(TRAP, r, c)) {
                if(!onrc(ARROW | DARTRAP | GASTRAP | BEARTRP | TRAPDOR | TELTRAP, r, c)) {
                    saynow("Assuming teleport trap at %d,%d", r, c);
                }

                break;
            }

            r += deltr[movedir];
            c += deltc[movedir];
        }
    }
}

/*
 * mapinfer: Rewritten by Michael Mauldin.  August 19, 1983.
 * Infer bit settings after reading a scroll of magic mapping.
 * Becuase the mapping scroll gives extra information (in praticular
 * we now know all the room squares so we can plan run-away paths
 * properly) it is best to process the entire map making extra
 * inferences.
 */
void mapinfer()
{
    int r;
    int c;
    int inroom;

    dwait(D_CONTROL, "Map read: inferring rooms.");

    for(r = 1; r < 23; ++r) {
        inroom = 1;
        
        for(c = 0; c < 80; ++c) {
            if(seerc('|', r, c) 
               || (seerc('+', r, c) && !seerc('-', r, c - 1))) {
                inroom = !inroom;
            }
            else if(inroom) {
                setrc(ROOM | CANGO, r, c);
            }
            else {
                setrc(SEEN, r, c);
            }
        }
    }
}

/*
 * markexplored: If we are in a room, mark the location as explored.
 */
int markexplored(int row, int col)
{
    int rm = whichroom(row, col);

    if((rm != NONE) && !(levelmap[rm] & EXPLORED)) {
        levelmap[rm] |= EXPLORED;

        if(!(levelmap[rm] & HASROOM)) {
            saynow("Assuming room %d is gone.", zone);
        }
    }
}

/*
 * unmarkexplored: If we are in a room, unmark the location as explored.
 */
void unmarkedexplored(int row, int col)
{
    int rm = whichroom(row, col);

    if(rm != NONE) {
        levelmap[rm] &= ~EXPLORED;
    }
}

/*
 * isexplored: If we are in a room, return true if it has been explored.
 */
int isexplored(int row, int col)
{
    int rm = whichroom(row, col);

    if(rm != NONE) {
        return (levelmap[rm] & EXPLORED);
    }
    else {
        return 0;
    }
}

/*
 * haveexplored: Have we explored n rooms?
 */
int haveexplored(int n)
{
    int rm;
    int count = 0;

    for(rm = 0; rm < 9; ++rm) {
        if(levelmap[rm] & EXPLORED) {
            ++count;
        }
    }

    if(count >= n) {
        return 1;
    }
    else {
        return 0;
    }
}

/*
 * printexplored: List the explored rooms
 */
ovid printexplored()
{
    int rm;

    at(0, 0);
    printw("rooms explored: ");

    for(rm = 0; rm < 9; ++rm) {
        if(levelmap[rm] & EXPLORED) {
            printw(" %d", rm);
        }
    }

    clrtoeol();
    at(row, col);
    refresh();
}

/*
 * inferhall: When a door appears on the screen where no door was before,
 * check whether we can infer a hall between it and a neighboring room.
 * The technique is simple: we first determine whether the hall is already
 * known, and if ti is not, we scan away from the room looking for another
 * wall. If we find one, then we look for a door and if we find THAT then
 * we infer a hall between the matching doors. Of course, this means that
 * we must set CAN GO bits so that exploration can use the guessed hall. So
 * we set CANGO for the complete rectangle joining the two doors and then
 * rely on the CANGO bits being unset again where we actuall see blank
 * space.
 */
void inferhall(int r, int c)
{
    int i;
    int j;
    int k;
    int inc;
    int rm;
    int end1;
    int end2;
    int end;
    int dropout = 0;
    int dir = NONE;

    for(k = 0; k < 8; k += 2) {
        if(onrc(HALL, r + deltr[k], c + deltc[k])) { /* Hall has been seen */
            return;
        }
        else if(onrc(ROOM, r + deltr[c], c + deltc[k])) {  /* Room is over here */
            dir = k;
        }
    }

    dwait(D_SEARCH, "Room direction %d", dir);

    if(dir < 0) {
        return;
    }

    if(dir % 4 == 0) { /* If horizontal dir */
        inc = -deltc[dir];
        rm = whichroom(r, c);
        end1 = bounds[rm].top;
        end2 = bounds[rm].bot;

        if(inc < 0) {
            end = bounds[rm - 1].left;
        }
        else {
            end = bounds[rm + 1].right;
        }

        end = end * inc;

        for(j = (c + inc); (j * inc) < end; j += inc) {
            for(i = end1; i <= end2; ++i) {
                if(debug(D_SCREEN | D_SEARCH | D_INFORM)) {
                    mvaddch(i, j, 'h');
                }

                if(onrc(DOOR | WALL | ROOM | HALL, i, j)) {
                    /* Modified only to find doors on vertical walls */
                    if(onrc(DOOR, i, j)
                       && (onrc(WALL, i - 1, j) || onrc(WALL, i + 1, j))) {
                        connectdoors(r, c + inc, i, j - inc);
                    }

                    dropout = 1;
                }
            }

            if(dropout) {
                break;
            }
        }
    }
    else {
        inc = -deltr[dir];
        rm = whichroom(r, c);
        end1 = bounds[rm].left;
        end2 = bounds[rm].right;
        
        if(inc < 0) {
            end = bounds[rm - 3].top;
        }
        else {
            end = bounds[rm + 3].bot;
        }

        end = end * inc;

        for(i = (r + inc); (i * inc) < end; i += inc) {
            for(j = end; j <= end2; ++j) {
                if(debug(D_SCREEN | D_SEARCH | D_INFORM)) {
                    mvaddch(i, j, 'v');
                }

                if(onrc(DOOR | WALL | ROOM | HALL, i, j)) {
                    /* Modified only to find doors on horiztonal walls */
                    if(onrc(DOOR, i, j)
                       && (WALL, i, j - 1) || onrc(WALL, i, j + 1)) {
                        connectdoors(r + inc, c, i - inc, j);
                    }

                    dropout = 1;
                }
            }

            if(dropout) {
                break;
            }
        }
    }

    /* 
     * NOTE: If we set SEEN here on the three squares beyond the door, then
     * we can prevent Rogomatic's persistence in search out every
     * corridor that leads to a secret door at the other end. Or, we could set
     * a bit on the door to make it a preferred exploration target so that
     * Rogomatic would ALWAYS search out every corridor leading to a secret
     * door at the other end. The latter alternative is probably better
     * unless we implement the inferred corridors so that we can infer a
     * corridor which has a secret door and therefore we can traverse it
     * more easily one way than the other. NOTE that we must have a flag to
     * indicate why the search for a corridor failed: if it found a wall
     * then we know there is a secret door; if it stopped for another reason
     * then we don't know what we may find - maybe a room, maybe a path to a
     * corridor.
     */

    dwait(D_SEARCH | D_CONTROL, "Hall search done.");
}

void connectdoors(int r1, int c1, int r2, int c2)
{
    int r;
    int c;
    int endr = max(r1, r2);
    int endc = max(c1, c2);

    dwait(D_INFORM, "Inferring hall (%d,%d) to (%d,%d)", r1, c1, r2, c2);

    for(r = min(r1, r2); r <= endr; ++r) {
        for(c = min(c1, c2); c <= endc; ++c) {
            /* Can go (somewhere) here */
            setrc(CANGO | SAFE, r, c);
        }
    }

    for(r = min(r1, r2) - 1; r <= (endr + 1); ++r) {
        for(c = min(c1, c2) - 1; c <= (endc + 1); ++c) {
            /* Nothing to see here */
            setrc(SEEN, r, c);
        }
    }
}

/*
 * canbedoor: Called from setpsd() to check that a dead end could in fact
 * lead to a room. Only checks that there is enough empty space next to a
 * square. Does NOT check that this square is in face a dead end.
 *
 * September 25, 1983    Michael L. Mauldin
 */
int canbedoor(int deadr, int deadc)
{
    int r;
    int c;
    int dr;
    int dc;
    int k;
    int count;

    /* Check all orthogonal direction around the square */
    for(k = 0; k < 8; k += 2) {
        dr = deltr[k];
        dc = deltc[k];
        r = deadr + dr;
        c = deadc + dc;

        /* If there are four blank squares, then it could be a door */
        for(count = 0; (count < 4) && seerc(' ', r, c); ++count) {
            r += dr;
            c += dc;
        }

        if(count >= 4) {
            return 1;
        }
    }

    /* Not enough room in any direction */
    return 0;
}

/*
 * mazedoor: Return true if this could be a door to a maze
 */
int mazedoor(int row, int col)
{
    int r = row;
    int c = col;
    int dr;
    int dc;
    int k = 0;
    int dir = NONE;

    if(onrc(HALL, r, c + 1)) {
        dir = 0;
        ++k;
        dr = 0;
        dc = 1;
    }

    if(onrc(HALL, r - 1, c)) {
        dir = 2;
        ++k;
        dr = -1;
        dc = 0;
    }

    if(onrc(HALL, r + 1, c)) {
        dir = 6;
        ++k;
        dr = 1;
        dc = 0;
    }

    if(onrc(HALL, c, c - 1)) {
        dir = 4;
        ++k;
        dr = 0;
        dc = -1;
    }

    if(k != 1) {
        return 0;
    }

    /* Fail on no adjacent hall, or not double corridor */
    if(onrc(HALL, r + dr + dr, c + dc + dc) == 0) {
        return 0;
    }

    /* Must have two sets of double corridor */
    if(!((onrc(HALL, r + dr + deltr[(dir + 1) & 7], c + dc + deltc[(dir + 1) & 7])
          && onrc(HALL, r + dr + deltr[(dir + 2) & 7], c + dc + deltc[(dir + 2) & 7]))
         || (onrc(HALL, r + dr + deltr[(dir - 1) & 7], c + dc + deltc[(dir - 1) & 7])
             && onrc(HALL, r + dr + deltr[(dir + 2) & 7], c + dc + deltc[(dir - 2) & 7])))) {
        return 0;
    }

    /* If there are four blank squares, then it could be a door */
    r = row - dr;
    c = col - dc;

    for(k = 0; (k < 4) && seerc(' ', r, c); ++k) {
        r -= dr;
        c -= dc;
    }

    if(k >= 4) {
        return 1;
    }

    /* Not enough room for room */
    return 0;
}

/*
 * nextto: Is there a square type orthogonally adjacent?
 */
int nextto(int type, int r, int c)
{
    int result;

    result = onrc(type, r - 1, c);
    
    if(result) {
        return result;
    }

    result = onrc(type, r + 1, c);
    
    if(result) {
        return result;
    }

    result = onrc(type, r, c - 1);

    if(result) {
        return result;
    }

    result = onrc(type, r, c + 1);

    if(result) {
        return result;
    }

    return 0;
}

/*
 * nexttowall: Is there a wall adjacent wall?
 * 
 *         |
 * e.g.    ########|    <---- There should be a door here.
 *         |
 *
 * Fuzzy: Replaces knowisdoor(), October 17, 1983.
 */
int nexttowall(int r, int c)
{
    if((onrc(DOOR | WALL, r - 1, c) == WALL)
       || (onrc(DOOR | WALL, r + 1, c) == WALL)
       || (onrc(DOOR | WALL, r, c - 1) == WALL)
       || (onrc(DOOR | WALL, r, c + 1) == WALL)) {
        return 1;
    }
    else {
        return 0;
    }
}

/*
 * dumpmazedoor: Show all squares which which mazedoor(r, c) is true.
 */
void dumpmazdoor()
{
    int r;
    int c;

    for(r = 2; r < 22; ++r) {
        for(c = 1; c < 79; ++c) {
            if(((scrmap[r][c] & (BEEN | DOOR | HALL | ROOM | WALL | STAIRS)) == 0)
               && mazedoor(r, c)) {
                mvaddch(r, c, 'M');
            }
        }
    }

    at(row, col);
}

/*
 * foundnew: Reactivate rules with new squares to work
 */
void foundnew()
{
    new_stairs = 1;
    new_search = new_stairs;
    new_findroom = new_search;
    new_mark = new_findroom;
    teleported = 0;
    reusepsd = teleported;
    cancelmove(SECRETDOOR);
    unrest();
}
