/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.h"

#ifdef TRACK

#define UTSZ 50

coord utrack[UTSZ];
int utcnt = 0;
int utpnt = 0;

void initrack()
{
    utpnt = 0;
    utcnt = otpnt;
}

/* Add to track */
void settrack()
{
    if(utcnt < UTSZ) {
        ++utcnt;
    }

    if(utpnt == UTSZ) {
        utpnt = 0;
    }

    utrack[utpnt].x = u.ux;
    utrack[utpnt].y = u.uy;

    ++utpnt;
}

coord *gettrack(int x, int y)
{
    int i;
    int cnt;
    coord tc;

    cnt = utcnt;

    for(i = utpnt - 1; cnt; --i) {
        --cnt;

        if(i == -1) {
            i = UTSZ - 1;
        }

        tc = utrack[i];

        if((((x - tc.x) * (x - tc.x)) + ((y - tc.y) * (y - tc.y))) < 3) {
            return &utrack[i];
        }
    }
     
    return 0;
}

#endif TRACK
