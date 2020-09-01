/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (C) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version  of the License, or (at your option) any later
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
 * reserve.c -- Set aside fuel and resources
 *
 * Author: Tim Brown smq@ucscb.ucsc.edu
 *
 * #ident  "@(#)reserve.c       1.14 11/5/93 "
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "doturn.h"
#include "power.h"
#include "races.h"
#include "ships.h"
#include "tweakables.h"
#include "vars.h"

void reserve(int playernum, int governor, int apcost)
{
    placetype where;
    planettype *p;

    /* Check scope */
    where.level = Dir[playernum - 1][governor].level;

    if (where.level != LEVEL_PLAN) {
        sprintf(buf, "Change scope to a planet\n");
        notify(playernum, governor, buf);

        return;
    }

    where.snum = Dir[playernum - 1][governor].snum;
    where.pnum = Dir[playernum - 1][governor].pnum;

    getplanet(&p, (int)where.snum, (int)where.pnum);

    /* Check args */
    if (argn < 3) {
        sprintf(buf,
                "Resource reserve set to: %d\nFuel reserve set to: %d\n",
                p->info[playernum - 1].res_reserve,
                p->info[playernum - 1].fuel_reserve);

        notify(playernum, governor, buf);
        free(p);

        return;
    }

    /* Check for reasonable amount */
    if (atoi(args[1]) < 0) {
        sprintf(buf,
                "USAGE: reserve <amt> res/fuel\nPlease provide a numeric amount > 0\n");

        notify(playernum, governor, buf);
        free(p);

        return;
    }

    if (match(args[2], "res")) {
        p->info[playernum - 1].res_reserve = atoi(args[1]);
    } else if (match(args[2], "fuel")) {
        p->info[playernum - 1].fuel_reserve = atoi(args[1]);
    } else {
        sprintf(buf,
                "USAGE: reserve <amt> res/fuel\nYou must designate \"res\" or \"fuel\"\n");

        notify(playernum, governor, buf);
    }

    sprintf(buf,
            "Resource reserve set to: %d\nFuel reserve set to: %d\n",
            p->info[playernum - 1].res_reserve,
            p->info[playernum - 1].fuel_reserve);

    notify(playernum, governor, buf);
    putplanet(p, where.snum, where.pnum);
    free(p);
}
