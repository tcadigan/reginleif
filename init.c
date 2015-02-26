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


struct monster monsters[26] = {
    // Name,             CARRY, FLAG,             str,    exp, lvl, arm, hpt, dmg
    { "giant ant"        ,   0,	ISMEAN        ,	{ 1, 1,   10,  2,  3, 1, "1d6"             } },
    { "bat"              ,   0,	0             ,	{ 1, 1,    1,  1,  3, 1, "1d2"             } },
    { "centaur"          ,  15,	0             ,	{ 1, 1,   15,  4,  4, 1, "1d6/1d6"         } },
    { "dragon"           , 100,	ISGREED       , { 1, 1, 9000, 10, -1, 1, "1d8/1d8/3d10"    } },
    { "floating eye"     ,   0,	0             ,	{ 1, 1,    5,  1,  9, 1, "0d0"             } },
    { "violet fungi"     ,   0,	ISMEAN        ,	{ 1, 1,   85,  8,  3, 1, "000d0"           } },
    { "gnome"            ,  10,	0             ,	{ 1, 1,    8,  1,  5, 1, "1d6"             } },
    { "hobgoblin"        ,   0,	ISMEAN        ,	{ 1, 1,    3,  1,  5, 1, "1d8"             } },
    { "invisible stalker",   0, ISINVIS       , { 1, 1,  120,  8,  3, 1, "4d4"             } },
    { "jackal"           ,   0,	ISMEAN        ,	{ 1, 1,    2,  1,  7, 1, "1d2"             } },
    { "kobold"           ,   0,	ISMEAN        ,	{ 1, 1,    1,  1,  7, 1, "1d4"             } },
    { "leprechaun"       ,   0,	0             ,	{ 1, 1,   10,  3,  8, 1, "1d1"             } },
    { "mimic"            ,  30,	0             ,	{ 1, 1,  140,  7,  7, 1, "3d4"             } },
    { "nymph"            , 100,	0             ,	{ 1, 1,   40,  3,  9, 1, "0d0"             } },
    { "orc"              ,  15,	ISBLOCK       , { 1, 1,    5,  1,  6, 1, "1d8"             } },
    { "purple worm"      ,  70,	0             ,	{ 1, 1, 7000, 15,  6, 1, "2d12/2d4"        } },
    { "quasit"           ,  30,	ISMEAN        ,	{ 1, 1,   35,  3,  2, 1, "1d2/1d2/1d4"     } },
    { "rust monster"     ,   0,	ISMEAN        ,	{ 1, 1,   25,  5,  2, 1, "0d0/0d0"         } },
    { "snake"            ,   0,	ISMEAN        ,	{ 1, 1,    3,  1,  5, 1, "1d3"             } },
    { "troll"            ,  50,	ISREGEN|ISMEAN, { 1, 1,   55,  6,  4, 1, "1d8/1d8/2d6"     } },
    { "umber hulk"       ,  40,	ISMEAN        ,	{ 1, 1,  130,  8,  2, 1, "3d4/3d4/2d5"     } },
    { "vampire"          ,  20,	ISREGEN|ISMEAN, { 1, 1,  380,  8,  1, 1, "1d10"            } },
    { "wraith"           ,   0,	0             ,	{ 1, 1,   55,  5,  4, 1, "1d6"             } },
    { "xorn"             ,   0,	ISMEAN        ,	{ 1, 1,  120,  7, -2, 1, "1d3/1d3/1d3/4d6" } },
    { "yeti"             ,  30,	0             ,	{ 1, 1,   50,  4,  6, 1, "1d6/1d6"         } },
    { "zombie"           ,   0,	ISMEAN        ,	{ 1, 1,    7,  2,  8, 1, "1d8"             } }
};

struct magic_item things[NUMTHINGS] = {
    { "Potion",  27, 0 },
    { "Scroll",  54, 0 },
    { "Food"  ,  72, 0 },
    { "Weapon",  81, 0 },
    { "Armor" ,  90, 0 },
    { "Ring"  ,  95, 0 },
    { "Stick" , 100, 0 }
};

struct magic_item s_magic[MAXSCROLLS] = {
    { "monster confusion" ,   8, 170 },
    { "magic mapping"     ,  13, 180 },
    { "light"             ,  23, 100 },
    { "hold monster"      ,  25, 200 },
    { "sleep"             ,  30,  50 },
    { "enchant armor"     ,  38, 130 },
    { "identify"          ,  59, 100 },
    { "scare monster"     ,  63, 180 },
    { "gold detection"    ,  67, 110 },
    { "teleportation"     ,  74, 175 },
    { "enchant weapon"    ,  84, 150 },
    { "create monster"    ,  89,  75 },
    { "remove curse"      ,  97, 105 },
    { "aggravate monsters",  98,  60 },
    { "blank paper"       ,  99,  50 },
    { "genocide"          , 100, 200 }
};

struct magic_item p_magic[MAXPOTIONS] = {
    { "confusion"        ,   8,  50 },
    { "paralysis"        ,  18,  50 },
    { "poison"           ,  26,  50 },
    { "gain strength"    ,  41, 150 },
    { "see invisible"    ,  43, 170 },
    { "healing"          ,  58, 130 },
    { "monster detection",  64, 120 },
    { "magic detection"  ,  70, 105 },
    { "raise level"      ,  72, 220 },
    { "extra healing"    ,  77, 180 },
    { "haste self"       ,  81, 200 },
    { "restore strength" ,  95, 120 },
    { "blindness"        ,  99,  50 },
    { "thirst quenching" , 100,  50 }
};

struct magic_item r_magic[MAXRINGS] = {
    { "protection"       ,   9, 200 },
    { "add strength"     ,  18, 200 },
    { "sustain strength" ,  23, 180 },
    { "searching"        ,  33, 200 },
    { "see invisible"    ,  43, 175 },
    { "adornment"        ,  44, 100 },
    { "aggravate monster",  55, 100 },
    { "dexterity"        ,  63, 220 },
    { "increase damage"  ,  71, 220 },
    { "regeneration"     ,  75, 260 },
    { "slow digestion"   ,  84, 240 },
    { "telportation"     ,  93, 100 },
    { "stealth"          , 100, 100 }
};

struct magic_item ws_magic[MAXSTICKS] = {
    { "light"        ,  12, 120 },
    { "striking"     ,  21, 115 },
    { "lightning"    ,  24, 200 },
    { "fire"         ,  27, 200 },
    { "cold"         ,  30, 200 },
    { "polymorph"    ,  45, 210 },
    { "magic missile",  55, 170 },
    { "haste monster",  64,  50 },
    { "slow monster" ,  75, 220 },
    { "drain life"   ,  84, 210 },
    { "nothing"      ,  85,  70 },
    { "teleport away",  90, 140 },
    { "teleport to"  ,  95,  60 },
    { "cancellation" , 100, 130 }
};

int a_class[MAXARMORS] = {
    8,
    7,
    7,
    6,
    5,
    4,
    4,
    3
};

char *a_names[MAXARMORS] = {
    "leather armor",
    "ring mail",
    "studded leather armor",
    "scale mail",
    "chain mail",
    "splint mail",
    "banded mail",
    "plate mail"
};

int a_chances[MAXARMORS] = {
    20,
    35,
    50,
    63,
    75,
    85,
    95,
    100
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
int badcheck(char *name, struct magic_item *magic, int bound)
{
    struct magic_item *end;

    if(magic[bound - 1].mi_prob == 100) {
	return 0;
    }
    
    printf("\nBad percentages for %s:\n", name);
    
    for(end = &magic[bound]; magic < end; ++magic) {
	printf("%3d%% %s\n", magic->mi_prob, magic->mi_name);
    }
    
    printf("[hit RETURN to continue]");
    fflush(stdout);
    
    while(getchar() != '\n') {
	continue;
    }

    return 0;
}
