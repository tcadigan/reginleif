#ifndef HACK_ENGRAVE_H_
#define HACK_ENGRAVE_H_

#include "config.h" /* xchar */

struct engr {
    struct engr *nxt_engr;
    char *engr_txt;
    xchar engr_x;
    xchar engr_y;

    /* For save and restore, not length of text */
    unsigned int engr_lth;
    
    /* Moment engraving was (will be) finished */
    long engr_time;
    xchar engr_type;

#define DUST 1
#define ENGRAVE 2
#define BURN 3
};

void read_engr_at(int x, int y);
void wipe_engr_at(xchar x, xchar y, xchar cnt);
int doengrave();
void del_engr(struct engr *ep);

#endif
