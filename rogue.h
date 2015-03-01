// Rogue defintions and variable declarations
//
// @(#)rogue.h 3.38 (Berkeley) 6/15/81

#ifndef ROGUE_H_
#define ROGUE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <ncurses.h>

// Various constants
enum constants {
    CALLABLE    =   -1,
    BOLT_LENGTH =    6,
    // Number of types of things (scrolls, rings, etc.)
    NUMTHINGS   =    7,
    ESCAPE_KEY  =   27,
    STOMACHSIZE = 2000
};

// Maximum number of different things
enum maximums {
    MAXROOMS  =  9,
    MAXTHINGS =  9,
    MAXOBJ    =  9,
    MAXPACK   = 23,
    MAXTRAPS  = 10
};

// Return values for get functions
enum return_values {
    // Normal exit
    NORM = 0,
    // Quit option setting
    QUIT,
    // Back up one level
    MINUS
};

// Things that appear on screens
enum syombols {
    PASSAGE    = '#',
    DOOR       = '+',
    FLOOR      = '.',
    PLAYER     = '@',
    TRAP       = '^',
    TRAPDOOR   = '>',
    ARROWTRAP  = '{',
    SLEEPTRAP  = '$',
    BEARTRAP   = '}',
    TELTRAP    = '~',
    DARTTRAP   = '`',
    SECRETDOOR = '&',
    STAIRS     = '%',
    GOLD       = '*',
    POTION     = '!',
    SCROLL     = '?',
    MAGIC      = '$',
    FOOD       = ':',
    WEAPON     = ')',
    ARMOR      = ']',
    AMULET     = ',',
    RING       = '=',
    STICK      = '/'
};

enum times {
    HOLDTIME   =    2,
    BEARTIME   =    3,
    SLEEPTIME  =    5,
    HEALTIME   =   30,
    WANDERTIME =  150,
    MORETIME   =  150,
    HUNGERTIME = 1300
};

enum durations {
    HUHDURATION =  20,
    SEEDURATION = 850
};

enum orderings {
    BEFORE = 0,
    AFTER
};

enum directions {
    LEFT = 0,
    RIGHT
};

// Save against things
enum saves {
    VS_POISON        = 0x0000,
    VS_PARALYZATION  = 0x0000,
    VS_DEATH         = 0x0000,
    VS_PETRIFICATION = 0x0001,
    VS_BREATH        = 0x0002,
    VS_MAGIC         = 0x0003
};

// Various flag bits
enum flags {
    ISDARK   = 0x0000,
    ISCURSED = ISDARK,
    ISBLIND  = ISDARK,
    ISGONE   = 0x0001,
    ISKNOW   = ISGONE,
    ISRUN    = 0x0002,
    ISFOUND  = 0x0004,
    ISINVIS  = 0x0008,
    ISMEAN   = 0x0010,
    ISGREED  = 0x0020,
    ISBLOCK  = 0x0040,
    ISHELD   = 0x0080,
    ISHUH    = 0x0100,
    ISREGEN  = 0x0200,
    CANHUH   = 0x0400,
    CANSEE   = 0x0800,
    ISMISL   = 0x1000,
    ISCANC   = ISMISL,
    ISMANY   = 0x2000,
    ISSLOW   = 0x2000,
    ISHASTE  = 0x4000
};

// Potion types
enum potion_type {
    P_CONFUSE = 0,
    P_PARALYZE,
    P_POISON,
    P_STRENGTH,
    P_SEEINVIS,
    P_HEALING,
    P_MFIND,
    P_TFIND,
    P_RAISE,
    P_XHEAL,
    P_HASTE,
    P_RESTORE,
    P_BLIND,
    P_NOP,
    MAXPOTIONS
};

// Scroll types
enum scroll_type {
    S_CONFUSE = 0,
    S_MAP,
    S_LIGHT,
    S_HOLD,
    S_SLEEP,
    S_ARMOR,
    S_IDENT,
    S_SCARE,
    S_GFIND,
    S_TELEP,
    S_ENCH,
    S_CREATE,
    S_REMOVE,
    S_AGGR,
    S_NOP,
    S_GENOCIDE,
    MAXSCROLLS
};

// Weapon types
enum weapon_type {
    MACE = 0,
    SWORD,
    BOW,
    ARROW,
    DAGGER,
    ROCK,
    TWOSWORD,
    SLING,
    DART,
    CROSSBOW,
    BOLT,
    SPEAR,
    MAXWEAPONS
};

// Armor types
enum armor_type {
    LEATHER = 0,
    RING_MAIL,
    STUDDED_LEATHER,
    SCALE_MAIL,
    CHAIN_MAIL,
    SPLINT_MAIL,
    BANDED_MAIL,
    PLATE_MAIL,
    MAXARMORS
};

// Ring types
enum ring_type {
    R_PROTECT = 0,
    R_ADDSTR,
    R_SUSTSTR,
    R_SEARCH,
    R_SEEINVIS,
    R_NOP,
    R_AGGR,
    R_ADDHIT,
    R_ADDDAM,
    R_REGEN,
    R_DIGEST,
    R_TELEPORT,
    R_STEALTH,
    MAXRINGS
};

// Rod/wand/staff types
enum stick_type {
    WS_LIGHT = 0,
    WS_HIT,
    WS_ELECT,
    WS_FIRE,
    WS_COLD,
    WS_POLYMORPH,
    WS_MISSILE,
    WS_HASTE_M,
    WS_SLOW_M,
    WS_DRAIN,
    WS_NOP,
    WS_TELAWAY,
    WS_TELTO,
    WS_CANCEL,
    MAXSTICKS
};

// Now we define the structures and types

// Help list
struct h_list {
    char h_ch;
    char *h_desc;
};

// Coordinate data type
typedef struct {
    int x;
    int y;
} coord;

// Linked list data type
struct linked_list {
    struct linked_list *l_next;
    struct linked_list *l_prev;
    // Various structure pointers
    char *l_data;
};

// Stuff about magic items
struct magic_item {
    char *mi_name;
    int mi_prob;
    int mi_worth;
};

// Room structure
struct room {
    // Upper left corner
    coord r_pos;
    // Size of room
    coord r_max;
    // Where the gold is
    coord r_gold;
    // How much gold is worth
    int r_goldval;
    // Info about the room
    int r_flags;
    // Number of exits
    int r_nexits;
    // Where the exits are
    coord r_exit[4];
};

// Array of all traps on this level
struct trap {
    // Where the trap is
    coord tr_pos;
    // What kind of trap
    char tr_type;
    // Info about trap (i.e ISFOUND)
    int tr_flags;
} traps[MAXTRAPS];

// Structure describing a fighting being
struct stats {
    // Strength
    short st_str;
    short st_add;
    // Experience
    long s_exp;
    // Level of mastery
    int s_lvl;
    // Armor class
    int s_arm;
    // Hit points
    int s_hpt;
    // String describing damage done
    char *s_dmg;
};

// Structure for monsters and player
struct thing {
    /* What to call the thing */
    char *t_name;
    // Position
    coord t_pos;
    // If slowed, is it a turn to move
    bool t_turn;
    // What is it
    char t_type;
    // Character that was where it was
    char t_oldch;
    // Where it is running to
    coord *t_dest;
    // State word
    short t_flags;

    /* These things are specific to monsters */
    char t_disguise; /* What mimic looks like */
    short t_carry; /* Probability of carrying something */

    // Physical description
    struct stats t_stats;
    // what the thing is carrying
    struct linked_list *t_pack;
};

// Struct for a thing that the rogue can carry
struct object {
    // The name of the object
    char *o_name;
    // What it says if you read it
    char *o_text;
    // What kind of object it is
    int o_type;
    // Which object of a type it is
    int o_which;
    // Group number for this object
    int o_group;

    // Where it lives on the screen
    coord o_pos;
    // Probability of the object
    int o_prob;
    // Count for plural objects
    int o_count;
    // Information about objects
    int o_flags;

    // Armor class
    int o_ac;    

    // What you need to launch it
    char o_launch;
    // Damage if used like a sword
    char *o_damage;
    // Damage if thrown
    char *o_hurldmg;
    // Plusses to hit
    int o_hplus;
    // Plusses to damage
    int o_dplus;
};

// Now all the global variables

// One for each room, A level
struct room rooms[MAXROOMS];
// Roomin(&oldpos)
struct room *oldrp;
// List of monsters on the level
struct linked_list *mlist;
// The rogue
struct thing player;
// The maximum for the player
struct stats max_stats;
// The initial monster stats
extern struct thing monsters[26];
// List of objects on this level
struct linked_list *lvl_obj;
// WHich weapon he is wielding
struct object *cur_weapon;
// What a well dressed rogue wears
struct object *cur_armor;
// Which rings are being worn
struct object *cur_ring[2];
// Chances for each type of item
extern struct magic_item things[NUMTHINGS];
// Names and chances for scrolls
extern struct magic_item s_magic[MAXSCROLLS];
// Names and chances for potions
extern struct magic_item p_magic[MAXPOTIONS];
// Names and chances for rings
extern struct magic_item r_magic[MAXRINGS];
// Names and chances for sticks
extern struct magic_item ws_magic[MAXSTICKS];
// Names and chances for armor
extern struct object armors[MAXARMORS];

// What leve rogue is on
int level;
// How much gold the rogue has
int purse;
// Where cursor is on top line
int mpos;
// Number of traps on this level
int ntraps;
// Number of turns held in place
int no_move;
// Number of turns asleep
int no_command;
// Number of things in pack
int inpack;
// Player's max hit points
int max_hp;
// Total dynamic memory bytes
int total;
// Score before this turn
int lastscore;
// Number of levels without food
int no_food;
// Random number seed
int seed;
// Number of times to repeat command
int count;
// Dungeon number
int dnum;
// Number of times fungi has hit
int fung_hit;
// Number of quiet turns
int quiet;
// Deepest player has gone
int max_level;
// Amount of food in hero's stomach
int food_left;
// Current group number
int group;
// How hungry is he
int hungry_state;

// Thing the rogue is taking
char take;
// Buffer for sprintf()'s
char prbuf[80];
// Output buffer for stdout
char outbuf[BUFSIZ];
// Direction player is running
char runch;
// Names of the scrolls
char *s_names[MAXSCROLLS];
// Colors of the potions
char *p_colors[MAXPOTIONS];
// Stone settings of the rings
char *r_stones[MAXRINGS];
// Names of the various weapons
char *w_names[MAXWEAPONS];
// What sticks are made of
char *ws_made[MAXSTICKS];
// Release number of rogue
char *release;
// Name of player
char whoami[80];
// Favorite fruit
char fruit[80];
// The last message printed
char huh[80];
// Player's guess at what scroll is
char *s_guess[MAXSCROLLS];
// Player's guess at what potion is
char *p_guess[MAXPOTIONS];
// Player's guess at what ring is
char *r_guess[MAXRINGS];
// Player's guess at what wand is
char *ws_guess[MAXSTICKS];
// Is it a wand or a staff
char *ws_type[MAXSTICKS];
// Save file name
char file_name[80];
// User's home directory
char home[80];

// Window that the player sees
WINDOW *cw;
// Used for the help command
WINDOW *hw;
// Used to store monsters
WINDOW *mw;

// True if the player is running
extern bool running;
// True until he quits
extern bool playing;
// True if allows wizard commands
extern bool wizard;
// True if we want after daemons
bool after;
// True if player wants to know
extern bool notify;
// True if toilet input
extern bool fight_flush;
// True if we should be short
extern bool terse;
// Stop running when we pass a door
extern bool door_stop;
// Show running as a series of jumps
extern bool jump;
// Inventory one line at a time
extern bool slow_invent;
// First move after setting door_stop
extern bool firstmove;
// Was a wizard sometime
bool waswizard;
// Ask about unidentified things
extern bool askme;
// Does he know what a scroll does
bool s_know[MAXSCROLLS];
// Does he know what a potion does
bool p_know[MAXPOTIONS];
// Does he know what a ring does
bool r_know[MAXRINGS];
// Does he know what a stick does
bool ws_know[MAXSTICKS];
// He found the amulet
extern bool amulet;
// True if executing a shell
extern bool in_shell;

// Position before last look() call
coord oldpos;
// Change indicated to get_dir()
coord delta;

#endif
