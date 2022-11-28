#ifndef CONSTANTS_H_
#define CONSTANTS_H_

/*
 * Note to the Wizard: 
 *     Tweaking these constants can *GREATLY* change the game. Two years of
 *     constant tuning have generated these values. Minor adjustments are
 *     encouraged, but you mus ve very careful not to unbalance the game. Moria
 *     was meant to be challenging, not a give away. Many adjustments can cause
 *     the game to act strangely, or even cause errors.
 */

/* Current version number of Moria */
#define CUR_VERSION 4.87

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* Changing values below this line may be hazardous to your health! */

/* Key bindings options */
/* Uses keypad for directions */
#define ORIGINAL 0

/* Uses 'rogue' style keys for directions */
#define ROGUE_LIKE 1

/* Message line location */
#define MSG_LINE 0

/* Number of messages to save in a buffer */
#define SAVED_MSGS 20

/* Dungeon size parameters */
/* Multiple of 11; >= 22 */
#define MAX_HEIGHT 66

/* Multiple of 33; >= 66 */
#define MAX_WIDTH 198

#define SCREEN_HEIGHT 22
#define SCREEN_WIDTH 66
#define QUART_HEIGHT (SCREEN_HEIGHT / 4)
#define QUART_WIDTH (SCREEN_WIDTH / 4)

/* Output dungeon section sizes */
/* 44 lines of dungeon per section */
#define OUTPAGE_HEIGHT 44

/* 100 columns of dungeon per section */
#define OUTPAGE_WIDTH 99

/*
 * Dungeon generation valuesa
 * Note: The entire design of dungeon can be changed by only slight adjustments
 *       here.
 */
/* Random direction (4 is min) */
#define DUN_TUN_RND 36

/* Chance of changing direction (99 max) */
#define DUN_TUN_CHG 70

/* Distance for auto find to kick in */
#define DUN_TUN_FND 12

/* Chance of extra tunneling */
#define DUN_TUN_CON 15

/* Mean of # of rooms, standard dev2 */
#define DUN_ROO_MEA 32

/* % chance of room doors */
#define DUN_TUN_PEN 25

/* % chance of doors at tunnel junctions */
#define DUN_TUN_JCT 15

/* Density of streamers */
#define DUN_STR_DEN 5

/* Width of streamers */
#define DUN_STR_RNG 2

/* Number of magma streamers */
#define DUN_STR_MAG 3

/* 1/x chance of treasure per magma */
#define DUN_STR_MC 95

/* Number of quartz streamers */
#define DUN_STR_QUA 2

/* 1/x chance of treasure per quartz */
#define DUN_STR_QC 55

/* Level/x chance of unusual room */
#define DUN_UNUSUAL 300

/* Store constants */
/* Number of owners to choose from */
#define MAX_OWNERS 18

/* Number of different stores */
#define MAX_STORES 6

/* Max number of discrete objects in inventory */
#define STORE_INVEN_MAX 24

/* Number of items to choose stock from */
#define STORE_CHOICES 26

/* Max different objects in score before auto sell */
#define STORE_MAX_INVEN 20

/* Min different objects in stock before auto buy */
#define STORE_MIN_INVEN 14

/* Amount of buying and selling normally */
#define STORE_TURN_AROUND 3

/* Size of store init array */
#define INVEN_INIT_MAX 105

/* Adjust prices for buying and selling */
#define COST_ADJ 1.00

/* Treasure constants */
/* Size of inventory array (do not change) */
#define INVEN_ARRAY_SIZE 35

/* Index of highest entry in inventory array */
#define INVEN_MAX 34

/* Maximum level of magic in dungeon */
#define MAX_OBJ_LEVEL 50

/* 1/n chance of item being a Great Item */
#define OBJ_GREAT 20

/* Number of objects for universe */
#define MAX_OBJECTS 344

/* Number of different types of gold */
#define MAX_GOLD 18

/* Max objects per level */
#define MAX_TALLOC 225

/* Amount of objects for rooms */
#define TREAS_ROOM_ALLOC 7

/* Amount of objects for corridors */
#define TREAS_ANY_ALLOC 2

/* Amount of gold (and gems) */
#define TREAS_GOLD_ALLOC 2

/*
 * Magic Treasure Generation constants
 * Note: Number of special objects, and degree of enchantments can be adjusted
 *       here.
 */
/* Adjust STD per level */
#define OBJ_STD_ADJ 1.25

/* Minimum STD */
#define OBJ_STD_MIN 7

/* Town object generation level */
#define OBJ_TOWN_LEVEL 20

/* Base amount of magic */
#define OBJ_BASE_MAGIC 15

/* Max amount of magic */
#define OBJ_BASE_MAX 70

/* magic_chance/# special magic */
#define OBJ_DIV_SPECIAL 6

/* magic_chance/# cursed items */
#define OBJ_DIV_CURSED 1.3

/* Constants describing limits of certain objects */
/* Maximum amount that lamp can be filled */
#define OBJ_LAMP_MAX 15000

/* Maximum range of bolts and balls */
#define OBJ_BOLT_RANGE 18

/* Rune of protection resistance */
#define OBJ_RUNE_PROT 3000

/* Creature constants */
/* Number of creatures defined for universe */
#define MAX_CREATURES 279

/* Max that can be allocated */
#define MAX_MALLOC 100 + 1

/* 1/x chance of new monster each round */
#define MAX_MALLOC_CHANCE 160

/* Maximum level of creatures */
#define MAX_MONS_LEVEL 40

/* Maximum distance a creature can be seen */
#define MAX_SIGHT 20

/* Maximum distance creature spell can be cast */
#define MAX_SPELL_DIS 20

/* Maximum reproductions on a level */
#define MAX_MON_MULT 75

/* High value slow multiplication */
#define MON_MULT_ADJ 7

/* dun_level/x chance of high level creature */
#define MON_NASTY 50

/* Minimum number of monsters/level */
#define MIN_MALLOC_LEVEL 14

/* Number of people on town level (day) */
#define MIN_MALLOC_TD 4

/* Number of people on town level (night) */
#define MIN_MALLOC_TN 8

/* Total number of "win" creatures */
#define WIN_MON_TOT 2

/* Level where winning creatures begin */
#define WIN_MON_APPEAR 50

/* Adjust level of summoned creatures */
#define MON_SUMMON_ADJ 2

/* Percent of player experience drained per hit */
#define MON_DRAIN_LIFE 2

/* Trap constants */
/* Number of defined traps */
#define MAX_TRAPA 18

/* Includes secret doors */
#define MAX_TRAPB 19

/* Descriptive constants */
/* Used with potions */
#define MAX_COLORS 67

/* Used with mushrooms */
#define MAX_MUSH 29

/* Used with staffs */
#define MAX_WOODS 41

/* Used with wands */
#define MAX_METALS 31

/* Used with rings */
#define MAX_ROCKS 52

/* Used with amulets */
#define MAX_AMULETS 39

/* Used with scrolls */
#define MAX_SYLLABLES 153

/* Player constants */
/* Maximum possible character level */
#define MAX_PLAYER_LEVEL 40

/* Number of defined races */
#define MAX_RACES 8

/* Number of defined classes */
#define MAX_CLASS 6

/* x> harder devices, x< easier devices */
#define USE_DEVICE 3

/* Number of types of histories for universe */
#define MAX_BACKGROUND 128

/* Getting full */
#define PLAYER_FOOD_FULL 10000

/* Maximum food value, beyond is wasted */
#define PLAYER_FOOD_MAX 15000

/* Character begins fainting */
#define PLAYER_FOOD_FAINT 300

/* Warn player that he is getting very low */
#define PLAYER_FOOD_WEAK 1000

/* Warn player that hs it getting low */
#define PLAYER_FOOD_ALERT 2000

/* Regen factor when fainting */
#define PLAYER_REGEN_FAINT 0.0005

/* Regen factor when weak */
#define PLAYER_REGEN_WEAK 0.0015

/* Regen factor when full */
#define PLAYER_REGEN_NORMAL 0.0030

/* Min amount hp regen */
#define PLAYER_REGEN_HPBASE 0.0220

/* Min amount mana regen */
#define PLAYER_REGEN_MNBASE 0.0080

/* "#" * (1/10 pounds) per strength point */
#define PLAYER_WEIGHT_CAP 130

/* Pause time before player can re-roll */
#define PLAYER_EXIT_PAUSE 2

/* Base to hit constants */
/* Adjust BTH per level */
#define BTH_LEV_ADJ 3

/* Adjust BTH per plus-to-hit */
#define BTH_PLUS_ADJ 3

/* Automatic hit; 1/bth_hit */
#define BTH_HIT 12

/* Table size for random() */
#define STATE_SIZE 128

/* Magic numbers for players inventory array */
#define INVEN_WIELD 22
#define INVEN_HEAD 23
#define INVEN_NECK 24
#define INVEN_BODY 25
#define INVEN_ARM 26
#define INVEN_HANDS 27
#define INVEN_RIGHT 28
#define INVEN_LEFT 29
#define INVEN_FEET 30
#define INVEN_OUTER 31
#define INVEN_LIGHT 32
#define INVEN_AUX 33

#endif
