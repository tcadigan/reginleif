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
 * Types and defines for Galactic Bloodshed file index see makeindex.c -mfw
 */
#ifndef IDX_H_
#define IDX_H_

#define LINE_SIZE 90
#define TOPIC_NAME_LEN 30

typedef struct {
    long pos; /* Index into help file */
    int len; /* Length of help entry */
    char topic[TOPIC_NAME_LEN + 1]; /* Topic of help entry */
} idx_mark;

#endif /* IDX_H_ */
