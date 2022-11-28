/*
 * csp_types.h: Variables and defines used internally in CSP related functions
 *
 * CSP, copyright (c) 1993 by John P. Deragon, Evan Koffler
 *
 * Please send any modifications of this file to:
 *    evank@netcom.com
 *    deragon@jethro.nyu.edu
 *
 * #ident  "@(#)csp_types.h     i.1 11/20/93 "
 */

#ifndef CSP_TYPES_H_
#define CSP_TYPES_H_

/*
 * Character to be found in column 0 of a CSP output line send by server to
 * client
 */
#define CSP_CLIENT '|'

/* Identifier sent by client to server when sending a command/request */
#define CSP_SERVER "CSP"

/* Maximum depth of scope, i.e. nested ships */
#define CPSD_MAXSHIP_SCOPE 10
#define CPSD_NOSHIP         0

/* Maximum number of unique letters in star and planet names */
#define MAX_SCOPE_LTRS 4

/* Levels for sending the prompt in CSP_PROMPT */
enum LOCATION {
    CSPD_UNIV, /* 0 */
    CSPD_STAR, /* 1 */
    CSPD_PLAN, /* 2 */
    CSPD_LOCATION_UNKNOWN = 99
};

/* Used in CSP_PROFILE among other places */
enum RACE_TYPE {
    CSPD_RACE_UNKNOWN, /* 0 */
    CSPD_RACE_MORPH,   /* 1 */
    CSPD_RACE_NORMAL   /* 2 */
};

/* Used in CSP_PROFILE and CSP_RELATION */
enum RELATION {
    CSPD_RELAT_UNKNOWN, /* 0 */
    CSPD_RELAT_ALLIED,  /* 1 */
    CSPD_RELAT_NEUTRAL, /* 2 */
    CSPD_RELAT_WAR      /* 3 */
};

/* Used in CSP_PROFILE for discoveries */
enum DISCOVERIES {
    CSPD_HYPERDRIVE, /* 0 */
    CSPD_CRYSTAL,    /* 1 */
    CSPD_LASER,      /* 2 */
    CSPD_CEW,        /* 3 */
    CSPD_AVPM,       /* 4 */
    CSPD_MAX_NUM_DISCOVERY,
    CSPD_DISCOVERY_UNKNOWN = 99
};

enum PLAYER_TYPE {
    CSPD_NORMAL, /* 0 */
    CSPD_DEITY,  /* 1 */
    CSPD_GUEST   /* 2 */
};

enum SECTOR_TYPES {
    CSPD_SECTOR_SEA,
    CSPD_SECTOR_LAND,
    CSPD_SECTOR_MOUNT,
    CSPD_SECTOR_GAS,
    CSPD_SECTOR_ICE,
    CSPD_SECTOR_FOREST,
    CSPD_SECTOR_DESERT,
    CSPD_SECTOR_PLATED,
    CSPD_SECTOR_WASTED,
    CSPD_SECTOR_UNKNOWN
};

enum PLANET_TYPES {
    CSPD_PLANET_CLASS_M,
    CSPD_PLANET_ASTEROID,
    CSPD_PLANET_AIRLESS,
    CSPD_PLANET_ICEBALL,
    CPSD_PLANET_JOVIAN,
    CPSD_PLANET_WATERBALL,
    CSPD_PLANET_FOREST,
    CSPD_PLANET_DESERT
};

enum COMMUNICATION {
    CSPD_BROADCAST,
    CSPD_ANNOUNCE,
    CSPD_THINK,
    CSPD_SHOUT,
    CSPD_EMOTE
};

#endif /* CSP_TYPES_H_ */
