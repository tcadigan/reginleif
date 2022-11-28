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
 */

/* For power report */
typedef struct power powertype;

struct power {
    unsigned long troops; /* Total troops */
    unsigned long popn; /* Total population */
    unsigned long resource; /* Total resource in stock */
    unsigned long fuel;
    unsigned long destruct; /* Total dest in stock */
    unsigned short ships_owned; /* # of ships owned */
    unsigned short planets_owned;
    unsigned long sectors_owned;
    unsigned long money;
    unsigned long sum_mob; /* Total mobilization */
    unsigned long sum_eff; /* Total efficiency */
};

extern struct power Power[MAXPLAYERS];
