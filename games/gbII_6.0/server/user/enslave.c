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
 * enslave.c -- Enslave the planet below.
 *
 * #ident  "@(#)enslave.c       1.8 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/enslave.c, v 1.4 2007/07/06 18:06:56 gbp Exp $
 */
#include "enslave.h"

#include <stdlib.h>
#include <string.h>

#include "../server/buffers.h"
#include "../server/files_shl.h"
#include "../server/first.h"
#include "../server/GB_server.h"
#include "../server/getplace.h"
#include "../server/max.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/ranks.h"
#include "../server/ships.h"
#include "../server/shlmisc.h"
#include "../server/vars.h"

extern void enslave(int, int, int);

void enslave(int playernum, int governor, int apcount)
{
    shiptype *s;
    shiptype *s2;
    planettype *p;
    int sh;
    int shipno;
    int i;
    int aliens = 0;
    int def = 0;
    int attack = 0;
    racetype *race;

    /* Check to see if past First Combat update */
    if (get_num_updates() < CombatUpdate) {
        sprintf(buf,
                "enslave command is disabled until after Combat Update: [%d]\n",
                CombatUpdate);

        notify(playernum, governor, buf);

        return;
    }

    if (args[1][0] == '#') {
        sscanf(args[1] + 1, "%d", &shipno);
    } else {
        sscanf(args[1], "%d", &shipno);
    }

    if (!getship(&s, shipno)) {
        return;
    }

    if (testship(playernum, governor, s)) {
        free(s);

        return;
    }

    if (s->type != STYPE_OAP) {
        sprintf(buf, "This ship is not an %s.\n", Shipnames[STYPE_OAP]);
        notify(playernum, governor, buf);
        free(s);

        return;
    }

    if (s->whatorbits != LEVEL_PLAN) {
        sprintf(buf, "%s doesn't orbit a planet.\n", Ship(s));
        notify(playernum, governor, buf);
        free(s);

        return;
    }

    if (!enufAP(playernum, governor, Stars[s->storbits]->AP[playernum - 1], apcount)) {
        free(s);

        return;
    }

    getplanet(&p, (int)s->storbits, (int)s->pnumorbits);

    if (p->info[playernum - 1].numsectsowned == 0) {
        sprintf(buf, "You don't have a garrison on the planet.\n");
        notify(playernum, governor, buf);
        free(s);
        free(p);

        return;
    }

    /* Add up forces attacking, defending */
    def = 0;
    aliens = def;
    attack = aliens;

    for (i = 1; i < MAXPLAYERS; ++i) {
        if (p->info[i - 1].numsectsowned && (i != playernum)) {
            if (races[i - 1]->Guest) {
                notify(playernum,
                       governor,
                       "May not enslave a planet with a Guest race.\n");

                free(s);
                free(p);

                return;
            }

            aliens = 1;
            def += p->info[i - 1].destruct;
        }
    }

    if (!aliens) {
        sprintf(buf, "there is no one else on this planet to enslave!\n");
        notify(playernum, governor, buf);
        free(s);
        free(p);

        return;
    }

    race = races[playernum - 1];

    sh = p->ships;

    while (sh) {
        getship(&s2, sh);

        if (s2->alive && s2->active) {
            if (p->info[s2->owner].numsectsowned && (s2->owner != playernum)) {
                def += s2->destruct;
            } else if (s2->owner == playernum) {
                attack += s2->destruct;
            }
        }

        sh = nextship(s2);
        free(s2);
    }

    deductAPs(playernum, governor, apcount, (int)s->storbits, 0);

    sprintf(buf,
            "\nFor successful enslavement this ship and the other ships here\n");

    notify(playernum, governor, buf);
    sprintf(buf, "that are yours must have weapons\n");
    notify(playernum, governor, buf);

    sprintf(buf,
            "capacity greater than twice that the enemy can muster, including\n");

    notify(playernum, governor, buf);
    sprintf(buf, "the planet and all ships orbiting it.\n");
    notify(playernum, governor, buf);

    sprintf(buf,
            "\nTotal forces bearing on %s:   %d\n",
            prin_ship_orbits(s),
            attack);

    notify(playernum, governor, buf);

    sprintf(telegram_buf,
            "ALERT!!!\n\nPlanet /%s/%s ",
            Stars[s->storbits]->name,
            Stars[s->storbits]->pnames[s->pnumorbits]);

    if (def <= (2 * attack)) {
        p->slaved_to = playernum;
        putplanet(p, (int)s->storbits, (int)s->pnumorbits);

        /* Send telegraphs to anyone there */
        sprintf(buf, "ENSLAVED by %s!!\n", Ship(s));
        strcat(telegram_buf, buf);

        sprintf(buf,
                "All material produced here will be\ndiverted to %s coffers.",
                race->name);

        strcat(telegram_buf, buf);

        sprintf(buf,
                "\nEnslavement successful. All material produced here will\n");

        notify(playernum, governor, buf);
        sprintf(buf, "be diverted to %s.\n", race->name);
        notify(playernum, governor, buf);

        sprintf(buf,
                "You must maintain a garrison of 0.1%% the population of the\n");

        notify(playernum, governor, buf);

        sprintf(buf,
                "planet (at least %.0f); otherwise there is a 50%% chance that\n",
                p->popn * 0.001);

        notify(playernum, governor, buf);
        sprintf(buf, "enslaved population will revolt.\n");
        notify(playernum, governor, buf);
    } else {
        sprintf(buf, "repulsed attempt at enslavement by %s!!\n", Ship(s));
        strcat(telegram_buf, buf);

        sprintf(buf,
                "Enslavement repulsed, defense/attack ratio: %d to %d.\n",
                def,
                attack);

        strcat(telegram_buf, buf);
        sprintf(buf, "Enslavement repulsed.\n");
        notify(playernum, governor, buf);
        sprintf(buf, "You needed more weapons bearing on the planet...\n");
        notify(playernum, governor, buf);
    }

    for (i = 1; i < MAXPLAYERS; ++i) {
        if (p->info[i - 1].numsectsowned && (i != playernum)) {
            warn(i, (int)Stars[s->storbits]->governor[i - 1], telegram_buf);
        }
    }

    free(p);
    free(s);
}
