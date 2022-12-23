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
 * CSP, copyright (c) 1993 by John P. Teragon, Evan Koffler
 * print out the csp version of profile
 *
 * #ident  "@(#)csp_prof.c      1.4 12/9/93 "
 *
 * $Headers: /var/cvs/gbp/GB+/user/csp_prof.c,v 1.4 2007/07/06 18:09:34 gbp Exp
 * $
 */
#include "csp_prof.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../server/buffers.h"
#include "../server/config.h"
#include "../server/csp.h"
#include "../server/csp_types.h"
#include "../server/debug.h"
#include "../server/GB_server.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/ranks.h"
#include "../server/ships.h"
#include "../server/shlmisc.h"
#include "../server/tweakables.h"
#include "../server/vars.h"

#include "prof.h"
#include "shootblast.h"

extern char *Desnames[];

void CSP_profile(int playernum, int governor, int apcount)
{
    int p;
    racetype *r;
    racetype *race;
    enum PLAYER_TYPE RStatus = -1;
    enum RACE_TYPE RType;
    enum SECTOR_TYPES RSector;
    char defScope[50];

    race = races[playernum - 1];
    debug(LEVEL_CSP, "CSP: Entering CSP_profile\n");

    if ((argn == 1) || race->God) {
        debug(LEVEL_CSP_DETAILED,
              "CSP: Calling CSP_profile() as god or own race\n");

        if (race->God) {
            if (argn != 1) {
                p = GetPlayer(args[1]);

                if (!p) {
                    sprintf(buf, "Player does not exist.\n");
                    notify(playernum, governor, buf);

                    return;
                }

                race = races[p - 1];
            } else {
                RStatus = CSPD_DEITY;
            }
        } else if (race->Guest) {
            RStatus = CSPD_GUEST;
        } else {
            RStatus = CSPD_NORMAL;
        }

        if (race->Metamorph) {
            RType = CSPD_RACE_MORPH;
        } else {
            RType = CSPD_RACE_NORMAL;
        }

        sprintf(defScope,
                "%s/%s",
                Stars[race->governor[governor].homesystem]->name,
                Stars[race->governor[governor].homesystem]->pnames[race->governor[governor].homeplanetnum]);

        /* Print out the profile_intro */
        sprintf(buf,
                "%c %d %d %d %s\n",
                CSP_CLIENT,
                CSP_PROFILE_INTRO,
                race->Playernum,
                RStatus,
                race->name);

        notify(playernum, governor, buf);

        /* Send the stupid motto along */
        sprintf(buf,
                "%c %d %s\n",
                CSP_CLIENT,
                CSP_PROFILE_PERSONAL,
                race->info);

        notify(playernum, governor, buf);

        /*
         * Send the dynamic information, i.e.: The only stuff that changes is
         * sent here
         */
        if (race->Gov_ship) {
            sprintf(buf,
                    "%c %d %d %d %d %ld %d %d %d %s\n",
                    CSP_CLIENT,
                    CSP_PROFILE_DYNAMIC,
                    race->turn,
                    100,
                    race->Gov_ship,
                    race->morale,
                    (int)gun_range(race, NULL, 1),
                    (int)tele_range(OTYPE_STELE, race->tech),
                    (int)tele_range(OTYPE_GTELE, race->tech),
                    defScope);
        } else {
            sprintf(buf,
                    "%c %d %d %d %d %ld %d %d %d %s\n",
                    CSP_CLIENT,
                    CSP_PROFILE_DYNAMIC,
                    race->turn,
                    100,
                    0,
                    race->morale,
                    (int)gun_range(race, NULL, 1),
                    (int)tele_range(OTYPE_STELE, race->tech),
                    (int)tele_range(OTYPE_GTELE, race->tech),
                    defScope);
        }

        notify(playernum, governor, buf);

        /*
         * Send the race stats format Type, fert, birth, mass, fight, metab,
         * sexes, explore, tech, iq
         */
        sprintf(buf,
                "%c %d %d %d %f %f %d %f %d %d %f %f\n",
                CSP_CLIENT,
                CSP_PROFILE_RACE_STATS,
                RType,
                race->fertilize,
                race->birthrate,
                race->mass,
                race->fighters,
                race->metabolism,
                race->number_sexes,
                (int)(race->adventurism * 100.0),
                (float)race->tech,
                (float)race->IQ);

        notify(playernum, governor, buf);

        /* Send the planet conditions */
        sprintf(buf,
                "%c %d %d %d %d %d %d %d %d %d %d\n",
                CSP_CLIENT,
                CSP_PROFILE_PLANET,
                Temp(race->conditions[TEMP]),
                Temp(race->conditions[METHANE]),
                Temp(race->conditions[OXYGEN]),
                Temp(race->conditions[CO2]),
                Temp(race->conditions[HYDROGEN]),
                Temp(race->conditions[NITROGEN]),
                Temp(race->conditions[SULFUR]),
                Temp(race->conditions[HELIUM]),
                Temp(race->conditions[OTHER]));

        notify(playernum, governor, buf);

        /* Send the sector prefs */
        sprintf(buf,
                "%c %d %d %d %d %d %d %d %d %d\n",
                CSP_CLIENT,
                CSP_PROFILE_SECTOR,
                (int)(race->likes[SEA] * 100),
                (int)(race->likes[LAND] * 100),
                (int)(race->likes[MOUNT] * 100),
                (int)(race->likes[GAS] * 100),
                (int)(race->likes[ICE] * 100),
                (int)(race->likes[FOREST] * 100),
                (int)(race->likes[DESERT] * 100),
                (int)(race->likes[PLATED] * 100));

        notify(playernum, governor, buf);

        sprintf(buf,
                "%c %d %d %d %d %d %d %d %d %d %d %d %d",
                CSP_CLIENT,
                CSP_PROFILE_DISCOVERY,
                Hyper_drive(race),
                Crystal(race),
                Atmos(race),
                Laser(race),
                Wormhole(race),
                Vn(race),
                Cew(race),
                Cloak(race),
                Avpm(race),
                Tractor_beam(race),
                Transporter(race));

        notify(playernum, governor, buf);
        sprintf(buf, "%c %d\n", CSP_CLIENT, CSP_PROFILE_END);
        notify(playernum, governor, buf);
    } else {
        debug(LEVEL_CSP_DETAILED,
              "CSP: Calling CSP_profile() on another race\n");

        RStatus = CSPD_NORMAL;
        p = GetPlayer(args[1]);

        if (!p) {
            sprintf(buf, "Player does not exist.\n");
            notify(playernum, governor, buf);

            return;
        }

        r = races[p - 1];

        if (race->translate[p - 1] > 50) {
            if (race->Metamorph) {
                RType = CSPD_RACE_MORPH;
            } else {
                RType = CSPD_RACE_NORMAL;
            }
        } else {
            RType = CSPD_RACE_UNKNOWN;
        }

        if (race->God) {
            if (r->God) {
                RStatus = CSPD_DEITY;
            } else if (r->Guest) {
                RStatus = CSPD_GUEST;
            }
        } else {
            RStatus = CSPD_NORMAL;
        }

        /* Print out the profile_intro */
        sprintf(buf,
                "%c %d %d %d %s\n",
                CSP_CLIENT,
                CSP_PROFILE_INTRO,
                p,
                RStatus,
                r->name);

        notify(playernum, governor, buf);

        /* Send the stupid motto along */
        sprintf(buf,
                "%c %d %s\n",
                CSP_CLIENT,
                CSP_PROFILE_PERSONAL,
                r->info);

        notify(playernum, governor, buf);

        /*
         * Send the dynamic information, i.e.: The only stuff that changes is
         * sent here
         */
        if (race->translate[p - 1] > 80) {
            sprintf(buf,
                    "%c %d %d %d %d %d %d %s\n",
                    CSP_CLIENT,
                    CSP_PROFILE_DYNAMIC_OTHER,
                    race->translate[p - 1],
                    IntEstimate_i((double)r->morale, race, p),
                    IntEstimate_i(gun_range(race, NULL, 1), race, p),
                    IntEstimate_i(tele_range(OTYPE_STELE, r->tech), race, p),
                    IntEstimate_i(tele_range(OTYPE_GTELE, r->tech), race, p),
                    Desnames[r->likesbest]);
        } else {
            sprintf(buf,
                    "%c %d %d %d %d %d %d ????\n",
                    CSP_CLIENT,
                    CSP_PROFILE_DYNAMIC_OTHER,
                    race->translate[p - 1],
                    IntEstimate_i((double)r->morale, race, p),
                    IntEstimate_i(gun_range(race, NULL, 1), race, p),
                    IntEstimate_i(tele_range(OTYPE_STELE, r->tech), race, p),
                    IntEstimate_i(tele_range(OTYPE_GTELE, r->tech), race, p));
        }

        notify(playernum, governor, buf);

        /*
         * Send the race stats format Type, fert, birth, mass, fight, metab,
         * sexes, explore, tech, iq
         */
        sprintf(buf,
                "%c %d %d %d %f %f %d %f %d %d %f %f\n",
                CSP_CLIENT,
                CSP_PROFILE_RACE_STATS,
                RType,
                IntEstimate_i((double)r->fertilize, race, p),
                (float)IntEstimate_i(r->birthrate * 100.0, race, p),
                (float)IntEstimate_i(r->mass, race, p),
                (int)IntEstimate_i((double)r->fighters, race, p),
                (float)IntEstimate_i(r->metabolism, race, p),
                (int)IntEstimate_i((double)r->number_sexes, race, p),
                (int)IntEstimate_i(r->adventurism * 100.0, race, p),
                (float)IntEstimate_i(r->tech, race, p),
                (float)IntEstimate_i((double)r->IQ, race, p));

        notify(playernum, governor, buf);

        /* Send the planet conditions */
        sprintf(buf,
                "%c %d %d %d %d %d %d %d %d %d %d\n",
                CSP_CLIENT,
                CSP_PROFILE_PLANET,
                IntEstimate_i((double)r->conditions[TEMP], race, p),
                IntEstimate_i((double)r->conditions[METHANE], race, p),
                IntEstimate_i((double)r->conditions[OXYGEN], race, p),
                IntEstimate_i((double)r->conditions[CO2], race, p),
                IntEstimate_i((double)r->conditions[HYDROGEN], race, p),
                IntEstimate_i((double)r->conditions[NITROGEN], race, p),
                IntEstimate_i((double)r->conditions[SULFUR], race, p),
                IntEstimate_i((double)r->conditions[HELIUM], race, p),
                IntEstimate_i((double)r->conditions[OTHER], race, p));

        notify(playernum, governor, buf);

        if (race->translate[p -1] > 80) {
            RSector = r->likesbest;
        } else {
            RSector = CSPD_SECTOR_UNKNOWN;
        }

        /* Sent the sector prefs */
        sprintf(buf,
                "%c %d %d %d %d %d %d %d %d %d\n",
                CSP_CLIENT,
                CSP_PROFILE_SECTOR,
                (int)RSector == SEA ? 100 : 0,
                (int)RSector == LAND ? 100 : 0,
                (int)RSector == MOUNT ? 100 : 0,
                (int)RSector == GAS ? 100 : 0,
                (int)RSector == ICE ? 100 : 0,
                (int)RSector == FOREST ? 100 : 0,
                (int)RSector == DESERT ? 100 : 0,
                (int)RSector == PLATED ? 100 : 0);

        notify(playernum, governor, buf);

        /*
         * -mfw
         * sprintf(buf,
         *         "%c %d %d %d %d %d %d\n",
         *         CSP_CLIENT,
         *         CSP_PROFILE_DISCOVERY,
         *         NULL,
         *         NULL,
         *         NULL,
         *         NULL,
         *         NULL);
         */

        sprintf(buf,
                "%c %d %d %d %d %d %d\n",
                CSP_CLIENT,
                CSP_PROFILE_DISCOVERY,
                0,
                0,
                0,
                0,
                0);

        notify(playernum, governor, buf);
        sprintf(buf, "%c %d\n", CSP_CLIENT, CSP_PROFILE_END);
        notify(playernum, governor, buf);
    }

    debug(LEVEL_CSP, "CSP: Leaving CSP_profile()\n");
}

int IntEstimate_i(double data, racetype *r, int p)
{
    int est;

    if (r->translate[p - 1] > 10) {
        est = round_perc((int)data, r, p);

        if ((int)abs(est) < 1000) {
            return est;
        } else if ((int)abs(est) < 10000) {
            return (est / 1000.0);
        } else if ((int)abs(est) < 1000000) {
            return (est / 1000.0);
        } else {
            return (est / 1000000.0);
        }
    }

    return 0;
}
