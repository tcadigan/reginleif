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
 * The GNU General Public License is contains in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * mobiliz.c -- Persuade people to build military stuff. Sectors that are
 *              mobilized produce Destruct Potential in proportion to the % they
 *              are mobilized. They are also more damage-resistant.
 *
 * #ident  "@(#)mobiliz.c       1.7 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/mobiliz.c,v 1.4 2007/07/06 18:09:34 gbp Exp $
 */

#include <ctype.h>
#include <stdlib.h>

#include "buffers.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "vars.h"

void mobilize(int, int, int);
void tax(int, int, int);
int control(int, int, startype *);

void mobilize(int playernum, int governor, int apcount)
{
    int sum_mob = 0;
    planettype *p;

    if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
        sprintf(buf, "Scope must be a planet.\n");
        notify(playernum, governor, buf);

        return;
    }

    if (!enufAP(playernum, governor, Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1], apcount)) {
        return;
    }

    getplanet(&p,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    getsmap(Smap, p);

    if (argn < 2) {
        sprintf(buf,
                "Current mobilization: %d    Quota: %d\n",
                p->info[playernum - 1].comread,
                p->info[playernum - 1].mob_set);

        notify(playernum, governor, buf);
        free(p);

        return;
    }

    sum_mob = atoi(args[2]);

    if ((sum_mob > 100) || (sum_mob < 0)) {
        notify(playernum, governor, buf);
        free(p);

        return;
    }

    p->info[playernum - 1].mob_set = sum_mob;

    putplanet(p,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    deductAPs(playernum,
              governor,
              apcount,
              Dir[playernum - 1][governor].snum,
              0);

    sprintf(buf,
            "New quota set to: %d at a cost of %d APs.\n",
            p->info[playernum - 1].mob_set,
            apcount);

    notify(playernum, governor, buf);
    free(p);
}

void tax(int playernum, int governor, int apcount)
{
    int sum_tax = 0;
    planettype *p;
    racetype *race;

    if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
        sprintf(buf, "Scope must be a planet.\n");
        notify(playernum, governor, buf);

        return;
    }

    race = races[playernum - 1];

    if (!race->Gov_ship) {
        notify(playernum, governor, "You have no government center active.\n");

        return;
    }

    if (race->Guest) {
        notify(playernum,
               governor,
               "Sorry, but you can't do this when you are a guest.\n");

        return;
    }

    if (!enufAP(playernum, governor, Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1], apcount)) {
        return;
    }

    getplanet(&p,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    if (argn < 2) {
        sprintf(buf,
                "Current tax rate: %d%%    Target: %d%%\n",
                p->info[playernum - 1].tax,
                p->info[playernum - 1].newtax);

        notify(playernum, governor, buf);
        free(p);

        return;
    }

    sum_tax = atoi(args[1]);

    if ((sum_tax > 100) || (sum_tax < 0)) {
        sprintf(buf, "Illegal value.\n");
        notify(playernum, governor, buf);
        free(p);

        return;
    }

    p->info[playernum - 1].newtax = sum_tax;

    putplanet(p,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    deductAPs(playernum,
              governor,
              apcount,
              Dir[playernum - 1][governor].snum,
              0);

    notify(playernum, governor, "Set.\n");
    free(p);
}

int control(int playernum, int governor, startype *star)
{
    return (!governor || (star->governor[playernum - 1] == governor));
}
