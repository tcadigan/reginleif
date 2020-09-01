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
 * csp_dispatch.h
 *
 * CSP, copyright (c) 1993 by John P. Deregon, Evan Koffler
 *
 * #ident "@(#)csp_dispatch.h 1.3 12/1/93 "
 */

extern void voidpoint(void);

/* For binary search */
typedef struct command_struct {
    int command;
    void (*func) ();
    int cnt;
} CSP_commands;

/*
 * typedef struct csp_recv_command_struct {
 *     int command;
 *     void (*func)();
 *     int cnt;
 * } CSP_Commands;
 */

/* These are commands the server will RECEIVE from the client */
static CSP_commands csp_client_commands[] = {
    {       CSP_KNOWLEDGE,      CSP_knowledge, 0}, /* ok */
    {   CSP_LOGIN_COMMADS,  CSP_client_toggle, 1}, /* ok */
    {  CSP_SURVEY_COMMAND,             survey, 1}, /* ok */
    { CSP_RELATION_COMAND,           relation, 1}, /* ok */
    { CSP_PROFILE_COMMAND,            profile, 1}, /* ok */
    /* {     CSP_WHO_COMMAND,         dump_users, 0}, /\* ok *\/ */
    { CSP_EXPLORE_COMMAND,        exploration, 0}, /* ok */
    {     CSP_MAP_COMMAND,                map, 0}, /* ok */
    {   CSP_SCOPE_COMMAND,         CSP_prompt, 0}, /* ok */
    /* {    CSP_SCOPE_PROMPT,         CSP_prompt, 0}, /\* ok *\/ */
    {CSP_PLANDUMP_COMMAND,    csp_planet_dump, 0}, /* ok */
    {CSP_SHIPDUMP_COMMAND,      csp_ship_dump, 0}, /* ok */
    {    CSP_ZOOM_COMMAND,           csp_zoom, 0}, /* ok */
    {   CSP_ORBIT_COMMAND,          csp_orbit, 0}, /* ok */
    { CSP_VERSION_COMMAND, CSP_client_version, 0}, /* ok */
    {CSP_SHIPLIST_COMMAND,      CSP_ship_list, 0},
    {CSP_STARDUMP_COMMAND,      CSP_star_dump, 0},
    { CSP_SECTORS_COMMAND,        CSP_sectors, 0},
    {CSP_UNIVDUMP_COMMAND,      csp_univ_dump, 0}
};

/* These are the commands the server will SEND the client */
static CSP_commands csp_server_commands[] = {
    {           CSP_BACKUP_END,      voidpoint, 0},
    {         CSP_BACKUP_START,      voidpoint, 0},
    {           CSP_CLIENT_OFF, CSP_client_off, 0}, /* ok */
    {            CSP_CLIENT_ON,  CSP_client_on, 0}, /* ok */
    {            CSP_RESET_END,      voidpoint, 0},
    {          CSP_RESET_START,      voidpoint, 0},
    {          CSP_SEGMENT_END,      voidpoint, 0}, /* ok */
    {        CSP_SEGMENT_START,      voidpoint, 0}, /* ok */
    {           CSP_SURVEY_END,      voidpoint, 0}, /* ok */
    {         CSP_SURVEY_INTRO,      voidpoint, 0}, /* ok */
    {        CSP_SURVEY_SECTOR,      voidpoint, 0}, /* ok */
    {        CSP_PROFILE_INTRO,      voidpoint, 0}, /* ok */
    {     CSP_PROFILE_PERSONAL,      voidpoint, 0}, /* ok */
    {      CSP_PROFILE_DYNAMIC,      voidpoint, 0}, /* ok */
    {CSP_PROFILE_DYNAMIC_OTHER,      voidpoint, 0}, /* ok */
    {   CSP_PROFILE_RACE_STATS,      voidpoint, 0}, /* ok */
    {       CSP_PROFILE_PLANET,      voidpoint, 0}, /* ok */
    {       CSP_PROFILE_SECTOR,      voidpoint, 0}, /* ok */
    {    CSP_PROFILE_DISCOVERY,      voidpoint, 0}, /* ok */
    {          CSP_PROFILE_END,      voidpoint, 0}, /* ok */
    {            CSP_WHO_INTRO,      voidpoint, 0}, /* ok */
    {             CSP_WHO_DATA,      voidpoint, 0}, /* ok */
    {          CSP_WHO_COWARDS,      voidpoint, 0}, /* ok */
    {              CSP_WHO_END,      voidpoint, 0}, /* ok */
    {            CSP_MAP_INTRO,      voidpoint, 0}, /* ok */
    {             CSP_MAP_DATA,      voidpoint, 0}, /* ok */
    {        CSP_MAP_DYNAMIC_1,      voidpoint, 0}, /* ok */
    {        CSP_MAP_DYNAMIC_2,      voidpoint, 0}, /* ok */
    {           CSP_MAP_ALIENS,      voidpoint, 0}, /* ok */
    {              CSP_MAP_END,      voidpoint, 0}, /* ok */
    {        CSP_EXPLORE_INTRO,      voidpoint, 0}, /* ok */
    {         CSP_EXPLORE_STAR,      voidpoint, 0}, /* ok */
    {    CSP_EXPLORE_STAR_DATA,      voidpoint, 0}, /* ok */
    {  CSP_EXPLORE_STAR_ALIENS,      voidpoint, 0}, /* ok */
    {     CSP_EXPLORE_STAR_END,      voidpoint, 0}, /* ok */
    {           CSP_EXPORE_END,      voidpoint, 0}, /* ok */
    {           CSP_UPDATE_END,      voidpoint, 0},
    {         CSP_UPDATE_START,      voidpoint, 0},
    {      CSP_UPDATES_RESUMED,      voidpoint, 0},
    {    CSP_UPDATES_SUSPENDED,      voidpoint, 0},
    {         CSP_VERSION_INFO,      voidpoint, 0},
    {      CSP_VERSION_OPTIONS,      voidpoint, 0},
    /* NOT SUPPORTED */
    {       CSP_PLANDUMP_INTRO,      voidpoint, 0},
    {  CSP_PLANDUMP_CONDITIONS,      voidpoint, 0},
    {       CSP_PLANDUMP_STOCK,      voidpoint, 0},
    {        CSP_PLANDUMP_PROD,      voidpoint, 0},
    {        CSP_PLANDUMP_MISC,      voidpoint, 0},
    {      CSP_PLANDUMP_NOEXPL,      voidpoint, 0},
    {          CSP_STAR_ENEXPL,      voidpoint, 0},
    {   CSP_ORBIT_OUTPUT_INTRO,      voidpoint, 0},
    {      CSP_ORBIT_STAR_DATA,      voidpoint, 0},
    {  CSP_ORBIT_UNEXP_PL_DATA,      voidpoint, 0},
    {    CSP_ORBIT_EXP_PL_DATA,      voidpoint, 0},
    {      CSP_ORBIT_SHIP_DATA,      voidpoint, 0},
    {     CSP_ORBIT_OUTPUT_END,      voidpoint, 0},
    {         CSP_SHIPDUMP_GEN,      voidpoint, 0},
    {       CSP_SHIPDUMP_STOCK,      voidpoint, 0},
    {      CSP_SHIPDUMP_STATUS,      voidpoint, 0},
    {     CSP_SHIPDUMP_WEAPONS,      voidpoint, 0},
    {     CSP_SHIPDUMP_FACTORY,      voidpoint, 0},
    {        CSP_SHIPDUMP_DEST,      voidpoint, 0},
    { CSP_SHIPDUMP_PTACT_PDIST,      voidpoint, 0},
    { CSP_SHIPDUMP_STACT_PDIST,      voidpoint, 0},
    {  CSP_SHIPDUMP_PTACT_INFO,      voidpoint, 0},
    {  CSP_SHIPDUMP_PTACT_INFO,      voidpoint, 0},
    {  CSP_SHIPDUMP_STACT_INFO,      voidpoint, 0},
    {      CSP_SHIPDUMP_ORDERS,      voidpoint, 0},
    {      CSP_SHIPDUMP_THRESH,      voidpoint, 0},
    {     CSP_SHIPDUMP_SPECIAL,      voidpoint, 0},
    {       CSP_SHIPDUMP_HYPER,      voidpoint, 0},
    {         CSP_SHIPDUMP_END,      voidpoint, 0},
    {       CSP_SHIPLIST_INTRO,      voidpoint, 0},
    {        CSP_SHIPLIST_DATA,      voidpoint, 0},
    {         CSP_SHIPLIST_END,      voidpoint, 0},
    {        CSP_SECTORS_INTRO,      voidpoint, 0},
    {           CSP_SECTOR_END,      voidpoint, 0},
    /* NOT SUPPORTED */
    {                  CSP_ERR,      voidpoint, 0}
};
