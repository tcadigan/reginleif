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
 * doturn.h -- Various turn things we are keeping track of.
 *
 * #ident  "@(#)doturn.h        1.2 12/15/92 "
 */

struct stinfo {
    short temp_add; /* Addition to temperature to each planet */
    unsigned char thing_add; /* New Thing colony on this planet */
    unsigned char inhab; /* Explored by anybody */
    unsigned char intimidated; /* Assault platform is here */
};

extern struct stinfo Stinfo[NUMSTARS][MAXPLANETS];

#ifdef USE_VN

struct vnbrain {
    u_short total_mad; /* Total # of VN's destroyed so far */
    short most_mad; /* Player most mad at */
};

extern struct vnbrain VN_brain;

struct sectinfo {
    char explored; /* Sector has been explored */
    unsigned char done; /* This sector has been updated */
    unsigned char VN; /* This sector has a VN */
};

#else

struct sectinfo {
    char explored; /* Sector has been explored */
    unsigned char done; /* This sector has been update */
};

#endif

extern struct sectinfo Sectinfo[MAX_X][MAX_Y];
