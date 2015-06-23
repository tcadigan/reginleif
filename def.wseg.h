/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#ifndef NOWORM

/* Worm structure */
struct wseg {
    struct wseg *nseg;
    xchar wx;
    xchar wy;
    unsigned int wdispl:1;
};

#define newseg() (struct wseg *)alloc(sizeof(struct wseg))

#endif
