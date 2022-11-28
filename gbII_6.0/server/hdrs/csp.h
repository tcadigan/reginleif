/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistributed it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Stree, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contains in the file LICENSE.
 *------------------------------------------------------------------------------
 *
 * csp.h: Contains the defines for all the Client-Server Protocal (CSP) numbers.
 *
 * CSP, copyright (c) 1993 by John P. Deragon, Evan Koffler
 *
 * #ident  "@(#)csp.h   1.9 12/7/93 "
 *
 * Please send any modifications of this file to:
 *   evank@netcom.com
 *   deragon@jethro.nyu.edu
 */

/* ----- Server responses ----- */

/* VERSION */
#define CSP_VERSION_INFO 10 /* Version XX */
#define CSP_VERSION_OPTIONS 11 /* Options set YY */

/* LOGIN */
#define CSP_CLIENT_ON 30 /* Client mode on */
#define CSP_CLIENT_OFF 31 /* Client mode off */
#define CSP_KNOWLEDGE 32
#define CSP_ZOOM 35

/* INFO */
#define CSP_SCOPE_PROMPT 40

/* UPDATE/SEGMENT/RESET */
#define CSP_UPDATE_START 50 /* Update started */
#define CSP_UPDATE_END 51 /* Update finished */
#define CSP_SEGMENT_START 52 /* Segment started */
#define CSP_SEGMENT_END 53 /* Segment finished */
#define CSP_RESET_START 54 /* Reset started */
#define CSP_RESET_END 55 /* Reset finished */
#define CSP_BACKUP_START 56 /* Backup started */
#define CSP_BACKUP_END 57 /* Backup finished */
#define CSP_UPDATES_SUSPENDED 58 /* Updates suspended */
#define CSP_UPDATES_RESUMED 59 /* Updates resumed */

/* CLIENT CONTROL */
#define CSP_PING 60 /* Ping pong */
#define CSP_PAUSE 61 /* Pause display -mfw */

/* SURVEY */
#define CSP_SURVEY_INTRO 101 /* Planet info */
#define CSP_SURVEY_SECTOR 102 /* Sector info */
#define CSP_SURVEY_END 103 /* End Of Command (EOC) */

/* RELATION */
#define CSP_RELATION_INTRO 201 /* Race # and name */
#define CSP_RELATION_DATA 202 /* The data */
#define CSP_RELATION_END 203 /* End Of Command (EOC) */

/*
 * PROFILE
 * DYNAMIC = Active Knowledge Capitol Morale Gun GTele Stele
 * DYNAMIC_OTHER = %Know Morale Gun GTele OTele SecPref
 */
#define CSP_PROFILE_INTRO 301 /* Header */
#define CSP_PROFILE_PERSONAL 302
#define CSP_PROFILE_DYNAMIC 303
#define CSP_PROFILE_DYNAMIC_OTHER 304
#define CSP_PROFILE_RACE_STATS 305
/* #define CSP_PROFILE_RACE_STATS_OTHER 306 */
#define CSP_PROFILE_PLANET 306
#define CSP_PROFILE_SECTOR 307
#define CSP_PROFILE_DISCOVERY 308 /* Discoveries */
#define CSP_PROFILE_END 309

/* WHO */
#define CSP_WHO_INTRO 401 /* Header */
#define CSP_WHO_DATA 402 /* Actual data */

/* Send either # of cowards or WHO_END as terminator */
#define CSP_WHO_COWARDS 403
#define CSP_WHO_END 403

/* EXPLORE */
/* STAR_DATA = # Name Ex Inhab Auto Slaved Toxic Comap #Sec Depo Xtal Type */
#define CSP_EXPLORE_INTRO 501
#define CSP_EXPLORE_STAR 502
#define CSP_EXPLORE_STAR_ALIENS 503
#define CSP_EXPLORE_STAR_DATA 504
#define CSP_EXPLORE_STAR_END 505
#define CSP_EXPLORE_END 506

/*
 * MAP
 * DYNAMIC_1 = Type Sects Guns MobPoints Res Des Fuel Xtals
 * DYNAMIC_2 = Mob AMob Compat Pop ^Pop ^TPop Mil Tax ATax Deposits Est Prod
 */
#define CSP_MAP_INTRO 601
#define CSP_MAP_DYNAMIC_1 602
#define CSP_MAP_DYNAMIC_2 603
#define CSP_MAP_ALIENS 604
#define CSP_MAP_DATA 605
#define CSP_MAP_END 606

/* CLIENT GENERATED COMMAND */
#define CSP_LOGIN_COMMAND 1101 /* Login command */
#define CSP_VERSION_COMMAND 1102 /* Version command */
#define CSP_SURVEY_COMMAND 1103 /* Imap command */
#define CSP_RELATION_COMMAND 1104 /* Relation command */
#define CSP_PROFILE_COMMAND 1105 /* Profile command */
#define CSP_WHO_COMMAND 1106 /* Who command */
#define CSP_EXPLORE_COMMAND 1107 /* Exploration command */
#define CSP_MAP_COMMAND 1108 /* Map command */
#define CSP_SCOPE_COMMAND 1110 /* Request a prompt */
#define CSP_SHIPLIST_COMMAND 1111 /* Request a ship list */
#define CSP_STARDUMP_COMMAND 1112 /* Request a star dump */
#define CSP_SECTORS_COMMAND 1113 /* Request a sector count */
#define CSP_UNIVDUMP_COMMAND 1114 /* Request a universe dump */

/* Dan Dickey for XGB *** NOT SUPPORTED FULLY *** */
#define CSP_ORBIT_COMMAND 1501 /* Orbit command */
#define CSP_ZOOM_COMMAND 1502 /* Zoom command */
#define CSP_PLANDUMP_COMMAND 1503 /* Planet dump command */
#define CSP_SHIPDUMP_COMMAND 1504 /* Ship dump command */

/* Planet dumps */
#define CSP_PLANDUMP_INTRO 2000 /* Planet name */
#define CSP_PLANDUMP_CONDITIONS 2001 /* Conditions */
#define CSP_PLANDUMP_STOCK 2002 /* Stockpiles */
#define CSP_PLANDUMP_PROD 2003 /* Production last update */
#define CSP_PLANDUMP_MISC 2004 /* Rest of stuff */
#define CSP_PLANDUMP_NOEXPL 2005 /* Planet not explored */
#define CSP_PLANDUMP_END 2009 /* Sent when done

/* General usage */
#define CSP_STAR_UNEXPL 2010 /* Star is not explored */

/* ORBIT */
#define CSP_ORBIT_OUTPUT_INTRO 2020 /* Orbit parameters */
#define CSP_ORBIT_STAR_DATA 2021 /* Star info */
#define CSP_ORBIT_UNEXP_PL_DATA 2022 /* Unexplored planet info */
#define CSP_ORBIT_EXP_PL_DATA 2023 /* Explored planet info */
#define CSP_ORBIT_SHIP_DATA 2024 /* Ship info */
#define CSP_ORBIT_OUTPUT_END 2025 /* End Of Communication (EOC) */

/* Ship dumps */
#define CSP_SHIPDUMP_GEN 2030 /* General information */
#define CSP_SHIPDUMP_STOCK 2031 /* Stock information */
#define CSP_SHIPDUMP_STATUS 2032 /* Status information */
#define CSP_SHIPDUMP_WEAPONS 2033 /* Weapons information */
#define CSP_SHIPDUMP_FACTORY 2034 /* Factory information */
#define CSP_SHIPDUMP_DEST 2035 /* Destination information */
#define CSP_SHIPDUMP_PTACT_GEN 2036 /* General planet tactical */
#define CSP_SHIPDUMP_PTACT_PDIST 2037 /* Distance between planets */
#define CSP_SHIPDUMP_STACT_PDIST 2038 /* Distance between a ship */
#define CSP_SHIPDUMP_PTACT_INFO 2039 /* For ship from a planet */
#define CSP_SHIPDUMP_STACT_INFO 2040 /* for a ship from a ship */
#define CSP_SHIPDUMP_ORDERS 2041 /* Ship orders */
#define CSP_SHIPDUMP_THRESH 2042 /* Ship thresholding */
#define CSP_SHIPDUMP_SPECIAL 2043 /* Ship specials */
#define CSP_SHIPDUMP_HYPER 2044 /* Hyper drive usage */
#define CSP_SHIPDUMP_END 2055 /* End Of Command (EOC) */

/* Ship list */
#define CSP_SHIPLIST_INTRO 3000 /* General info */
/* A shipno - this will repeat for each ship in star */
#define CSP_SHIPLIST_DATA 3001
/* Done. in case client is waiting for a signal */
#define CSP_SHIPLIST_END 3002

#define CSP_STARDUMP_INTRO 4000
#define CSP_STARDUMP_CONDITION 4001
#define CSP_STARDUMP_PLANET 4002
#define CSP_STARDUMP_END 4003
#define CSP_STARDUMP_WORMHOLE 4004

#define CSP_UNIVDUMP_INTRO 4010
#define CSP_UNIVDUMP_STAR 4011
#define CSP_UNIVDUMP_END 4012

#define CSP_SECTORS_INTRO 4100
#define CSP_SECTORS_END 4101

/* Dan Dickey for XGB *** NOT SUPPORTED FULLY *** */

/* ----- Error responses ----- */
#define CSP_ERROR 9000 /* Error */
#define CSP_ERR_TOO_MANY_ARGS 9901 /* Too many args */
#define CSP_ERR_TOO_FEW_ARGS 9902 /* Too few args */
#define CSP_ERR_UNKNOWN_COMMAND 9903 /* Unknown command */
#define CSP_ERR_NOSUCH_PLAYER 9904 /* No such player */
#define CSP_ERR_NOSUCH_PLACE 9905 /* No such place - scope err */

#define CSP_MAX_SERVER_COMMAND 9905
