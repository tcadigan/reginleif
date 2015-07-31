/*
 * header.h
 * Larn is copyrighted 1986 by Noah Morgan.
 */

#include <stdio.h>

/* Max # of levels in the dungeon */
#define MAXLEVEL 11

/* Max # of levels in the temple of luran */
#define MAXVLEVEL 3

#define MAXX 67
#define MAXY 17

/* This is the number of people on a scoreboard max */
#define SCORESIZE 10

/* Maximum player level allowed */
#define MAXPLEVEL 100

/* Maximum # monsters in the dungeon */
#define MAXMONST 56

/* Maximum number of spells in existance */
#define SPNUM 38

/* Maximum number of scrolls that are possible */
#define MAXSCROLL 28

/* Maximum number of potions that are possible */
#define MAXPOTION 35

/* The maximum number of moves before the game is called */
#define TIMELIMIT 30000

/* The tax rate for the LRS */
#define TAXRATE 1/20

/* The maximum number of objects n < MAXOBJ */
#define MAXOBJ 93

/* This is the structure definition of the monster data */
struct monst {
    char *name;
    char level;
    short armorclass;
    char damage;
    char attack;
    char defense;
    char genocided;
    /* Monster's intelligence -- used to choose movement */
    char intelligence;
    short gold;
    short hitpoints;
    unsigned long experience;
};

/* This is the structure definition for the items in the dnd store */
struct _itm {
    short price;
    char **mem;
    char obj;
    char arg;
    char qty;
};

/* This is the structure that holds the entire dungeon specifications */
struct cel {
    /* Monter's hit points */
    char hitp;

    /* The monster ID */
    char mitem;

    /* The object's ID */
    char item;

    /* The object's argument */
    short iarg;

    /* Have webeen here before */
    char know;
};

/* 
 * This is the structure for maintaining and moving the spehere of
 * annihilation 
 */
struct sphere {
    /* Pointer to the next structure */
    struct sphere *p;

    /* Location of the sphere */
    char x;
    char y;
    char lev;

    /* Direction sphere is going in */
    char dir;

    /* Duration of the sphere */
    char lifetime;
};

/* Defines for the character array c[] */
/* Character's physical strength not due to objects */
#define STRENGTH 0
#define INTELLIGENCE 1
#define WISDOM 2
#define CONSTITUTION 3
#define DEXTERITY 4
#define CHARISMA 5
#define HPMAX 6
#define HP 7
#define GOLD 8
#define EXPERIENCE 9
#define LEVEL 10
#define REGEN 11
#define WCLASS 12
#define AC 13
#define BANKACCOUNT 14
#define SPELLMAX 15
#define SPELLS 16
#define ENERGY 17
#define ECOUNTER 18
#define MOREDEFENCES 19
#define WEAR 20
#define PROTECTIONTIME 21
#define WIELD 22
#define AMULET 23
#define REGENCOUNTER 24
#define MOREDAM 25
#define DEXCOUNT 26
#define STRCOUNT 27
#define BLINDCOUNT 28
#define CAVELEVEL 29
#define CONFUSE 30
#define ALTPRO 31
#define HERO 32
#define CHARMCOUNT 33
#define INVISIBILITY 34
#define CANCELLATION 35
#define HASTESELF 36
#define EYEOFLARN 37
#define AGGRAVATE 38
#define GLOBE 39
#define TELEFLAG 40
#define SLAYING 41
#define NEGATESPIRIT 42
#define SCAREMONST 43
#define AWARENESS 44
#define HOLDMONST 45
#define TIMESTOP 46
#define HASTEMONST 47
#define CUBEOFUNDEAD 48
#define GIANTSTR 49
#define FIRERESISTANCE 50
#define BESSMANN 51
#define NOTHEFT 52
#define HARDGAME 53
#define CPUTIME 54
#define BYTESIN 55
#define BYTESOUT 56
#define MOVESMADE 57
#define MONSTKILLED 58
#define SPELLSCAST 59
#define LANCEDEATH 60
#define SPIRITPRO 61
#define UNDEADPRO 62
#define SHIELD 63
#define STEALTH 64
#define ITCHING 65
#define LAUGHING 66
#define DRAINSTRENGTH 67
#define CLUMSINESS 68
#define INFEEBLEMENT 69
#define HALFDAM 70
#define SEEINVISIBLE 71
#define FILLROOM 72
#define RANDOMWALK 73

/* NZ if an active sphere of annihilation */
#define SPHCAST 74

/* Walk through walls */
#define WTW 75

/* Character strength due to objects or enchantments */
#define STREXTRA 76

/* Misc scratch space */
#define TMP 77

/* Life protection counter */
#define LIFEPROT 78

/* Defines for the objects in the game */
#define OALTAR 1
#define OTHRONE 2
#define OORB 3
#define OPIT 4
#define OSTAIRSUP 5
#define OELEVATORUP 6
#define OFOUNTAIN 7
#define OSTATUE 8
#define OTELEPORTER 9
#define OSCHOOL 10
#define OMIRROR 11
#define ODNDSTORE 12
#define OSTAIRSDOWN 13
#define OELEVATORDOWN 14
#define OBANK2 15
#define OBANK 16
#define ODEADFOUNTAIN 17
#define OMAXGOLD 70
#define OGOLDPILE 18
#define OOPENDOOR 19
#define OCLOSEDOOR 20
#define OWALL 21
#define OTRAPARROW 66
#define OTRAPARROWIV 67
#define OLARNEYE 22
#define OPLATE 23
#define OCHAIN 34
#define OLEATHER 25
#define ORING 60
#define OSTUDLEATHER 61
#define OSPLINT 62
#define OPLATEARMOR 63
#define OSSPLATE 64
#define OSHIELD 68
#define OELVENCHAIN 92
#define OSWORDOFSLASHING 26
#define OHAMMER 27
#define OSWORD 28
#define O2SWORD 29
#define OSPEAR 30
#define ODAGGER 31
#define OBATTLEAXE 57
#define OLONGSWORD 58
#define OFLAIL 59
#define OLANCE 65
#define OVORPAL 90
#define OSLAYER 91
#define ORINGOFEXTRA 32
#define OREGENRING 33
#define OPROTRING 34
#define OENERGYRING 35
#define ODEXRING 36
#define OSTRRING 37
#define OCLEVERRING 38
#define ODAMRING 39
#define OBELT 40
#define OSCROLL 41
#define OPOTION 42
#define OBOOK 43
#define OCHEST 44
#define OAMULET 45
#define OORBOFDRAGON 46
#define OSPIRITSCARAB 47
#define OCUBEOFUNDEAD 48
#define ONOTHEFT 49
#define ODIAMOND 50
#define ORUBY 51
#define OEMERALD 52
#define OSAPPHIRE 53
#define OENTRANCE 54
#define OVOLDOWN 55
#define OVOLUP 56
#define OHOME 69
#define OKGOLD 71
#define ODGOLD 72
#define OIVDARTRAP 73
#define ODARTRAP 74
#define OTRAPDOOR 75
#define OIVTRAPDOOR 76
#define OTRADEPOST 77
#define OIVTELETRAP 78
#define ODEADTHRONE 79

/* Sphere of annihilation */
#define OANNIHILATION 80

#define OTHRONE2 81

/* Larn Revenue Service */
#define OLRS 82

#define OCOOKIE 83
#define OURN 84;
#define OBRASSLAMP 85

/* Hand of fear */
#define OHANDOFFEAR 86

/* Tailsman of the sphere */
#define OSPHTAILSMAN

/* Wand of wonder */
#define OWWAND 88

/* Staff of power */
#define OPSTAFF 89

/* Used up to 92 */

/* Defines for the monsters as objects */
#define BAT 1
#define GNOME 2
#define HOBGOBLIN 3
#define JACKAL 4
#define KOBOLD 5
#define ORC 6
#define SNAKE 7
#define CENTIPEDE 8
#define JACULI 9
#define TROGLODYTE 10
#define ANT 11
#define EYE 12
#define LEPRECHAUN 13
#define NYMPH 14
#define QUASIT 15
#define RUSTMONSTER 16
#define ZOMBIE 17
#define ASSASSINBUG 18
#define BUGBEAR 19
#define HELLHOUND 20
#define ICELIZARD 21
#define CENTAUR 22
#define TROLL 23
#define YETI 24
#define WHITEDRAGON 25
#define ELF 26
#define CUBE 27
#define METAMORPH 28
#define VORTEX 29
#define ZILLER 30
#define VIOLETFUNGI 31
#define WRAITH 32
#define FORVALAKA 33
#define LAMANOBE 34
#define OSEQUIP 35
#define ROTHE 36
#define XORN 37
#define VAMPIRE 38
#define INVISIBLESTALKER 39
#define POLTERGEIST 40
#define DISENCHANTRESS 41
#define SHAMBLINGMOUND 42
#define YELLOWMOULD 43
#define UMBERHULK 44
#define GNOMEKING 45
#define MIMIC 46
#define WATERLORD 27
#define BRONZEDRAGON 48
#define GREENDRAGON 49
#define PURPLEWORM 50
#define XVART 51
#define SPIRITNAGA 52
#define SILVERDRAGON 53
#define PLATINUMDRAGON 54
#define GREENURCHIN 55
#define REDDRAGON 56
#define DEMONLORD 57
#define DEMONPRINCE 64

/* Size of the output buffer */
#define BUFBIG 4096

/* Size of the input buffer */
#define MAXIBUF 4096

/* Max size of the player's name */
#define LOGNAMESIZE 40

/* Max size of the process name */
#define PSNAMESIZE 40

#ifndef NODEFS
extern char VERSION;
extern char SUBVERSION;
extern char aborted[];
extern char alpha[];
extern char beenhere[];
extern char boldon;
extern char cheat;
extern char ckpfile[];
extern char ckpflag;
extern char *class[];
extern char course[];
extern char diagfile[];
extern char fortfile[];
extern char helpfile[];
extern char *inbuffer;
extern char is_alpha[];
extern char is_digit[];
extern char item[MAXX][MAXY];
extern char iven[];
extern char know[MAXX][MAXY];
extern char larnlevels[];
extern char lastmonst[];
extern char level;
extern char *levelname[];
extern char logfile[];
extern char loginname[];
extern char logname[];
extern char *lpbuf;
extern char *lpend;
extern char *lpnt;
extern char moved[MAXX][MAXY];
extern char mitem[MAXX][MAXY];
extern char monstlevel[];
extern char monstnamelist[];
extern char nch[];
extern char ndgg[];
extern char nlpts[];
extern char nomove;
extern char nosignal;
extern char nowelcome;
extern char nplt[];
extern char nsw[];
extern char *objectname[];
extern char objnamelist[];
extern char optsfile[];
extern char *potionname[];
extern char playerids[];
extern char potprob[];
extern char predostuff;
extern char psname[];
extern char restorflag;
extern char savefilename[];
extern char scorefile[];
extern char scprob[];
extern char screen[MAXX][MAXY];
extern char *scrollname[];
extern char sex;
extern char *spelcode[];
extern char *speldescript[];
extern char spelknow[];
extern char *spelname[];
extern char *spelmes[];
extern char spelweird[MAXMONST + 8][SPNUM];
extern char splev[];
extern char stealth[MAXX][MAXY];
extern char to_lower[];
extern char to_upper[];
extern char wizard;
extern short diroffx[];
extern short diroffy[];
extern short hitflag;
extern short hit2flag;
extern short hit3flag;
extern short hitp[MAXX][MAXY];
extern short iarg[MAXX][MAXY];
extern short ivenarg[];
extern short lasthx;
extern short lasthy;
extern short lastnum;
extern short lastpx;
extern short lastpy;
extern short nobeep;
extern short oldx;
extern short oldy;
extern short playerx;
extern short playery;
extern int dayplay;
extern int enable_scroll;
extern int srcount;
extern int yrepcount;
extern int userid;
extern int wisid;
extern FILE *lfd;
extern FILE *fd;
extern long initialtime;
extern long outstanding_taxes;
extern long skill[];
extern long gtime;
extern long c[];
extern long cbak[];
extern unsigned long randx;
extern struct cel *cell;
extern struct monst monster[];
extern struct sphere *spheres;
extern struct _itm itm[];

/* Macro to create scroll #'s with probability of occurrence */
#define newscroll() (scprob[rand() % 81])

/* Macro to return a potion # created with probability of occurrence */
#define newpotion() (potprob[rand() % 41])

/* Macro to destroy object at present location */
#define forget()					\
    know[playerx][playery] = 0;				\
    item[playerx][playery] = know[playerx][playery]

/* Macro to wipe out a monster at a location */
#define disappear(x, y)				\
    know[x][y] = 0;				\
    mitem[x][y] = know[x][y]

#ifdef VT100
/* Macro to turn on bold display for the terminal */
#define setbold()				\
    if(boldon) {				\
	lprcat("\33[1m");			\
    }						\
    else {					\
	lprcat("\33[7m");			\
    }

/* Macro to turn off bold display for the terminal */
#define resetbold() (lprcat("\33[m"))

/* Macro to setup the scrolling region for the terminal */
#define setscroll() (lprcat("\33[20;24r"))

/* Macro to clear the scrolling region for the terminal */
#define resetscroll() (lprcat("\33[;24r"))

#define cltoeoln() lprcat("\33[K")

#else
/* Defines below are for use in the termcap mode only */
#define ST_START 1
#define ST_END 2
#define BOLD 3
#define END_BOLD 4
#define CLEAR 5
#define CL_LINE 6
#define CL_DOWN 14
#define CURSOR 15

/* Macro to turn on bold display for the terminal */
#define setbold() (*lpnt++ = ST_START)

/* Macro to turn off bold display for the terminal */
#define resetbold() (*lpnt++ = ST_END)

/* Macro to setup the scrolling region for the terminal */
#define setscroll() enable_scroll = 1;

/* Macro to clear the scrolling region for the terminal */
#define resetscroll() enable_scroll = 0;

/* Macro to clear to end of line */
#define cltoeoln() (*lpnt++ = CL_LINE)

#endif

/* Macro to output one byte to the output buffer */
#define lprc(ch)				\
    if(lpnt >= lpend) {				\
	*lpnt++ = (ch);				\
	lflush();				\
    }						\
    else {					\
	*lpnt++ = (ch);				\
    }

/* Macros for miscellaneous data conversion */
#define min(x, y) (((x) > (y)) ? (y) : (x))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define isalpha(x) (is_alpha[x])
#define isdigit(x) (is_digit[x])
#define tolower(x) (to_lower[x])
#define toupper(x) (to_upper[x])
#define lcc(x) (to_lower[x])
#define ucc(x) (to_upper[x])

#endif
