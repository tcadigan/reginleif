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
 * #ident "@(#)command.h 1.2 12/7/93 "
 *
 * $Header: /u/opi/32/rauta/gbdoc/HUTSERVER/return/hdrs/RCS/command.h,v 1.2
 * 1997/08/05 05:57:50 jkoi Exp $
 */

#include "proto.h"
#include "apcosts.h"

extern void voidpoint(void);

/*
 * For binary search
 */
typedef struct command_struct {
    const char *name;
    void (*func)();
    int god;
    int guest;
    int rank;
    int ap_cost;
    int args;
    int arg1;
    int arg2;
} Command;

#define NUM_COMMANDS (sizeof(command_list) / sizeof(Command))

/*
 * Command parsing table
 *
 * command function god guest rank ap arg? arg1, arg2
 */
static Command command_list[] = {
    /* God Commands */
    {      "@backup",          backup, 1, 0,  NOVICE,                0, 0, 0, 0},
    {       "@bless",           bless, 1, 0,  NOVICE,                0, 0, 0, 0},
    {        "@boot",       boot_user, 1, 0,  NOVICE,                0, 0, 0, 0},
    {       "@close",      close_game, 1, 0,  NOVICE,                0, 0, 0, 0},
    {       "@debug",        setdebug, 1, 0,  NOVICE,                0, 0, 0, 0},
    {       "@dinfo",   CSP_developer, 0, 1, CAPTAIN,            DINFO, 0, 0, 0},
    {     "@emulate",        _emulate, 1, 0,  NOVICE,                0, 0, 0, 0},
    {         "@fix",             fix, 1, 0,  NOVICE,                0, 0, 0, 0},
    {   "@freeships",       _freeship, 1, 0,  NOVICE,                0, 0, 0, 0},
    {      "@garble",     garble_chat, 1, 0,  NOVICE,                0, 0, 0, 0},
    {        "@last",      last_logip, 1, 0,  NOVICE,                0, 0, 0, 0},
    {        "@list",            list, 1, 0,  NOVICE,                0, 0, 0, 0},
    {       "@purge",           purge, 1, 0,  NOVICE,                0, 0, 0, 0},
    {       "@query",       CSP_query, 1, 0,  NOVICE,                0, 0, 0, 0},
    {       "@reset",          _reset, 1, 0,  NOVICE,                0, 0, 0, 0},
    {    "@schedule",      _sechedule, 1, 0,  NOVICE,                0, 0, 0, 0},
    {     "@segment",      do_segment, 1, 0,  NOVICE,                0, 1, 1, 0},
    {    "@shutdown",       shut_game, 1, 0,  NOVICE,                0, 0, 0, 0},
    {     "@suspend",         suspend, 1, 0,  NOVICE,                0, 0, 0, 0},
    {      "@update",       do_update, 1, 0,  NOVICE,                0, 0, 0, 0},
    {      "@uptime",     show_uptime, 1, 0,  NOVICE,                0, 0, 0, 0},
    {     "@vnbrain",        vn_brain, 1, 0,  NOVICE,                0, 0, 0, 0},
    {         "@who", dump_users_priv, 1, 0,  NOVICE,                0, 0, 0, 0},
    /* Player commands */
    {     "allocate",     allocateAPs, 0, 0, CAPTAIN,      ALLOCATE_AP, 0, 0, 0},
    {     "analysis",        analysis, 0, 0, PRIVATE,      ANALYSIS_AP, 0, 0, 0},
    {      "appoint",       governors, 0, 0, GENERAL,       APPOINT_AP, 0, 0, 0},
    {          "arm",             arm, 0, 0, CAPTAIN,           ARM_AP, 1, 1, 0},
    {      "assault",            dock, 0, 0, CAPTAIN,       ASSAULT_AP, 1, 1, 0},
    {   "autoreport",      autoreport, 0, 0, GENERAL,    AUTOREPORT_AP, 0, 0, 0},
    {          "bid",             bid, 0, 0, GENERAL,           BID_AP, 0, 0, 0},
    {       "blocks",           block, 0, 0, PRIVATE,        BLOCKS_AP, 0, 0, 0},
    {      "bombard",         bombard, 0, 1, CAPTAIN,       BOMBARD_AP, 0, 0, 0},
    {          "bug",             bug, 0, 0, PRIVATE,           BUG_AP, 0, 0, 0},
    {        "build",           build, 0, 0, CAPTAIN,         BUILD_AP, 0, 0, 0},
    {      "capital",         capital, 0, 0, GENERAL,       CAPITAL_AP, 0, 0, 0},
    {      "capture",         capture, 0, 0, CAPTAIN,       CAPTURE_AP, 0, 0, 0},
    {       "center",          center, 0, 0,  NOVICE,        CENTER_AP, 0, 0, 0},
    {          "cew",            fire, 0, 1, CAPTAIN,           CEW_AP, 1, 1, 0},
    {      "channel",         channel, 0, 0,  NOVICE,       CHANNEL_AP, 1, 0, 0},
    {"client_survey",          survey, 0, 0,  NOVICE, CLIENT_SURVEY_AP, 1, 1, 0},
    {     "colonies",        colonies, 0, 0, PRIVATE,      COLONIES_AP, 1, 0, 0},
    {           "cs",              cs, 0, 0,  NOVICE,            CS_AP, 0, 0, 0},
    {      "declare",         declare, 0, 0, GENERAL,       DECLARE_AP, 0, 0, 0},
    {       "defend",          defend, 0, 0, CAPTAIN,        DEFEND_AP, 0, 0, 0},
    {       "delete", delete_dispatch, 0, 0,  NOVICE,        DELETE_AP, 0, 0, 0},
    {       "demote",       governors, 0, 0, GENERAL,        DEMOTE_AP, 0, 0, 0},
    {       "deploy",       move_popn, 0, 1, CAPTAIN,        DEPLOY_AP, 0, 0, 0},
    {     "detonate",        detonate, 0, 1, CAPTAIN,      DETONATE_AP, 0, 0, 0},
    {       "disarm",             arm, 0, 1, CAPTAIN,        DISARM_AP, 1, 0, 0},
    {     "dismount",        dismount, 0, 1, CAPTAIN,      DISMOUNT_AP, 0, 0, 0},
    {     "dissolve",        dissolve, 0, 1,  LEADER,      DISSOLVE_AP, 0, 0, 0},
    {     "distance",        distance, 0, 0, PRIVATE,      DISTANCE_AP, 0, 0, 0},
    {         "dock",            dock, 0, 0, CAPTAIN,          DOCK_AP, 1, 0, 0},
    {         "dump",            dump, 0, 1, GENERAL,          DUMP_AP, 0, 0, 0},
    {      "enslave",         enslave, 0, 1, CAPTAIN,       ENSLAVE_AP, 0, 0, 0},
    {      "examine",         examine, 0, 0,  NOVICE,       EXAMINE_AP, 0, 0, 0},
    {      "explore",     exploration, 0, 1, PRIVATE,       EXPLORE_AP, 0, 0, 0},
    {    "factories",             rst, 0, 1,  NOVICE,     FACTORIES_AP, 1, 6, 0},
    {         "fire",            fire, 0, 1, CAPTAIN,          FIRE_AP, 1, 0, 0},
    {        "fleet",           fleet, 0, 1,  NOVICE,         FLEET_AP, 0, 0, 0},
    {         "fuel",       proj_fuel, 0, 1, CAPTAIN,          FUEL_AP, 0, 0, 0},
    {         "give",            give, 0, 1, GENERAL,          GIVE_AP, 0, 0, 0},
    {    "governors",       governors, 0, 0,  NOVICE,     GOVERNORS_AP, 0, 0, 0},
    {        "grant",           grant, 0, 0, CAPTAIN,         GRANT_AP, 0, 0, 0},
    {    "highlight",       highlight, 0, 0,  NOVICE,     GOVERNORS_AP, 0, 0, 0},
    {     "identify",           whois, 0, 0,  NOVICE,      IDENTIFY_AP, 0, 0, 0},
    {   "insurgency",      insurgency, 0, 1, GENERAL,    INSURGENCY_AP, 0, 0, 0},
    {       "invite",          invite, 0, 1, GENERAL,        INVITE_AP, 1, 1, 0},
    {     "jettison",        jettison, 0, 1, CAPTAIN,      JETTISON_AP, 0, 0, 0},
    {         "land",            land, 0, 0, CAPTAIN,          LAND_AP, 0, 0, 0},
    {       "launch",          launch, 0, 0, CAPTAIN,        LAUNCH_AP, 0, 0, 0},
    {         "load",            load, 0, 0, CAPTAIN,          LOAD_AP, 1, 0, 0},
    {          "map",             map, 0, 0,  NOVICE,           MAP_AP, 0, 0, 0},
    {         "make",        make_mod, 0, 0, CAPTAIN,          MAKE_AP, 1, 0, 0},
    {     "mobilize",        mobilize, 0, 0, GENERAL,      MOBILIZE_AP, 0, 0, 0},
    {       "modify",        make_mod, 0, 0, CAPTAIN,        MODIFY_AP, 1, 1, 0},
    {        "mount",           mount, 0, 0, CAPTAIN,          MOVE_AP, 0, 0, 0},
    {         "move",       move_popn, 0, 0, CAPTAIN,          MOVE_AP, 0, 0, 0},
    {         "name",            name, 0, 0, CAPTAIN,          NAME_AP, 0, 0, 0},
    {        "orbit",           orbit, 0, 0,  NOVICE,         ORBIT_AP, 0, 0, 0},
    {        "order",           order, 0, 0, PRIVATE,         ORDER_AP, 0, 0, 0},
    {         "page",            page, 0, 1, PRIVATE,          PAGE_AP, 0, 0, 0},
    {          "pay",             pay, 0, 1, GENERAL,           PAY_AP, 0, 0, 0},
    {       "pledge",          pledge, 0, 1, GENERAL,        PLEDGE_AP, 1, 1, 0},
    {         "post",    send_message, 0, 0,  NOVICE,          POST_AP, 1, 1, 0},
    {        "power",           power, 0, 0, PRIVATE,         POWER_AP, 0, 0, 0},
    {   "production",        colonies, 0, 0, PRIVATE,    PRODUCTION_AP, 1, 1, 0},
    {      "profile",         profile, 0, 0,  NOVICE,       PROFILE_AP, 0, 0, 0},
    {      "promote",       governors, 0, 0, GENERAL,       PROMOTE_AP, 0, 0, 0},
    {        "purge",  purge_messages, 0, 0,  NOVICE,         PURGE_AP, 0, 0, 0},
    {         "read",   read_messages, 0, 0,  NOVICE,          READ_AP, 0, 0, 0},
    {     "relation",        relation, 0, 0, NOVICES,      RELATION_AP, 0, 0, 0},
    {       "repair",          repair, 0, 0, CAPTAIN,        REPARI_AP, 0, 0, 0},
    {       "report",             rst, 0, 0, CAPTAIN,        REPORT_AP, 1, 0, 0},
    {      "reserve",         reserve, 0, 0, CAPTAIN,       RESERVE_AP, 1, 1, 0},
    {       "revoke",       governors, 0, 0, GENERAL,        REVOKE_AP, 0, 0, 0},
    {        "route",           route, 0, 1, CAPTAIN,         ROUTE_AP, 0, 0, 0},
    {     "schedule",     GB_schedule, 0, 0,  NOVICE,      SCHEDULE_AP, 0, 0, 0},
    {        "scrap",           scrap, 0, 1, CAPTAIN,         SCRAP_AP, 1, 1, 0},
    {      "sectors",     CSP_sectors, 0, 0,  NOVICE,                0, 0, 0, 0},
    {         "sell",            sell, 0, 0, GENERAL,          SELL_AP, 0, 0, 0},
    {         "send",    send_message, 0, 0,  NOVICE,          SEND_AP, 1, 0, 0},
    {         "ship",             rst, 0, 0, GENERAL,          SELL_AP, 0, 0, 0},
    {        "stars",  star_locations, 0, 0,  NOVICE,         STARS_AP, 0, 0, 0},
    {        "stats",             rst, 0, 0, CAPTAIN,         STATS_AP, 1, 4, 0},
    {       "status",     tech_status, 0, 0, PRIVATE,        STATUS_AP, 0, 0, 0},
    {        "stock",             rst, 0, 0, CAPTAIN,         STOCK_AP, 1, 1, 0},
    {       "survey",          survey, 0, 0, CAPTAIN,        SURVEY_AP, 1, 0, 0},
    {     "tactical",             rst, 0, 0, CAPTAIN,      TACTICAL_AP, 1, 2, 0},
    {          "tax",             tax, 0, 1, GENERAL,           TAX_AP, 0, 0, 0},
    {   "technology",      technology, 0, 0, GENERAL,    TECHNOLOGY_AP, 0, 0, 0},
    {    "techlevel",       techlevel, 0, 0, GENERAL,     TECHLEVEL_AP, 0, 0, 0},
    {         "time",         GB_time, 0, 0,  NOVICE,          TIME_AP, 0, 0, 0},
    {       "toggle",          toggle, 0, 0,  NOVICE,        TOGGLE_AP, 0, 0, 0},
    {     "toxicity",        toxicity, 0, 0, CAPTAIN,      TOXICITY_AP, 0, 0, 0},
    {     "transfer",        transfer, 0, 1, CAPTAIN,       TRANSER_AP, 0, 0, 0},
    {     "treasury",        treasury, 0, 1,  NOVICE,      TREASURY_AP, 0, 0, 0},
    {       "undock",          launch, 0, 1, CAPTAIN,        UNDOCK_AP, 0, 0, 0},
    {     "uninvite",          invite, 0, 1, GENERAL,      UNINVITE_AP, 1, 0, 0},
    {       "unload",            load, 0, 0, CAPTAIN,        UNLOAD_AP, 1, 1, 0},
    {     "unpledge",          pledge, 0, 1, GENERAL,      UNPLEDGE_AP, 1, 0, 0},
    {      "upgrade",         upgrade, 0, 1, CAPTAIN,       UPGRADE_AP, 0, 0, 0},
    {      "victory",         victory, 0, 0,  NOVICE,       VICTORY_AP, 0, 0, 0},
    {         "vote",            vote, 0, 1, GENERAL,          VOTE_AP, 0, 0, 0},
    {         "walk",            walk, 0, 1, CAPTAIN,          WALK_AP, 0, 0, 0},
    {      "weapons",             rst, 0, 1, CAPTAIN,       WEAPONS_AP, 1, 5, 0},
    {        "whois",           whois, 0, 0,  NOVICE,         WHOIS_AP, 0, 0, 0},
    {         "zoom",            zoom, 0, 0,  NOVICE,          ZOOM_AP, 0, 0, 0}
};
