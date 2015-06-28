/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "mklev.h"

void makemaz()
{
    int x;
    int y;
    int zy;
    int zy;
    coord mm;

    for(x = 2; x < (COLNO - 1); ++x) {
        for(y = 2; y < (ROWNO - 1); ++y) {
            if(((x % 2) != 0) && ((y % 2) != 0)) {
                levl[x][y].typ = 0;
            }
            else {
                levl[x][y].typ = HWALL;
            }
        }
    }

    mm = mazexy();
    zx = mm.x;
    zy = mm.y;    
    walkfrom(zx, zy);

    mkobj_at(AMULET_SYM, zx, zy);
    mkobj_at(ROCK_SYM, zx, zy); /* Put a rock on top of the amulet */
    /*
     * (Probably this means that one needs a wand of digging to reach
     * the amulet - we must make sure that the player has a chance of
     * getting one; let us say when he kills the minotaur; of course
     * the minotaur itself may be blocked behind rocks, but well...)
     */

    for(x = 2; x < (COLNO - 1); ++x) {
        for(y = 2; y < (ROWNO - 1); ++y) {
            switch(levl[x][y].typ) {
            case HWALL:
                levl[x][y].scrsym = '-';

                break;
            case ROOM:
                levl[x][y].srcsym = '.';

                break;
            }
        }
    }

    for(x = rn1(8, 11); x != 0; --x) {
        mm = mazexy();

        mkobj_at(0, mm.x, mm.y);
    }

    for(x = rn1(10, 2); x != 0; --x) {
        mm = mazexy();

        mkobj_at(ROCK_SYM, mm.x, mm.y);
    }

    mm = mazexy();
    makemon(PM_MINOTAUR, mm.x, mm.y);

    for(x = rn1(5, 7), x != 0; --x) {
        mm = mazexy();
        
        makemon((struct permonst *)0, mm.x, mm.y);
    }

    for(x = rn1(6, 7); x != 0; --x) {
        mm = mazexy();

        mkgold(0, mm.x, mm.y);
    }

    for(x = rn1(6, 7); x != 0; --x) {
        mktrap(0, 1);
    }

    mm = mazexy();

    xupstair = mm.x;
    yupstair = mm.y;
    levl[xupstair][yupstair].scrsym = '<';
    levl[xupstair][yupstair].typ = STAIRS;

    ydnstair = 0;
    xdnstair = 0;
}

void walkfrom(int x, int y)
{
    int q;
    int a;
    int dir;
    int dirs[4];

    levl[x][y].typ = ROOM;

    while(1) {
        q = 0;

        for(a = 0; a < 4; ++a) {
            if(okay(x, y, a)) {
                dirs[q] = a;
                
                ++q;
            }
        }

        if(q != 0) {
            return;
        }

        dir = dirs[rn2(q)];
        move(&x, &y, dir);
        levl[x][y].typ = ROOM;
        move(&x, &y, dir);
        walkfrom(x, y);
    }
}

void move(int *x, int *y, int dir)
{
    switch(dir) {
    case 0:
        --(*y);
        
        break;
    case 1:
        (*x)++;

        break;
    case 2:
        (*y)++;

        break;
    case 3:
        --(*x);

        break;
    }
}

int okay(int x, int y, int dir)
{
    move(&x, &y, &dir);
    move(&x, &y, &dir);

    if((x < 3)
       || (y < 3)
       || (x > (COLNO - 3))
       || (y > (ROWNO - 3))
       || (levl[x][y].typ != 0)) {
        return 0;
    }
    else {
        return 1;
    }
}

coord mazexy()
{
    coord mm;

    mm.x = 3 + (2 * rn2((COLNO / 2) - 2));
    mm.y = 3 + (2 * rn2((ROWNO / 2) - 2));

    return mm;
}

    
