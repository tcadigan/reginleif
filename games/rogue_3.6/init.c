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
bool after = FALSE;
bool waswizard = FALSE;
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
int max_hp = 0;

struct thing monsters[26] = {
    {"giant ant", {0, 0}, 0, 'A', '\0', NULL, ISMEAN, '\0', 0, {1, 1, 10, 2, 3, 1, "1d6"}, NULL},
    {"bat", {0, 0}, 0, 'B', '\0', NULL, 0, '\0', 0, {1, 1, 1, 4, 4, 1, "1d2"}, NULL},
    {"centaur", {0, 0}, 0, 'C', '\0', NULL, 0, '\0', 15, {1, 1, 15, 4, 4, 1, "1d6/1d6"}, NULL},
    {"dragon", {0, 0}, 0, 'D', '\0', NULL, ISGREED, '\0', 100, {1, 1, 9000, 10, -1, 1, "1d8/1d8/3d10"}, NULL},
    {"floating eye", {0, 0}, 0, 'E', '\0', NULL, 0, '\0', 0, {1, 1, 5, 1, 9, 1, "0d0"}, NULL},
    {"violet fungi", {0, 0}, 0, 'F', '\0', NULL, ISMEAN, '\0', 0, {1, 1, 85, 8, 3, 1, "000d0"}, NULL},
    {"gnome", {0, 0}, 0, 'G', '\0', NULL, 0, '\0', 10, {1, 1, 8, 1, 5, 1, "1d6"}, NULL},
    {"hobgoblin", {0, 0}, 0, 'H', '\0', NULL, ISMEAN, '\0', 0, {1, 1, 3, 1, 5, 1, "1d8"}, NULL},
    {"invisible stalker", {0, 0}, 0, 'I', '\0', NULL, ISINVIS, '\0', 0, {1, 1, 120, 8, 3, 1, "4d4"}, NULL},
    {"jackel", {0, 0}, 0, 'J', '\0', NULL, ISMEAN, '\0', 0, {1, 1, 2, 1, 7, 1, "1d2"}, NULL},
    {"kobold", {0, 0}, 0, 'K', '\0', NULL, ISMEAN, '\0', 0, {1, 1, 1, 1, 7, 1, "1d4"}, NULL},
    {"leprechaun", {0, 0}, 0, 'L', '\0', NULL, 0, '\0', 0, {1, 1, 10, 3, 8, 1, "1d1"}, NULL},
    {"mimic", {0, 0}, 0, 'M', '\0', NULL, 0, '\0', 30, {1, 1, 140, 7, 7, 1, "3d4"}, NULL},
    {"nymph", {0, 0}, 0, 'N', '\0', NULL, 0, '\0', 100, {1, 1, 40, 3, 9, 1, "0d0"}, NULL},
    {"orc", {0, 0}, 0, 'O', '\0', NULL, ISBLOCK, '\0', 15, {1, 1, 5, 1, 6, 1, "1d8"}, NULL},
    {"purple worm", {0, 0}, 0, 'P', '\0', NULL, 0, '\0', 70, {1, 1, 7000, 15, 6, 1, "2d12/2d4"}, NULL},
    {"quasit", {0, 0}, 0, 'Q', '\0', NULL, ISMEAN, '\0', 30, {1, 1, 35, 3, 2, 1, "1d2/1d2/1d4"}, NULL},
    {"rust monster", {0, 0}, 0, 'R', '\0', NULL, ISMEAN, '\0', 0, {1, 1, 25, 5, 2, 1, "0d0/0d0"}, NULL},
    {"snake", {0, 0}, 0, 'S', '\0', NULL, ISMEAN, '\0', 0, {1, 1, 3, 1, 5, 1, "1d3"}, NULL},
    {"troll", {0, 0}, 0, 'T', '\0', NULL, ISREGEN | ISMEAN, '\0', 50, {1, 1, 55, 6, 4, 1, "1d8/1d8/2d6"}, NULL},
    {"umber hulk", {0, 0}, 0, 'U', '\0', NULL, ISMEAN, '\0', 40, {1, 1, 130, 8, 2, 1, "3d4/3d4/2d5"}, NULL},
    {"vampire", {0, 0}, 0, 'V', '\0', NULL, ISREGEN | ISMEAN, '\0', 20, {1, 1, 380, 8, 1, 1, "1d10"}, NULL},
    {"wraith", {0, 0}, 0, 'W', '\0', NULL, 0, '\0', 0, {1, 1, 55, 5, 4, 1, "1d6"}, NULL},
    {"xorn", {0, 0}, 0, 'X', '\0', NULL, ISMEAN, '\0', 0, {1, 1, 120, 7, -2, 1, "1d3/1d3/1d3/4d5"}, NULL},
    {"yeti", {0, 0}, 0, 'Y', '\0', NULL, 0, '\0', 30, {1, 1, 50, 5, 6, 1, "1d6/1d6"}, NULL},
    {"zombie", {0, 0}, 0, 'Z', '\0', NULL, ISMEAN, '\0',  0,  {1, 1, 7, 2, 4, 1, "1d8"},  NULL}
};

struct object things[NUMTHINGS] = {
    {"Potion", "", POTION, -1, -1, {0, 0}, 27, 1, 0, 0, 11, '\0', "", "", 0, 0},
    {"Scroll", "", SCROLL, -1, -1, {0, 0}, 54, 1, 0, 0, 11, '\0', "", "", 0, 0},
    {"Food", "", FOOD, -1, -1, {0, 0}, 72, 1, 0, 0, 11, '\0', "", "", 0, 0},
    {"Weapon", "", WEAPON, -1, -1, {0, 0}, 81, 1, 0, 0, 11, '\0', "", "", 0, 0},
    {"Armor", "", ARMOR, -1, -1, {0, 0}, 90, 1, 0, 0, 11, '\0', "", "", 0, 0},
    {"Ring", "", RING, -1, -1, {0, 0}, 95, 1, 0, 0, 11, '\0', "", "", 0, 0},
    {"Stick", "", STICK, -1, -1, {0, 0}, 100, 1, 0, 0, 11, '\0', "", "", 0, 0}
};

struct object s_magic[MAXSCROLLS] = {
    {"monster confusion", "", SCROLL, S_CONFUSE, -1, {0, 0}, 8, 1, 0, 170, 11, '\0', "", "", 0, 0},
    {"magic mapping", "", SCROLL, S_MAP, -1, {0, 0}, 13, 1, 0, 180, 11, '\0', "", "", 0, 0},
    {"light", "", SCROLL, S_LIGHT, -1, {0, 0}, 23, 1, 0, 100, 11, '\0', "", "", 0, 0},
    {"hold monster", "", SCROLL, S_HOLD, -1, {0, 0}, 25, 1, 0, 200, 11, '\0', "", "", 0, 0},
    {"sleep", "", SCROLL, S_SLEEP, -1, {0, 0}, 30, 1, 0, 50, 11, '\0', "", "", 0, 0},
    {"enchant armor", "", SCROLL, S_ARMOR, -1, {0, 0}, 38, 1, 0, 130, 11, '\0', "", "", 0, 0},
    {"identify", "", SCROLL, S_IDENT, -1, {0, 0}, 59, 1, 0, 100, 11, '\0', "", "", 0, 0},
    {"scare monster", "", SCROLL, S_SCARE, -1, {0, 0}, 63, 1, 0, 180, 11, '\0', "", "", 0, 0},
    {"gold detection", "", SCROLL, S_GFIND, -1, {0, 0}, 67, 1, 0, 110, 11, '\0', "", "", 0, 0},
    {"teleportation", "", SCROLL, S_TELEP, -1, {0, 0}, 74, 1, 0, 175, 11, '\0', "", "", 0, 0},
    {"enchant weapon", "", SCROLL, S_ENCH, -1, {0, 0}, 84, 1, 0, 150, 11, '\0', "", "", 0, 0},
    {"create monster", "", SCROLL, S_CREATE, -1, {0, 0}, 89, 1, 0, 75, 11, '\0', "", "", 0, 0},
    {"remove curse", "", SCROLL, S_REMOVE, -1, {0, 0}, 97, 1, 0, 105, 11, '\0', "", "", 0, 0},
    {"aggravate monsters", "", SCROLL, S_AGGR, -1, {0, 0}, 98, 1, 0, 60, 11, '\0', "", "", 0, 0},
    {"blank paper", "", SCROLL, S_NOP, -1, {0, 0}, 99, 1, 0, 50, 11, '\0', "", "", 0, 0},
    {"genocide", "", SCROLL, S_GENOCIDE, -1, {0, 0}, 100, 1, 0, 200, 11, '\0', "", "", 0, 0}
};

struct object p_magic[MAXPOTIONS] = {
    {"confusion", "", POTION, P_CONFUSE, -1, {0, 0}, 8, 1, 0, 50, 11, '\0', "", "", 0, 0},
    {"paralysis", "", POTION, P_PARALYZE, -1, {0, 0}, 18, 1, 0, 50, 11, '\0', "", "", 0, 0},
    {"poison", "", POTION, P_POISON, -1, {0, 0}, 26, 1, 0, 50, 11, '\0', "", "", 0, 0},
    {"gain strength", "", POTION, P_STRENGTH, -1, {0, 0}, 41, 1, 0, 150, 11, '\0', "", "", 0, 0},
    {"see invisible", "", POTION, P_SEEINVIS, -1, {0, 0}, 43, 1, 0, 170, 11, '\0', "", "", 0, 0},
    {"healing", "", POTION, P_HEALING, -1, {0, 0}, 58, 1, 0, 130, 11, '\0', "", "", 0, 0},
    {"monster detection", "", POTION, P_MFIND, -1, {0, 0}, 64, 1, 0, 120, 11, '\0', "", "", 0, 0},
    {"magic detection", "", POTION, P_TFIND, -1, {0, 0}, 70, 1, 0, 105, 11, '\0', "", "", 0, 0},
    {"raise level", "", POTION, P_RAISE, -1, {0, 0}, 72, 1, 0, 220, 11, '\0', "", "", 0, 0},
    {"extra healing", "", POTION, P_XHEAL, -1, {0, 0}, 77, 1, 0, 180, 11, '\0', "", "", 0, 0},
    {"haste self", "", POTION, P_HASTE, -1, {0, 0}, 81, 1, 0, 200, 11, '\0', "", "", 0, 0},
    {"restore strength", "", POTION, P_RESTORE, -1, {0, 0}, 95, 1, 0, 120, 11, '\0', "", "", 0, 0},
    {"blindness", "", POTION, P_BLIND, -1, {0, 0}, 99, 1, 0, 50, 11, '\0', "", "", 0, 0},
    {"thirst quenching", "", POTION, P_NOP, -1, {0, 0}, 100, 1, 0, 50, 11, '\0', "", "", 0, 0}
};

struct object r_magic[MAXRINGS] = {
    {"protection", "", RING, R_PROTECT, -1, {0, 0}, 9, 1, 0, 200, 11, '\0', "", "", 0, 0},
    {"add strength", "", RING, R_ADDSTR, -1, {0, 0}, 18, 1, 0, 200, 11, '\0', "", "", 0, 0},
    {"sustain strength", "", RING, R_SUSTSTR, -1, {0, 0}, 23, 1, 0, 180, 11, '\0', "", "", 0, 0},
    {"searching", "", RING, R_SEARCH, -1, {0, 0}, 33, 1, 0, 200, 11, '\0', "", "", 0, 0},
    {"see invisible", "", RING, R_SEEINVIS, -1, {0, 0}, 43, 1, 0, 175, 11, '\0', "", "", 0, 0},
    {"adornment", "", RING, R_NOP, -1, {0, 0}, 44, 1, 0, 100, 11, '\0', "", "", 0, 0},
    {"aggravate monster", "", RING, R_AGGR, -1, {0, 0}, 55, 1, 0, 100, 11, '\0', "", "", 0, 0},
    {"dexterity", "", RING, R_ADDHIT, -1, {0, 0}, 63, 1, 0, 220, 11, '\0', "", "", 0, 0},
    {"increase damage", "", RING, R_ADDDAM, -1, {0, 0}, 71, 1, 0, 220, 11, '\0', "", "", 0, 0},
    {"regeneration", "", RING, R_REGEN, -1, {0, 0}, 75, 1, 0, 260, 11, '\0', "", "", 0, 0},
    {"slow digestion", "", RING, R_DIGEST, -1, {0, 0}, 84, 1, 0, 240, 11, '\0', "", "", 0, 0},
    {"telportation", "", RING, R_TELEPORT, -1, {0, 0}, 93, 1, 0, 100, 11, '\0', "", "", 0, 0},
    {"stealth", "", RING, R_STEALTH, -1, {0, 0}, 100, 1, 0, 100, 11, '\0', "", "", 0, 0}
};

struct object ws_magic[MAXSTICKS] = {
    {"light", "", STICK, WS_LIGHT, -1, {0, 0}, 12, 1, 0, 120, 11, '\0', "", "", 0, 0},
    {"striking", "", STICK, WS_HIT, -1, {0, 0}, 21, 1, 0, 115, 11, '\0', "", "", 0, 0},
    {"lightning", "", STICK, WS_ELECT, -1, {0, 0}, 24, 1, 0, 200, 11, '\0', "", "", 0, 0},
    {"fire", "", STICK, WS_FIRE, -1, {0, 0}, 27, 1, 0, 200, 11, '\0', "", "", 0, 0},
    {"cold", "", STICK, WS_COLD, -1, {0, 0}, 30, 1, 0, 200, 11, '\0', "", "", 0, 0},
    {"polymorph", "", STICK, WS_POLYMORPH, -1, {0, 0}, 45, 1, 0, 210, 11, '\0', "", "", 0, 0},
    {"magic missile", "", STICK, WS_MISSILE, -1, {0, 0}, 55, 1, 0, 170, 11, '\0', "", "", 0, 0},
    {"haste monster", "", STICK, WS_HASTE_M, -1, {0, 0}, 64, 1, 0, 50, 11, '\0', "", "", 0, 0},
    {"slow monster", "", STICK, WS_SLOW_M, -1, {0, 0}, 75, 1, 0, 220, 11, '\0', "", "", 0, 0},
    {"drain life", "", STICK, WS_DRAIN, -1, {0 , 0}, 84, 1, 0, 210, 11, '\0', "", "", 0, 0},
    {"nothing", "", STICK, WS_NOP, -1, {0, 0}, 85, 1, 0, 70, 11, '\0', "", "", 0, 0},
    {"teleport away", "", STICK, WS_TELAWAY, -1, {0, 0}, 90, 1, 0, 140, 11, '\0', "", "", 0, 0},
    {"teleport to", "", STICK, WS_TELTO, -1, {0, 0}, 95, 1, 0, 60, 11, '\0', "", "", 0, 0},
    {"cancellation", "", STICK, WS_CANCEL, -1, {0, 0}, 100, 1, 0, 130, 11, '\0', "", "", 0, 0}
};

struct object armors[MAXARMORS] = {
    {"leather armor", "", ARMOR, LEATHER, -1, {0, 0}, 20, 1, 0, 0, 8, '\0', "", "", 0, 0},
    {"ring mail", "", ARMOR, RING_MAIL, -1, {0, 0}, 35, 1, 0, 0, 7, '\0', "", "", 0, 0},
    {"studded leather armor", "", ARMOR, STUDDED_LEATHER, -1, {0 , 0}, 50, 1, 0, 0, 7, '\0', "", "", 0, 0},
    {"scale mail", "", ARMOR, SCALE_MAIL, -1, {0, 0}, 63, 1, 0, 0, 6, '\0', "", "", 0, 0},
    {"chain mail", "", ARMOR, CHAIN_MAIL, -1, {0, 0}, 75, 1, 0, 0, 5, '\0', "", "", 0, 0},
    {"splint mail", "", ARMOR, SPLINT_MAIL, -1, {0, 0}, 85, 1, 0, 0, 4, '\0', "", "", 0, 0},
    {"banded mail", "", ARMOR, BANDED_MAIL, -1, {0, 0}, 95, 1, 0, 0, 4, '\0', "", "", 0, 0},
    {"plate mail", "", ARMOR, PLATE_MAIL, -1, {0, 0}, 100, 1, 0, 0, 3, '\0', "", "", 0, 0}
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

struct thing player;
struct stats max_stats;
char take;
char runch;
int dnum;
int ntraps;
int seed;
int max_level = 0;

struct object *cur_armor = NULL;
struct room *oldrp = NULL;
struct object *cur_ring[2];
char prbuf[80];
char whoami[80];
char fruit[80];
char huh[80];
char file_name[80];
char home[80];

struct object traps[MAXTRAPS];
struct room rooms[MAXROOMS];

char *s_names[MAXSCROLLS];
char *p_colors[MAXPOTIONS];
char *r_stones[MAXRINGS];
char *ws_made[MAXSTICKS];
char *s_guess[MAXSCROLLS];
char *p_guess[MAXPOTIONS];
char *r_guess[MAXRINGS];
char *ws_guess[MAXSTICKS];
char *ws_type[MAXSTICKS];
bool s_know[MAXSCROLLS];
bool p_know[MAXPOTIONS];
bool r_know[MAXRINGS];
bool ws_know[MAXSTICKS];

struct coord oldpos;
struct coord delta;

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
