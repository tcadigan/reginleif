/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#ifndef MKLEV_H_
#define MKLEV_H_

#include "config.h"

#include <stdio.h>

#ifdef BSD
#include <strings.h> /* Declarations for strcat etc. */
#else
#include <string.h> /* Idem on System V */

#define index strchr
#define rindex strrchr
#endif

#include "def.objclass.h"

typedef struct {
    xchar x;
    xchar y;
} coord;

#include "def.gen.h"
#include "def.obj.h"

#define BUFSZ 256 /* For getline buffers */
#define PL_NSIZ 32 /* Name of player, ghost, shopkeeper */

#define HWALL 1 /* Level location types */
#define VWALL 2
#define SDOOR 3
#define SCORR 4
#define LDOOR 5
#define DOOR 6 /* Smallest accessible type */
#define CORR 7
#define ROOM 8
#define STAIRS 9

#ifdef QUEST
#define CORR_SYM ':'
#else
#define CORR_SYM '#'
#endif

#define ERRCHAR '{'

#define TRAPNUM 9

struct rm {
    char scrsym;
    unsigned int typ:5;
    unsigned int new:1;
    unsigned int seen:1;
    unsigned int lit:1;
};

extern struct rm levl[COLNO][ROWNO];

#ifndef QUEST
struct mkroom {
    xchar lx;
    xchar hx;
    xchar ly;
    xchar hy;
    schar rtype;
    schar rlit;
    schar doorct;
    schar fdoor;
};

#define MAXNROFROOMS 15

extern struct mkroom rooms[MAXNROFROOMS + 1];

#define DOORMAX 100

extern coord doors[DOORMAX];
#endif

#include "def.permonst.h"

extern struct permonst mons[];

#define PM_ACIDBLOB &mons[7]
#define PM_PIERC &mons[17]
#define PM_MIMIC &mons[37]
#define PM_CHAM &mons[47]
#define PM_DEMON &mons[54]
#define PM_MINOTAUR &mons[55] /* Last in the mons array */
#define PM_SHK &mons[56] /* Very last */
#define CMNUM 55 /* Number of common monsters */

extern long *alloc();

/* Stairs up and down. */
extern xchar xdnstair;
extern xchar ydnstair;
extern xchar xupstair;
extern xchar yupstair;

extern xchar dlevel;

#ifdef WIZARD
extern boolean wizard;
#endif

#define newstring(x) (char *)alloc((unsigned int)(x))

#endif
