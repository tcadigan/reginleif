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
 * Franklin Street, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * game_info.h - Game-variable parameters for compiling racegen
 *
 * $HEADER: /var/cvs/gbp/GB+/hdrs/game_info.h,v 1.2 2007/07/06 16:55:26 gbp Exp $
 */

/* Game Dependencies. These will likely change for each game. */
#define DEADLINE "NONE"
#define GAME "Name of GB"
#define MODERATOR "Admin's Name"
#define PLAYERS "50"
#define STARS "To be determined"
#define STARTS "01/01/447 BC"
#define UPDATE_SCH "24 h / 3 seg"
#define OTHER_STUFF "Some notes here"

/* Racegen options. */
#define COVARIANCES
#define STARTING_POINTS 1000
#define MIN_PASSWORD_LENGTH 4
#define DEFAULT_MESO_IQ_LIMIT 180
