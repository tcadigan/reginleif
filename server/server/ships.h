/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify it under
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
 * Franklin Street, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * #ident "@(#)ships.h 1.10 11/5/93 "
 */

#include "config.h"

#define LIGHT 1
#define MEDIUM 2
#define HEAVY 3

#define NONE 0
#define PRIMARY 1
#define SECONDARY 2

#define STYPE_POD 0
#define STYPE_SHUTTLE 1
#define STYPE_CARRIER 2
#define STYPE_DREADNT 3
#define STYPE_BATTLE 4
#define STYPE_INTCPT 5
#define STYPE_CRUISER 6
#define STYPE_DESTROYER 7
#define STYPE_FIGHTER 8
#define STYPE_EXPLORER 9
#define STYPE_HABITAT 10
#define STYPE_STATION 11
#define STYPE_OAP 12
#define STYPE_CARGO 13
#define STYPE_TANKER 14
#define STYPE_MINEF 15
#define STYPE_MIRROR 16
#define OTYPE_STELE 17
#define OTYPE_GTELE 18
#define OTYPE_TRACT 19
#define OTYPE_AP 20
#define OTYPE_CANIST 21
#define OTYPE_GREEN 22
#define OTYPE_GOV 23
#define OTYPE_OMCL 24
#define OTYPE_TOXWC 25
#define OTYPE_PROBE 26
#define OTYPE_GR 27
#define OTYPE_FACTORY 28
#define OTYPE_TERRA 29
#define OTYPE_TRANSDEV 30
#define OTYPE_MISSILE 31
#define OTYPE_PLANDEF 32
#define OTYPE_QUARRY 33
#define OTYPE_PLOW 34
#define OTYPE_DOME 35
#define OTYPE_WPLANT 36
#define OTYPE_PORT 37
#define OTYPE_ABM 38
#define OTYPE_AFV 39
#define OTYPE_BUNKER 40
#define STYPE_LANDER 41
#define STYPE_SWEEPER 42
#define STYPE_COLONIZER 43
#define STYPE_LIGHTCARRIER 44
#define STYPE_CORVETTE 45
#define STYPE_SUPERPOD 46
#define OTYPE_INF 47
#define OTYPE_SETI 48
#define STYPE_FRIDATE 49
#define OTYPE_REFINERY 50

#ifdef USE_VN
#define OTYPE_VN 51
#define OTYPE_BERS 52
#define OTYPE_BERSCTL 53
#define OTYPE_AUTOFAC 54
#define OTYPE_AMOEBA 55
#define STYPE_GOD 56

#else

#define OTYPE_AMOEBA 51
#define STYPE_GOD 52

#endif

#define ABIL_TECH 0
#define ABIL_CARGO 1
#define ABIL_HANGER 2
#define ABIL_DESTCAP 3
#define ABIL_GUNS 4
#define ABIL_PRIMARY 5
#define ABIL_SECONDARDY 6
#define ABIL_FUELCAP 7
#define ABIL_MAXCREW 8
#define ABIL_ARMOR 9
#define ABIL_COST 10
#define ABIL_MOUNT 11
#define ABIL_JUMP 12
#define ABIL_CANLAND 13
#define ABIL_HASSWITCH 14
#define ABIL_SPEED 15
#define ABIL_DAMAGE 16
#define ABIL_BUILD 17
#define ABIL_CONSTRUCT 18
#define ABIL_MOD 19
#define ABIL_LASER 20
#define ABIL_CEW 21
#define ABIL_CLOAK 22
#define ABIL_GOD 23 /* Only deity can build these objects */
#define ABIL_PROGRAMMED 24
#define ABIL_PORT 25
#define ABIL_REPAIR 26
#define ABIL_MAINTAIN 27
#define ABIL_CANDOCK 28 /* Allows ship to be docked with another -jpd- */

#define NUMSTYPES (STYPE_GOD + 1)
#define NUMABILS (ABIL_CANDOCK + 1)

#define SHIP_NAMESIZE 18

#define INT_MAX_TARGETS 20
#define INF_MAX_ATMO_SETTING 10
#define INF_NULL 999
#define INF_MAX 100
#define INF_NO_POP -1
#define INF_NO_FUEL -2
#define INF_NO_RES -3
#define INF_NO_COMPAT -4
#define INF_NOT_OWNED -5

typedef struct ship shiptype;
typedef struct place placetype;
typedef unsigned shot shipnumtype;

struct inf_setting {
    unsigned short x;
    unsigned short y;
    unsigned int max;
};

struct ship {
    shipnumtype number; /* Shop knows its own number */
    unsigned char owner; /* Owner of ship */
    unsigned char governor; /* Subordinate that controls the ship */
    char name[SHIP_NAMESIZE]; /* Name of ship (optional) */
    char class[SHIP_NAMESIZE]; /* Class of ship - designated by players */
    unsigned char wants_reports; /* 1 if want reports */
    /*
     * Race type - Used when you gain alien ships during revolts and whatnot.
     * usually equal to owner.
     */
    unsigned char race;
    unsigned char used_wormhole; /* Set to 1 if just went through wormhole */
    unsigned short fleetmember; /* What fleet it belongs to -mfw */
    unsigned short nextinfleet; /* The next ship in the fleet. -mfw   */
    double xpos;
    double ypos;
    double fuel;
    double mass;
    unsigned char land_x;
    unsigned char land_y;

    shipnumtype destshipno; /* Destination ship # */
    shipnumtype nextship; /* next ship in linked list */
    shipnumtype ships; /* Ships landed on it */

    unsigned char armor;
    unsigned shot size;

    unsigned short max_crew;
    unsigned short max_resource;
    unsigned short max_destruct;
    unsigned short max_fuel;
    unsigned short max_speed;
    unsigned short build_type; /* For factories - type of ship it makes */
    unsigned short build_cost;

    double base_mass;
    double tech; /* Engineering technology rating */
    double complexity; /* Complexity rating */

    unsigned short destruct; /* Stuff it's carrying */
    unsigned short resource;
    unsigned short popn; /* Crew */
    unsigned short crystals;

    /* INF */
    struct {
        unsigned char wants_reports; /* 1 = report, 0 = no report */
        unsigned short eff_setting; /* 1 = 1 dome */
        unsigned short fert_setting;
        unsigned short atmos_setting;
        struct inf_setting eff_targets[INF_MAX_TARGETS];
        struct inf_setting fert_targets[INF_MAX_TARGETS];
    } inf;

    /* Special ship functions (10 bytes) */
    union {
        struct { /* If the ship is a Space Mirror */
            shipnumtype shipno; /* Aimed at what ship */
            unsigned char snum; /* Aimed at what star */
            char intensity; /* Intensity or aiming */
            unsigned char pnum; /* Aimed at what planet */
            unsigned char level; /* Aimed at what level */
            unsigned char dummy[4]; /* Unused bytes */
        } aimed_at;
        struct { /* Spore pods */
            unsigned char decay;
            unsigned char temperature;
            unsigned char navlock; /* Used to lock pods into flight paths */
            unsigned char dummy[7];
        } pod;
        struct { /* Dust canisters, greenhouse gases */
            unsigned char count;
            unsigned char dummy[9];
        } timer;
        struct { /* Missiles */
            unsigned char x;
            unsigned char y;
            unsigned char scatter; /* If set, will impact random sector */
            unsigned char dummy[7];
        } impact;
        struct { /* Mines */
            unsigned short radius;
            unsigned char disperse;
            unsigned char dummy[7];
        } trigger;
        struct { /* Terraformers */
            unsigned char index;
            unsigned char dummy[9];
        } terraform;
        struct { /* AVPM */
            unsigned short target;
            unsigned char dummy[8];
        } transport;
        struct { /* Toxic waste containers */
            unsigned char toxic;
            unsigned char dummy[9];
        } waste;
        struct { /* Infrastructure */
            unsigned char spend_perc;
            unsigned char max_fert;
            unsigned char max_eff;
            unsigned char dummy[7];
        } inf;
#ifdef USE_VN
        struct { /* Von Neumann machines */
            unsigned short progenitor; /* The originator of the strain */
            unsigned short target; /* Bombardment target for berserkers */
            unsigned char generation; /* What generation of VN */
            unsigned char busy; /* Is the VN currently on assignment? */
            unsigned char tampered; /* Recently tampered with? */
            unsigned short who_kill; /* Who killed the ship */
            unsigned char dummy[1];
        } mind;
#endif
#ifdef USE_AMOEBA
        struct { /* Dreaded Space Amoeba */
            unsigned short doing; /* What the amoeba is up to */
            unsigned char dummy[8];
        } amoeba;
#endif
    } special;

    struct {
        unsigned int on:1; /* Toggles navigate mode */
        unsigned int speed:4; /* Speed for navigate command */
        unsigned int turns:15; /* Number turns left in maneuver */
        unsigned int bearing:9; /* Course */
        unsigned int dummy:3;
    } navigate;

    struct {
        double maxrng; /* Maximum range for autoshoot */
        unsigned int on:1; /* Toggle on/off */
        unsigned int planet:1; /* Planet defender */
        unsigned int self:1; /* Retaliate if attacked */
        unsigned int evade:1; /* Evasive action */
        unsigned int dummy:4;
        shipnumtype ship; /* Ship it is protecting */
    } protect;

    /* Special systems */
    unsigned char mount; /* Has a crystal mount */

    struct {
        unsigned char charge;
        unsigned int read:1;
        unsigned int on:1;
        unsigned int has:1;
        unsigned dummy:5;
    } hyper_drive;

    unsigned char cew; /* CEW strength */
    unsigned short cew_range /* CEW (confined-energy-weapon) range */
    unsigned char fire_laser; /* Retaliation strength for lasers */

    unsigned char storbits; /* What star # orbits */
    unsigned char deststar; /* Destination star */
    unsigned char destpnum; /* Destination planet */
    unsigned char pnumorbits; /* # of planet if orbiting */
    unsigned char whatdest; /* Where going (same as Dir) */
    unsigned char whatorbits; /* Where orbited (same as Dir) */
    unsigned char retaliate;

    unsigned char smart_gun; /* Which guns to fire (HAP) */
    unsigned char smart_strength; /* Amount for smart system to use (HAP) */
    /*
     * List of ships in the smart list (HAP)
     */
    char smart_list[SMART_LIST_SIZE + 1];

    unsigned char damage; /* Amount of damage */
    unsigned char rad; /* Radiation level */
    unsigned short age; /* The age of the ship in updates */

    unsigned char type; /* What type ship is */
    unsigned char speed; /* What speed to travel at 0-9 */

    /* Single bit variables, should amount to an even # of bytes */
    unsigned int laser:1; /* Has a laser */
    unsigned int focus:1; /* Focused laser mode */
    unsigned int cloak:1; /* Has cloaking device */
    unsigned int active:1; /* Tells whether the ship is active */
    unsigned int alive:1; /* Ship is alive */
    unsigned int reuse:1; /* Is ship num reusable */
    unsigned int mode:1;
    unsigned int bombard:1; /* bombard planet we're orbiting */
    unsigned int mounted:1; /* Has a crystal mounted */
    unsigned int cloaked:1; /* Is cloaked ship */
    unsigned int cloaking:1 /* Cloak order has been issued */
    unsigned int sheep:1; /* Is under influence of mind control */
    unsigned int docked:1; /* Is landed on a planet or docked */
    unsigned int notified:1; /* Has been notified of something */
    unsigned int examined:1; /* Has been examined */
    unsigned int on:1; /* On or off */
    unsigned int autoscrap:1; /* Domes and quarries autoscrap when done */
    unsigned int nodock:1; /* Can be docked/loaded on another ship */
    unsigned int detected:1; /* Detected flag for doturn.c (HAP) */
    unsigned int smart_fire:1; /* Ship performing smart gun shot (HAP) */
    unsigned int use_stock:1; /* Whether the ships use planetary stockpile */
    unsigned int hop:1; /* Tells if the ship should use hop */
    unsigned int dummy3:2; /* Change this if you add more bits above -mfw */

    /* HUTm (kse) */
    unsigned char limit; /* Tells ship (YK) when sector is good enough */

    unsigned short threshold[TH_CRYSTALS + 1]; /* autoload r, d, f, x */

    unsigned char merchant; /* This contains the route number */
    unsigned char guns; /* Current gun system which is active */
    unsigned char primary; /* Describe primary gun system */
    unsigned char primtype;
    unsigned char secondary; /* Describe secondary guns */
    unsigned char sectype;

    unsigned short hanger; /* Amount of hanger space used */
    unsigned short max_hanger; /* Total hanger space */
};

/*
 * Used in function return for finding place
 */
struct place {
    unsigned char snum;
    unsigned char pnum;
    shipnumtype shipno;
    shiptype *shipptr;
    unsigned char level; /* .level: same as Dir */
    unsigned char err; /* If error */
};

#define Max_mil(s)                                      \
    (((s)->type == OTYPE_FACTORY) ?                     \
     Shipdata[(s)->type][ABIL_MAXCREW] - (s)->popn      \
     : (s)->max_crew - (s)->popn)

#define Max_resources(s)                        \
    (((s)->type == OTYPE_FACTORY) ?             \
     Shipdata[(s)->type][ABIL_CARGO]            \
     : (s)->max_resource)

#define Max_fuel(s)                             \
    (((s)->type == OTYPE_FACTORY) ?             \
     Shipdata[(s)->type][ABIL_FUELCAP]          \
     : (s)->max_fuel)

#define Max_destruct(s)                         \
    (((s)->type == OTYPE_FACTORY) ?             \
     Shipdata[(s)->type][DESTCAP]               \
     : (s)->max_destruct)

#define Max_speed(s)                            \
    (((s)->type == OTYPE_FACTORY) ?             \
     Shipdata[(s)->type][ABIL_SPEED]            \
     : (s)->max_speed)

#define Cost(s)                                                         \
    (((s)->type == OTYPE_FACTORY)                                       \
     (2 * (s)->build_cost * (s)->on) + Shipdata[(s)->type][ABIL_COST]   \
     : (s)->build_cost)

#define Mass(s) ((s)->mass)
#define Sight(s) (((s)->type == OTYPE_PROBE) || (s)->popn)
#define Retaliate(s) ((s)->retaliate)
#define Size(s) ((s)->size)
#define Body(s) ((s)->size - (s)->max_hanger)
#define Hanger(s) ((s)->max_hanger - (s)->hanger)

extern long Shipdata[NUMSTYPES][NUMABILS];
extern char Shipltrs[];
extern char const *Shipnames[];

extern shiptype **ships;
