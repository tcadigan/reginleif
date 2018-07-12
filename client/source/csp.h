/*
 * csp.h: Contains the defines for all the Client-Server Protocol (CSP) numbers.
 *
 * CSP, copyright (c) 1993 by John P. Deregon, Evan Koffler
 *
 * #ident  "@(#)cps.h   1.1 11/20/93 "
 *
 * Please send any modifications of this file to:
 *    evank@netcom.com
 *    deragon@jethro.nyu.edu
 */
#ifndef CSP_H_
#define CSP_H_

#include "types.h"

void process_special(char *s);
int cspr_qsort_cmp(void const *a, void const *b);
int csps_qsort_cmp(void const *a, void const *b);
void init_csp(void);
void cspr_backup_end(int cnum, char *line);
void cspr_update_end(int cnum, char *line);
void cspr_segment_end(int cnum, char *line);
void cspr_map(int cnum, char *line);
char *get_map_info_buf(int cnt, Pmap *map);
void cspr_ping(int cnum, char *line);
void cspr_pause(int cnum, char *line);
void cspr_survey(int cnum, char *line);
void cspr_updates_suspended(int cnum, char *line);
void cspr_updates_resumed(int cnum, char *line);
void cspr_knowledge(int cnum, char *line);
void cspr_err(int cnum, char *line);
CSPSendVal *csps_binary_search(int cnum);
CSPReceiveVal *cspr_binary_search(int cnum);
void waitFor(char *buf, int lo, int hi);
int csp_send_request(int comm_num, char *buf);
void cspr_profile(int cnum, char *line);
void csp_profile_output(Profile *prof);
void cspr_relation(int cnum, char *line);
void cspr_print(int cnum, char *line);
void cspr_client_on(int cnum, char *line);
void cspr_client_off(int cnum, char *line);
void cspr_event(int cnum, char *line);
void cspr_scope_prompt(int cnum, char *line);
void cspr_explore(int cnum, char *line);
void cspr_who(int cnum, char *line);
char *csps_relation(char *s);
char *csps_knowledge(char *s);
char *csps_login(char *s);
char *csps_map(char *s);
char *csps_ping(char *s);
char *csps_survey(char *s);
char *csps_null(char *s);
void csp_msg(char *fmt, ...);
void cspr_orbit(int cnum, char *line);
void plot_orbit_object(void);
void orbit_info_box(void);

extern Orbit orbit;

/*---- Server responses ----*/

/* VERSION */
#define CSP_VERSION_INFO    10 /* Version XX */
#define CSP_VERSION_OPTIONS 11 /* Options set YY */

/* LOGIN */
#define CSP_CLIENT_ON  30 /* Client mode on */
#define CSP_CLIENT_OFF 31 /* Client mode off */
#define CSP_KNOWLEDGE  32

/* INFO */
#define CSP_SCOPE_PROMPT 40

/* UPDATE/SEGMENT/RESET */
#define CSP_UPDATE_START      50 /* Update started */
#define CSP_UPDATE_END        51 /* Update finished */
#define CSP_SEGMENT_START     52 /* Segment started */
#define CSP_SEGMENT_END       53 /* Segment finished */
#define CSP_RESET_START       54 /* Reset started */
#define CSP_RESET_END         55 /* Reset finished */
#define CSP_BACKUP_START      56 /* Backup started */
#define CSP_BACKUP_END        57 /* Backup finished */
#define CSP_UPDATES_SUSPENDED 58 /* Updates suspended */
#define CSP_UPDATES_RESUMED   59 /* Updates resumed */
#define CSP_PING              60 /* Ping pong */
#define CSP_PAUSE             61 /* Pause display */

/* SURVEY */
#define CSP_SURVEY_INTRO  101 /* Planet info */
#define CSP_SURVEY_SECTOR 102 /* Sector info */
#define CSP_SURVEY_END    103 /* End of command (EOC) */

/* RELATION */
#define CSP_RELATION_INTRO 201 /* Race # and name */
#define CSP_RELATION_DATA  202 /* The data */
#define CSP_RELATION_END   203 /* End of command (EOC) */

/*
 * PROFILE
 * DYNAMIC = Active Knowledge Capitol Morale Gun GTele STele
 * DYNAMIC_OTHER = %Know Morale Gun GTele OTele SecPref
 */
#define CSP_PROFILE_INTRO         301 /* Header */
#define CSP_PROFILE_PERSONAL      302
#define CSP_PROFILE_DYNAMIC       303
#define CSP_PROFILE_DYNAMIC_OTHER 304
#define CSP_PROFILE_RACE_STATS    305
#define CSP_PROFILE_PLANET        306
#define CSP_PROFILE_SECTOR        307
#define CSP_PROFILE_DISCOVERY     308 /* Discoveries */
#define CSP_PROFILE_END           309

/* WHO */
#define CSP_WHO_INTRO   401 /* Header */
#define CSP_WHO_DATA    402 /* Actual data */
#define CSP_WHO_COWARDS 403 /* Send either # of cowards or WHO_END as terminator */
#define CSP_WHO_END     403 /* Send either # of cowards or WHO_END as terminator */

/*
 * EXPLORE
 * STAR_DATA = # Name Ex Inhab Auto Slaved Toxic Compat Type
 */
#define CSP_EXPLORE_INTRO       501
#define CSP_EXPLORE_STAR        502
#define CSP_EXPLORE_STAR_ALIENS 503
#define CSP_EXPLORE_STAR_DATA   504
#define CSP_EXPLORE_STAR_END    505
#define CSP_EXPLORE_END         506

/*
 * MAP
 * DYNAMIC_1 = Type Sects Guns MobPoints Res Des Fuel Xtals
 * DYNAMIC_2 = Mob AMob Compat Pop ^Pop ^TPop Mil Tax ATax Deposits Est Prod
 */
#define CSP_MAP_INTRO     601
#define CSP_MAP_DYNAMIC_1 602
#define CSP_MAP_DYNAMIC_2 603
#define CSP_MAP_ALIENS    604
#define CSP_MAP_DATA      605
#define CSP_MAP_END       606

/* CLIENT GENERATED COMMANDS */
#define CSP_LOGIN_COMMAND    1101 /* Login command */
#define CSP_VERSION_COMMAND  1102 /* Version command */
#define CSP_SURVEY_COMMAND   1103 /* Imap command */
#define CSP_RELATION_COMMAND 1104 /* Relation command */
#define CSP_PROFILE_COMMAND  1105 /* Profile command */
#define CSP_WHO_COMMAND      1106 /* Who command */
#define CSP_EXPLORE_COMMAND  1107 /* Exploration command */
#define CSP_MAP_COMMAND      1108 /* Map command */
#define CSP_SCOPE_COMMAND    1110 /* Request a prompt */
#define CSP_SHIPLIST_COMMAND 1111 /* Request ship list */
#define CSP_STARDUMP_COMMAND 1112 /* Request star dump */
#define CSP_SECTORS_COMMAND  1113 /* Request sector count */
#define CSP_UNIVDUMP_COMMAND 1114 /* Request a universe dump */

/*---- Error Responses ----*/
#define CSP_ERR                 9900 /* Error */
#define CSP_ERR_TOO_MANY_ARGS   9901 /* Too many args */
#define CSP_ERR_TOO_FEW_ARGS    9902 /* Too few args */
#define CSP_ERR_UNKNOWN_COMMAND 9903 /* Unknown command */
#define CSP_ERR_NOSUCH_PLAYER   9904 /* No such player */
#define CSP_ERR_NOSUCH_PLACE    9905 /* No such place - scope error */

#define CSP_MAX_SERVER_COMMAND 1110

/* Do we need these at all? I do not! */
#define RACE_TYPE_UNKNOWN  0
#define RACE_TYPE_MORPH    1
#define RACE_TYPE_NORMAL   2
#define RELAT_UNKNOWN      0
#define RELAT_ALLIED       1
#define RELAT_NEUTRAL      2
#define RELAT_WAR          3
#define CSP_ZOOM          35

/* Broadcast/Think/Announce/Shout */
#define CSP_BROADCAST 802 /* Broadcast */
#define CSP_ANNOUNCE  803 /* Announce */
#define CSP_THINK     804 /* Think */
#define CSP_SHOUT     805 /* Shout */
#define CSP_EMOTE     805 /* Emote */

#define CSP_ORBIT_COMMAND    1501 /* Orbit command */
#define CSP_ZOOM_COMMAND     1503 /* Zoom command */
#define CSP_PLANDUMP_COMMAND 1503 /* Planet dump command */
#define CSP_SHIPDUMP_COMMAND 1504 /* Ship dump command */

/* Planet Dumps */
#define CSP_PLANDUMP_INTRO      2000 /* Planet name */
#define CSP_PLANDUMP_CONDITIONS 2001 /* Conditions */
#define CSP_PLANDUMP_STOCK      2002 /* Stockpiles */
#define CSP_PLANDUMP_PROD       2003 /* Production last update */
#define CSP_PLANDUMP_MISC       2004 /* Rest of stuff */
#define CSP_PLANDUMP_NOEXPL     2005 /* Planet not explored */
#define CSP_PLANDUMP_END        2009 /* Sent when done */

/* General usage */
#define CSP_START_ENEXPL 2010 /* Star is not explored */

/* ORBIT */
#define CSP_ORBIT_OUTPUT_INTRO  2020 /* Orbit parameters */
#define CSP_ORBIT_STAR_DATA     2021 /* Star info */
#define CSP_ORBIT_UNEXP_PL_DATA 2022 /* Unexplored planet info */
#define CSP_ORBIT_EXP_PL_DATA   2023 /* Explored planet info */
#define CSP_ORBIT_SHIP_DATA     2024 /* Ship info */
#define CSP_ORBIT_OUTPUT_END    2025 /* End of command (EOC) */

/* Ship dumps */
#define CSP_SHIPDUMP_GEN         2030 /* General information */
#define CSP_SHIPDUMP_STOCK       2031 /* Stock information */
#define CSP_SHIPDUMP_STATUS      2032 /* Status information */
#define CSP_SHIPDUMP_WEAPONS     2033 /* Weapons information */
#define CSP_SHIPDUMP_FACTORY     2034 /* Factory information */
#define CSP_SHIPDUMP_DEST        2035 /* Destination information */
#define CSP_SHIPDUMP_PTACT_GEN   2036 /* General planet tactical */
#define CSP_SHIPDUMP_PTACT_PDIST 2037 /* Distance between planets */
#define CSP_SHIPDUMP_STACT_PDIST 2038 /* Distance between a ship */
#define CSP_SHIPDUMP_PTACT_INFO  2039 /* For a ship from a planet */
#define CSP_SHIPDUMP_STACT_INFO  2040 /* For a ship from a ship */
#define CSP_SHIPDUMP_ORDERS      2041 /* Ship orders */
#define CSP_SHIPDUMP_THRESH      2042 /* Ship threshloading */
#define CSP_SHIPDUMP_SPECIAL     2043 /* Ship specials */
#define CSP_SHIPDUMP_HYPER       2044 /* Hyper drive usage */
#define CSP_SHIPDUMP_END         2055 /* End of command (EOC) */

/* Ship list */
#define CSP_SHIPLIST_INTRO 3000 /* General info */
#define CSP_SHIPLIST_DATA  3001 /* A shipno - this will repeat for each ship in star */
#define CSP_SHIPLIST_END   3002 /* Done. In case client is waiting for a signal */

#define CSP_STARDUMP_INTRO     4000
#define CSP_STARDUMP_CONDITION 4001
#define CSP_STARDUMP_PLANET    4002
#define CSP_STARDUMP_END       4003
#define CSP_STARDUMP_WORMHOLE  4004

#define CSP_UNIVDUMP_INTRO 4010
#define CSP_UNIVDUMP_STAR  4011
#define CSP_UNIVDUMP_END   4012

#define CSP_SECTORS_INTRO 4100
#define CSP_SECTORS_END   4101

#endif // CSP_H_
