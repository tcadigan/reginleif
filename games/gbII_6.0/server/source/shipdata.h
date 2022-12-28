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
#ifndef SHIPDATA_H_
#define SHIPDATA_H_

#include "config.h"
#include "ships.h"

extern char Shipltrs[];

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

extern long Shipdata[NUMSTYPES][NUMABILS];

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

extern char const *Shipnames[NUMSTYPES];

#endif /* SHIPDATA_H_ */
