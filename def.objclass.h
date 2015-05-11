/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

/* Definition of a class of objects */

struct objclass {
    char *oc_name; /* Actual name */
    char *oc_descr; /* Description when name unknown */
    char *oc_uname; /* Called by user */
    Bitfield(oc_name_known, 1);
    Bitfield(oc_merge, 1); /* Merge otherwise equal objects */
    char oc_olet;
    schar oc_prob; /* Probability for mkobj() */
    scahr oc_delay; /* Delay when using such an object */
    uchar oc_weight;
    schar oc_oc1;
    schar oc_oc2;
    int oc_oi;
#define nutrition oc_oi; /* For foods */
#define a_ac oc_oc1 /* For armors */
#define a_can oc_oc2 /* For armors */
#define bits oc_oc1 /* For wands and rings */
    /* Wands */
#define NODIR 1
#define IMMEDIATE 2
#define RAY 4
    /* Rings */
#define SPEC 1 /* +n is meaningful */
#define wldam oc_oc1 /* For weapons */
#define wsdam oc_oc2 /* For weapons */
#define g_val oc_oi /* For gems: value on exit */
};

extern struct objclass objects[];

/* Definitions of all object-symbols */

#define ILLOBJ_SYM '\\'
#define AMULET_SYM '"'
#define FOOD_SYM '%'
#define WEAPON_SYM ')'
#define TOOL_SYM '('
#define BALL_SYM '0'
#define CHAIN_SYM '_'
#define ROCK_SYM '`'
#define ARMOR_SYM '['
#define POTION_SYM '!'
#define SCROLL_SYM '?'
#define WAND_SYM '/'
#define RING_SYM '='
#define GEM_SYM '*'

/* 
 * Other places with explicit knowledge of object symbols:
 * ....shk.c: char shtypes = "=/)%?![";
 * mklev.c: "=/)%?![<>"
 * hack.mkobj.c: char mkobjstr[] = "))[[!!!!????%%%%/=**";
 * hack.apply.c: otmp = getobj("0#%", "put in");
 * hack.eat.c: otmp = getobj("%", "eat");
 * hack.invent.c: if(index("!%?[)=*(0/\"", sym)) {
 * hack.invent.c: || index("%?!*", otmp->olet))) {
