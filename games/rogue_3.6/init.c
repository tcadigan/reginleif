// Global variable initialization
//
// @(#)init.c 3.33 (Berkeley) 6/15/81

#include "init.h"

#include "list.h"
#include "main.h"

#include <ctype.h>

/* TC_DEBUG: Start */
#include "debug.h"
#include <stdio.h>
/* TC_DEBUG: Finish */

bool playing = TRUE;
bool running = FALSE;
bool wizard = FALSE;
bool notify = TRUE;
bool fight_flush = FALSE;
bool terse = FALSE;
bool door_stop = FALSE;
bool jump = FALSE;
bool slow_invent = FALSE;
bool firstmove = FALSE;
bool askme = FALSE;
bool amulet = FALSE;
bool in_shell = FALSE;
struct linked_list *lvl_obj = NULL;
struct linked_list *mlist = NULL;
struct object *cur_weapon = NULL;
int mpos = 0;
int no_move = 0;
int no_command = 0;
int level = 1;
int purse = 0;
int inpack = 0;
int total = 0;
int no_food = 0;
int count = 0;
int fung_hit = 0;
int quiet = 0;
int food_left = HUNGERTIME;
int group = 1;
int hungry_state = 0;
int lastscore = -1;


struct thing monsters[26] = {
    { "giant ant", /* Name */
      { 0, 0 },    /* Current position */
      0,           /* Turn to move */
      'A',         /* Type */
      '\0',        /* Prior location character */
      NULL,        /* Destination */
      ISMEAN,      /* Flags */
      '\0',        /* Mimic disguise */
      0,           /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,   10,   2,     3,  1,             "1d6" },
      NULL         /* Pack */
    },
    { "bat",    /* Name */
      { 0, 0 }, /* Current position */
      0,        /* Turn to move */
      'B',      /* Type */
      '\0',     /* Prior location character */
      NULL,     /* Destination */
      0,        /* Flags */
      '\0',     /* Mimic disguise */
      0,        /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,    1,   4,     4,  1,             "1d2" },
      NULL      /* Pack */
    },
    { "centaur", /* Name */
      { 0, 0 },  /* Current position */
      0,         /* Turn to move */
      'C',       /* Type */
      '\0',      /* Prior location character */
      NULL,      /* Destination */
      0,         /* Flags */
      '\0',      /* Mimic disguise */
      15,        /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,   15,   4,     4,  1,         "1d6/1d6" },
      NULL       /* Pack */
    },
    { "dragon", /* Name */
      { 0, 0 }, /* Current position */
      0,        /* Turn to move */
      'D',      /* Type */
      '\0',     /* Prior location character */
      NULL,     /* Destination */
      ISGREED,  /* Flags */
      '\0',     /* Mimic disguise */
      100,      /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,                Dmg */
      {    1,         1, 9000,  10,    -1,  1,     "1d8/1d8/3d10" },
      NULL      /* Pack */
    },
    { "floating eye", /* Name */
      { 0, 0 },       /* Current position */
      0,              /* Turn to move */
      'E',            /* Type */
      '\0',           /* Prior location character */
      NULL,           /* Destination */
      0,              /* Flags */
      '\0',           /* Mimic disguise */
      0,              /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,                Dmg */
      {    1,         1,    5,   1,     9,  1,              "0d0" },
      NULL            /* Pack */
    },
    { "violet fungi", /* Name */
      { 0, 0 },       /* Current position */
      0,              /* Turn to move */
      'F',            /* Type */
      '\0',           /* Prior location character */
      NULL,           /* Destination */
      ISMEAN,         /* Flags */
      '\0',           /* Mimic disguise */
      0,              /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,                Dmg */
      {    1,         1,   85,   8,     3,  1,            "000d0" },
      NULL            /* Pack */
    },
    { "gnome",  /* Name */
      { 0, 0 }, /* Current position */
      0,        /* Turn to move */
      'G',      /* Type */
      '\0',     /* Prior location character */
      NULL,     /* Destination */
      0,        /* Flags */
      '\0',     /* Mimic disguise */
      10,       /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,                Dmg */
      {    1,         1,    8,   1,     5,  1,              "1d6" },
      NULL      /* Pack */
    },
    { "hobgoblin", /* Name */
      { 0, 0 },    /* Current position */
      0,           /* Turn to move */
      'H',         /* Type */
      '\0',        /* Prior location character */
      NULL,        /* Destination */
      ISMEAN,      /* Flags */
      '\0',        /* Mimic disguise */
      0,           /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,    3,   1,     5,  1,             "1d8" },
      NULL         /* Pack */
    },
    { "invisible stalker", /* Name */
      { 0, 0 },            /* Current position */
      0,                   /* Turn to move */
      'I',                 /* Type */
      '\0',                /* Prior location character */
      NULL,                /* Destination */
      ISINVIS,             /* Flags */
      '\0',                /* Mimic disguise */
      0,                   /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,  120,   8,     3,  1,              "4d4" },
      NULL                 /* Pack */
    },
    { "jackel", /* Name */
      { 0, 0 }, /* Current position */
      0,        /* Turn to move */
      'J',      /* Type */
      '\0',     /* Prior location character */
      NULL,     /* Destination */
      ISMEAN,   /* Flags */
      '\0',     /* Mimic disguise */
      0,        /* Probabilty to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,    2,   1,     7,  1,             "1d2" },
      NULL      /* Pack */
    },
    { "kobold", /* Name */
      { 0, 0 }, /* Current position */
      0,        /* Turn to move */
      'K',      /* Type */
      '\0',     /* Prior location character */
      NULL,     /* Destination */
      ISMEAN,   /* Flags */
      '\0',     /* Mimic disguise */
      0,        /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,    1,   1,     7,   1,            "1d4" },
      NULL      /* Pack */
    },
    { "leprechaun", /* Name */
      { 0, 0 },     /* Current position */
      0,            /* Turn to move */
      'L',          /* Type */
      '\0',         /* Prior location character */
      NULL,         /* Destination */
      0,            /* Flags */
      '\0',         /* Mimic disguise */
      0,            /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,   10,   3,     8,  1,             "1d1" },
      NULL          /* Pack */
    },
    { "mimic",  /* Name */
      { 0, 0 }, /* Current position */
      0,        /* Turn to move */
      'M',      /* Type */
      '\0',     /* Prior location character */
      NULL,     /* Destination */
      0,        /* Flags */
      '\0',     /* Mimic disguise */
      30,       /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,  140,   7,     7,  1,             "3d4" },
      NULL      /* Pack */
    },
    { "nymph",  /* Name */
      { 0, 0 }, /* Current position */
      0,        /* Turn to move */
      'N',      /* Type */
      '\0',     /* Prior location character */
      NULL,     /* Destination */
      0,        /* Flags */
      '\0',     /* Mimic disguise */
      100,      /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,   40,   3,     9,  1,             "0d0" },
      NULL      /* Pack */
    },
    { "orc",    /* Name */
      { 0, 0 }, /* Current position */
      0,        /* Turn to move */
      'O',      /* Type */
      '\0',     /* Prior location character */
      NULL,     /* Destination */
      ISBLOCK,  /* Flags */
      '\0',     /* Mimic disguise */
      15,       /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,    5,   1,     6,  1,             "1d8" },
      NULL      /* Pack */
    },
    { "purple worm", /* Name */
      { 0, 0 },      /* Current position */
      0,             /* Turn to move */
      'P',           /* Type */
      '\0',          /* Prior location character */
      NULL,          /* Destination */
      0,             /* Flags */
      '\0',          /* Mimic disguise */
      70,            /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1, 7000,  15,     6,  1,        "2d12/2d4" },
      NULL           /* Pack */
    },
    { "quasit", /* Name */
      { 0, 0 }, /* Current position */
      0,        /* Turn to move */
      'Q',      /* Type */
      '\0',     /* Prior location character */
      NULL,     /* Destination */
      ISMEAN,   /* Flags */
      '\0',     /* Mimic disguise */
      30,       /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,   35,   3,     2,  1,     "1d2/1d2/1d4" },
      NULL      /* Pack */
    },
    { "rust monster", /* Name */
      { 0, 0 },       /* Current position */
      0,              /* Turn to move */
      'R',            /* Type */
      '\0',           /* Prior location character */
      NULL,           /* Destination */
      ISMEAN,         /* Flags */
      '\0',           /* Mimic disguise */
      0,              /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,   25,   5,     2,  1,         "0d0/0d0" },
      NULL            /* Pack */
    },
    { "snake",  /* Name */
      { 0, 0 }, /* Current position */
      0,        /* Turn to move */
      'S',      /* Type */
      '\0',     /* Prior location character */
      NULL,     /* Destination */
      ISMEAN,   /* Flags */
      '\0',     /* Mimic disguise */
      0,        /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,    3,   1,     5,  1,             "1d3" },
      NULL      /* Pack */
    },
    { "troll",          /* Name */
      { 0, 0 },         /* Current position */
      0,                /* Turn to move */
      'T',              /* Type */
      '\0',             /* Prior location character */
      NULL,             /* Destination */
      ISREGEN | ISMEAN, /* Flags */
      '\0',             /* Mimic disguise */
      50,               /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,   55,   6,     4,  1,     "1d8/1d8/2d6" },
      NULL              /* Pack */
    },
    { "umber hulk", /* Name */
      { 0, 0 },     /* Current position */
      0,            /* Turn to move */
      'U',          /* Type */
      '\0',         /* Prior location character */
      NULL,         /* Destination */
      ISMEAN,       /* Flags */
      '\0',         /* Mimic disguise */
      40,           /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,  130,   8,     2,  1,     "3d4/3d4/2d5" },
      NULL          /* Pack */
    },
    { "vampire",        /* Name */
      { 0, 0 },         /* Current position */
      0,                /* Turn to move */
      'V',              /* Type */
      '\0',             /* Prior location character */
      NULL,             /* Destination */
      ISREGEN | ISMEAN, /* Flags */
      '\0',             /* Mimic disguise */
      20,               /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,  380,   8,     1,  1,            "1d10" },
      NULL              /* Pack */
    },
    { "wraith", /* Name */
      { 0, 0 }, /* Current position */
      0,        /* Turn to move */
      'W',      /* Type */
      '\0',     /* Prior location character */
      NULL,     /* Destination */
      0,        /* Flags */
      '\0',     /* Mimic disguise */
      0,        /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,   55,   5,     4,  1,             "1d6" },
      NULL      /* Pack */
    },
    { "xorn",   /* Name */
      { 0, 0 }, /* Current position */
      0,        /* Turn to move */
      'X',      /* Type */
      '\0',     /* Prior location character */
      NULL,     /* Destination */
      ISMEAN,   /* Flags */
      '\0',     /* Mimic disguise */
      0,        /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,  120,   7,    -2,  1, "1d3/1d3/1d3/4d5" },
      NULL      /* Pack */
    },
    { "yeti",   /* Name */
      { 0, 0 }, /* Current position */
      0,        /* Turn to move */
      'Y',      /* Type */
      '\0',     /* Prior location character */
      NULL,     /* Destination */
      0,        /* Flags */
      '\0',     /* Mimic disguise */
      30,       /* Probability to carry */

      /* Str, Bonus str,  Exp, Lvl, Armor, Hp,               Dmg */
      {    1,         1,   50,   5,     6,  1,         "1d6/1d6" },
      NULL      /* Pack */
    },
    { "zombie", /* Name */
      { 0, 0 }, /* Current position */
      0,        /* Turn to move */
      'Z',      /* Type */
      '\0',     /* Prio location character */
      NULL,     /* Destination */
      ISMEAN,   /* Flags */
      '\0',     /* Mimic disguise */
      0,        /* Probability to carry */

      /* Str, Bonus str, Exp, Lvl, Armor, Hp,                Dmg */
      {    1,         1,   7,   2,     4,  1,               "1d8" },
      NULL      /* Pack */
    }
};

struct object things[NUMTHINGS] = {
    { "Potion", /* Name */
      "",       /* Read text */
      POTION,   /* Type */
      -1,       /* Which of type */
      -1,       /* Group */
      { 0, 0 }, /* Position */
      27,       /* Probability */
      1,        /* Count of object */
      0,        /* Flags */
      0,        /* Worth */
      11,       /* Armor class */
      '\0',     /* Launcher */
      "",       /* Damage */
      "",       /* Hurl damage */
      0,        /* Hit bonus */
      0         /* Damage bonus */
    },
    { "Scroll", /* Name */
      "",       /* Read text */
      SCROLL,   /* Type */
      -1,       /* Which of type */
      -1,       /* Group */
      { 0, 0 }, /* Position */
      54,       /* Probability */
      1,        /* Count of object */
      0,        /* Flags */
      0,        /* Worth */
      11,       /* Armor class */
      '\0',     /* Launcher */
      "",       /* Damage */
      "",       /* Hurl damage */
      0,        /* Hit bonus */
      0         /* Damage bonus */
    },
    { "Food",   /* Name */
      "",       /* Read text */
      FOOD,     /* Type */
      -1,       /* Which of type */
      -1,       /* Group */
      { 0, 0 }, /* Position */
      72,       /* Probability */
      1,        /* Count of object */
      0,        /* Flags */
      0,        /* Worth */
      11,       /* Armor class */
      '\0',     /* Launcher */
      "",       /* Damage */
      "",       /* Hurl damage */
      0,        /* Hit bonus */
      0         /* Damage bonus */
    },
    { "Weapon", /* Name */
      "",       /* Read text */
      WEAPON,   /* Type */
      -1,       /* Which of type */
      -1,       /* Group */
      { 0, 0 }, /* Position */
      81,       /* Probability */
      1,        /* Count of object */
      0,        /* Flags */
      0,        /* Worth */
      11,       /* Armor class */
      '\0',     /* Launcher */
      "",       /* Damage */
      "",       /* Hurl damage */
      0,        /* Hit bonus */
      0         /* Damage bonus */
    },
    { "Armor",  /* Name */
      "",       /* Read text */
      ARMOR,    /* Type */
      -1,       /* Which of type */
      -1,       /* Group */
      { 0, 0 }, /* Position */
      90,       /* Probability */
      1,        /* Count of object */
      0,        /* Flags */
      0,        /* Worth */
      11,       /* Armor class */
      '\0',     /* Launcher */
      "",       /* Damage */
      "",       /* Hurl Damage */
      0,        /* Hit bonus */
      0         /* Damage bonus */
    },
    { "Ring",   /* Name */
      "",       /* Read text */
      RING,     /* Type */
      -1,       /* Which of type */
      -1,       /* Group */
      { 0, 0 }, /* Position */
      95,       /* Probability */
      1,        /* Count of object */
      0,        /* Flags */
      0,        /* Worth */
      11,       /* Armor class */
      '\0',     /* Launcher */
      "",       /* Damage */
      "",       /* Hurl damage */
      0,        /* Hit bonus */
      0         /* Damage bonus */
    },
    { "Stick",  /* Name */
      "",       /* Read text */
      STICK,    /* Type */
      -1,       /* Which of type */
      -1,       /* Group */
      { 0, 0 }, /* Position */
      100,      /* Probability */
      1,        /* Count of object */
      0,        /* Flags */
      0,        /* Worth */
      11,       /* Armor class */
      '\0',     /* Launcher */
      "",       /* Damage */
      "",       /* Hurl Damage */
      0,        /* Hit bonus */
      0         /* Damage bonus */
    }
};

struct object s_magic[MAXSCROLLS] = {
    { "monster confusion", /* Name */
      "",                  /* Read text */
      SCROLL,              /* Type */
      S_CONFUSE,           /* Which of type */
      -1,                  /* Group */
      { 0, 0 },            /* Position */
      8,                   /* Probability */
      1,                   /* Count of object */
      0,                   /* Flags */
      170,                 /* Worth */
      11,                  /* Armor class */
      '\0',                /* Launcher */
      ""                   /* Damage */
      "",                  /* Hurl damage */
      0,                   /* Hit bonus */
      0                    /* Damage bonus */
    },
    { "magic mapping", /* Name */
      "",              /* Read text */
      SCROLL,          /* Type */
      S_MAP,           /* Which of type */
      -1,              /* Group */
      { 0, 0 },        /* Position */
      13,              /* Probability */
      1,               /* Count of object */
      0,               /* Flags */
      180,             /* Worth */
      11,              /* Armor class */
      '\0',            /* Launcher */
      "",              /* Damage */
      "",              /* Hurl damage */
      0,               /* Hit bonus */
      0                /* Damage bonus */
    },
    { "light",  /* Name */
      "",       /* Read text */
      SCROLL,   /* Type */
      S_LIGHT,  /* Which of type */
      -1,       /* Group */
      { 0, 0 }, /* Position */
      23,       /* Probability */
      1,        /* Count of object */
      0,        /* Flags */
      100,      /* Worth */
      11,       /* Armor class */
      '\0',     /* Launcher */
      "",       /* Damage */
      "",       /* Hurl damage */
      0,        /* Hit bonus */
      0         /* Damage bonus */
    },
    { "hold monster", /* Name */
      "",             /* Read text */
      SCROLL,         /* Type */
      S_HOLD,         /* Which of type */
      -1,             /* Group */
      { 0, 0 },       /* Position */
      25,             /* Probability */
      1,              /* Count of object */
      0,              /* Flags */
      200,            /* Worth */
      11,             /* Armor class */
      '\0',           /* Launcher */
      "",             /* Damage */
      "",             /* Hurl  damage */
      0,              /* Hit bonus */
      0               /* Damage bonus */
    },
    { "sleep",  /* Name */
      "",       /* Read text */
      SCROLL,   /* Type */
      S_SLEEP,  /* Which of type */
      -1,       /* Group */
      { 0, 0 }, /* Position */
      30,       /* Probability */
      1,        /* Count of object */
      0,        /* Flags */
      50,       /* Worth */
      11,       /* Armor class */
      '\0',     /* Launcher */
      "",       /* Damage */
      "",       /* Hurl damage */
      0,        /* Hit bonus */
      0         /* Damage bonus */
    },
    { "enchant armor", /* Name */
      "",              /* Read text */
      SCROLL,          /* Type */
      S_ARMOR,         /* Which of type */
      -1,              /* Group */
      { 0, 0 },        /* Position */
      38,              /* Probability */
      1,               /* Count of object */
      0,               /* Flags */
      130,             /* Worth */
      11,              /* Armor class */
      '\0',            /* Launcher */
      "",              /* Damage */
      "",              /* Hurl damage */
      0,               /* Hit bonus */
      0                /* Damage bonus */
    },
    { "identify", /* Name */
      "",         /* Read text */
      SCROLL,     /* Type */
      S_IDENT,    /* Which of type */
      -1,         /* Group */
      { 0, 0 },   /* Position */
      59,         /* Probability */
      1,          /* Count of object */
      0,          /* Flags */
      100,        /* Worth */
      11,         /* Armor class */
      '\0',       /* Launcher */
      "",         /* Damage */
      "",         /* Hurl damage */
      0,          /* Hit bonus */
      0           /* Damage bonus */
    },
    { "scare monster", /* Name */
      "",              /* Read text */
      SCROLL,          /* Type */
      S_SCARE,         /* Which of type */
      -1,              /* Group */
      { 0, 0 },        /* Position */
      63,              /* Probability */
      1,               /* Count of object */
      0,               /* Flags */
      180,             /* Worth */
      11,              /* Armor class */
      '\0',            /* Launcher */
      "",              /* Damage */
      "",              /* Hurl damage */
      0,               /* Hit bonus */
      0                /* Damage bonus */
    },
    { "gold detection", /* Name */
      "",               /* Read text */
      SCROLL,           /* Type */
      S_GFIND,          /* Which of type */
      -1,               /* Group */
      { 0, 0 },         /* Position */
      67,               /* Probability */
      1,                /* Count of object */
      0,                /* Flags */
      110,              /* Worth */
      11,               /* Armor class */
      '\0',             /* Launcher */
      "",               /* Damage */
      "",               /* Hurl damage */
      0,                /* Hit bonus */
      0                 /* Damage bonus */
    },
    { "teleportation", /* Name */
      "",              /* Read text */
      SCROLL,          /* Type */
      S_TELEP,         /* Which of type */
      -1,              /* Group */
      { 0, 0 },        /* Position */
      74,              /* Probability */
      1,               /* Count of object */
      0,               /* Flags */
      175,             /* Worth */
      11,              /* Armor class */
      '\0',            /* Launcher */
      "",              /* Damage */
      "",              /* Hurl damage */
      0,               /* Hit bonus */
      0                /* Damage bonus */
    },
    { "enchant weapon", /* Name */
      "",               /* Read text */
      SCROLL,           /* Type */
      S_ENCH,           /* Which of type */
      -1,               /* Group */
      { 0, 0 },         /* Position */
      84,               /* Probability */
      1,                /* Count of object */
      0,                /* Flags */
      150,              /* Worth */
      11,               /* Armor class */
      '\0',             /* Launcher */
      "",               /* Damage */
      "",               /* Hurl damage */
      0,                /* Hit bonus */
      0                 /* Damage bonus */
    },
    { "create monster", /* Name */
      "",               /* Read text */
      SCROLL,           /* Type */
      S_CREATE,         /* Which of type */
      -1,               /* Group */
      { 0, 0 },         /* Position */
      89,               /* Probability */
      1,                /* Count of object */
      0,                /* Flags */
      75,               /* Worth */
      11,               /* Armor class */
      '\0',             /* Launcher */
      "",               /* Damage */
      "",               /* Hurl damage */
      0,                /* Hit bonus */
      0                 /* Damage bonus */
    },
    { "remove curse", /* Name */
      "",             /* Read text */
      SCROLL,         /* Type */
      S_REMOVE,       /* Which of type */
      -1,             /* Group */
      { 0, 0 },       /* Position */
      97,             /* Probability */
      1,              /* Count of object */
      0,              /* Flags */
      105,            /* Worth */
      11,             /* Armor class */
      '\0',           /* Launcher */
      "",             /* Damage */
      "",             /* Hurl damage */
      0,              /* Hit bonus */
      0               /* Damage bonus */
    },
    { "aggravate monsters", /* Name */
      "",                   /* Read text */
      SCROLL,               /* Type */
      S_AGGR,               /* Which of type */
      -1,                   /* Group */
      { 0, 0 },             /* Position */
      98,                   /* Probability */
      1,                    /* Count of object */
      0,                    /* Flags */
      60,                   /* Worth */
      11,                   /* Armor class */
      '\0',                 /* Launcher */
      "",                   /* Damage */
      "",                   /* Hurl damage */
      0,                    /* Hit bonus */
      0                     /* Damage bonus */
    },
    { "blank paper", /* Name */
      "",            /* Read text */
      SCROLL,        /* Type */
      S_NOP,         /* Which of type */
      -1,            /* Group */
      { 0, 0 },      /* Position */
      99,            /* Probability */
      1,             /* Count of object */
      0,             /* Flags */
      50,            /* Worth */
      11,            /* Armor class */
      '\0',          /* Launcher */
      "",            /* Damage */
      "",            /* Hurl damage */
      0,             /* Hit bonus */
      0              /* Damage bonus */
    },
    { "genocide", /* Name */
      "",         /* Read text */
      SCROLL,     /* Type */
      S_GENOCIDE, /* Which of type */
      -1,         /* Group */
      { 0, 0 },   /* Position */
      100,        /* Probability */
      1,          /* Count of object */
      0,          /* Flags */
      200,        /* Worth */
      11,         /* Armor class */
      '\0',       /* Launcher */
      "",         /* Damage */
      "",         /* Hurl damage */
      0,          /* Hit bonus */
      0           /* Damage bonus */
    }
};

struct object p_magic[MAXPOTIONS] = {
    { "confusion", /* Name */
      "",          /* Read text */
      POTION,      /* Type */
      P_CONFUSE,   /* Which of type */
      -1,          /* Group */
      { 0, 0 },    /* Position */
      8,           /* Probability */
      1,           /* Count of object */
      0,           /* Flags */
      50,          /* Worth */
      11,          /* Armor class */
      '\0',        /* Launcher */
      "",          /* Damage */
      "",          /* Hurl damage */
      0,           /* Hit bonus */
      0            /* Damage bonus */
    },
    { "paralysis", /* Name */
      "",          /* Read text */
      POTION,      /* Type */
      P_PARALYZE,  /* Which of type */
      -1,          /* Group */
      { 0, 0 },    /* Position */
      18,          /* Probability */
      1,           /* Count of object */
      0,           /* Flags */
      50,          /* Worth */
      11,          /* Armor class */
      '\0',        /* Launcher */
      "",          /* Damage */
      "",          /* Hurl damage */
      0,           /* Hit bonus */
      0            /* Damage bonus */
    },
    { "poison", /* Name */
      "",       /* Read text */
      POTION,   /* Type */
      P_POISON, /* Which of type */
      -1,       /* Group */
      { 0, 0 }, /* Position */
      26,       /* Probability */
      1,        /* Count of object */
      0,        /* Flags */
      50,       /* Worth */
      11,       /* Armor class */
      '\0',     /* Launcher */
      "",       /* Damage */
      "",       /* Hurl damage */
      0,        /* Hit bonus */
      0         /* Damage bonus */
    },
    { "gain strength", /* Name */
      "",              /* Read text */
      POTION,          /* Type */
      P_STRENGTH,      /* Which of type */
      -1,              /* Group */
      { 0, 0 },        /* Position */
      41,              /* Probability */
      1,               /* Count of object */
      0,               /* Flags */
      150,             /* Worth */
      11,              /* Armor class */
      '\0',            /* Launcher */
      "",              /* Damage */
      "",              /* Hurl damage */
      0,               /* Hit bonus */
      0                /* Damage bonus */
    },
    { "see invisible", /* Name */
      "",              /* Read text */
      POTION,          /* Type */
      P_SEEINVIS,      /* Which of type */
      -1,              /* Group */
      { 0, 0 },        /* Position */
      43,              /* Probability */
      1,               /* Count of object */
      0,               /* Flags */
      170,             /* Worth */
      11,              /* Armor class */
      '\0',            /* Launcher */
      "",              /* Damage */
      "",              /* Hurl damage */
      0,               /* Hit bonus */
      0                /* Damage bonus */
    },
    { "healing", /* Name */
      "",        /* Read text */
      POTION,    /* Type */
      P_HEALING, /* Which of type */
      -1,        /* Group */
      { 0, 0 },  /* Position */
      58,        /* Probability */
      1,         /* Count of object */
      0,         /* Flags */
      130,       /* Worth */
      11,        /* Armor class */
      '\0',      /* Launcher */
      "",        /* Damage */
      "",        /* Hurl damage */
      0,         /* Hit bonus */
      0          /* Damage bonus */
    },
    { "monster detection", /* Name */
      "",                  /* Read text */
      POTION,              /* Type */
      P_MFIND,             /* Which of type */
      -1,                  /* Group */
      { 0, 0 },            /* Position */
      64,                  /* Probability */
      1,                   /* Count of object */
      0,                   /* Flags */
      120,                 /* Worth */
      11,                  /* Armor class */
      '\0',                /* Launcher */
      "",                  /* Damage */
      "",                  /* Hurl damage */
      0,                   /* Hit bonus */
      0                    /* Damage bonus */
    },
    { "magic detection", /* Name */
      "",                /* Read text */
      POTION,            /* Type */
      P_TFIND,           /* Which of type */
      -1,                /* Group */
      { 0, 0 },          /* Position */
      70,                /* Probability */
      1,                 /* Count of object */
      0,                 /* Flags */
      105,               /* Worth */
      11,                /* Armor class */
      '\0',              /* Launcher */
      "",                /* Damage */
      "",                /* Hurl damage */
      0,                 /* Hit bonus */
      0                  /* Damage bonus */
    },
    { "raise level", /* Name */
      "",            /* Read text */
      POTION,        /* Type */
      P_RAISE,       /* Which of type */
      -1,            /* Group */
      { 0, 0 },      /* Position */
      72,            /* Probability */
      1,             /* Count of object */
      0,             /* Flags */
      220,           /* Worth */
      11,            /* Armor class */
      '\0',          /* Launcher */
      "",            /* Damage */
      "",            /* Hurl damage */
      0,             /* Hit bonus */
      0              /* Damage bonus */
    },
    { "extra healing", /* Name */
      "",              /* Read text */
      POTION,          /* Type */
      P_XHEAL,         /* Which of type */
      -1,              /* Group */
      { 0, 0 },        /* Position */
      77,              /* Probability */
      1,               /* Count of object */
      0,               /* Flags */
      180,             /* Worth */
      11,              /* Armor class */
      '\0',            /* Launcher */
      "",              /* Damage */
      "",              /* Hurl damage */
      0,               /* Hit bonus */
      0                /* Damage bonus */
    },
    { "haste self", /* Name */
      "",           /* Read text */
      POTION,       /* Type */
      P_HASTE,      /* Which of type */
      -1,           /* Group */
      { 0, 0 },     /* Position */
      81,           /* Probability */
      1,            /* Count of object */
      0,            /* Flags */
      200,          /* Worth */
      11,           /* Armor class */
      '\0',         /* Launcher */
      "",           /* Damage */
      "",           /* Hurl damage */
      0,            /* Hit bonus */
      0             /* Damage bonus */
    },
    { "restore strength", /* Name */
      "",                 /* Read text */
      POTION,             /* Type */
      P_RESTORE,          /* Which of type */
      -1,                 /* Group */
      { 0, 0 },           /* Position */
      95,                 /* Probability */
      1,                  /* Count of object */
      0,                  /* Flags */
      120,                /* Worth */
      11,                 /* Armor class */
      '\0',               /* Launcher */
      "",                 /* Damage */
      "",                 /* Hurl damage */
      0,                  /* Hit bonus */
      0                   /* Damage bonus */
    },
    { "blindness", /* Name */
      "",          /* Read text */
      POTION,      /* Type */
      P_BLIND,     /* Which of type */
      -1,          /* Group */
      { 0, 0 },    /* Position */
      99,          /* Probability */
      1,           /* Count of object */
      0,           /* Flags */
      50,          /* Worth */
      11,          /* Armor class */
      '\0',        /* Launcher */
      "",          /* Damage */
      "",          /* Hurl damage */
      0,           /* Hit bonus */
      0            /* Damage bonus */
    },
    { "thirst quenching", /* Name */
      "",                 /* Read text */
      POTION,             /* Type */
      P_NOP,              /* Which of type */
      -1,                 /* Group */
      { 0, 0 },           /* Position */
      100,                /* Probability */
      1,                  /* Count of object */
      0,                  /* Flags */
      50,                 /* Worth */
      11,                 /* Armor class */
      '\0',               /* Launcher */
      "",                 /* Damage */
      "",                 /* Hurl damage */
      0,                  /* Hit bonus */
      0                   /* Damage bonus */
    }
};

struct object r_magic[MAXRINGS] = {
    { "protection", /* Name */
      "",           /* Read text */
      RING,         /* Type */
      R_PROTECT,    /* Which of type */
      -1,           /* Group */
      { 0, 0 },     /* Position */
      9,            /* Probability */
      1,            /* Count of object */
      0,            /* Flags */
      200,          /* Worth */
      11,           /* Armor class */
      '\0',         /* Launcher */
      "",           /* Damage */
      "",           /* Hurl damage */
      0,            /* Hit bonus */
      0             /* Damage bonus */
    },
    { "add strength", /* Name */
      "",             /* Read text */
      RING,           /* Type */
      R_ADDSTR,       /* Which of type */
      -1,             /* Group */
      { 0, 0 },       /* Position */
      18,             /* Probability */
      1,              /* Count of object */
      0,              /* Flags */
      200,            /* Worth */
      11,             /* Armor class */
      '\0',           /* Launcher */
      "",             /* Damage */
      "",             /* Hurl damage */
      0,              /* Hit bonus */
      0               /* Damage bonus */
    },
    { "sustain strength", /* Name */
      "",                 /* Read text */
      RING,               /* Type */
      R_SUSTSTR,          /* Which of type */
      -1,                 /* Group */
      { 0, 0 },           /* Position */
      23,                 /* Probability */
      1,                  /* Count of object */
      0,                  /* Flags */
      180,                /* Worth */
      11,                 /* Armor class */
      '\0',               /* Launcher */
      "",                 /* Damage */
      "",                 /* Hurl damage */
      0,                  /* Hit bonus */
      0                   /* Damage bonus */
    },
    { "searching", /* Name */
      "",          /* Read text */
      RING,        /* Type */
      R_SEARCH,    /* Which of type */
      -1,          /* Group */
      { 0, 0 },    /* Position */
      33,          /* Probability */
      1,           /* Count of object */
      0,           /* Flags */
      200,         /* Worth */
      11,          /* Armor class */
      '\0',        /* Launcher */
      "",          /* Damage */
      "",          /* Hurl damage */
      0,           /* Hit bonus */
      0            /* Damage bonus */
    },
    { "see invisible", /* Name */
      "",              /* Read text */
      RING,            /* Type */
      R_SEEINVIS,      /* Which of type */
      -1,              /* Group */
      { 0, 0 },        /* Position */
      43,              /* Probability */
      1,               /* Count of object */
      0,               /* Flags */
      175,             /* Worth */
      11,              /* Armor class */
      '\0',            /* Launcher */
      "",              /* Damage */
      "",              /* Hurl damage */
      0,               /* Hit bonus */
      0                /* Damage bonus */
    },
    { "adornment", /* Name */
      "",          /* Read text */
      RING,        /* Type */
      R_NOP,       /* Which of type */
      -1,          /* Group */
      { 0, 0 },    /* Position */
      44,          /* Probability */
      1,           /* Count of object */
      0,           /* Flags */
      100,         /* Worth */
      11,          /* Armor class */
      '\0',        /* Launcher */
      "",          /* Damage */
      "",          /* Hurl damage */
      0,           /* Hit bonus */
      0            /* Damage bonus */
    },
    { "aggravate monster", /* Name */
      "",                  /* Read text */
      RING,                /* Type */
      R_AGGR,              /* Which of type */
      -1,                  /* Group */
      { 0, 0 },            /* Position */
      55,                  /* Probability */
      1,                   /* Count of object */
      0,                   /* Flags */
      100,                 /* Worth */
      11,                  /* Armor class */
      '\0',                /* Launcher */
      "",                  /* Damage */
      "",                  /* Hurl damage */
      0,                   /* Hit bonus */
      0                    /* Damage bonus */
    },
    { "dexterity", /* Name */
      "",          /* Read text */
      RING,        /* Type */
      R_ADDHIT,    /* Which of type */
      -1,          /* Group */
      { 0, 0 },    /* Position */
      63,          /* Probability */
      1,           /* Count of object */
      0,           /* Flags */
      220,         /* Worth */
      11,          /* Armor class */
      '\0',        /* Launcher */
      "",          /* Damage */
      "",          /* Hurl damage */
      0,           /* Hit bonus */
      0            /* Damage bonus */
    },
    { "increase damage", /* Name */
      "",                /* Read text */
      RING,              /* Type */
      R_ADDDAM,          /* Which of type */
      -1,                /* Group */
      { 0, 0 },          /* Position */
      71,                /* Probability */
      1,                 /* Count of object */
      0,                 /* Flags */
      220,               /* Worth */
      11,                /* Armor class */
      '\0',              /* Launcher */
      "",                /* Damage */
      "",                /* Hurl damage */
      0,                 /* Hit bonus */
      0                  /* Damage bonus */
    },
    { "regeneration", /* Name */
      "",             /* Read text */
      RING,           /* Type */
      R_REGEN,        /* Which of type */
      -1,             /* Group */
      { 0, 0 },       /* Position */
      75,             /* Probability */
      1,              /* Count of object */
      0,              /* Flags */
      260,            /* Worth */
      11,             /* Armor class */
      '\0',           /* Launcher */
      "",             /* Damage */
      "",             /* Hurl damage */
      0,              /* Hit bonus */
      0               /* Damage bonus */
    },
    { "slow digestion", /* Name */
      "",               /* Read text */
      RING,             /* Type */
      R_DIGEST,         /* Which of type */
      -1,               /* Group */
      { 0, 0 },         /* Position */
      84,               /* Probability */
      1,                /* Count of object */
      0,                /* Flags */
      240,              /* Worth */
      11,               /* Armor class */
      '\0',             /* Launcher */
      "",               /* Damage */
      "",               /* Hurl damage */
      0,                /* Hit bonus */
      0                 /* Damage bonus */
    },
    { "telportation", /* Name */
      "",             /* Read text */
      RING,           /* Type */
      R_TELEPORT,     /* Which of type */
      -1,             /* Group */
      { 0, 0 },       /* Position */
      93,             /* Probability */
      1,              /* Count of object */
      0,              /* Flags */
      100,            /* Worth */
      11,             /* Armor class */
      '\0',           /* Launcher */
      "",             /* Damage */
      "",             /* Hurl damage */
      0,              /* Hit bonus */
      0               /* Damage bonus */
    },
    { "stealth", /* Name */
      "",        /* Read text */
      RING,      /* Type */
      R_STEALTH, /* Which of type */
      -1,        /* Group */
      { 0, 0 },  /* Position */
      100,       /* Probability */
      1,         /* Count of object */
      0,         /* Flags */
      100,       /* Worth */
      11,        /* Armor class */
      '\0',      /* Launcher */
      "",        /* Damage */
      "",        /* Hurl damage */
      0,         /* Hit bonus */
      0          /* Damage bonus */
    }
};

struct object ws_magic[MAXSTICKS] = {
    { "light",  /* Name */
      "",       /* Read text */
      STICK,    /* Type */
      WS_LIGHT, /* Which of type */
      -1,       /* Group */
      { 0, 0 }, /* Position */
      12,       /* Probability */
      1,        /* Count of object */
      0,        /* Flags */
      120,      /* Worth */
      11,       /* Armor class */
      '\0',     /* Launcher */
      "",       /* Damage */
      "",       /* Hurl damage */
      0,        /* Hit bonus */
      0         /* Damage bonus */
    },
    { "striking", /* Name */
      "",         /* Read text */
      STICK,      /* Type */
      WS_HIT,     /* Which of type */
      -1,         /* Group */
      { 0, 0 },   /* Position */
      21,         /* Probability */
      1,          /* Count of object */
      0,          /* Flags */
      115,        /* Worth */
      11,         /* Armor class */
      '\0',       /* Launcher */
      "",         /* Damage */
      "",         /* Hurl damage */
      0,          /* Hit bonus */
      0           /* Damage bonus */
    },
    { "lightning", /* Name */
      "",          /* Read text */
      STICK,       /* Type */
      WS_ELECT,    /* Which of type */
      -1,          /* Group */
      { 0, 0 },    /* Position */
      24,          /* Probability */
      1,           /* Count of object */
      0,           /* Flags */
      200,         /* Worth */
      11,          /* Armor class */
      '\0',        /* Launcher */
      "",          /* Damage */
      "",          /* Hurl damage */
      0,           /* Hit bonus */
      0            /* Damage bonus */
    },
    { "fire",   /* Name */
      "",       /* Read text */
      STICK,    /* Type */
      WS_FIRE,  /* Which of type */
      -1,       /* Group */
      { 0, 0 }, /* Position */
      27,       /* Probability */
      1,        /* Count of object */
      0,        /* Flags */
      200,      /* Worth */
      11,       /* Armor class */
      '\0',     /* Launcher */
      "",       /* Damage */
      "",       /* Hurl damage */
      0,        /* Hit bonus */
      0         /* Damage bonus */
    },
    { "cold",   /* Name */ 
      "",       /* Read text */
      STICK,    /* Type */
      WS_COLD,  /* Which of type */
      -1,       /* Group */
      { 0, 0 }, /* Position */
      30,       /* Probability */
      1,        /* Count of object */
      0,        /* Flags */
      200,      /* Worth */
      11,       /* Armor class */
      '\0',     /* Launcher */
      "",       /* Damage */
      "",       /* Hurl damage */
      0,        /* Hit bonus */
      0         /* Damage bonus */
    },
    { "polymorph",  /* Name */
      "",           /* Read text */
      STICK,        /* Type */
      WS_POLYMORPH, /* Which of type */
      -1,           /* Group */
      { 0, 0 },     /* Position */
      45,           /* Probability */
      1,            /* Count of object */
      0,            /* Flags */
      210,          /* Worth */
      11,           /* Armor class */
      '\0',         /* Launcher */
      "",           /* Damage */
      "",           /* Hurl damage */
      0,            /* Hit bonus */
      0             /* Damage bonus */
    },
    { "magic missile", /* Name */
      "",              /* Read text */
      STICK,           /* Type */
      WS_MISSILE,      /* Which of type */
      -1,              /* Group */
      { 0, 0 },        /* Position */
      55,              /* Probability */
      1,               /* Count of object */
      0,               /* Flags */
      170,             /* Worth */
      11,              /* Armor class */
      '\0',            /* Launcher */
      "",              /* Damage */
      "",              /* Hurl damage */
      0,               /* Hit bonus */
      0                /* Damage bonus */
    },
    { "haste monster", /* Name */
      "",              /* Read text */
      STICK,           /* Type */
      WS_HASTE_M,      /* Which of type */
      -1,              /* Group */
      { 0, 0 },        /* Position */
      64,              /* Probability */
      1,               /* Count of object */
      0,               /* Flags */
      50,              /* Worth */
      11,              /* Armor class */
      '\0',            /* Launcher */
      "",              /* Damage */
      "",              /* Hurl damage */
      0,               /* Hit bonus */
      0                /* Damage bonus */
    },
    { "slow monster", /* Name */
      "",             /* Read text */
      STICK,          /* Type */
      WS_SLOW_M,      /* Which of type */
      -1,             /* Group */
      { 0, 0 },       /* Position */
      75,             /* Probability */
      1,              /* Count of object */
      0,              /* Flags */
      220,            /* Worth */
      11,             /* Armor class */
      '\0',           /* Launcher */
      "",             /* Damage */
      "",             /* Hurl damage */
      0,              /* Hit bonus */
      0               /* Damage bonus */
    },
    { "drain life", /* Name */
      "",           /* Read text */
      STICK,        /* Type */
      WS_DRAIN,     /* Which of type */
      -1,           /* Group */
      { 0 , 0 },    /* Position */
      84,           /* Probability */
      1,            /* Count of object */
      0,            /* Flags */
      210,          /* Worth */
      11,           /* Armor class */
      '\0',         /* Launcher */
      "",           /* Damage */
      "",           /* Hurl damage */
      0,            /* Hit bonus */
      0             /* Damage bonus */
    },
    { "nothing", /* Name */
      "",        /* Read text */
      STICK,     /* Type */
      WS_NOP,    /* Which of type */
      -1,        /* Group */
      { 0, 0 },  /* Position */
      85,        /* Probability */
      1,         /* Count of object */
      0,         /* Flags */
      70,        /* Worth */
      11,        /* Armor class */
      '\0',      /* Launcher */
      "",        /* Damage */
      "",        /* Hurl damage */
      0,         /* Hit bonus */
      0          /* Damage bonus */
    },
    { "teleport away", /* Name */
      "",              /* Read text */
      STICK,           /* Type */
      WS_TELAWAY,      /* Which of type */
      -1,              /* Group */
      { 0, 0 },        /* Position */
      90,              /* Probability */
      1,               /* Count of object */
      0,               /* Flags */
      140,             /* Worth */
      11,              /* Armor class */
      '\0',            /* Launcher */
      "",              /* Damage */
      "",              /* Hurl daamage */
      0,               /* Hit bonus */
      0                /* Damage bonus */
    },
    { "teleport to", /* Name */
      "",            /* Read text */
      STICK,         /* Type */
      WS_TELTO,      /* Which of type */
      -1,            /* Group */
      { 0, 0 },      /* Position */
      95,            /* Probability */
      1,             /* Count of object */
      0,             /* Flags */
      60,            /* Worth */
      11,            /* Armor class */
      '\0',          /* Launcher */
      "",            /* Damage */
      "",            /* Hurl damage */
      0,             /* Hit bonus */
      0              /* Damage bonus */
    },
    { "cancellation", /* Name */
      "",             /* Read text */
      STICK,          /* Type */
      WS_CANCEL,      /* Which of type */
      -1,             /* Group */
      { 0, 0 },       /* Position */
      100,            /* Probability */
      1,              /* Count of object */
      0,              /* Flags */
      130,            /* Worth */
      11,             /* Armor class */
      '\0',           /* Launcher */
      "",             /* Damage */
      "",             /* Hurl damage */
      0,              /* Hit bonus */
      0               /* Damage bonus */
    }
};

struct object armors[MAXARMORS] = {
    { "leather armor", /* Name */
      "",              /* Read text */
      ARMOR,           /* Type */
      LEATHER,         /* Which of type */
      -1,              /* Group */
      { 0, 0 },        /* Position */
      20,              /* Probability */
      1,               /* Count of object */
      0,               /* Flags */
      0,               /* Worth */
      8,               /* Armor class */
      '\0',            /* Lancher */
      "",              /* Damage */
      "",              /* Hurl damage */
      0,               /* Hit bonus */
      0                /* Damage bonus */
    },
    { "ring mail", /* Name */
      "",          /* Read text */
      ARMOR,       /* Type */
      RING_MAIL,   /* Which of type */
      -1,          /* Group */
      { 0, 0 },    /* Position */
      35,          /* Probability */
      1,           /* Count of object */
      0,           /* Flags */
      0,           /* Worth */
      7,           /* Armor class */
      '\0',        /* Launcher */
      "",          /* Damage */
      "",          /* Hurl damage */
      0,           /* Hit bonus */
      0            /* Damage bonus */
    },
    { "studded leather armor", /* Name */
      "",                      /* Read text */
      ARMOR,                   /* Type */
      STUDDED_LEATHER,         /* Which of type */
      -1,                      /* Group */
      { 0 , 0 },               /* Position */
      50,                      /* Probability */
      1,                       /* Count of object */
      0,                       /* Flags */
      0,                       /* Worth */
      7,                       /* Armor class */
      '\0',                    /* Launcher */
      "",                      /* Damage */
      "",                      /* Hurl Damage */
      0,                       /* Hit bonus */
      0                        /* Damage bonus */
    },
    { "scale mail", /* Name */
      "",           /* Read text */
      ARMOR,        /* Type */
      SCALE_MAIL,   /* Which of type */
      -1,           /* Group */
      { 0, 0 },     /* Position */
      63,           /* Probability */
      1,            /* Count of object */
      0,            /* Flags */
      0,            /* Worth */
      6,            /* Armor class */
      '\0',         /* Launcher */
      "",           /* Damage */
      "",           /* Hurl damage */
      0,            /* Hit bonus */
      0             /* Damage bonus */
    },
    { "chain mail", /* Name */
      "",           /* Read text */
      ARMOR,        /* Type */
      CHAIN_MAIL,   /* Which of type */
      -1,           /* Group */
      { 0, 0 },     /* Position */
      75,           /* Probability */
      1,            /* Count of object */
      0,            /* Flags */
      0,            /* Worth */
      5,            /* Armor class */
      '\0',         /* Launcher */
      "",           /* Damage */
      "",           /* Hurl damage */
      0,            /* Hit bonus */
      0             /* Damage bonus */
    },
    { "splint mail", /* Name */
      "",            /* Read text */
      ARMOR,         /* Type */
      SPLINT_MAIL,   /* Which of type */
      -1,            /* Group */
      { 0, 0 },      /* Position */
      85,            /* Probability */
      1,             /* Count of object */
      0,             /* Flags */
      0,             /* Worth */
      4,             /* Armor class */
      '\0',          /* Launcher */
      "",            /* Damage */
      "",            /* Hurl damage */
      0,             /* Hit bonus */
      0              /* Damage bonus */
    },
    { "banded mail", /* Name */
      "",            /* Read text */
      ARMOR,         /* Type */
      BANDED_MAIL,   /* Which of type */
      -1,            /* Group */
      { 0, 0 },      /* Position */
      95,            /* Probability */
      1,             /* Count of object */
      0,             /* Flags */
      0,             /* Worth */
      4,             /* Armor class */
      '\0',          /* Launcher */
      "",            /* Damage */
      "",            /* Hurl damage */
      0,             /* Hit bonus */
      0              /* Damage bonus */
    },
    { "plate mail", /* Name */
      "",           /* Read text */
      ARMOR,        /* Type */
      PLATE_MAIL,   /* Which of type */
      -1,           /* Group */
      { 0, 0 },     /* Position */
      100,          /* Probability */
      1,            /* Count of object */
      0,            /* Flags */
      0,            /* Worth */
      3,            /* Armor class */
      '\0',         /* Launcher */
      "",           /* Damage */
      "",           /* Hurl Damage */
      0,            /* Hit bonus */
      0             /* Damage bonus */
    }
};

// Contains definitions and functions for dealing with things like
// potions and scrolls
static char *rainbow[] = {
    "red",
    "blue",
    "green",
    "yellow",
    "black",
    "brown",
    "orange",
    "pink",
    "purple",
    "grey",
    "white",
    "silver",
    "gold",
    "violet",
    "clear",
    "vermilion",
    "ecru",
    "turquoise",
    "magenta",
    "amber",
    "topaz",
    "plaid",
    "tan",
    "tangerine"
};

static char *sylls[] = {
    "a",
    "ab",
    "ag",
    "aks",
    "ala",
    "an",
    "ankh",
    "app",
    "arg",
    "arze",
    "ash",
    "ban",
    "bar",
    "bat",
    "bek",
    "bie",
    "bin",
    "bit",
    "bjor",
    "blu",
    "bot",
    "bu",
    "byt",
    "comp",
    "con",
    "cos",
    "cre",
    "dalf",
    "dan",
    "den",
    "do",
    "e",
    "eep",
    "el",
    "eng",
    "er",
    "ere",
    "erk",
    "esh",
    "evs",
    "fa",
    "fid",
    "for",
    "fri",
    "fu",
    "gan",
    "gar",
    "glen",
    "gop",
    "gre",
    "ha",
    "he",
    "hyd",
    "i",
    "ing",
    "ion",
    "ip",
    "ish",
    "it",
    "ite",
    "iv",
    "jo",
    "kho",
    "kli",
    "klis",
    "la",
    "lech",
    "man",
    "mar",
    "me",
    "mi",
    "mic",
    "mik",
    "mon",
    "mung",
    "mur",
    "nej",
    "nelg",
    "nep",
    "ner",
    "nes",
    "nes",
    "nih",
    "nin",
    "o",
    "od",
    "ood",
    "org",
    "orn",
    "ox",
    "oxy",
    "pay",
    "pet",
    "ple",
    "plu",
    "po",
    "pot",
    "prok",
    "re",
    "rea",
    "rhov",
    "ri",
    "ro",
    "rog",
    "rok",
    "rol",
    "sa",
    "san",
    "sat",
    "see",
    "sef",
    "seh",
    "shu",
    "ski",
    "sna",
    "sne",
    "snik",
    "sno",
    "so",
    "sol",
    "sri",
    "sta",
    "sun",
    "ta",
    "tab",
    "tem",
    "ther",
    "ti",
    "tox",
    "trol",
    "tue",
    "turs",
    "u",
    "ulk",
    "um",
    "un",
    "uni",
    "ur",
    "val",
    "viv",
    "vly",
    "vom",
    "wah",
    "wed",
    "werg",
    "wex",
    "whon",
    "wun",
    "xo",
    "y",
    "yot",
    "yu",
    "zant",
    "zap",
    "zeb",
    "zim",
    "zok",
    "zon",
    "zum"
};

static char *stones[] = {
    "Agate",
    "Alexandrite",
    "Amethyst",
    "Carnelian",
    "Diamond",
    "Emerald",
    "Granite",
    "Jade",
    "Kryptonite",
    "Lapus lazuli",
    "Moonstone",
    "Obsidian",
    "Onyx",
    "Opal",
    "Pearl",
    "Ruby",
    "Saphire",
    "Tiger eye",
    "Topaz",
    "Turquoise"
};

static char *wood[] = {
    "Avocado wood",
    "Balsa",
    "Banyan",
    "Birch",
    "Cedar",
    "Cherry",
    "Cinnibar",
    "Driftwood",
    "Ebony",
    "Eucalyptus",
    "Hemlock",
    "Ironwood",
    "Mahogany",
    "Manzanita",
    "Maple",
    "Oak",
    "Persimmon wood",
    "Redwood",
    "Rosewood",
    "Teak",
    "Walnut",
    "Zebra wood"
};

static char *metal[] = {
    "Aluminium",
    "Bone",
    "Brass",
    "Bronze",
    "Copper",
    "Iron",
    "Lead",
    "Pewter",
    "Steel",
    "Tin",
    "Zinc"
};

// init_player:
//     Roll up the rogue
int init_player()
{    
    player.t_stats.s_lvl = 1;
    player.t_stats.s_exp = 0L;
    player.t_stats.s_hpt = 12;
    max_hp = player.t_stats.s_hpt;

    if(rnd(100) == 7) {
        player.t_stats.st_str = 18;
        player.t_stats.st_add = rnd(100) + 1;
    }
    else {
        player.t_stats.st_str = 16;
        player.t_stats.st_add = 0;
    }
    
    player.t_stats.s_dmg = "1d4";
    player.t_stats.s_arm = 10;
    max_stats = player.t_stats;
    player.t_pack = NULL;

    /* TC_DEBUG: Start */
    FILE *output;
    output = fopen("debug.txt", "a+");
    print_thing(&player, output);
    fclose(output);
    /* TC_DEBUG: Finish */
    
    return 0;
}

// init_things:
//     Initialize the probabilities for types of things
int init_things()
{
    badcheck("things", things, NUMTHINGS);

    return 0;
}

// init_colors:
//     Initialize the potion color scheme for this time
int init_colors()
{
    int i;
    int j;
    char *str;

    int used_colors = 0;
    int been_used = 0;

    for(i = 0; i < MAXPOTIONS; ++i) {
        been_used = 0;
        str = rainbow[rnd(sizeof(rainbow) / sizeof(char *))];

        for(j = 0; j < used_colors; ++j) {
            if(strncmp(str, p_colors[j], strlen(p_colors[j])) == 0) {
                been_used = 1;
                break;
            }
        }

        while(been_used) {
            been_used = 0;
            str = rainbow[rnd(sizeof(rainbow) / sizeof(char *))];

            for(j = 0; j < used_colors; ++j) {
                if(strncmp(str, p_colors[j], strlen(p_colors[j])) == 0) {
                    been_used = 1;
                    break;
                }
            }
        }
        
        p_colors[i] = str;
        p_know[i] = FALSE;
        p_guess[i] = NULL;
    }

    badcheck("potions", p_magic, MAXPOTIONS);

    return 0;
}

// init_names:
//     Generate the names of the various scrolls
int init_names()
{
    int nsyl;
    char *cp;
    char *sp;
    int i;
    int nwords;

    for(i = 0; i < MAXSCROLLS; ++i) {
        cp = prbuf;
        nwords = rnd(4) + 2;
        
        while(nwords) {
            nsyl = rnd(3) + 1;
            
            while(nsyl) {
                sp = sylls[rnd((sizeof sylls) / (sizeof (char *)))];
                
                while(*sp) {
                    *cp = *sp;
                    ++cp;
                    ++sp;
                }

                --nsyl;
            }
            
            *cp = ' ';
            ++cp;
            --nwords;
        }

        --cp;
        *cp = '\0';
        
        s_names[i] = (char *)new(strlen(prbuf) + 1);
        s_know[i] = FALSE;
        s_guess[i] = NULL;
        strcpy(s_names[i], prbuf);
    }
    
    badcheck("scrolls", s_magic, MAXSCROLLS);

    return 0;
}

// init_stones:
//     Initialize the ring stone setting scheme for this time
int init_stones()
{
    int i;
    int j;
    char *str;

    int used_stones = 0;
    int been_used = 0;
    
    for(i = 0; i < MAXRINGS; ++i) {
        been_used = 0;
        str = stones[rnd(sizeof(stones) / sizeof(char *))];

        for(j = 0; j < used_stones; ++j) {
            if(strncmp(str, r_stones[j], strlen(r_stones[j])) == 0) {
                been_used = 1;
                break;
            }
        }

        while(been_used) {
            been_used = 0;
            str = stones[rnd(sizeof(rainbow) / sizeof(char *))];

            for(j = 0; j < used_stones; ++j) {
                if(strncmp(str, r_stones[j], strlen(r_stones[j])) == 0) {
                    been_used = 1;
                    break;
                }
            }
        }
        r_stones[i] = str;
        r_know[i] = FALSE;
        r_guess[i] = NULL;
    }
    
    badcheck("rings", r_magic, MAXRINGS);

    return 0;
}

// init_materials:
//     Initialize the construction materials for wands and staffs
int init_materials()
{
    int i;
    int j;
    char *str;

    int used_materials = 0;
    int been_used = 0;
    
    for(i = 0; i < MAXSTICKS; ++i) {
        been_used = 0;
        
        if(rnd(100) > 50) {
            str = metal[rnd(sizeof(metal) / sizeof(char *))];
            ws_type[i] = "wand";
        }
        else {
            str = wood[rnd(sizeof(wood) / sizeof(char *))];
            ws_type[i] = "staff";
        }
        
        for(j = 0; j < used_materials; ++j) {
            if(strncmp(str, ws_made[j], strlen(ws_made[j])) == 0) {
                been_used = 1;
                break;
            }
        }
        
        while(been_used) {
            been_used = 0;

            if(strncmp("wand", ws_type[i], strlen(ws_type[i])) == 0) {               
                str = metal[rnd(sizeof(metal) / sizeof(char *))];
            }
            else {
                str = wood[rnd(sizeof(wood) / sizeof(char *))];
            }
            
            for(j = 0; j < used_materials; ++j) {
                if(strncmp(str, ws_made[j], strlen(ws_made[j])) == 0) {
                    been_used = 1;
                    break;
                }
            }
        }
        
        ws_made[i] = str;
        ws_know[i] = FALSE;
        ws_guess[i] = NULL;
    }
    
    badcheck("sticks", ws_magic, MAXSTICKS);

    return 0;
}

// badcheck:
//     Something...
int badcheck(char *name, struct object *magic, int bound)
{
    struct object *end;
    
    if(magic[bound - 1].o_prob == 100) {
        return 0;
    }
    
    printf("\nBad percentages for %s:\n", name);
    
    for(end = &magic[bound]; magic < end; ++magic) {
        printf("%3d%% %s\n", magic->o_prob, magic->o_name);
    }
    
    printf("[hit RETURN to continue]");
    fflush(stdout);
    
    while(getchar() != '\n') {
        continue;
    }

    return 0;
}
