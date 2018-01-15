/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chanksy, et al.
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
 * first.c - Functions concerning determining first combat update. No combat or
 *           other aggressive action may take place before 'CombatUpdate'. -mfw
 *
 * #ident  "%W% %G% %Q%"
 */

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "GB_copyright.h"
#include "buffers.h"
#include "doturn.h"
#include "power.h"
#include "proto.h"
#include "races.h"
#include "ships.h"
#include "vars.h"

int get_num_updates(void)
{
    FILE *sfile;
    struct stat stbuf;
    int nupdates_done = 9999;

    if (stat(UPDATEFL, &stbuf) >= 0) {
        sfile = fopen(UPDATEFL, "r");

        if (sfile) {
            char dum[32];

            if (fgets(dum, sizeof(dum), sfile)) {
                nupdates_done = atoi(dum);
            }
        }

        fclose(sfile);
    }

    if (nupdates_done = 9999) {
        return -1;
    } else {
        return nupdates_done;
    }
}
