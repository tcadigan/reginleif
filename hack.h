/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#ifndef HACK_H_
#define HACK_H_

#include "config.h" /* xchar */
#include "def.monst.h" /* monst */
#include "def.obj.h" /* obj */

#include "mklev.h"
#include "hack.onames.h"

#define ON 1
#define OFF 0

extern struct obj *invent;
extern struct obj *uwep;
extern struct obj *uarm;
extern struct obj *uarm2;
extern struct obj *uarmh;
extern struct obj *uarms;
extern struct obj *uarmg;
extern struct obj *uleft;
extern struct obj *uright;
extern struct obj *fcobj;

/* Defined if and only if PUNISHED */
extern struct obj *uchain;

/* Defined if PUNISHED */
extern struct obj *uball;

struct flag {
    /* Social security number for each monster */
    unsigned ident;

    /*
     * A top line (message) has been printed 
     * 0: Top line empty
     * 2: No --More-- required
     */
    unsigned int topl:2;

    /* In cbreak mode, rogue fomat */
    unsigned int cbreak:1;
    
    /* Give inventories 1 line at a time */
    unsigned int oneline:1;
    unsigned int move:1;
    unsigned int mv:1;

    /*
     * 0: h (etc.)
     * 1: H (etc.)
     * 2: fh (etc.)
     * 3: FH
     * 4: ff+
     * 5: ff-
     * 6: FF+
     * 7: FF-
     */
    unsigned int run:3;
    
    /* Do not pickup objects */
    unsigned int nopick:1;
    
    /* 1 to echo characters */
    unsigned int echo:1;
    
    /* Partially redo status line */
    unsigned int botl:1;
    
    /* Print an entirely new bottom line */
    unsigned int botlx:1;

    /* Inhibit nscr() in pline() */
    unsigned int nscrinh:1;
};

extern struct flag flags;

struct prop {
    /* Mask */
#define TIMEOUT 007777
    
    /* 010000L */
#define LEFT_RING W_RINGL
    
    /* 020000L */
#define RIGHT_RING W_RINGR
#define INTRINSIC 040000L
#define LEFT_SIDE LEFT_RING
#define RIGHT_SIDE RIGHT_RING
#define BOTH_SIDES (LEFT_SIDE | RIGHT_SIDE)

    long p_flgs;
    
    /* Called after timeout */
    void (*p_tofn)();
};

struct you {
    xchar ux;
    xchar uy;

    /* Direction of fast move */
    schar dx;
    schar dy;

#ifdef QUEST
    /* Direction of FF */
    schar di;

    /* Initial position FF */
    xchar ux0;
    xchar uy0;
#endif

    /* Last display pos */
    xchar udisx;
    xchar udisy;

    /* Usually '@' */
    char usym;
    schar uluck;

    /*
     * 0: None
     * 1: Half turn
     * 2: Full turn
     * +: Turn right
     * -: Turn left
     */
    int last_str_turn:3;

    /* @ on display */
    unsigned int udispl:1;
    unsigned int ulevel:5;

#ifdef QUEST
    unsigned int uhorizon:7;
#endif

    /* Trap timeout */
    unsigned int utrap:3;

    /* Defined in utrap nonzero */
    unsigned int utraptype:1;

#define TT_BEARTRAP 0
#define TT_PIT 1

    unsigned int uinshop:1;

    /* Perhaps these #define's should also be generated by makedefs */
    /* Not a ring */
#define TELEPAT LAST_RING
#define Telepat u.uprops[TELEPAT].p_flgs
    /* Not a ring */
#define FAST (LAST_RING + 1)
#define Fast u.uprops[FAST].p_flgs
    /* Not a ring */
#define CONFUSION (LAST_RING + 2)
#define Confusion u.uprops[CONFUSION].p_flgs
    /* Not a ring */
#define INVIS (LAST_RING + 3)
#define Invis u.uprops[INVIS].p_flgs
    /* Not a ring */
#define GLIB (LAST_RING + 4)
#define Glib u.uprops[GLIB].p_flgs
    /* Not a ring */
#define PUNISHED (LAST_RING + 5)
#define Punished u.uprops[PUNISHED].p_flgs
    /* Not a ring */
#define SICK (LAST_RING + 6)
#define Sick u.uprops[SICK].p_flgs
    /* Not a ring */
#define BLIND (LAST_RING + 7)
#define Blind u.uprops[BLIND].p_flgs
    /* Not a ring */
#define WOUNDED_LEGS (LAST_RING + 8)
#define Wounded_legs u.uprops[WOUNDED_LEGS].p_flgs
    /* Convert ring to index in uprops */
#define PROP(x) (x - RIN_ADORNMENT)

    unsigned int umconf:1;
    
    char *usick_cause;
    struct prop uprops[LAST_RING + 9];

    /* Set if swallowed by a monster */
    unsigned int uswallow:1;
    
    /* Time you have been swallowed */
    unsigned int uswldtim:4;

    /* Hunger state - see hack.eat.c */
    unsigned int uhs:3;

    schar ustr;
    schar ustrmax;
    schar udaminc;
    schar uac;
    int uhp;
    int uhpmax;
    long int ugold;
    long int ugold0;
    long int uexp;
    long int urexp;

    /* Referred only in eat.c and shk.c */
    int uhunger;
    int uinvault;

    struct monst *ustuck;

    /* Used for experience bookkeeping */
    int nr_killed[CMNUM + 2];
};

extern struct you u;
extern char *traps[];

extern char vowels[];

/* Cursor location on screen */
extern xchar curx;
extern xchar cury;

/* Place where thrown weapon falls to the ground */
extern coord bhitpos;

/* Where to see */
extern xchar seehx;
extern xchar seelx;
extern xchar seehy;
extern xchar seely;
extern char *save_cm;
extern char *killer;

/* Dungeon level */
extern xchar dlevel;
extern xchar maxdlevel;
extern long moves;
extern int multi;
extern char lock[];

#define DIST(x1, y1, x2, y2) ((((x1) - (x2)) * ((x1) - (x2))) + ((y1) - ((y2)) * ((y1) - (y2))))

/* Size of pl_character */
#define PL_CSIZ 20

/* So that boulders can be heavier */
#define MAX_CARR_CAP 120

/* Position outside screen */
#define FAR (COLNO + 2)

void losehp_m(int n, struct monst *mtmp);
void nomul(int nval);
void setsee();
int cansee(xchar x, xchar y);
void unsee();
int inv_cnt();
void pickup();
void movobj(struct obj *obj, int ox, int oy);
int abon();
int dbon();
int inv_weight();
void domove();
int movecm(char *cmd);
void seeoff(int mode);
void losestr(int num);
void losehp(int n, char *knam);
void losexp();
void lookaround();
int isok(int x, int y);
int sgn(int a);

#endif
