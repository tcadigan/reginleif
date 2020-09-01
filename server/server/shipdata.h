/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_Copyright.h for additional authors and details.
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
 * #ident "@(#)shipdata.h 1.13 12/1/93 "
 */

#include "config.h"

char Shipltrs[] = {
    'p', 's', 'X', 'D', 'B', 'I', 'C', 'd', 'f', 'e', 'H', 'S', 'O', 'c', 't',
    '+', 'M', '=', '/', '-', 'a', 'g', 'h', '@', 'l', 'w', ':', 'G', 'F', 'T',
    '[', '^', 'n', 'q', 'K', 'Y', 'W', 'J', '&', 'R', 'b', 'L', ',', 'k', 'j',
    'P', 'i', 'u', 'E', 'r',
#ifdef USE_VN
    'v', 'V', 'Z', 'z',
#endif
    'A', '!'
};

/*
 * Table for [ABIL_BUILD]. (bd). Sum the numbers to get the correct value.
 *   1) To allow it to be built on a planet.
 *   2) For building by warships (d, B, C, ...). Currently only Space
 *      Probe. Mines used to be this way too. Built in hanger of building ship.
 *   4) For building by Shuttles, Cargo ship, Habitats, etc. Also forces
 *      construction on outside of ship. Not in hanger.
 *   8) For building in Factories. Build on planet, or in hanger of carrying
 *      ship (Habitat).
 *   16) For building in Habitats. Used by Pods for instance. Also used by
 *       Factories. Build inside Habitat.
 *
 * Table for [ABIL_CONSTRUCT]. (cn). Sum the numbers to get the correct value.
 *   1) To allow it to build like a planet.
 *   2) For building like warships (d, B, C, ...).
 *   4) For building like Shuttles, Cargo ship, Habitats, etc.
 *   8) For building like Factories.
 *   16) For building like Habitats.
 *
 * Changes here to use the new build routine using above tables. Maarten
 * Also changed:
 *   - Pods, Factories, Weapons Planets, Terraforming Devices, Orbital Mind
 *     Control Lasers and Government centers can be built inside Habitats.
 *   - Probes, and other type 2 ships (currently none), are now built inside
 *     ships, requiring hanger space. This gives more incentive to keep some
 *     hanger space in the big warships.
 *   - The big space stations (Habitats, Stations, and Orbital Assault
 *     Platforms) can now build Probes as well.
 *   - Habitats and Stations had their ability to use a crystal mount
 *     removed. Since they cannot use it anyway, it was rather useless. It only
 *     confused the required resources to build the ship, though this has been
 *     taken care of too.
 *   - Orbital Mind Control Lasers having 10 guns of caliber 0 seems
 *     strange. Now 0 guns. Also removed the 100 destruct carrying capacity.
 *     Added 25 cargo space so it can repair itself.
 *
 * Gardan 9.1.1997 Added:
 *   - Dhuttles
 *   - Several ship stats modified
 *
 * Gardan code 23.1.1997 starts here
 */

long Shipdata[NUMSTYPES][NUMABILS] = {
    /* tech carg bay dest guns prim sec fuelcap crw arm cst mt jp ld sw sp dm bd cn mod las cew clk god prg port rep pay dock */
    {   /* Pd */    0,     0,    0,     0,   0, 0, 0,    20,    1,  0,    0, 0, 0, 1, 0, 3,  0,  1,  0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
    {  /* Shu */    5,    10,    0,     1,   1, 1, 0,    20,   10,  0,    2, 0, 0, 1, 0, 2,  0,  8,  4, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1},
    {  /* Car */  250,   600,  200,   800,  30, 2, 0,  1000,   30,  5,   30, 1, 1, 0, 0, 3, 48, 20,  2, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1},
    {  /* Drn */  250,   200,   10,   780,  60, 3, 3,   500,   60, 10,   40, 1, 1, 1, 0, 6, 48,  8,  2, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1},
    {   /* BB */  200,   150,   10,   450,  40, 3, 2,   200,   50,  7,   20, 1, 1, 1, 0, 6, 48,  8,  2, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1},
    {  /* Int */  150,    50,    5,   160,  20, 2, 2,   200,   20,  3,   12, 1, 1, 1, 0, 8, 48,  8,  2, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1},
    {   /* CA */  150,   100,    5,   300,  20, 3, 1,   120,   20,  5,   10, 1, 1, 1, 0, 6, 48,  8,  2, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1},
    {   /* DD */  100,    50,    5,   120,  15, 2, 2,    80,   15,  3,    5, 1, 1, 1, 0, 6, 48,  8,  2, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1},
    {   /* FF */  100,     0,    0,    40,  20, 2, 0,    10,    1,  2,    1, 0, 0, 1, 0, 9,  0,  8,  2, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1},
    {  /* Exp */   40,    10,    0,    15,   5, 2, 0,    35,    5,  1,    2, 1, 1, 1, 0, 6, 48,  8,  0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1},
    /* tech carg bay dest guns prim sec fuelcap crw arm cst mt jp ld sw sp dm bd cn mod las cew clk god prg port rep pay dock */
    {  /* Hab */  100,  5000,  100,   500,  20, 3, 0,  2000, 2000,   3,  50, 0, 0, 0, 1, 4, 72, 20, 18, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1},
    {  /* Stn */  100,   700,  500,  1000,  20, 2, 0,  2000,   50,   5,  10, 0, 0, 0, 0, 0, 72, 20,  6, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1},
    {  /* OAP */  200,  1400,   20,  1000,  50, 3, 0,  2000,  200,   5,  45, 1, 1, 0, 0, 4, 72, 20,  6, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1},
    {  /* Crg */  100,  1000,    5,  1000,  10, 1, 0,  1000,  100,   2,  10, 1, 1, 1, 0, 4,  0,  8,  4, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1},
    {  /* Tnk */  100,   200,    0,   200,  10, 1, 0,  5000,   10,   2,  10, 1, 1, 1, 0, 4,  0,  8,  2, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1},
    {  /* SMn */   65,     0,    0,     5,   0, 0, 0,     4,    0,   1,  30, 0, 0, 1, 1, 1,  0,  8,  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {  /* mir */  100,    50,    0,    10,   1, 1, 0,    20,    5,   0, 100, 0, 0, 0, 0, 2, 72, 20,  0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1},
    {  /* Stc */   50,     0,    0,     0,   0, 0, 0,    20,    2,   0,  10, 1, 1, 1, 0, 4,  0,  8,  0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1},
    {  /* Tsc */    5,     0,    0,     0,   0, 0, 0,     0,    2,   0,   2, 0, 0, 1, 0, 0,  0,  1,  0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
    {  /* T-R */  200,     0,    0,     0,   0, 0, 0,  1000,    5,   0,  20, 0, 0, 1, 1, 2,  0,  1,  0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    /* tech carg bay dest guns prim sec fuelcap crw arm cst mt jp ld sw sp dm bd cn mod las cew clk god prg port rep pay dock */
    {  /* APr */   80,     0,    0,     0,   0, 0, 0,    30,   10,   0,  32, 0, 0, 1, 1, 0,  0,  1,  0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
    {   /* CD */   40,     0,    0,     0,   0, 0, 0,     1,    0,   0,  10, 0, 0, 1, 1, 1,  0,  1,  0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
    {  /* Grn */   40,     0,    0,     0,   0, 0, 0,     1,    0,   0,  10, 0, 0, 1, 0, 1,  0,  1,  0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
    {  /* Gov */    0,   500,    0,   100,  10, 1, 0,  1000,   10,  20, 500, 0, 0, 1, 0, 0, 72, 17,  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0},
    { /* OMCL */  350,    25,    0,     0,   0, 0, 0,   100,    2,   1,  50, 0, 0, 1, 1, 4,  0, 17,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {  /* TWC */    0,     0,    0,     0,   0, 0, 0,     5,    0,   0,   5, 0, 0, 1, 0, 4,  0,  1,  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {  /* Prb */   25,     0,    0,     0,   0, 0, 0,    20,    0,   0,  10, 0, 0, 1, 0, 9,  0, 19,  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {  /* GRL */  100,    50,    0,   120,  15, 2, 0,     0,   40,   3,  30, 0, 0, 1, 1, 0, 72,  1,  0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1},
    {  /* Fac */    0,     0,    0,     0,   0, 0, 0,     0,   20,   0,  20, 0, 0, 1, 1, 0, 48, 17,  8, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0},
    {  /* TFD */   80,    40,    0,     0,   0, 0, 0,    40,   10,   0,  30, 0, 0, 1, 1, 2,  0, 17,  0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1},
    /* tech carg bay dest guns prim sec fuelcap crw arm cst mt jp ld sw sp dm bd cn mod las cew clk god prg port rep pay dock */
    {   /* TD */  200,  1000,    0,  1000,   0, 0, 0,  1000,  100,   0, 300, 0, 0, 1, 1, 0, 48,  1,  0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
    {  /* Mis */   50,     0,    0,    10,   0, 0, 0,     5,    0,   0,   5, 0, 0, 0, 1, 6,  0,  8,  0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {  /* PDN */  200,    50,    0,   500,  20, 3, 0,     0,   50,  10, 100, 0, 0, 1, 1, 0, 72,  1,  0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0},
    {  /* Qua */    0,     0,    0,     0,   0, 0, 0,    20,   10,   0,  15, 0, 0, 1, 1, 0,  0,  1,  0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0},
    {  /* Plo */    5,     0,    0,     0,   0, 0, 0,    20,   10,   0,  19, 0, 0, 1, 1, 0,  0,  1,  0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {  /* Dom */   10,    20,    0,     0,   0, 0, 0,     0,   10,   0,  19, 0, 0, 1, 1, 0,  0,  1,  0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1},
    {  /* Wea */    0,   500,    0,     0,   0, 0, 0,   500,   20,   5,  20, 0, 0, 1, 0, 0, 62, 17,  0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
    { /* Port */    0,     0,    0,     0,   0, 0, 0,     0,  100,   1,  50, 0, 0, 1, 0, 0, 72,  1,  0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0},
    {  /* ABM */   80,     5,    0,   100,   5, 1, 0,     0,    5,   5,  50, 0, 0, 1, 1, 0, 24,  1,  0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0},
    {  /* AFV */   50,     5,    0,    20,   2, 1, 0,    20,    1,   2,  20, 0, 0, 0, 0, 0,  0,  8,  0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1},
    /* tech carg bay dest guns prim sec fuelcap crw arm cst mt jp ld sw sp dm bd cn mod las cew clk god prg port rep pay dock */
    {  /* Bun */   10,   100,   20,   100,   0, 0, 0,   100,  100,  15, 100, 0, 0, 0, 0, 0, 48,  1,  0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0},
    {  /* Lnd */  150,   100,   10,   100,  10, 2, 0,   700,  500,   7,  50, 1, 1, 1, 0, 4, 48,  8,  0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1},
    {  /* Swp */   65,    25,    0,    80,   8, 1, 0,    80,    8,   3,  16, 1, 1, 1, 0, 5, 48,  8,  2, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1},
    {  /* Col */   40,    20,    0,     0,   0, 0, 0,    70,   40,   2,  18, 1, 1, 1, 0, 6,  0,  8,  2, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {  /* LCa */   70,   100,    8,    40,   2, 1, 0,   200,    6,   1,  25, 1, 1, 1, 0, 5, 48,  8,  2, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1},
    { /* Corv */   40,    15,    1,    70,   1, 2, 1,    35,   10,   2,  20, 0, 0, 1, 0, 7, 50,  8,  2, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1},
    {  /* Spd */    0,     0,    0,     0,   0, 0, 0,    20,    1,   0,   5, 0, 0, 1, 0, 4,  0,  1,  0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
    {  /* INF */    5,     0,    0,     0,   0, 0, 0,     0,    0,  20,   2, 0, 0, 1, 1, 0,  0,  1,  0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1},
    { /* SETI */   30,    10,    0,     0,   0, 0, 0,   100,   20,   0,  40, 1, 0, 0, 1, 0, 50,  1,  0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0}, / *CWL */
    { /* Frig */  100,    25,    2,   100,  10, 1, 1,   100,   10,   2,  25, 1, 1, 1, 0, 3, 50,  8,  2, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1},
    {  /* Rfn */   25,  1000,    0,     0,   0, 0, 0,     0,   20,   2,  20, 0, 0, 1, 0, 0, 25, 17,  0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
#ifdef USE_VN
    {   /* VN */  120,    30,    0,     0,   0, 0, 0,    50,    0,   1,  30, 0, 0, 1, 1, 4,  0,  1,  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {  /* Ber */ 9999,    50,    0,   200,  20, 3, 2,   250,    0,  15,  30, 0, 0, 1, 1, 6,  0,  1,  0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1},
    {  /* BCC */ 9999,   200,    0,    50,   0, 3, 0,     0,    0,  10,   3, 0, 0, 1, 1, 0,  0,  1,  0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0},
    {  /* BAf */ 9999,  1000,    0,  1000,   0, 0, 0,  1000,    0,  10,   8, 0, 0, 1, 1, 0,  0,  1,  0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0},
#endif
    {  /* Amo */ 9999,     0,    0,     0,   0, 0, 0,  1000, 1001,   0, 100, 0, 0, 1, 0, 6,  0,  1,  0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0},
    {  /* GOD */ 9999, 20000, 1000, 20000, 255, 3, 3, 20000, 1000, 100, 100, 1, 1, 1, 0, 9,  0,  1,  6, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}
    /* tech carg bay dest guns prim sec fuelcap crw arm cst mt jp ls sw sp dm bd cn mod las cew clk god prg port rep pay dock */
};

/* Gardan code ends here */

/*
 * Legend:
 *   tech ABIL_TECH
 *   carg ABIL_CARGO
 *   bay ABIL HANGER
 *   dest ABIL_DESTCAP
 *   guns ABIL_GUNS
 *   prim ABIL_PRIMARY
 *   sec ABIL_SECONDARY
 *   fuel ABIL_FUELCAP
 *   crw ABIL_MAXCREW
 *   arm ABIL_ARMOR
 *   cst ABIL_COST
 *   mt ABIL_MOUNT
 *   jp ABIL_JUMP
 *   ld ABIL_CANLAND
 *   sw ABIL_HASSWITCH
 *   sp ABIL_SPEED
 *   dm ABIL_DAMAGE
 *   bd ABIL_BUILD
 *   cn ABIL_CONSTRUCT
 *   mod ABIL_MOD
 *   las ABIL_LASER
 *   cew ABIL_CEW
 *   clk ABIL_CLOAK
 *   god ABIL_GOD
 *   prg ABIL_PROGRAMMED
 *   port ABIL_PORT
 *   rep ABIL_REPAIR
 *   pay ABIL_MAINTAIN
 *   dock ABIL_CANDOCK
 */

char const *Shipnames[NUMSTYPES] = {
    "Spore Pod",
    "Shuttle",
    "Carrier",
    "Dreadnaught",
    "Battleship",
    "Interceptor",
    "Cruiser",
    "Destroyer",
    "Fighter Group",
    "Explorer",
    "Habitat",
    "Station",
    "Ob Asst Pltfrm",
    "Cargo Ship",
    "Tanker",
    "Mine Field",
    "Space Mirror",
    "Space Telescope",
    "Ground Telescope",
    "* T-R beam ",
    "Atmosph Processor",
    "Dust Canister",
    "Greenhouse Gases",
    "Govrnmnt. Center",
    "Mind Control Lsr",
    "Tox Waste Canistr",
    "Space Probe",
    "Gamma Ray Laser",
    "Factory",
    "Terraform Device",
    "AVPM Transporter",
    "Missile",
    "Planet Def Net",
    "Quarry",
    "Space Plow",
    "Dome",
    "Weapons Plant",
    "Space Port",
    "ABM Battery",
    "Mech",
    "Bunker",
    "Lander",
    "Mine Sweeper",
    "Colonizer",
    "Light Carrier",
    "Covette",
    "Super Pod",
    "Infrastructure",
    "SETI",
    "Frigate",
    "Refinery",
#ifdef USE_VN
    "V.Neumann Machine",
    "VN Berserker",
    "Bers Cntrl Center",
    "Bers Atuofac",
#endif
    "Space Amoeba",
    "GODSHIP"
};
