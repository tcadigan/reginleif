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

// Maximum number of different things
#define MAXROOMS 9
#define MAXTHINGS 9
#define MAXOBJ 9
#define MAXPACK 23
#define MAXTRAPS 10
// Number of types of things (scrolls, rings, etc.)
#define	NUMTHINGS 7

// Return values for get functions
// Normal exit
#define	NORM 0
// Quit option setting
#define	QUIT 1
// Back up one option
#define	MINUS 2

// Things that appear on screens
#define PASSAGE '#'
#define DOOR '+'
#define FLOOR '.'
#define PLAYER '@'
#define TRAP '^'
#define TRAPDOOR '>'
#define ARROWTRAP '{'
#define SLEEPTRAP '$'
#define BEARTRAP '}'
#define TELTRAP '~'
#define DARTTRAP '`'
#define SECRETDOOR '&'
#define STAIRS '%'
#define GOLD '*'
#define POTION '!'
#define SCROLL '?'
#define MAGIC '$'
#define FOOD ':'
#define WEAPON ')'
#define ARMOR ']'
#define AMULET ','
#define RING '='
#define STICK '/'
#define CALLABLE -1

// Various constants
#define PASSWD "mTQTOe4GHXZEI"
#define BEARTIME 3
#define SLEEPTIME 5
#define HEALTIME 30
#define HOLDTIME 2
#define WANDERTIME 150
#define BEFORE 1
#define AFTER 2
#define HUHDURATION 20
#define SEEDURATION 850
#define HUNGERTIME 1300
#define MORETIME 150
#define STOMACHSIZE 2000
#define ESCAPE 27
#define LEFT 0
#define RIGHT 1
#define BOLT_LENGTH 6

// Save against things
#define VS_POISON 00
#define VS_PARALYZATION 00
#define VS_DEATH 00
#define VS_PETRIFICATION 01
#define VS_BREATH 02
#define VS_MAGIC  03

// Various flag bits
#define ISDARK	0000001
#define ISCURSED 000001
#define ISBLIND 0000001
#define ISGONE	0000002
#define ISKNOW  0000002
#define ISRUN	0000004
#define ISFOUND 0000010
#define ISINVIS 0000020
#define ISMEAN  0000040
#define ISGREED 0000100
#define ISBLOCK 0000200
#define ISHELD  0000400
#define ISHUH   0001000
#define ISREGEN 0002000
#define CANHUH  0004000
#define CANSEE  0010000
#define ISMISL  0020000
#define ISCANC	0020000
#define ISMANY  0040000
#define ISSLOW	0040000
#define ISHASTE 0100000

// Potion types
#define P_CONFUSE 0
#define P_PARALYZE 1
#define P_POISON 2
#define P_STRENGTH 3
#define P_SEEINVIS 4
#define P_HEALING 5
#define P_MFIND 6
#define P_TFIND 7
#define P_RAISE 8
#define P_XHEAL 9
#define P_HASTE 10
#define P_RESTORE 11
#define P_BLIND 12
#define P_NOP 13
#define MAXPOTIONS 14

// Scroll types
#define S_CONFUSE 0
#define S_MAP 1
#define S_LIGHT 2
#define S_HOLD 3
#define S_SLEEP 4
#define S_ARMOR 5
#define S_IDENT 6
#define S_SCARE 7
#define S_GFIND 8
#define S_TELEP 9
#define S_ENCH 10
#define S_CREATE 11
#define S_REMOVE 12
#define S_AGGR 13
#define S_NOP 14
#define S_GENOCIDE 15
#define MAXSCROLLS 16

// Weapon types
#define MACE 0
#define SWORD 1
#define BOW 2
#define ARROW 3
#define DAGGER 4
#define ROCK 5
#define TWOSWORD 6
#define SLING 7
#define DART 8
#define CROSSBOW 9
#define BOLT 10
#define SPEAR 11
#define MAXWEAPONS 12

// Armor types
#define LEATHER 0
#define RING_MAIL 1
#define STUDDED_LEATHER 2
#define SCALE_MAIL 3
#define CHAIN_MAIL 4
#define SPLINT_MAIL 5
#define BANDED_MAIL 6
#define PLATE_MAIL 7
#define MAXARMORS 8

// Ring types
#define R_PROTECT 0
#define R_ADDSTR 1
#define R_SUSTSTR 2
#define R_SEARCH 3
#define R_SEEINVIS 4
#define R_NOP 5
#define R_AGGR 6
#define R_ADDHIT 7
#define R_ADDDAM 8
#define R_REGEN 9
#define R_DIGEST 10
#define R_TELEPORT 11
#define R_STEALTH 12
#define MAXRINGS 13

// Rod/wand/staff types
#define WS_LIGHT 0
#define WS_HIT 1
#define WS_ELECT 2
#define WS_FIRE 3
#define WS_COLD 4
#define WS_POLYMORPH 5
#define WS_MISSILE 6
#define WS_HASTE_M 7
#define WS_SLOW_M 8
#define WS_DRAIN 9
#define WS_NOP 10
#define WS_TELAWAY 11
#define WS_TELTO 12
#define WS_CANCEL 13
#define MAXSTICKS 14

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

typedef struct {
    short st_str;
    short st_add;
} str_t;

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
    str_t s_str;
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
    // Position
    coord t_pos;
    // If slowed, is it a turn to move
    bool t_turn;
    // What is it
    char t_type;
    // What mimic looks like
    char t_disguise;
    // Character that was where it was
    char t_oldch;
    // Where it is running to
    coord *t_dest;
    // State word
    short t_flags;
    // Physical description
    struct stats t_stats;
    // what the thing is carrying
    struct linked_list *t_pack;
};

// Array containing information on all the various types of monsters
struct monster {
    // What to call the monster
    char *m_name;
    // Probability of carrying something
    short m_carry;
    // Things about the monster
    short m_flags;
    // Initial stat
    struct stats m_stats;
};

// Struct for a thing that the rogue can carry
struct object {
    // What kind of object it is
    int o_type;
    // Where it lives on the screen
    coord o_pos;
    // What it says if you read it
    char *o_text;
    // What you need to launch it
    char o_launch;
    // Damage if used like a sword
    char *o_damage;
    // Damage if thrown
    char *o_hurldmg;
    // Count for plural objects
    int o_count;
    // Which object of a type it is
    int o_which;
    // Plusses to hit
    int o_hplus;
    // Plusses to damage
    int o_dplus;
    // Armor class
    int o_ac;
    // Information about objects
    int o_flags;
    // Group number for this object
    int o_group;
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
extern struct monster monsters[26];
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
// Probabilities for armor
extern int a_chances[MAXARMORS];
// Armor class for various armors
extern int a_class[MAXARMORS];
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
// Names of armor types
extern char *a_names[MAXARMORS];
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
