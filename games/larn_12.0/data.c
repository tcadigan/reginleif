/*
 * data.c
 * Larn is copyright 1986 by Noah Morgan.
 * 
 * Copyright (c) 1988 Regents of the University of California. All rights
 * reserved.
 *
 * Redistribution and use in tsource and binary forms are permitted provided
 * that the above copyright notice and this paragraph are duplicated in all such
 * forms and that any documentation, advertising materials, and other materials
 * related to such distribution and use acknoledge that the software was
 * developed by the University of California, Berkeley. The name of the
 * University may not be used to endorse or promote products derived from this
 * software without specific prior written permission. THIS SOFTWARE IS PROVIDED
 * ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, WITHOUT
 * LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * @(#)data.c    5.2 (Berkeley) 7/22/88
 */

/*
 * data.c
 * Larn is copyright 1986 by Noah Morgan.
 */

#include "header.h"

#define NODEFS

/*
 * class[c[LEVEL] - 1] gives the correct name of the player's experience level
 */
static char aa1[] = " mighty evil master";
static char aa2[] = "apprentice demi-god";
static char aa3[] = "  minor demi-god   ";
static char aa4[] = "  major demi-god   ";
static char aa5[] = "    minor deity    ";
static char aa6[] = "    major deity    ";
static char aa7[] = "  novice gaurdian  ";
static char aa8[] = "apprentice gaurdian";
static char aa9[] = "    The Creator    ";

char *class[] = {
    "  novice explorer  ",
    "apprentice explorer",
    " practiced explorer", /*   -3 */
    "   expert explorer ",
    "  novice adventurer",
    "     adventurer    ", /*   -6 */
    "apprentice conjurer",
    "     conjurer      ",
    "  master conjurer  ", /*   -9 */
    "  apprentice mage  ",
    "        mage       ",
    "  experienced mage ", /*  -12 */
    "     master mage   ",
    " apprentice warlord",
    "   novice warlord  ", /*  -15 */
    "   expert warlord  ",
    "   master warlord  ",
    " apprentice gorgon ", /*  -18 */
    "     gorgon        ",
    "  practiced gorgon ",
    "   master gorgon   ", /*  -21 */
    "    demi-gorgon    ",
    "    evil master    ",
    " great evil master ", /*  -24 */
    aa1,
    aa1,
    aa1, /*  -27 */
    aa1,
    aa1,
    aa1, /*  -30 */
    aa1,
    aa1,
    aa1, /*  -33 */
    aa1,
    aa1,
    aa1, /*  -36 */
    aa1,
    aa1,
    aa1, /*  -39 */
    aa2,
    aa2,
    aa2, /*  -42 */
    aa2,
    aa2,
    aa2, /*  -45 */
    aa2,
    aa2,
    aa2, /*  -48 */
    aa3,
    aa3,
    aa3, /*  -51 */
    aa3,
    aa3,
    aa3, /*  -54 */
    aa3,
    aa3,
    aa3, /*  -57 */
    aa4,
    aa4,
    aa4, /*  -60 */
    aa4,
    aa4,
    aa4, /*  -63 */
    aa4,
    aa4,
    aa4, /*  -66 */
    aa5,
    aa5,
    aa5, /*  -69 */
    aa5,
    aa5,
    aa5, /*  -72 */
    aa5,
    aa5,
    aa5, /*  -75 */
    aa6,
    aa6,
    aa6, /*  -78 */
    aa6,
    aa6,
    aa6, /*  -81 */
    aa6,
    aa6,
    aa6, /*  -84 */
    aa7,
    aa7,
    aa7, /*  -87 */
    aa8,
    aa8,
    aa8, /*  -90 */
    aa8,
    aa8,
    aa8, /*  -93 */
    "  earth guardian   ",
    "   air gaurdian    ",
    "   fire guardian   ", /*  -96 */
    "  water gaurdian   ",
    "  time gaurdian    ",
    " ethereal gaurdian ", /*  -99 */
    aa9,
    aa9,
    aa9, /* -102 */
};

/*
 * Table of experience needed to be a certain level of player, skill[c[LEVEL]]
 * is the experience required to attain the next level
 */
#define MEG 1000000

long skill[] = {
            0,        10,        20,        40,        80,       160,       320,       640,      1280,      2560, /*  1-10 */
         5120,     10240,     20480,     40960,    100000,    200000,    400000,    700000,   1 * MEG,   2 * MEG, /* 11-20 */
      3 * MEG,   4 * MEG,   5 * MEG,   6 * MEG,   8 * MEG,  10 * MEG,  12 * MEG,  14 * MEG,  16 * MEG,  18 * MEG, /* 21-30 */
     20 * MEG,  22 * MEG,  24 * MEG,  26 * MEG,  28 * MEG,  30 * MEG,  32 * MEG,  34 * MEG,  36 * MEG,  38 * MEG, /* 31-40 */
     40 * MEG,  42 * MEG,  44 * MEG,  46 * MEG,  48 * MEG,  50 * MEG,  52 * MEG,  54 * MEG,  56 * MEG,  58 * MEG, /* 41-50 */
     60 * MEG,  62 * MEG,  64 * MEG,  66 * MEG,  68 * MEG,  70 * MEG,  72 * MEG,  74 * MEG,  76 * MEG,  78 * MEG, /* 51-60 */
     80 * MEG,  82 * MEG,  84 * MEG,  86 * MEG,  88 * MEG,  90 * MEG,  92 * MEG,  94 * MEG,  96 * MEG,  98 * MEG, /* 61-70 */
    100 * MEG, 105 * MEG, 110 * MEG, 115 * MEG, 120 * MEG, 125 * MEG, 130 * MEG, 135 * MEG, 140 * MEG, 145 * MEG, /* 71-80 */
    150 * MEG, 155 * MEG, 160 * MEG, 165 * MEG, 170 * MEG, 175 * MEG, 180 * MEG, 185 * MEG, 190 * MEG, 195 * MEG, /* 81-90 */
    200 * MEG, 210 * MEG, 220 * MEG, 230 * MEG, 240 * MEG, 250 * MEG, 260 * MEG, 270 * MEG, 280 * MEG, 290 * MEG, /* 91-100 */
    300 * MEG
};

#undef MEG

/* Input/output pointer to the buffers */
char *lpbuf;
char *lpnt;
char *inbuffer;
char *lpend;

/* Pointer to the dungeon storage */
struct cel *cell;

/* Monster hp on level */
short hitp[MAXX][MAXY];

/* Arg for the item array */
short iarg[MAXX][MAXY];

/* Objects in maze if any */
char item[MAXX][MAXY];

/* 1 or 0 if here before */
char know[MAXX][MAXY];

/* Monster item array */
char mitem[MAXX][MAXY];

/*
 * Monster movement flags:
 *   0 = sleeping
 *   1 = awake
 */
char moved[MAXX][MAXY];

char stealth[MAXX][MAXY];

/* Inventory for player */
char iven[26];

/* Inventory for player */
short ivenarg[26];

/* This has the name of the current monster */
char lastmonst[40];

/* 1 if have been on this level */
char beenhere[MAXLEVEL + MAXVLEVEL] = {0};

/* This is the present verion # of the program */
char VERSION = (char)12;

char SUBVERSION = (char)0;

/* 
 * Set to 1 to disable the signals from doing anything,
 * 2 means that the trap handling routines must do a showplayer() after a tra.
 * 0 means don't showplayer()
 */
char nosignal = 0;

/*
 * 0 - We are in create player screen
 * 1 - We are in welcome screen
 * 2 - We are in the normal game
 */
char predostuff = 0;

/* Player's login name */
char loginname[20];

/* Player's name storage for scoring */
char logname[LOGNAMESIZE];

/* Default is a man, 0 = a woman */
char sex = 1;

/* 1 = bold objects, 0 = inverse objects */
char boldon = 1;

/* 1 if want checkpointing of game, 0 otherwise */
char ckpflag = 0;

/* 1 if palyer has fudged save file */
char cheat = 0;

/* cavelevel player is on = c[CAVELEVEL] */
char level = 0;

/* The wizard mode flag */
char wizard = 0;

/* The number of the monster last hitting player */
short lastnum = 0;

/* Flag for if player has been hit when running */
short hitflag = 0;

/* Flag for if player has been hit when running */
short hit2flag = 0;

/* Flag for if player has been hit flush input */
short hit3flag = 0;

/* The room on the present level of the player */
short playerx;
short playery;

/* 0 --- MAXX - 1 or 0 --- MAXY - 1 */
short lastpx;
short lastpy;

short oldx;
short oldy;

/* Location of monster last hit player */
short lasthx = 0;
short lasthy = 0;

/* True if program is not to beep */
short nobeep;

/* The random number seed */
unsigned long randx = 33601;

/* Time playing began */
long initialtime = 0;

/* The clock for the game */
long gtime = 0;

/* Present tax bill from score file */
long outstanding_taxes = 0;

/* The character discription arrays */
long c[100];
long cbak[100];

/* Constant for enabled/disabled scrolling region */
int enable_scroll = 0;

char aborted[] = " aborted";

/* Pointer to linked list for spheres of annihilation */
struct sphere *spheres = 0;
    
char *levelname[] = {
    " H",
    " 1",
    " 2",
    " 3",
    " 4",
    " 5",
    " 6",
    " 7",
    " 8",
    " 9",
    "10",
    "V1",
    "V2",
    "V3"
};

char objnamelist[] = " ATOP%^F&^+M=%^$$f*OD#~][[)))(((||||||||{?!BC}o:@.<<<<EVV))([[]]])^ [H*** ^^ S tsTLc............................................";
char monstnamelist[] = " BGHJKOScjtAELNQRZabhiCTYdegmvzFWflorXV pqsyUKMcDDPxnDDuD        .............................................................";

char *objectname[] = {
    0,
    "a holy altar",
    "a handsome jewel encrusted throne",
    "the orb",
    "a pit",
    "a staircase leading upwards",
    "an elevator going up",
    "a bubbling fountain",
    "a great marble statue",
    "a teleport trap",
    "the college of Larn",
    "a dead fountain",
    "gold",
    "an open door",
    "a closed door",
    "a wall",
    "The Eye of Larn",
    "plate mail",
    "chain mail",
    "leather armor",
    "a sword of slashing",
    "Bessman's flailing hammer",
    "a sunsword",
    "a two handed sword",
    "a spear",
    "a dagger",
    "ring of extra regeneration",
    "a ring of regeneration",
    "a ring of protection",
    "an energy ring",
    "a ring of dexterity",
    "a ring of strength",
    "a ring of cleverness",
    "a ring of increase damage",
    "a belt of striking",
    "a magic scroll",
    "a magic potion",
    "a book",
    "a chest",
    "an amulet of invisibility",
    "an orb of dragon slaying",
    "a scarab of negat spirit",
    "a cube of undead control",
    "device of theft prevention",
    "a brilliant diamond",
    "a ruby",
    "an enchanting emerald",
    "a sparkling sapphire",
    "the dungeon entrance",
    "a volcanic shaft leaning downward",
    "the base of a volcanic shaft",
    "a battle axe",
    "a longsword",
    "a flail",
    "ring mail",
    "studded leather armor",
    "splint mail",
    "plate armor",
    "stainless plate armor",
    "a lance of death",
    "an arrow trap",
    "an arrow trap",
    "a shield",
    "your home",
    "gold",
    "gold",
    "gold",
    "a dart trap",
    "a dart trap",
    "a trapdoor",
    "a trapdoor",
    "the local trading post",
    "a teleport trap",
    "a massive throne",
    "a sphere of annihilation",
    "a handsome jewel encrusted throne",
    "the Larn Revenue Service",
    "a fortune cookie",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};

/*
 * For the monster data
 *
 * Array to do rnd() to creat monsters <= a given level
 */
char monstlevel[] = { 5, 11, 17, 22, 27, 33, 39, 42, 46, 50, 52, 56, 59 };

struct monst monster[] = {
    /*                 NAME  LV    AC DAM ATT DEF GEN INT GOLD  HP     EXP */
    /*---------------------------------------------------------------------*/
    {                    "",  0,    0,  0,  0, 0, 0,  3,    0,   0,      0 },
    {                 "bat",  1,    0,  1,  0, 0, 0,  3,    0,   1,      1 },
    {               "gnome",  1,   10,  1,  0, 0, 0,  8,   30,   2,      2 },
    {           "hobgoblin",  1,   14,  2,  0, 0, 0,  5,   25,   3,      2 },
    {              "jackal",  1,   17,  1,  0, 0, 0,  4,    0,   1,      1 },
    {              "kobold",  1,   20,  1,  0, 0, 0,  7,   10,   1,      1 },
    {                 "orc",  2,   12,  1,  0, 0, 0,  9,   40,   4,      2 },
    {               "snake",  2,   15,  1,  0, 0, 0,  3,    0,   3,      1 },
    {     "giant centipede",  2,   14,  0,  4, 0, 0,  3,    0,   1,      2 },
    {              "jaculi",  2,   20,  1,  0, 0, 0,  3,    0,   1,      2 },
    {          "troglodyte",  2,   10,  2,  0, 0, 0,  5,   80,   4,      3 },
    {           "giant ant",  2,    8,  1,  4, 0, 0,  4,    0,   5,      5 },
    {        "floating eye",  3,    8,  1,  0, 0, 0,  3,    0,   5,      2 },
    {          "leprechaun",  3,    3,  0,  8, 0, 0,  3, 1500,  13,     45 },
    {               "nymph",  3,    3,  0, 14, 0, 0,  9,    0,  18,     45 },
    {              "quasit",  3,    5,  3,  0, 0, 0,  3,    0,  10,     15 },
    {        "rust monster",  3,    4,  0,  1, 0, 0,  3,    0,  18,     25 },
    {              "zombie",  3,   12,  2,  0, 0, 0,  3,    0,   6,      7 },
    {        "assassin bug",  4,    9,  3,  0, 0, 0,  3,    0,  20,     15 },
    {             "bugbear",  4,    5,  4, 15, 0, 0,  5,   40,  20,     35 },
    {          "hell hound",  4,    5,  2,  2, 0, 0,  6,    0,  16,     35 },
    {          "ice lizard",  4,   11,  2, 10, 0, 0,  6,   50,  16,     25 },
    {             "centaur",  4,    6,  4,  0, 0, 0, 10,   40,  24,     25 },
    {               "troll",  5,    4,  5,  0, 0, 0,  9,   80,  50,    300 },
    {                "yeti",  5,    6,  4,  0, 0, 0,  5,   50,  35,    100 },
    {        "white dragon",  5,    2,  4,  5, 0, 0, 16,  500,  55,   1000 },
    {                 "elf",  5,    8,  1,  0, 0, 0, 15,   50,  22,     35 },
    {     "gelatinous cube",  5,    9,  1,  0, 0, 0,  3,    0,  22,     45 },
    {           "metamorph",  6,    7,  3,  0, 0, 0,  3,    0,  30,     40 },
    {              "vortex",  6,    4,  3,  0, 0, 0,  3,    0,  30,     55 },
    {              "ziller",  6,   15,  3,  0, 0, 0,  3,    0,  30,     35 },
    {        "violet fungi",  6,   12,  3,  0, 0, 0,  3,    0,  38,    100 },
    {              "wraith",  6,    3,  1,  6, 0, 0,  3,    0,  30,    325 },
    {           "forvalaka",  6,    2,  5,  0, 0, 0,  7,    0,  50,    280 },
    {           "lama nobe",  7,    7,  3,  0, 0, 0,  6,    0,  35,     80 },
    {             "osequip",  7,    4,  3, 16, 0, 0,  4,    0,  35,    100 },
    {               "rothe",  7,   15,  5,  0, 0, 0,  3,  100,  50,    250 },
    {                "xorn",  7,    0,  6,  0, 0, 0, 13,    0,  60,    300 },
    {             "vampire",  7,    3,  4,  6, 0, 0, 17,    0,  50,   1000 },
    {   "invisible stalker",  7,    3,  6,  0, 0, 0,  5,    0,  50,    350 },
    {         "poltergeist",  8,    1,  4,  0, 0, 0,  3,    0,  50,    450 },
    {      "disenchantress",  8,    3,  0,  9, 0, 0,  3,    0,  50,    500 },
    {     "shambling mound",  8,    2,  5,  0, 0, 0,  6,    0,  45,    400 },
    {        "yellow mould",  8,   12,  4,  0, 0, 0,  3,    0,  35,    250 },
    {          "umber hulk",  8,    3,  7, 11, 0, 0, 14,    0,  65,    600 },
    {          "gnome king",  9,   -1, 10,  0, 0, 0, 18, 2000, 100,   3000 },
    {               "mimic",  9,    5,  6,  0, 0, 0,  8,    0,  55,     99 },
    {          "water lord",  9,  -10, 15,  7, 0, 0, 20,    0, 150,  15000 },
    {       "bronze dragon",  9,    2,  9,  3, 0, 0, 16,  300,  80,   4000 },
    {        "green dragon",  9,    3,  8, 10, 0, 0, 15,  200,  70,   2500 },
    {         "purple worm",  9,   -1, 11,  0, 0, 0,  3,  100, 120,  15000 },
    {               "xvart",  9,   -2, 12,  0, 0, 0, 13,    0,  90,   1000 },
    {         "spirit naga", 10,  -20, 12, 12, 0, 0, 23,    0,  95,  20000 },
    {       "silver dragon", 10,   -1, 12,  3, 0, 0, 20,  700, 100,  10000 },
    {     "platinum dragon", 10,   -5, 15, 13, 0, 0, 22, 1000, 130,  24000 },
    {        "green urchin", 10,   -3, 12,  0, 0, 0,  3,    0,  85,   5000 },
    {          "red dragon", 10,   -2, 13,  3, 0, 0, 19,  800, 110,  14000 },
    {   "type I demon lord", 12,  -30, 18,  0, 0, 0, 20,    0, 140,  50000 },
    {  "type II demon lord", 13,  -30, 18,  0, 0, 0, 21,    0, 160,  75000 },
    { "type III demon lord", 14,  -30, 18,  0, 0, 0, 22,    0, 180, 100000 },
    {  "type IV demon lord", 15,  -35, 20,  0, 0, 0, 23,    0, 200, 125000 },
    {   "type V demon lord", 16,  -40, 22,  0, 0, 0, 24,    0, 220, 150000 },
    {  "type VI demon lord", 17,  -45, 24,  0, 0, 0, 25,    0, 240, 175000 },
    { "type VII demon lord", 18,  -70, 27,  6, 0, 0, 26,    0, 260, 200000 },
    {        "demon prince", 25, -127, 30,  6, 0, 0, 28,    0, 345, 300000 }
};

/* Name array for scrolls */
char *scrollname[] = {
    "\0enchant armor",
    "\0enchant weapon",
    "\0enlightenment",
    "\0blank paper",
    "\0create artifact",
    "\0aggravate monsters",
    "\0time warp",
    "\0teleportation",
    "\0expanded awareness",
    "\0haste monsters",
    "\0monster healing",
    "\0spirit protection",
    "\0undead protection",
    "\0stealth",
    "\0magic mapping",
    "\0hold monsters",
    "\0gem perfection",
    "\0spell extension",
    "\0identify",
    "\0remove curse",
    "\0annihiliation",
    "\0pulverization",
    "\0life protection",
    "\0 ",
    "\0 ",
    "\0 ",
    "\0 "
};

/* Name array for magic potions */
char *potionname[] = {
    "\0sleep",
    "\0healing",
    "\0raise level",
    "\0increase ability",
    "\0wisdom",
    "\0strength",
    "\0raise charisma",
    "\0dizziness",
    "\0learning",
    "\0gold detection",
    "\0forgetfulness",
    "\0water",
    "\0blindness",
    "\0confusion",
    "\0heroism",
    "\0sturdiness",
    "\0giant strength",
    "\0fire resistance",
    "\0treasure finding",
    "\0instant healing",
    " cure diathroritis",
    "\0poison",
    "\0see invisible",
    "\0 ",
    "\0 ",
    "\0 ",
    "\0 ",
    "\0 ",
    "\0 ",
    "\0 ",
    "\0 ",
    "\0 ",
    "\0 ",
    "\0 "
};

/* Spell data */
char spelknow[SPNUM] = {0};
char splev[] = { 1, 4, 9, 14, 18, 22, 26, 29, 32, 35, 37, 37, 37, 37, 37 };

char *spelcode[] = {
    "pro",
    "mle",
    "dex",
    "sle",
    "chm",
    "ssp",
    "web",
    "str",
    "enl",
    "hel",
    "cbl",
    "cre",
    "pha",
    "inv",
    "bal",
    "cld",
    "ply",
    "can",
    "has",
    "ckl",
    "vpr",
    "dry",
    "lit",
    "drl",
    "glo",
    "flo",
    "fgr",
    "sca",
    "hld",
    "stp",
    "tel",
    "mfi", /* 31 */
    "sph",
    "gen",
    "sum",
    "wtw",
    "alt",
    "per"
};

char *spelname[] = {
    "protection",
    "magic missile",
    "dexterity",
    "sleep",
    "charm monster",
    "sonic spear",
    "web",
    "strength",
    "enlightenment",
    "healing",
    "cure blindness",
    "create monster",
    "phantasmal forces",
    "invisibility",
    "fireball",
    "cold",
    "polymorph",
    "cancellation",
    "haste self",
    "cloud kill",
    "vaporize rock",
    "dehydration",
    "lightning",
    "drain life",
    "invulnerability",
    "flood",
    "finger of death",
    "scare monster",
    "hold monster",
    "time stop",
    "teleport away",
    "magic fire"
    "sphere of annihilation",
    "genocide",
    "summon demon",
    "walk through walls",
    "alter reality",
    "permanence",
    ""
};

char *speldescript[] = {
    /* 1 */
    "generates a +2 protection field",
    "creates and hurls a magic missile equivalent to a +1 magic arrow",
    "adds +2 to the caster's dexterity",
    "cuases some monsters to go to sleep",
    "some monsters may be awed at your magnificence",
    "causes your hands to emit a screeching sound toward what they point",
    /* 7 */
    "causes strands of sticky thread to entangle an enemy",
    "adds +2 to the caster's strength for a short time",
    "the caster becomes aware of things around him",
    "restores some hp to the caster",
    "restores sight to one so unfortunate as to be blinded",
    "creates a monster near the caster appropriate for the location",
    "creates illusions, and if believed, monsters die",
    "the caster becomes invisible",
    /* 15 */
    "makes a ball of fire that burns on what it hits",
    "sends forth a cone of cold which freezes what it touches",
    "you can find out what this does yourself",
    "negates the ability of a monster to use his special abilities",
    "speeds up the caster's movements",
    "creates a fog of poisonous gas which kills all that is within it",
    "this changes rock to air",
    /* 22 */
    "dries up water in the immediate vicinity",
    "your finger will emit a lightning bolt when this spell is cast",
    "subtracts hit points from both you and a monster",
    "this globe helps to protect the player from physical attack",
    "this creates an avalanche of H2O to flood the immediate chamber",
    "this is a holy spell and calls upon your god to back you up",
    /* 28 */
    "terrifies the monster so that hopefully he won't hit the magic user",
    "the monster is frozen in his tracks if this is successful",
    "all movement in the caverns ceases for a limited duration",
    "moves a particular monster around in the dungeon (hopefully away from you)",
    "this causes a curtain of fire to appear all around you",
    /* 33 */
    "anything caught in this sphere is instantly killed. Warning -- dangerous",
    "eliminates a species of monster from the game -- use sparingly",
    "summons a demon who hopefully helps you out",
    "allows the player to walk through walls for a short period of time",
    "god only knows what this will do",
    "makes a character spell permanent, i.e. protection, strength, etc.",
    ""
};

char spelweird[MAXMONST + 8][SPNUM] = {
    /*                        p   m  d  s  c   s   w  s  e  h  c  c   p  i  b   c  p  c  h  c  v  d  l  d  g   f  f  s  h  s  t  m  s  g  s  w  a  p */
    /*                        r   l  e  l  h   s   e  t  n  e  b  r   h  n  a   l  l  a  a  k  p  r  i  r  l   l  g  c  l  t  e  f  p  e  u  t  l  e */
    /*                        o   e  x  e  m   p   b  r  l  l  l  e   a  v  l   d  y  n  s  l  r  y  t  l  o   o  r  a  d  p  l  i  h  n  m  w  t  r */
     
    /*               bat */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 
    /*             gnome */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 5, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*         hobgoblin */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*            jackal */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*            kobold */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 5, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*               orc */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 4, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*             snake */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*   giant centipede */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*            jaculi */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*        troglodyte */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*         giant ant */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*      floating eye */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*        leprechaun */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*             nymph */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*            quasit */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*      rust monster */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 4, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*            zombie */ { 0,  0, 0, 8, 0,  4,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 4, 0, 4, 0, 0, 0,  0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*      assassin bug */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*           bugbear */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 5, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*        hell hound */ { 0,  6, 0, 0, 0,  0, 12, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*        ice lizard */ { 0,  0, 0, 0, 0,  0, 11, 0, 0, 0, 0, 0,  0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*           centaur */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*             troll */ { 0,  7, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 5, 0,  0, 0, 0, 0, 0, 0, 4, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*              yeti */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*      white dragon */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 14, 0, 0, 15, 0, 0, 0, 0, 0, 4, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*               elf */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 14, 5, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*   gelatinous cube */ { 0,  0, 0, 0, 0,  0,  2, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 4, 0, 0, 0, 0, 0,  0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*         metamorph */ { 0, 13, 0, 0, 0,  0,  2, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 4, 0, 4, 0, 0, 0,  0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*            vortex */ { 0, 13, 0, 0, 0, 10,  1, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 4, 0, 4, 0, 0, 0,  4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*            ziller */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*      violet fungi */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*            wraith */ { 0,  0, 0, 8, 0,  4,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 4, 0, 4, 0, 0, 0,  0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*         forvalaka */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 5, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*         lama nobe */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*           osequip */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*             rothe */ { 0,  7, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 5, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*              xorn */ { 0,  7, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 5, 0,  0, 0, 0, 0, 0, 0, 4, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*           vampire */ { 0,  0, 0, 8, 0,  4,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 4, 0, 0, 0, 0, 0,  0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* invisible stalker */ { 0,  0, 0, 0, 0,  0,  1, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*       poltergeist */ { 0, 13, 0, 8, 0,  4,  1, 0, 0, 0, 0, 0,  0, 0, 0,  4, 0, 0, 0, 4, 0, 4, 0, 0, 0,  4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*    disenchantress */ { 0,  0, 0, 8, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*   shambling mound */ { 0,  0, 0, 0, 0, 10,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*      yellow mould */ { 0,  0, 0, 8, 0,  0,  1, 0, 0, 0, 0, 0,  4, 0, 0,  0, 0, 0, 0, 4, 0, 0, 0, 0, 0,  0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*        umber hulk */ { 0,  7, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 5, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*        gnome king */ { 0,  7, 0, 0, 3,  0,  0, 0, 0, 0, 0, 0,  0, 5, 0,  0, 9, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*             mimic */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*        water lord */ { 0, 13, 0, 8, 3,  4,  1, 0, 0, 0, 0, 0,  0, 0, 0,  0, 9, 0, 0, 4, 0, 0, 0, 0, 0, 16, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*     bronze dragon */ { 0,  7, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*      green dragon */ { 0,  7, 0, 0, 0,  0, 11, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*       purple worm */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*             xvart */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*       spirit naga */ { 0, 13, 0, 8, 3,  4,  1, 0, 0, 0, 0, 0,  0, 5, 0,  4, 9, 0, 0, 4, 0, 4, 0, 0, 0,  4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*     silver dragon */ { 0,  6, 0, 9, 0,  0, 12, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*   platinum dragon */ { 0,  7, 0, 9, 0,  0, 11, 0, 0, 0, 0, 0, 14, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*      green urchin */ { 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*        red dragon */ { 0,  6, 0, 0, 0,  0, 12, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /*        demon lord */ { 0,  7, 0, 4, 3,  0,  1, 0, 0, 0, 0, 0, 14, 5, 0,  0, 4, 0, 0, 4, 0, 4, 0, 0, 0,  4, 4, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0 },
    /*        demon lord */ { 0,  7, 0, 4, 3,  0,  1, 0, 0, 0, 0, 0, 14, 5, 0,  0, 4, 0, 0, 4, 0, 4, 0, 0, 0,  4, 4, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0 },
    /*        demon lord */ { 0,  7, 0, 4, 3,  0,  1, 0, 0, 0, 0, 0, 14, 5, 0,  0, 4, 0, 0, 4, 0, 4, 0, 0, 0,  4, 4, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0 },
    /*        demon lord */ { 0,  7, 0, 4, 3,  0,  1, 0, 0, 0, 0, 0, 14, 5, 0,  0, 4, 0, 0, 4, 0, 4, 0, 0, 0,  4, 4, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0 },
    /*        demon lord */ { 0,  7, 0, 4, 3,  0,  1, 0, 0, 0, 0, 0, 14, 5, 0,  0, 4, 0, 0, 4, 0, 4, 0, 0, 0,  4, 4, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0 },
    /*        demon lord */ { 0,  7, 0, 4, 3,  0,  1, 0, 0, 0, 0, 0, 14, 5, 0,  0, 4, 0, 0, 4, 0, 4, 0, 0, 0,  4, 4, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0 },
    /*        demon lord */ { 0,  7, 0, 4, 3,  0,  1, 0, 0, 0, 0, 0, 14, 5, 0,  0, 4, 0, 0, 4, 0, 4, 0, 0, 0,  4, 4, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0 },
    /*      demon prince */ { 0,  7, 0, 4, 3,  9,  1, 0, 0, 0, 0, 0, 14, 5, 0,  0, 4, 0, 0, 4, 0, 4, 0, 0, 0,  4, 4, 4, 0, 0, 0, 4, 9, 0, 0, 0, 0, 0 }
};

char *spelmes[] = {
    /*  0 */ "",
    /*  1 */ "the web had no effect on the %s",
    /*  2 */ "the %s changed shape to avoif the web",
    /*  3 */ "the %s isn't afraid of you",
    /*  4 */ "the %s isn't affected",
    /*  5 */ "the %s can see you with his infravision",
    /*  6 */ "the %s vaporizes your missile",
    /*  7 */ "your missile bounces off the %s",
    /*  8 */ "the %s doesn't sleep",
    /*  9 */ "the %s resists",
    /* 10 */ "the %s can't hear the noise",
    /* 11 */ "the %s's tail cuts it free of the web",
    /* 12 */ "the %s burns through the web",
    /* 13 */ "your missiles pass right through the %s",
    /* 14 */ "the %s sees through your illusions",
    /* 15 */ "the %s loves the cold!",
    /* 16 */ "the %s loves the water!"
};

/*
 * Function to create scroll numbers with appropriate probability of occurrence
 * 
 *  0 - armor                  1 - weapon                2 - enlightenment
 *  3 - paper                  4 - create monster        5 - create item
 *  6 - aggravate              7 - time warp             8 - teleportation
 *  9 - expanded awareness    10 - haste monster        11 - heal monster
 * 12 - spirit protection     13 - undead protection    14 - stealth
 * 15 - magic mapping         16 - hold monster         17 - gem perfection
 * 18 - spell extraction      19 - identify             18 - spell extension
 * 21 - annihilation          22 - puverization         13 - life protection
 */
char scprob[] = {
     0,  0,  0,  0,  1,  1,  1,  1,  1,  2,
     2,  2,  2,  2,  2,  3,  3,  3,  3,  3,
     4,  4,  4,  5,  5,  5,  5,  5,  6,  6,
     6,  6,  6,  7,  7,  7,  7,  8,  8,  8,
     9,  9,  9,  9, 10, 10, 10, 11, 11, 11,
    12, 12, 12, 13, 13, 13, 13, 14, 14, 15,
    15, 16, 16, 16, 17, 17, 18, 18, 19, 19,
    20, 20, 20, 21, 22, 22, 22, 23
};

/*
 * Function to return a potion numer created with appropriate probability of
 * occurrence
 *
 *  0 - sleep                     1 - healing               2 - raise level
 *  3 - increase ability          4 - gain wisdom           5 - gain strenth
 *  6 - charismatic character     7 - dizziness             8 - learning
 *  9 - gold detection           10 - monster detection    11 - forgetfulness
 * 12 - water                    13 - blindness            14 - confusion
 * 15 - heroism                  16 - sturdiness           17 - giant strength
 * 18 - fire resistance          19 - treasure finding     20 - instant healing
 * 21 - cure dianthroritis       22 - poison               23 - see invisible
 */
char potprob[] = {
     0,  0,  1,  1,  1,  2,  3,  3,  4,  4,
     5,  5,  6,  6,  7,  7,  8,  9,  9,  9,
    10, 10, 10, 11, 11, 12, 12, 13, 14, 15,
    16, 17, 18, 19, 19, 20, 20, 22, 22, 23,
    23
};

char nlpts[] = { 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 6, 7 };
char nch[] = { 0, 0, 0, 1, 1, 1, 2, 2, 3, 4 };
char nplt[] = { 0, 0, 0, 0, 1, 1, 2, 2, 3, 4 };
char ndgg[] = { 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 4, 5 };
char nsw[] = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 3 };
