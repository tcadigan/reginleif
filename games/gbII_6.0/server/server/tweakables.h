/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it an/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, 5th Floor, Boston, MA 012110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * Tweakable constants and other things -- Changing the following may cause GB
 * to freak if the functions using them are not recompiled so be careful.
 *
 * $Header: /var/cvs/gbp/GB+/hdrs/tweakables.h,v 1.7 2007/07/06 16:55:26 gbp Exp $
 */
#ifndef TWEAKABLES_H_
#define TWEAKABLES_H_

#include "config.h"

/*
 * Select the pseudo-random number generator. Choose ONE of the three:
 *   rand()
 *   random()
 *   lrand48()
 * Note that rand48 is considered by SVID 3. -mfw
 */
#define RAND
#undef RANDOM
#undef RAND48

/*
 * If set non-zero the racegen/enroll program will put #sexes (as popn) on the
 * planet surface where the gov ship is located. This means population will grow
 * starting with the first update whether or not the player has logged
 * in. Setting this to zero means the only population will exist in the Gov
 * ship. The player will have to unload popn to the planet surface before the
 * planet is population. Really, the only purpose here is to get player to log
 * in before the first update. -mfw
 */
#define ADAM_AND_EVE 0

/*
 * For 'UNIV level' survey command, what's the name of our galaxy and what type
 * (cosmetic only) -mfw
 */
#define GAL_NAME "Solkan"
#define GAL_TYPE "Lenticular"

/*
 * New code by Kharush. Added for planetary stockpile overflow checking. Also
 * constants for absorbing in pod bursts. Start of new code.
 */
#ifndef USHRT_MAX
#include <limits.h>

#ifndef USHRT_MAX
/* Check suitable values for unsigned short int in your system!!! */
#define USHRT_MAX 65535
#endif
#endif

/*
 * Combat update - Set this to amount of damage orbiting ships incur when
 * orbiting occupied planets
 */
#define COMBAT_ORBIT_DAMAGE 20

/* Landed ships shooting */
/* #define NO_LANDED_SHOOT */

/* Cloaking constants */
#define CLOAK_PROBE_DIST 500
#define CLOAK_OTHER_DIST 200
#define CLOAK_TECH_FACTOR 200
#define CLOAK_FUEL_CONSUMPTION 0.5

#define ABSORB_RATE 0.40
#define MILITARY_PROPORTION 4.0
#define MILITARY_WEIGHT 4.0

/* End of new code. */

/* #define GB_VERSION "5.1.1 'HUT' 23.12.1997" */
/* #define GB_VERSION "2.0.0 JH_1.2 08/05/98" */
/* #define GB_VERSION "GB+ 6.0b 01/25/05 */
#define GB_VERSION "GB+ 6.0c 07/06/07"

#define MOVE_FACTOR 1

#define DEFAULT_BACKUP_TIME (1440) /* Backup time (minutes!) */
#define DEFAULT_UPDATE_TIME (24 * 60) /* Update time (minutes!) */
#define DEFAULT_RANDOM_UPDATE_RANGE 0 /* Again, in minutes. */
#define DEFAULT_RANDOM_SEGMENT_RANGE 0 /* Again, in minutes. */

/* #define DEFAULT_UPDATE_TIME (12 * 60) /\* Update time (minutes!) *\/ */
/* #define DEFAULT_RANDOM_UPDATE_RANGE 60 /\* Again, in minutes. *\/ */
/* #define DEFAULT_RANDOM_SEGMENT_RANGE 60 /\* Again, in minutes. *\/ */

/*
 * If MOVES_PER_UPDATE is set to 1, there will be no movement segments between
 * updates; the move is counted as part of the update. Set this to something
 * higher to have evenly spaced movement segments.
 */
#define MOVES_PER_UPDATE 3

#define LOGIN_NAME_SIZE 64

#define NUM_TIMES_TO_WAIT_FOR_LOCK 200
#define NEUTRAL_FD 1000

#define MAXCOMMSTRSIZE 250
#define COMMANDSIZE 42
#define MAXARGS 256
#define MAXOPTS 256

#define MAXLOGIN_ATTEMPTS 3

#define CHAR_CURR_SCOPE ':' /* For getplace */

#define RTEMP 0 /* Regular temp for planet */
#define TEMP 1 /* Temperature */
#define METHANE 2 /* % of gases for terraforming */
#define OXYGEN 3
#define CO2 4
#define HYDROGEN 5
#define NITROGEN 6
#define SULFUR 7
#define HELIUM 8
#define OTHER 9
#define TOXIC 10

#define CHAR_LAND '*'
#define CHAR_SEA '.'
#define CHAR_MOUNT '^'
#define CHAR_DIFFOWNED '?'
#define CHAR_PLATED 'o'
#define CHAR_WASTED '%'
#define CHAR_GAS '~'
#define CHAR_CLOAKED ' '
#define CHAR_ICE '#'
#define CHAR_CRYSTAL 'x'
#define CHAR_DESERT '-'
#define CHAR_FOREST '"'
#define CHAR_WORM '?'
#define CHAR_SWAMP '(' /* Not used yet */

#define CHAR_MY_TROOPS 'X'
#define CHAR_ALLIED_TROOPS 'A'
#define CHAR_ATWAR_TROOPS 'E'
#define CHAR_NEUTRAL_TROOPS 'N'

#define NAMESIZE 18
#define RNAMESIZE 35
#define MOTTOSIZE 64
#define PERSONALSIZE 128
#define PLACENAMESIZE (NAMESIZE + NAMESIZE + 13)
#define NUMSTARS 256

/*
 * MAXPLANETS should be the desired number + 1. One planet is always reserved
 * for potential wormhole
 */
#define MAXPLANETS 11

/* #define MAXMOONS 3 */

/* Set to 0 if you want unlimited resource availability (pre 3.0) */
#define LIMITED_RESOURCES 1

/*
 * Once resources run out from a sector, this is the number of resources that
 * may still be milked out of a sector. This came from the Treehouse code and
 * was originally set to 2. -mfw
 */
#define TRICKLE_RESOURCES 0

/* Ranges of map sizes (x usually) */
#define MAX_X 45 /* Top range for planet */
#define MAX_Y 19
#define RATIOXY 3.70 /* Map ratio between x and y */

/* #define DISCONNECT_TIME 30 /\* rjn was 60 *\/ */
#define DISCONNECT_TIME 120 /* rjn was 60 */

/* The size (in arbitrary units) of the GB game universe (actually galaxy) */
/* #define UNIVSIZE 300000 *///
#define UNIVSIZE 100000

/* The size of solar systems (before you're in solar system scope) */
#define SYSTEMSIZE 2000

/* The distance form a planet before being considered in orbit */
#define PLORBITSIZE 50

#define WEEKLY 300
#define DAILY 180

#define VICTORY_PERCENT 21
#define VICTORY_UPDATES 10

#define AUTO_TELEG_SIZE 2000
#define UNTRANS_MSG "[ ? ]"
#define TELEG_TRANS_APCOST 1
#define TELEG_TRANS_RPCOST 5
#define TELEG_TRANS_AMT 0.45
#define TELEG_LETTERS 7
#define TELEG_PLAYER_AUTO (-2)
#define TELEG_MAX_AUTO 7 /* When changing, alter field in plinfo */
#define TELEG_DELIM '~'
#define TELEG_NDELIM "%[^~]"

#define MASS_FUEL 0.05
#define MASS_RESOURCE 0.1
#define MASS_DESTRUCT 0.15
#define MASS_ARMOR 1.0
#define MASS_SIZE 0.3
#define MASS_HANGER 0.1
#define MASS_GUNS 0.2

#define SIZE_GUNS 0.1
#define SIZE_CREW 0.01
#define SIZE_RESOURCE 0.02
#define SIZE_FUEL 0.01
#define SIZE_DESTRUCT 0.02
#define SIZE_HANGER 0.1

/* Constants for Factory mass and size */
#define HAB_FACT_SIZE 0.2

/* Cost factors for factory activation cost */
#define HAB_FACT_ON_COST 4
#define PLAN_FACT_ON_COST 2

#define SECTOR_DAMAGE 0.3
#define SHIP_DAMAGE 2.0

/* Von Neumann Machine tweakables */
#define VN_ROGUE_RATE 2 /* % each turn to go rogue */
#define VN_EFFICIENCY 0.75 /* VN mining efficiency */
#define VN_RES_TAKE 50 /* Percent of resource of a sector the VN's take */
#define VN_DEATHTOLL 50 /* Number of VN's killed before berserkers made */
#define VNS_LIKE_TO_LIVE 10 /* % change VN's say no to scrap not implemented */

#define LAUNCH_GRAV_MASS_FACTOR 0.18 /* Fuel use modifier for taking off */
#define LAND_GRAV_MASS_FACTOR 0.0145

#define FUEL_USE 0.02 /* Fuel use per ship mass pt. per speed factor */
#define HABITAT_PROD_RATE 0.05
#define HABITAT_POP_RATE 0.20

#define REPAIR_RATE 25.0 /* Rate at which ships get repaired */
/* How much it costs to remove a wasted status from a sector */
#define SECTOR_REPAIR_COST 10
#define NATURAL_REPAIR 5 /* Chance of the wasted status being removed/update */

#define CREAT_UNIV_ITERAT 10 /* Iterations for star movement */

#define GRAV_FACTOR 0.0025 /* Not sure as to what this should be */

#define FACTOR_FERT_SUPPORT 1 /* # of people/fert pt sector supports */
#define EFF_PROD 0.20 /* Production of effcncy/pop */
#define RESOURCE_PRODUCTION 0.00008 /* Adjust these to change prod */
#define FUEL_PRODUCTION 0.0008
#define DEST_PRODUCTION 0.0008
#define POPN_PROD 0.3

#define HYPER_DRIVE_READY_CHARGE 1
#define HYPER_DRIVE_FUEL_USE 5.0
#define HYPER_DIST_FACTOR 200.0

#define TECH_INVEST 0.01 /* Invest factor */
#define TECH_SCALE 2.0 /* Investment scale */

/* Mobility cost in resources per update per sector in doplanet.c */
#define MOB_COST 0.00 /* Was 0.01 in orig GB -mfw */
/* #define RESOURCE_DEPLETION 0.015 */
#define RESOURCE_DEPLETION 0.0
#define FACTOR_MOBPROD 0.06 /* Mobilization production/person */
#define MESO_POP_SCALE 20000.0

#define FUEL_COST_TERRA 3.0 /* Cost to terraform */
#define FUEL_COST_QUARRY 2.0 /* Cost to mine resources */
#define FUEL_COST_PLOW 2.0
#define RES_COST_DOME 1.0
#define FUEL_COST_INF 1.0
#define RES_COST_WPLANT 1.0
#define FUEL_COST_WPLANT 1.0
#define FUEL_COST_AP 3.0

#define ENLIST_TROOP_COST 5 /* Money it costs to pay a trooper */
#define UPDATE_TROOP_COST 1

#define PLAN_FIRE_LIM 20 /* Max fire strength from planets */

#define TECH_SEE_STABILITY 15 /* Min tech to see star stability */
#define TECH_EXPLORE 10 /* Min tech to see your whole planet */

#define ENVIR_DAMAGE_TOX 70 /* Min tox to damage planet */

#define PLANETGRAVCONST 0.05
#define SYSTEMGRAVCONST 150000.0

#define FUEL_MANEUVER 0.3 /* order.c -- Fuel it costs to aim */
/*
 * Changes to fix a bug. Maarten
 * Description: You could when you just entered planet scope assault/dock with a
 * ship in close orbit, and then immediately land.
 */
#define DIST_TO_LAND 10.0 /* moveship.c, land.c */
#define DIST_TO_DOCK 10.0

/* Distance from sun needed to destroy ship, unfinished */
/* #define DIST_TO_BURN 50 */

#define FACTOR_DAMAGE 2.0
#define FACTOR_DESTPLANET 0.35

/* Various compiler options that may save CPU time/disk space */
#define NO_SLIDING_SCALE_AUTOMOVE 0 /* Move to all four adjacent spots */
#define POPN_MOVE_SCALE_1 400 /* Limit at which popn moves to all */
#define POPN_MOVE_SCALE_2 3000 /* " " " popn moves 2 adj. spaces */
/* Otherwise move to only 1 adj. space */
#define SHIP_MOVE_SCALE 3.0

/* To save object code */
#define getchr() fgetc(stdin)
#define putchr(c) fputc((c), stdout)

/* Assorted macros */

#ifndef MIN
#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define MAX(x, y) (((x) < (y)) ? (y) : (x))
#endif

/* Swap two variable */
#define swap(a, b) ((a) ^= (b); (b) ^= (a); (a) ^= (b))

/* Euclidean distance */
#define Distsq(x1, y1, x2, y2) \
    (((x1) - (x2)) * ((x1) - (x2)) + ((y1) - (y2)) * ((y1) - (y2)))

#ifdef COLLECTIVE_MONEY
#define MONEY(r0, g0) (r0->governor[0].money)

#else

#define MONEY(r0, g0) (r0->governor[(unsigned)g0].money)
#endif

/* Look up sector */
#define Sector(pl, x, y) (Smap[(x) + ((y) * (pl).Maxx)])

/* Adjust temperature to be displayed */
#define Temp(x) ((int)(x))

/* Number of AP's to add to each player in ea. system (look in doturn) */
#define GLOBAL_LIMIT_APS 1023 /* Max # of AP's you can have global */
#define LIMIT_APS 255 /* Max # of AP's you can have */

#define TYPE_EARTH 0
#define TYPE_ASTEROID 1
#define TYPE_MARS 2
#define TYPE_ICEBALL 3
#define TYPE_GASGIANT 4
#define TYPE_WATER 5
#define TYPE_FOREST 6
#define TYPE_DESERT 7
#define TYPE_WORMHOLE 8
#define TYPE_SWAMP 9 /* Not used yet */

/* Number of global AP's each planet is worth */
#define EARTH_POINTS int_rand(5, 8)
#define ASTEROID_POINTS 1
#define MARS_POINTS int_rand(2, 3)
#define ICEBALL_POINTS int_rand(2, 3)
#define GASGIANT_POINTS int_rand(8, 20)
#define WATER_POINTS int_rand(2, 3)
#define FOREST_POINTS int_rand(2, 3)
#define DESERT_POINTS int_rand(2, 3)

/* Numbers used for VP's... */
#define VP_EARTH 5
#define VP_ASTEROID 0
#define VP_MARS 5
#define VP_ICE 5
#define VP_GAS 5
#define VP_WATER 5
#define VP_FOREST 5
#define VP_DESERT 5
#define VP_SWAMP 5 /* Not used yet */

#define SEA 0
#define LAND 1
#define MOUNT 2
#define GAS 3
#define ICE 4
#define FOREST 5
#define DESERT 6
#define PLATED 7
#define WASTED 8
#define WORM 9
#define SWAMP 10 /* Not used yet */

#define CIV 0
#define MIL 1

#define MAX_SECT_POPN 32767

#define TOXMAX 20 /* Max a toxwc can hold */
#define TOXCAN_LEAK 5 /* % chance that a toxwc leaks per update (cumulative) */

/* Signals to block... */
#define SIGBLOCKS (SIGHUP|SIGTERM|SIGINT|SIGQUIT|SIGSTOP|SIGTSTP)

#define Bzero(x) memset((char *)&(x), 0, sizeof(x))
#define Malloc(x) (x *)malloc(sizeof(x))

/* For compiling on NeXt machines */
/* #define _STDLIB_H */

#define RESOURCE 0 /* For market */
#define DESTRUCT 1
#define FUEL 2
#define CRYSTAL 3

#define MERCHANT_LENGTH 200000.0
#define INCOME_FACTOR 0.002
#define INSURG_FACTOR 1
#define UP_BID 0.10

#define GUN_COST 1.00
#define CREW_COST 0.05
#define CARGO_COST 0.05
#define FUEL_COST 0.05
#define AMMO_COST 0.05
#define SPEED_COST 0.50
#define CEW_COST 0.003
#define ARMOR_COST 3.50
#define HANGER_COST 0.50
#define AFV_FUEL_COST 1.0

#define MECH_ATTACK 3.0 /* HAP = 4.0 */

#define VICT_SECT 1000 /* HAP = 200 */
#define VICT_SHIP 333 /* HAP = 100 */
#define VICT_TECH 0.10
#define VICT_MORALE 200 /* HAP = 100 */
#define VICT_RES 100 /* HAP = 75 */
#define VICT_FUEL 15 /* HAP = 100 */
#define VICT_MONEY 5
#define VICT_DIVISOR 10000

#define MAX_NOVA 16 /* Number of updates a star will stay in nova_stage -mfw */

#define STRIKE_DISTANCE_FACTOR 5.5
/* Determines steepness of design complexity function */
#define COMPLEXITY_FACTOR 10.0

/* This character makes the previous command to repeat */
#define REPEAT_CHARACTER ' '

#define MAXGOVERNORS 5
#define POD_THREHOLD 18
#define POD_DECAY 4

/* Hoe planet size affects the rate of atmosphere processing */
#define AP_FACTOR 50.0
#define DISSIPATE 80 /* Updates to dissipate dust and gases */

#define FIRST_USER_SHIP 100
/* Maximum ship number -- should fit in unsigned short until code is changed */
#define SHIP_NUM_LIMIT 0xFFFF

#define MAXFLEETS 26 /* Gives us A-Z fleets per governor -mfw */
#define FLEET_NAMESIZE 18 /* String length of fleet name -mfw */

/* Garble code from HAP server -mfw */
#define USE_GARBLEFILE 1 /* Should words be substituted during TRANS_CHAT? */

/*
 * Defines the uniform length of words in GARBLEFILE. You must run the perl
 * script makewords over the GARBLEFILE file if you want to use this option. It
 * speeds up access to the GARBLEFILE file by allowing direct access to words,
 * rather than sequential access. If 0, not used.
 */
#define UNIFORM_WORDS 20

#define UNIFORM_FILLER '%' /* Filler character to exclude */

/* SETI code from HAP -mfw */
#define SETI_FUEL_CONSUMPTION 1.0
#define SETI_RESOURCE_CONSUMPTION 0.0
#define SETI_SEE_FACTOR 2.0 /* higher number increases set detection rate */

#define RES_COST_REFINERY 1.0

/* Define this if you will allow players to name their own stars (untested) */
#undef NAME_STARS

/* Number of update after obsolescense starts to occur */
#define SHIP_TOO_OLD 50

/* Upgraded ships receive a bit of damage (will need repair) */
#define DAMAGE_TO_UPGRADE 1

/*
 * Production factors, there are multipliers to alter resource production see
 * dosector.c, produce() to see how these function
 */
#define FUEL_PROD_FACTOR 1.0 /* Multiplier for the quantity of fuel produced */
#define DEST_PROD_FACTOR 1.0 /* Modifies chance of des or res produced */

/*
 * This define makes learning new technology not so automatic. The higher the
 * number, the less chance of learning. Good values: 5, very fast, 10 fast, 15
 * slow, 20 very slow, 25 crawling. If 0, tech is learned as normal.
 */
#define TECH_LEARN_FACTOR 0

#define DECLARE_KNOW_GAIN 15 /* % know gained from declare */
#define DECLARE_MAX_KNOW 80 /* Maximum know gained from declare */
#define BATTLE_KNOW_GAIN 2 /* % know gained from battles, etc. */
#define BATTLE_MAX_KNOW 70 /* Maximum % know gained from battle */

#endif /* TWEAKABLES_H_ */
