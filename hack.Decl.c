/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "def.gen.h" /* gen */
#include "def.monst.h" /* monst */
#include "def.obj.h" /* obj */
#include "hack.h"
#include "mklev.h" /* coord */

/* Contains zeros */
char nul[40];

/* Player name */
char plname[PL_NSIZ] = "player";

/* Long enough for login name */
char lock[32] = "1lock";

#ifdef WIZARD
/* TRUE when called as hack -w */
boolean wizard;
#endif

/* Level map */
struct rm levl[COLNO][ROWNO];

#ifndef QUEST
struct mkroom rooms[MAXNROFROOMS + 1];
coord doors[DOORMAX];
#endif

struct monst *fmon = 0;
struct gen *fgold = 0;
struct gen *ftrap = 0;
struct obj *fobj = 0;
struct obj *fcobj = 0;
struct obj *invent = 0;
struct obj *uwep = 0;
struct obj *uarm = 0;
struct obj *uarm2 = 0;
struct obj *uarmh = 0;
struct obj *uarms = 0;
struct obj *uarmg = 0;
struct obj *uright = 0;
struct obj *uleft = 0;
struct obj *uchain = 0;
struct obj *uball = 0;
struct flag flags;
struct you u;

xchar dlevel = 1;
xchar xupstair;
xchar yupstair;
xchar xdnstair;
xchar ydnstair;

unsigned char *save_cm = 0;
char *killer;
char *nomovemsg;

long moves = 1;
long wailmsg = 0;

int multi = 0;
char genocided[60];
char fut_geno[60];

xchar curx;
xchar cury;

/* Corners of lit room */
xchar seelx;
xchar seehx;
xchar seely;
schar seehy;

coord bhitpos;
