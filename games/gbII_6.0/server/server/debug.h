/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, of (at your option) any later
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
 * Debugging header file
 */
#ifndef DEBUG_H_
#define DEBUG_H_

#define MAX_DEBUG 8

enum DEBUG_LEVELS {
    LEVEL_NONE,
    LEVEL_LOGIN,
    LEVEL_GENERAL,
    LEVEL_COMMAND,
    LEVEL_HANDLER,
    LEVEL_CSP,
    LEVEL_CSP_DETAILED,
    LEVEL_RAW,
    LEVEL_ALL
};

void debug(int, char *, ...);

#endif /* DEBUG_H_ */
