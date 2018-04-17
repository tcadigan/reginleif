/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (C) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * foundation; either version 2 of the License, or (at your option) any later
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
 * *****************************************************************************
 *
 * toxi.c
 *
 * Created:
 * Author: Robert Chansky
 *
 * Version 1.6 17:35:18
 *
 * Contains:
 *   toxicity()
 *
 * #ident  "@(#)toxic.c  1.8 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/toxi.c,v 1.4 2007/07/06 18:09:34 gbp Exp $
 * *****************************************************************************
 */

#include <ctype.h>
#include <math.h>
#include <stdlib.h>

#include "GB_copyright.h"
#include "buffers.h"
#include "power.h"
#include "proto.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "vars.h"

/*
 * clearlog:
 *
 * arguments:
 *   playernum
 *   governor
 *   apcount
 *
 * called by: process_commands
 *
 * description: Called from process_commands. This sets the toxicity value on
 *              the planet to automatically launch twc when reached.
 */
void toxicity(int playernum, int governor, int apcount)
{
    int thresh;
    planettype *p;

    sscanf(args[1], "%d", &thresh);

    if ((thresh > 100) || (thresh < 0)) {
        sprintf(buf, "Illegal value.\n");
        notify(playernum, governor, buf);

        return;
    }

    if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
        sprintf(buf, "scope must be a planet.\n");
        notify(playernum, governor, buf);

        return;
    }

    if (!enufAP(playernum, governor, Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1], apcount)) {
        return;
    }

    getplanet(&p,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    /* Set the tox value for the planet */
    p->info[playernum - 1].tox_thresh = thresh;

    putplanet(p,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    deductAPs(playernum,
              governor,
              apcount,
              Dir[playernum - 1][governor].snum,
              0);

    sprintf(buf, " New threshold is: %u\n", p->info[playernum - 1].tox_thresh);
    notify(playernum, governor, buf);
    free(p);
}
