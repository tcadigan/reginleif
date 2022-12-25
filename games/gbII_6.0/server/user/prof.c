/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (C) 1989-90 by Robert P, Chansky, et al.
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
 * prof.c -- Print out racial profile
 *
 * Tue Apr  9 16:14:58 MDT 1991 (Evan D. Koffler)
 *     Reformatted the profile and treasury command.
 *
 * #ident  "@(#)prof.c  1.16 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/prof.c,v 1.3 2007/07/06 18:09:34 gbp Exp $
 */
#include "prof.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../server/buffers.h"
#include "../server/client.h"
#include "../server/config.h"
#include "../server/GB_server.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/ranks.h"
#include "../server/ships.h"
#include "../server/shlmisc.h"
#include "../server/tweakables.h"
#include "../server/vars.h"

#include "csp_prof.h"
#include "shootblast.h"
#include "tele.h"

extern char *Desnames[];

extern void whois(int, int, int);
extern void profile(int, int, int, int, orbitinfo *);
extern char *Estimate_f(double, racetype *, int);
extern char *Estimate_i(int, racetype *, int);
extern int round_perc(int, racetype *, int);

static char est_buf[20];

void whois(int playernum, int governor, int apcount)
{
    int i;
    int j;
    int numraces;
    racetype *race;

    if (argn <= 1) {
        sprintf(args[1], "%d", playernum); /* The coward's way out */
        argn = 2;
    }

    numraces = Num_races;

    for (i = 1; i <= (argn - 1); ++i) {
        j = atoi(args[i]);

        if ((j >= 1) && (j <= numraces)) {
            race = races[j - 1];

            if (j == playernum) {
                sprintf(buf,
                        "[%2d, %d] %s \"%s\"\n",
                        j,
                        governor,
                        race->name,
                        race->governor[governor].name);
            } else {
                sprintf(buf, "[%2d] %s\n", j, race->name);
            }
        } else {
            sprintf(buf,
                    "Identify: Invalid player number #%d. Try again.\n",
                    j);
        }

        notify(playernum, governor, buf);
    }
}

void treasury(int playernum, int governor)
{
    racetype *race;
    int grep;

    race = races[playernum - 1];

#ifdef COLLECTIVE_MONEY
    grep = 0;

#else
    grep = governor;
#endif

    sprintf(buf,
            "Income last update was: %ld                Cost last update was: %ld\n",
            race->governor[grep].income + race->governor[grep].profit_market,
            race->governor[grep].maintain + race->governor[grep].cost_tech + race->governor[grep].cost_market);

    notify(playernum, governor, buf);

    sprintf(buf,
            "    Market: %5ld                               Market: %5ld\n",
            race->governor[grep].profit_market,
            race->governor[grep].cost_market);

    notify(playernum, governor, buf);

    sprintf(buf,
            "    Taxes:  %5ld                               Tech:   %5ld\n",
            race->governor[grep].income,
            race->governor[grep].cost_tech);

    notify(playernum, governor, buf);

    sprintf(buf,
            "                                               Maint:  %5ld\n",
            race->governor[grep].maintain);

    notify(playernum, governor, buf);
    sprintf(buf, "You have: %ld\n", MONEY(race, governor));
    notify(playernum, governor, buf);
}

void profile(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    int p;
    racetype *r;
    racetype *race;

    race = races[playernum - 1];

    if (client_can_understand(playernum, governor, CSP_PROFILE_INTRO)) {
        CSP_profile(playernum, governor, 0);
    } else {
        if ((argn == 1) || race->God) {
            if (race->God) {
                if (argn != 1) {
                    p = GetPlayer(args[1]);

                    if (!p) {
                        sprintf(buf, "Player does not exist.\n");
                        notify(playernum, governor, buf);

                        return;
                    }

                    race = races[p - 1];
                }
            }

            sprintf(buf,
                    "--==** Racial profile for %s (player %d) **==--\n",
                    race->name,
                    race->Playernum);

            notify(playernum, governor, buf);

            if (race->God) {
                sprintf(buf, "*** Deity Status ***\n");
                notify(playernum, governor, buf);
            }

            sprintf(buf, "Personal: %s\n", race->info);
            notify(playernum, governor, buf);

            sprintf(buf,
                    "Default Scope: /%s/%s\n",
                    Stars[race->governor[governor].homesystem]->name,
                    Stars[race->governor[governor].homesystem]->pnames[race->governor[governor].homeplanetnum]);

            notify(playernum, governor, buf);

            if (race->Gov_ship == 0) {
                sprintf(buf, "NO DESIGNATED CAPITOL!!!");
            } else {
                sprintf(buf, "Designated Capitol: #%-8d", race->Gov_ship);
            }

            notify(playernum, governor, buf);

            sprintf(buf,
                    "\t\tRanges:   guns:   %5.0f\n",
                    gun_range(race, (shiptype *)NULL, 1));

            notify(playernum, governor, buf);

            sprintf(buf,
                    "Morale: %5ld\t\t\t\t\t    space:  %5.0f\n",
                    race->morale,
                    tele_range(OTYPE_STELE, race->tech));

            notify(playernum, governor, buf);

            sprintf(buf,
                    "Updates active: %d\t\t\t\t      ground:%5.0f\n\n",
                    race->turn,
                    tele_range(OTYPE_GTELE, race->tech));

            notify(playernum, governor, buf);

            sprintf(buf,
                    "%s      Planet Conditions\t      Sector Preferences\n",
                    race->Metamorph ? "Metamorphic Race\t" : "Normal Race\t\t");

            notify(playernum, governor, buf);

            sprintf(buf,
                    "Fert:    %3d%%          Temp       %d\n",
                    race->fertilize,
                    Temp(race->conditions[TEMP]));

            notify(playernum, governor, buf);

            sprintf(buf,
                    "Rate:    %3.1f              methane  %5d%%           %-8.8s %c %3.0f%%\n",
                    race->birthrate,
                    race->conditions[METHANE],
                    Desnames[SEA],
                    CHAR_SEA,
                    race->likes[SEA] * 100.0);

            notify(playernum, governor, buf);

            sprintf(buf,
                    "Mass:   %4.2f            oxygen   %5d%%           %8.8s %c %3.0f%%\n",
                    race->mass,
                    race->conditions[OXYGEN],
                    Desnames[GAS],
                    CHAR_GAS,
                    race->likes[GAS] * 100.0);

            notify(playernum, governor, buf);

            sprintf(buf,
                    "Fight:     %d            helium   %5d%%           %-8.8s %c %3.0f%%\n",
                    race->fighters,
                    race->conditions[HELIUM],
                    Desnames[ICE],
                    CHAR_ICE,
                    race->likes[ICE] * 100.0);

            notify(playernum, governor, buf);

            sprintf(buf,
                    "Metab:  %4.2f            nitrogen %5d%%           %-8.8s %c %3.0f%%\n",
                    race->metabolism,
                    race->conditions[NITROGEN],
                    Desnames[MOUNT],
                    CHAR_MOUNT,
                    race->likes[MOUNT] * 100.0);

            notify(playernum, governor, buf);

            sprintf(buf,
                    "Sexes:     %1d            CO2      %5d%%           %-8.8s %c %3.0f%%\n",
                    race->number_sexes,
                    race->conditions[CO2],
                    Desnames[LAND],
                    CHAR_LAND,
                    race->likes[LAND] * 100.0);

            notify(playernum, governor, buf);

            sprintf(buf,
                    "Explore:  %-3.0f%%          hyrdogen %5d%%           %-8.8s %c %3.0f%%\n",
                    race->adventurism * 100.0,
                    race->conditions[HYDROGEN],
                    Desnames[DESERT],
                    CHAR_DESERT,
                    race->likes[DESERT] * 100.0);

            notify(playernum, governor, buf);

            sprintf(buf,
                    "Avg Int: %3d            sulfur   %5d%%           %-8.8s %c %3.0f%%\n",
                    race->IQ,
                    race->conditions[SULFUR],
                    Desnames[FOREST],
                    CHAR_FOREST,
                    race->likes[FOREST] * 100.0);

            notify(playernum, governor, buf);

            sprintf(buf,
                    "Tech:   %-6.2f          other    %5d%%           %-8.8s %c %3.0f%%\n",
                    race->tech,
                    race->conditions[OTHER],
                    Desnames[PLATED],
                    CHAR_PLATED,
                    race->likes[PLATED] * 100.0);

            notify(playernum, governor, buf);
            notify(playernum, governor, "Discoveries:");

            if (Hyper_drive(race)) {
                notify(playernum, governor, "  Hyperdrive");
            }

            if (Crystal(race)) {
                notify(playernum, governor, "  Crystals");
            }

            if (Atmos(race)) {
                notify(playernum, governor, "  Atmospheric");
            }

            if (Laser(race)) {
                notify(playernum, governor, "  Combat Lasers");
            }

            if (Wormhole(race)) {
                notify(playernum, governor, "  Wormhole");
            }

#ifdef USE_VN
            if (Vn(race)) {
                notify(playernum, governor, "  Von Neumann Machines");
            }
#endif

            if (Cew(race)) {
                notify(playernum, governor, "  Confined Energy Weapons");
            }

            if (Cloak(race)) {
                notify(playernum, governor, "  Cloaking");
            }

            if (Avpm(race)) {
                notify(playernum, governor, "  AVPM");
            }

            if (Tractor_beam(race)) {
                notify(playernum, governor, "  Tractor Beam");
            }

            if (Transporter(race)) {
                notify(playernum, governor, "  Transporter");
            }

            notify(playernum, governor, "\n");
        } else {
            p = GetPlayer(args[1]);

            if (!p) {
                sprintf(buf, "Player does not exist.\n");
                notify(playernum, governor, buf);

                return;
            }

            r = races[p - 1];
            sprintf(buf, "------ Race report on %s (%d) ------\n", r->name, p);
            notify(playernum, governor, buf);

            if (race->God) {
                if (r->God) {
                    sprintf(buf, "*** Deity Status ***\n");
                    notify(playernum, governor, buf);
                }
            }

            sprintf(buf, "Personal: %s\n", r->info);
            notify(playernum, governor, buf);
            sprintf(buf, "%%Know:  %3d%%\n", race->translate[p - 1]);
            notify(playernum, governor, buf);

            if (race->translate[p - 1] > 50) {
                sprintf(buf,
                        "%s\t  Planet Conditions\n",
                        r->Metamorph ? "Metamorphic Race" : "Normal Race\t");

                notify(playernum, governor, buf);
                sprintf(buf,
                        "Fert:    %s",
                        Estimate_i((int)r->fertilize, race, p));

                notify(playernum, governor, buf);

                sprintf(buf,
                        "\t\t  Temp:\t%s\n",
                        Estimate_i((int)r->conditions[TEMP], race, p));

                notify(playernum, governor, buf);

                sprintf(buf,
                        "Race:    %s%%",
                        Estimate_f(r->birthrate * 100.0, race, p));

                notify(playernum, governor, buf);
            } else {
                sprintf(buf, "Unknown Race\t\t  Planet Conditions\n");
                notify(playernum, governor, buf);

                sprintf(buf,
                        "Fert:    %s",
                        Estimate_i((int)r->fertilize, race, p));

                notify(playernum, governor, buf);

                sprintf(buf,
                        "\t\t  Temp:\t%s\n",
                        Estimate_i((int)r->conditions[TEMP], race, p));

                notify(playernum, governor, buf);
                sprintf(buf, "Rate:    %s", Estimate_f(r->birthrate, race, p));
                notify(playernum, governor, buf);
            }

            sprintf(buf,
                    "\t\t  methane  %4s%%\t\tRanges:\n",
                    Estimate_i((int)r->conditions[METHANE], race, p));

            notify(playernum, governor, buf);

            sprintf(buf, "Mass:    %s", Estimate_f(r->mass, race, p));
            notify(playernum, governor, buf);

            sprintf(buf,
                    "\t\t  oxygen   %4s%%",
                    Estimate_i((int)r->conditions[OXYGEN], race, p));

            notify(playernum, governor, buf);

            sprintf(buf,
                    "\t\t  guns:   %6s\n",
                    Estimate_f(gun_range(r, (shiptype *)NULL, 1), race, p));

            notify(playernum, governor, buf);
            sprintf(buf, "Fight:   %s", Estimate_i((int)r->fighters, race, p));
            notify(playernum, governor, buf);

            sprintf(buf,
                    "\t\t  helium   %4s%%",
                    Estimate_i((int)r->conditions[HELIUM], race, p));

            notify(playernum, governor, buf);

            sprintf(buf,
                    "\t\t  space:  %6s\n",
                    Estimate_f(tele_range(OTYPE_STELE, r->tech), race, p));

            notify(playernum, governor, buf);
            sprintf(buf, "Metab:   %s", Estimate_f(r->metabolism, race, p));
            notify(playernum, governor, buf);

            sprintf(buf,
                    "\t\t nitrogen %4s%%",
                    Estimate_i((int)r->conditions[NITROGEN], race, p));

            notify(playernum, governor, buf);

            sprintf(buf,
                    "\t\t  ground: %6s\n",
                    Estimate_f(tele_range(OTYPE_GTELE, r->tech), race, p));

            notify(playernum, governor, buf);

            sprintf(buf,
                    "Sexes:   %s",
                    Estimate_i((int)r->number_sexes, race, p));

            notify(playernum, governor, buf);

            sprintf(buf,
                    "\t\t  CO2      %4s%%\n",
                    Estimate_i((int)r->conditions[CO2], race, p));

            notify(playernum, governor, buf);

            sprintf(buf,
                    "Explore: %s%%",
                    Estimate_f(r->adventurism * 100.0, race, p));

            notify(playernum, governor, buf);

            sprintf(buf,
                    "\t\t  hydrogen %4s%%\n",
                    Estimate_i((int)r->conditions[HYDROGEN], race, p));

            notify(playernum, governor, buf);
            sprintf(buf, "Avg Int: %s", Estimate_i((int)r->IQ, race, p));
            notify(playernum, governor, buf);

            sprintf(buf,
                    "\t\t  sulfur   %4s%%\n",
                    Estimate_i((int)r->conditions[SULFUR], race, p));

            notify(playernum, governor, buf);
            sprintf(buf, "Tech:    %s", Estimate_f(r->tech, race, p));
            notify(playernum, governor, buf);

            sprintf(buf,
                    "\t\t  other    %4s%%",
                    Estimate_i((int)r->conditions[OTHER], race, p));

            notify(playernum, governor, buf);

            sprintf(buf,
                    "\t\tMorale:   %6s\n",
                    Estimate_i((int)r->morale, race, p));

            notify(playernum, governor, buf);

            sprintf(buf,
                    "Sector type preference : %s\n",
                    race->translate[p - 1] > 80 ? Desnames[r->likesbest] : " ? ");
            notify(playernum, governor, buf);
        }
    } /* csp check */
}

char *Estimate_f(double data, racetype *r, int p)
{
    int est;

    sprintf(est_buf, "?");

    if (r->translate[p - 1] > 10) {
        est = round_perc((int)data, r, p);

        if (est < 1000) {
            sprintf(est_buf, "%d", est);
        } else if (est < 10000) {
            sprintf(est_buf, "%.1fK", (double)est / 1000.0);
        } else if (est < 1000000) {
            sprintf(est_buf, "%.0fK", (double)est / 1000.0);
        } else {
            sprintf(est_buf, "%.1fM", (double)est / 1000000.0);
        }
    }

    return est_buf;
}

char *Estimate_i(int data, racetype *r, int p)
{
    int est;

    sprintf(est_buf, "?");

    if (r->translate[p - 1] > 10) {
        est = round_perc((int)data, r, p);

        if ((int)abs(est) < 1000) {
            sprintf(est_buf, "%d", est);
        } else if ((int)abs(est) < 10000) {
            sprintf(est_buf, "%.1fK", (double)est / 1000.0);
        } else if ((int)abs(est) < 1000000) {
            sprintf(est_buf, "%.1fM", (double)est / 1000.0);
        } else {
            sprintf(est_buf, "%.1fM", (double)est / 1000000.0);
        }
    }

    return est_buf;
}

int round_perc(int data, racetype *r, int p)
{
    int k;

    /* r->capture_prisoners[p - 1] is never actually used elsewhere */
#if 0
    k = 101 - MIN(r->translate[p - 1] + r->captured_prisoners[p - 1], 100);

#else
    k = 101 - MIN(r->translate[p - 1], 100);

#endif

    return ((data / k) * k);
}
