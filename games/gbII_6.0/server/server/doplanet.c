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
 * doplanet.c -- Do one turn on a planet.
 *
 * #ident  "@(#)doplanet.c        1.14 11/5/93 "
 *
 * $Header: /var/cvs/gbp/GB+/server/doplanet.c,v 1.6 2007/07/06 17:14:07 gbp Exp $
 *
 * static char *ver = "@(#)        $RCSfile: doplanet.c,v $ $Revision: 1.6 $";
 */
#include "doplanet.h"

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

extern long Shipdata[NUMSTYPES][NUMABILS];
int Sectormappos;

/* Types of accidents that can occur on a toxic planet. */
char const *accidents_uninhab[] = {
    /* On water sectors */
    "Widespread waste spill",
    "Ecological instability",
    /* On land sectors */
    "Massive volcanic eruptions",
    "Ecological instability",
    /* On mountain sectors */
    "Massive volcanic eruptions",
    "Ecological instability",
    /* On gas sectors */
    "Lethal toxin concentration",
    "Ecological instability",
    /* On ice sectors */
    "Leak in isolated chemical plant",
    "Continental warming cause glacial melting",
    /* On plate sectors */
    "Nuclear accident",
    "Untended nuclear planet explodes"
};

char const *accident_inhab[] = {
    "Nuclear accident",
    "Terrorists trigger nuclear weapon",
    "Release of toxic waste",
    "Weapons dump explosion",
    "Massive starvation",
    "Virus epidemic",
    "Famine",
    "Starvation",
    "Widespread cultist suicide",
    "Atomic experiment gone wrong",
    "Great Hrung collapse"
};

/* Few macros to make life easier. (kse) */
#define has_fuel(S, P, A)                                               \
    (((S)->fuel + ((S)->use_stock * (P)->info[(S)->owner - 1].fuel)) >= (A))

#define has_res(S, P, A)                                                \
    (((S)->resource + ((S)->use_stock * (P)->info[(S)->owner - 1].resource)) >= (A))

#define put_OOF_msg(S)                          \
    if (!(S)->notified) {                       \
        (S)->notified = 1;                      \
        msg_OOF((S));                           \
    }

int doplanet(int, planettype *, int);
int moveship_onplanet(shiptype *, planettype *);
void terraform(shiptype *, planettype *);
void do_quarry(shiptype *, planettype *);
void do_recover(planettype *, int, int);
double est_production(sectortype *);

/* New code by Kharush. Prevent stockpile overflow in planets. */
void check_overflow(planettype *, int, int, unsigned char, int);

/* New code by kse to break up code to more manageable chunks. */
int autoscrap(shiptype *, planettype *, sectortype *);
int do_wplant(shiptype *, planettype *);
int hop_dome(shiptype *, planettype *);
int hop_terra(shiptype *, planettype *);
int hop_plow(shiptype *, planettype *);
int usep_fuel(shiptype *, planettype *, int);
int usep_res(shipttype *, planettype *, int);
int ship_err(shiptime *, int);

int doplanet(int starnum, planettype *planet, int planetnum)
{
    char *nukem;
    sectortype *p;
    shiptype *ship;
    double fadd;
    int shipno;
    int x;
    int y;
    int i;
    int nukx = -1;
    int nukey = -1;
    int o = 0;
    int timer = 20;
    int max_fuel = 0;
    unsigned char allmod = 0;
    unsigned char allexp = 0;
    float totatmo = 0.0;
    short totgases = 0;
    /* CWL */
    shiptype *seti;
    int tpsh;
    int setihere;
    float seticrew;
    float setimaxcrew;
    float setidamage;
    float setimult;
    int maxrate;
    int rate;
    /* End CWL */

    Sectormappos = planet->sectormappos;

    /*
     * No one's here now
     *
     * if (!(Stars[starnum]->inhabited[0] + Stars[starnum]->inhabited[1])) {
     *     return 0;
     * }
     */

    getsmap(Smap, planet);
    PermuteSects(planet);
    memset(Sectinfo, 0, sizeof(Sectinfo));
    memset(avg_mob, 0, sizeof(avg_mob));
    memset(sects_gained, 0, sizeof(sects_gained));
    memset(sects_lost, 0, sizeof(sects_lots));
    memset(prod_res, 0, sizeof(prod_res));
    memset(prod_fuel, 0, sizeof(prod_fuel));
    memset(prod_destruct, 0, sizeof(prod_destruct));
    memset(prod_crystals, 0, sizeof(prod_crystals));
    tot_captured = 0;
    prod_mob = tot_captured;
    prod_eff = prod_mob;
    tot_resdep = prod_eff;
    Claims = 0;

    planet->maxpopn = 0;
    planet->popn = 0; /* Initialize population for recount */
    planet->troops = 0;
    planet->total_resources = 0;

    /* Reset global variables */
    for (i = 1; i <= Num_races; ++i) {
        Compat[i - 1] = compatibility(planet, races[i - 1]);
        planet->info[i - 1].numsectsowned = 0;
        planet->info[i - 1].troops = 0;
        planet->info[i - 1].popn = 0;
        planet->info[i - 1].est_production = 0.0;
        prod_crystals[i - 1] = 0;
        prod_fuel[i - 1] = 0;
        prod_destruct[i - 1] = 0;
        prod_res[i - 1] = 0;
        avg_mob[i - 1] = 0;
    }

    /* Fix any atmosphere discrepancies -mfw */
    for (i = METHANE; i <= OTHER; ++i) {
        totatmo += planet->conditions[i];
    }

    if (totatmo > 100) {
        for (i = METHANE; i <= HELIUM; ++i) {
            planet->conditions[i] = (short)(((float)planet->conditions[i] / totatmo) * 100);
            totgases += planet->conditions[i];
        }

        planet->conditions[OTHER] = 100 - totgases;
    }

    shipno = planet->ships;

    while (shipno) {
        if (!valid_ship(shipno)) {
            break;
        }

        ship = ships[shipno];

        if (ship->alive && !ship->rad) {
            /*
             * Planet level functions - do these here beause they use the sector
             * map or affect planet production
             */
            switch (ship->type) {
            case OTYPE_TERRA:
                if (ship->on && landed(ship) && ship->popn) {
                    if (has_fuel(ship, planet, FUEL_COST_TERRA)) {
                        terraform(ship, planet);
                    } else {
                        put_OOF_msg(ship);
                    }
                }

                break;
            case OTYPE_INF:
                /* All infrastructure functions handled in do_inf() -mfw */
                do_inf(ship, planet);

                break;
            case OTYPE_PLOW:
                if (ship->on && landed(ship) && ship->popn) {
                    if (has_fuel(ship, planet, FUEL_COST_PLOW)) {
                        do_plow(ship, planet);
                    } else {
                        put_OOF_msg(ship);
                    }
                } else {
                    ship_err(ship, 'K');
                }

                break;
            case OTYPE_DOME:
                if (ship->on && landed(ship) && ship->popn) {
                    if (has_res(ship, planet, RES_COST_DOME)) {
                        do_dome(ship, planet);
                    } else {
                        sprintf(buf,
                                "Y%d does not have enough resources.",
                                ship->number);

                        push_telegram(ship->owner, ship->governor, buf);
                    }
                } else {
                    ship_err(ship, 'Y');
                }

                break;
            case OTYPE_REFINERY:
                /* CWL added code for refineries -mfw */
                maxrate = (int)(races[ship->owner - 1]->tech / 2.0);
                rate = round_rand((float)ship->resource / (float)RES_COST_REFINERY);
                rate = MIN(maxrate, rate);

                if (!ship->damage && (ship->resource > 0)) {
                    if (landed(ship)) {
                        if ((int)((float)rate * RES_COST_REFINERY) > (int)ship->resource) {
                            use_resource(ship, ship->resource);
                        } else {
                            use_resource(ship, (short)((float)rate * RES_COST_REFINERY));
                        }

                        prod_fuel[ship->owner - 1] += rate;

                        /* Now, increase planetary temperature and pollution */
                        if (Stinfo[ship->storbits][ship->pnumorbits].temp_add > 99) {
                            Stinfo[ship->storbits][ship->pnumorbits].temp_add = 100;
                        } else {
                            Stinfo[ship->storbits][ship->pnumorbits].temp_add += int_rand(0, 2);
                        }

                        /* And the pollution */
                        planet->conditions[TOXIC] += (short)sqrt((double)rate);

                        if (planet->conditions[TOXIC] > 100) {
                            planet->conditions[TOXIC] = 100;
                        }
                    } /* If landed */
                } /* If ship is able to produce */

                break; /* End CWL */
            case OTYPE_WPLANT:
                if (landed(ship) && ship->popn) {
                    prod_destruct[ship->owner - 1] += do_wplant(ship, planet);
                } else {
                    ship_err(ship, 'W');
                }

                break;
            case OTYPE_QUARRY:
                if (ship->on && landed(ship) && ship->popn) {
                    if (has_fuel(ship, planet, FUEL_COST_QUARRY)) {
                        do_quarry(ship, planet);
                    } else {
                        put_OOF_msg(ship);
                    }
                } else {
                    ship_err(ship, 'q');
                }

                break;
#ifdef USE_VN
            case OTYPE_VN:
            case OTYPE_BERS:
                planet_do_vn(ship, planet);

                break;
#endif
#ifdef USE_AMOEBA
            case OTYPE_AMOEBA:
                amoeba_planet(ship, planet);

                break;
#endif
            }

            if (ship->type == OTYPE_FACTORY) {
                max_fuel = Shipdata[ship->type][ABIL_FUELCAP];
            } else {
                max_fuel = ship->max_fuel;
            }

            /* Add fuel for ships orbiting a gas giant */
            if (!landed(ship) && (planet->type == TYPE_GASGIANT)) {
                switch (ship->type) {
                case STYPE_TANKER:
                    fadd = (int)(max_fuel / 5);

                    break;
                case STYPE_HABITAT:
                    fadd = (int)(max_fuel / 5); /* Was 200 -mfw */

                    break;
                default:
                    fadd = (int)(max_fuel / 8);

                    break;
                }

                fadd = MIN((double)max_fuel - ship->fuel, fadd);
                rcv_fuel(ship, fadd);
            }
        }

        shipno = nextship(ship);
    }

    /*
     * No one's explored the planet
     *
     * if (!Stinfo[starnum][planetnum].inhab) {
     *     return 0;
     * }
     */

    /*
     * Check for space mirrors (among other things) warming the planet. if a
     * change in any artificial warming/cooling trends
     */
    planet->conditions[TEMP] = planet->conditions[RTEMP]
        + Stinfo[starnum][planetnum].temp_add + int_rad(-5, 5);

    /* This is set to 0 at the end of each turn -mfw */
    if (planet->slaved_to) {
        planet->sheep = 1;
    }

    Getxysect(planet, &x, &y, 1);

    while (Getxysect(planet, &x, &y, 0)) {
        p = &Sector(*planet, x, y);

        if (p->owner && (p->popn || p->troops)) {
            allmod = 1;

            if (!Stars[starnum]->nova_stage) {
                produce(Stars[starnum], planet, p);

                if (p->owner) {
                    planet->info[p->owner - 1].est_production += est_production(p);
                }

                spread(planet, p, x, y);
            } else {
                /* Damage sector from supernova */
                ++p->resource;
                p->fert *= 0.8;

                if (Stars[starnum]->nova_stage == MAX_NOVA) {
                    p->troops = 0;
                    p->owner = p->troops;
                    p->popn = p->owner;

                    /* Need a chance that sector is wasted -mfw */
                    if (int_rand(0, 5) > 5) {
                        p->conditions = WASTED;
                    }
                } else {
                    /* p->popn = round_rand((double)p->popn * 0.5); */
                    /*
                     * Need a different rate of popn loss based on nova_stage
                     * -mfw
                     */
                    p->popn = round_rand((double)p->popn * ((100 / MAX_NOVA) / 100));
                }
            }

            Sectinfo[x][y].done = 1;
        }

        if ((!p->popn && !p->troops) || !p->owner) {
            p->owner = 0;
            p->troops = 0;
            p->popn = p->troops;
        }

        /*
         * if (p->wasted) {
         *     if ((x > 1) && (x < (planet->Maxx - 2))) {
         *         if ((p->des == DES_SEA) || (p->des == DES_GAS)) {
         *             if ((y > 1)
         *                 && (y < (planet->Maxy - 2))
         *                 && (!(p - 1)->wasted || !(p + 1)->wasted)
         *                 && !(gb_rand() % 5)) {
         *                 p->wasted = 0;
         *             } else if ((p->des == DES_LAND)
         *                        || (p->des == DES_MOUNT)
         *                        || (p->des == DES_ICE)) {
         *                 if ((y > 1)
         *                     && (y < (planet->Maxy - 2))
         *                     && ((p - 1)->popn || (p + 1)->popn)
         *                     && !(gb_rand() % 10)) {
         *                         p->wasted = 0;
         *                 }
         *             }
         *         }
         *     }
         * }
         *
         * if (Stars[starnum]->nova_stage) {
         *     if (p->des == DES_ICE) {
         *         if (gb_rand() % 2 == 1) {
         *             p->des = DES_LAND;
         *         } else if (p->des == DES_SEA) {
         *             if (gb_rand() %2 == 1) {
         *                 if (((x > 0) && ((p - 1)->des == DES_LAND))
         *                     || ((x < (planet->Maxx - 1))
         *                         && ((p + 1)->des == DES_LAND))
         *                     || ((y > 0) && ((p - planet->Maxx)->des == DES_LAND))
         *                     || ((y < (planet->Maxy - 1))
         *                         && ((p + planet->Maxx)->des == DES_LAND))) {
         *                     p->des = DES_LAND;
         *                     p->troops = 0;
         *                     p->owner = p->troops;
         *                     p->popn = p->owner;
         *                     p->resource += int_rand(1, 5);
         *                     p->fert = int_rand(1, 4);
         *                 }
         *             }
         *         }
         *     }
         * }
         */
    }

    Getxysect(planet, &x, &y, 1);

    while (Getxysect(planet, &x, &y, 0)) {
        p = &Sector(*planet, x, y);

        if (p->owner) {
            ++planet->info[p->owner - 1].numsectsowned;
        }
    }

    if (planet->expltimer >= 1) {
        --planet->expltimer;
    }

    if (!Stars[starnum]->nova_stage && !planet->expltimer) {
        if (!planet->expltimer) {
            planet->expltimer = 5;
        }

        for (i = 1; !Claims && !allexp && (i <= Num_races); ++i) {
            /* Sectors have been modified for this player */
            if (planet->info[i - 1].numsectsowned) {
                while(!Claims && !allexp && (timer > 0)) {
                    timer -= 1;
                    o = 1;
                    Getxysect(planet, &x, &y, 1);

                    while (!Claims && Getxysect(planet, &x, &y, 0)) {
                        /* Find out if all sectors have been explored */
                        o &= Sectinfo[x][y].explored;
                        p = &Sector(*planet, x, y);

                        if (((Sectinfo[x][y].explored == i)
                             && !(gb_rand() % 2 == 0))
                            && (!p->owner
                                && (p->condition != WASTED)
                                && (p->condition == races[i - 1]->likesbest))) {
                            /* Explorations have found an island */
                            Claims = i;
                            p->popn = races[i = 1]->number_sexes;
                            p->owner = i;
                            tot_captured = 1;
                        } else {
                            explore(planet, p, x, y, i);
                        }
                    }

                    allexp |= 0; /* All sectors explored for this player */
                }
            }
        }
    }

    if (allexp) {
        planet->expltimer = 5;
    }

    /* Environment nukes a random sector */
    if (planet->conditions[TOXIC] > ENVIR_DAMAGE_TOX) {
        nukex = int_rand(0, (int)planet->Maxx - 1);
        nukey = int_rand(0, (int)planet->Maxy - 1);
        p = &Sector(*planet, nukex, nukey);
        p->condition = WASTED;

        /* Index into accident type array */
        if (p->popn + p->troops) {
            nukem = accidents_inhab[int_rand(0, sizeof(accidents_inhab) / sizeof(char *))];
        } else {
            nukem = accidents_uninhab[(p->type * 2) + (gb_rand() % 2)];
        }

        p->troops = 0;
        p->owner = p->troops;
        p->popn = p->owner;
    }

    for (i = 1; i <= Num_races; ++i) {
        if (sects_gained[i - 1] || sects_lost[i - 1]) {
            sprintf(telegram_buf,
                    "****** Report: Planet /%s/%s ******\n",
                    Stars[starnum]->name,
                    Stars[starnum]->pnames[planetnum]);

            sprintf(buf,
                    " WAR STATUS: %d sectors gained, %d sectors lost.\n",
                    sects_gained[i - 1],
                    sects_lost[i - 1]);

            strcat(telegram_buf, buf);
            push_telegram(i,
                          (int)Stars[starnum]->governor[i - 1],
                          telegram_buf);
        }
    }

    for (i = 1; i < Num_races; ++i) {
        planet->info[i - 1].prod_crystals = prod_crystals[i - 1];
        planet->info[i - 1].prod_res = prod_res[i - 1];
        planet->info[i - 1].prod_fuel = prod_fuel[i - 1];
        planet->info[i - 1].prod_dest = prod_destruct[i - 1];

        if (planet->info[ i - 1].autorep) {
            --planet->info[i - 1].autorep;

            sprintf(telegram_buf,
                    "\nFrom /%s/%s\n",
                    Stars[starnum]->name,
                    Stars[starnum]->pnames[planetnum]);

            if (Stinfo[starnum][planetnum].temp_add) {
                sprintf(buf,
                        "Temp: %d to %d\n",
                        planet->conditions[RTEMP],
                        planet->conditions[TEMP]);

                strcat(telegram_buf, buf);
            }

            sprintf(buf,
                    "Total      Prod: %ldr %df %ldd\n",
                    prod_res[i - 1],
                    prod_fuel[i - 1],
                    prod_destruct[i - 1]);

            strcat(telegram_buf, buf);

            if (prod_crystals[i - 1]) {
                sprintf(buf, "    %ld crystals found\n", prod_crystals[i - 1]);
                strcat(telegram_buf, buf);
            }

            if (tot_captured) {
                sprintf(buf, "%ld sectors captured\n", tot_captured);
                strcat(telegram_buf, buf);
            }

            if (Stars[starnum]->nova_stage) {
                sprintf(buf,
                        "This planet's primary is in a Stage %d nova.\n",
                        Stars[starnum]->nova_stage);

                strcat(telegram_buf, buf);
            }

            /* Remind the player that he should clean up the environment. */
            if (planet->conditions[TOXIC] > ENVIR_DAMAGE_TOX) {
                sprintf(buf,
                        "Environment damage on sector %d,%d\n",
                        nukex,
                        nukey);

                strcat(telegram_buf, buf);
            }

            if (planet->slaved_to) {
                sprintf(buf, "ENSLAVED to player %d\n", planet->slaved_to);
                strcat(telegram_buf, buf);
            }

            push_telegram(i, Stars[starnum]->governor[i - 1], telegram_buf);
        }
    }

    /* Find out who is on this planet, for nova notification */
    if (Stars[starnum]->nova_stage == 1) {
        sprintf(telegram_buf,
                "BULLETIN form /%s/%s\n",
                Stars[starnum]->name,
                Stars[starnum]->pnames[planetnum]);

        sprintf(buf, "\nStar %s is undergoing nova.\n", Stars[starnum]->name);
        strcat(telegram_buf, buf);

        if ((planet->type == TYPE_EARTH)
            || (planet->type == TYPE_WATER)
            || (planet->type == TYPE_FOREST)) {
            strcat(buf, "Seas and rivers are boiling!\n");
            strcat(telegram_buf, buf);
        }

        sprintf(buf,
                "This planet must be evacuated immediately!\n%s",
                TELEG_DELIM);

        strcat(telegram_buf, buf);

        for (i = 1; i <= Num_races; ++i) {
            if (planet->info[i -1].numsectsowned) {
                push_telegram(i, Stars[starnum]->governor[i - 1], telegram_buf);
            }
        }
    }

    do_recover(planet, starnum, planetnum);
    planet->popn = 0;
    planet->troops = 0;
    planet->maxpopn = 0;
    planet->total_resources = 0;

    for (i = 1; i <= Num_races; ++i) {
        planet->info[i - 1].numsectsowned = 0;
        planet->info[i - 1].popn = 0;
        planet->info[i - 1].troops = 0;
    }

    Getxysect(planet, &x, &y, 1);

    while (Getxysect(planet, &x, &y, 0)) {
        p = &Sector(*planet, x, y);

        if (p->owner) {
            ++planet->info[p->owner - 1].numsectsowned;
            planet->info[p->owner - 1].troops += p->troops;
            planet->info[p->owner - 1].popn += p->popn;
            planet->popn += p->popn;
            planet->troops += p->troops;
            planet->maxpopn += maxsupport(races[p->onwer - 1],
                                          p,
                                          Compat[p->owner - 1],
                                          planet->conditions[TOXIC]);
            Power[p->owner - 1].troops += p->troops;
            Power[p->owner - 1].popn += p->popn;
            Power[p->owner - 1].sum_eff += p->ff;
            Power[p->owner - 1].sum_mob += p->mobilization;
            starpopns[starnum][p->owner - 1] += p->popn;
        } else {
            p->popn = 0;
            p->troops = 0;
        }

        planet->total_resource += p->resource;
    }

    /* Deal with enslaved planets */
    if (planet->slaved_to) {
        /*
         * HUTm (kse) military is also counted when checking stability of
         * enslavement.
         *
         * if (planet->info[planet->slaved_to - 1].popn > (planet->popn / 1000)) {
         */
        if ((planet->info[planet->slaved_to - 1].troops + planet->info[planet->slaved_to - 1].popn) > (planet->popn / 1000)) {
            for (i = 1; i < Num_races; ++i) {
                /* Add production to slave holder planet */
                if (planet->info[i - 1].numsectsowned) {
                    /*
                     * New code by Kharush. Let's be sure that added production
                     * does not cause stockpile overflow.
                     */
                    check_overflow(planet,
                                   planet->slaved_to,
                                   prod_fuel[i - 1],
                                   FUEL,
                                   starnum);

                    prod_fuel[i - 1] = 0;

                    check_overflow(planet,
                                   planet->slaved_to,
                                   prod_destruct[i - 1],
                                   DESTRUCT,
                                   starnum);

                    prod_destruct[i - 1] = 0;

                    check_overflow(planet,
                                   planet->slaved_to,
                                   prod_destruct[i - 1],
                                   RESOURCE,
                                   starnum);

                    prod_res[i - 1] = 0;

                    /*
                     * Old code
                     *
                     * planet->info[planet->slaved_to - 1].resource += prod_res[i - 1];
                     * prod_res[i - 1] = 0;
                     * planet->info[planet->slaved_to - 1].fuel += prod_fuel[i - 1];
                     * prod_fuel[i - 1] = 0;
                     * planet->info[planet->slaved_to - 1].destruct += prod_destruct[i - 1];
                     * prod_destruct = 0;
                    */
                }
            }
        } else {
            /* Slave revolt! */
            /* First nuke some random sectors from the revolt */
            i = (planet->popn / 1000) + 1;
            --i;
            while (i) {
                p = &Sector(*planet,
                            int_rand(0, (int)planet->Maxx - 1),
                            int_rand(0, (int)planet->Maxy - 1));

                if (p->popn + p->troops) {
                    p->troops = 0;
                    p->popn = p->troops;
                    p->owner = p->popn;
                    p->condition = WASTED;
                }

                --i;
            }

            /* Now nuke all sectors belonging to former master */
            Getxysect(planet, &x, &y, 1);

            while (Getxysect(planet, &x, &y, 0)) {
                if (Stinfo[starnum][planetnum].intimidated && (gb_rand() % 2 == 1)) {
                    p = &Sector(*planet, x, y);

                    if (p->owner == planet->slaved_to) {
                        p->owner = 0
                        p->popn = 0;
                        p->troops = 0;
                        p->condition = WASTED;
                    }
                }

                /* Also add up the population while here */
            }

            sprintf(telegram_buf,
                    "\nThere has been a SLAVE REVOLT on /%s/%s!\n",
                    Stars[starnum]->name,
                    Stars[starnum]->pnames[planetnum]);

            strcat(telegram_buf, buf);

            sprintf(buf,
                    "All population belonging to planet #%d on the planet have been killed!\n",
                    planet->slaved_to);

            strcat(telegram_buf, buf);
            strcat(buf, "Productions now go to their rightful owners.\n");
            strcat(telegram_buf, buf);

            for (i = 1; i <= Num_races; ++i) {
                if (planet->info[i - 1].numsectsowned) {
                    /* Send to previous slave holders */
                    push_telegram(i,
                                  (int)Stars[starnum]->governor[i - 1],
                                  telegram_buf);
                }
            }

            planet->slaved_to = 0;
            planet->sheep = 0;
        }
    }

    /* Add production to all people here */
    for (i = 1; i <= Num_races; ++i) {
        if (planet->info[i - 1].numsectsowned) {
            if (TAX_PROTEST) {
                /* CWL Can the people tolerate this tax rate??? */
                /* Fixed this up a bit. -mfw */
                double targetr;
                float pptax; /* Percent per tax */
                int mloss;

                targetr = 100.0 * morale_factor(races[i - 1]->morale);

                if (planet->info[i - 1].popn
                    && ((planet->info[i - 1].tax > targetr)
                        || ((planet->info[i - 1].tax > (targetr - 10))
                            && (int_rand(1, 100) > targetr)))) {
                    pptax = planet->info[i - 1].tax - targetr;

                    /* The people get grumbly */
                    sprintf(telegram_buf,
                            "The people of %s/%s protest your unfair taxes! (%.1f%%)",
                            Stars[starnum]->name,
                            Stars[starnum]->pnames[planetnum],
                            pptax);

                    push_telegram(i,
                                  (int)Stars[starnum]->governor[i - 1],
                                  telegram_buf);

                    mloss = int_rand(1, (int)((2 * planet->info[i - 1].tax) / 5));
                    mloss = MIN(mloss, planet->info[i - 1].popn);
                    races[i - 1]->morale -= mloss;

                    if (int_rand(1, targetr) <= planet->info[t - 1].tax) {
                        /* Strikes occur! */
                        int wcommod;

                        /* Can the troops put it down? */
                        targetr = (int)(((float)planet->info[i - 1].troops * (10.0 + morale_factor(races[i - 1]->morale)) / planet->info[i - 1].popn) * 100.0);

                        if (int_rand(1, 100) <= targetr) {
                            sprintf(telegram_buf,
                                    " Your troops move into position to stop possible strikes!");
                            /* Sadden the people more */
                            races[i - 1]->morale -= 5;
                        } else {
                            wcommand = int_rand(1, 3);

                            switch (wcommand) {
                            case 1:
                                sprintf(telegram_buf,
                                        " The fuel refiners have gone on strike!");
                                prod_fuel[i - 1] = int_rand(0, (int)(prod_fuel[i - 1] / 2));
                                break;
                            case 2:
                                sprintf(telegram_buf,
                                        " The miners have gone on strike!");
                                prod_res[i - 1] = int_rand(0, (int)(prod_res[i - 1] / 2));

                                break;
                            default:
                                sprintf(telegram_buf,
                                        " The weapon manufacturers have gone on strike!");
                                prod_destruct[i - 1] = int_rand(0, (int)(prod_destruct[i - 1] / 2));
                                break;
                            } /* End case */
                        }

                        push_telegram(i,
                                      (int)Stars[starnum]->governor[i - 1],
                                      telegram_buf);
                    } /* Strikes */
                } /* End CWL */
            } /* End TAX_PROTEST */

            /* New code by Kharush. */
            check_overflow(planet, i, prod_fuel[i - 1], FUEL, starnum);
            check_overflow(planet, i, prod_res[i - 1], RESOURCE, starnum);
            check_overflow(planet, 1, prod_destruct[i - 1], DESTRUCT, starnum);

            /*
             * Old code
             *
             * planet->info[i - 1].fuel += prod_fuel[i - 1];
             * planet->info[i - 1].resource += prod_resource[i - 1];
             * planet->info[i - 1].destruct += prod_destruct[i - 1];
             */

            planet->info[i - 1].crystals += prod_crystals[i - 1];

            /* Tax the population - set new tax rate when done */
            if (races[i - 1]->Gov_ship) {
                planet->info[i - 1].prod_money = round_rand(INCOME_FACTOR * (double)planet->info[i - 1].tax * (double)planet->info[i - 1].popn);
                MONEY(races[i - 1], Stars[starnum]->governor[i - 1]) += planet->info[i - 1].prod_money;
                planet->info[i - 1].tax += MIN((int)planet->info[i - 1].newtax - (int)planet->info[i - 1].tax, 5);
            } else {
                planet->info[i - 1].prod_money = 0;
            }

#ifdef COLLECTIVE_MONEY
            races[i - 1].governor[0].income += planet->info[i - 1].prod_money;

#else

            races[i - 1].governor[(unsigned int)Stars[starnum]->governor[i - 1]].income += planet->info[i - 1].prod_money;
#endif

            /* Do tech investments */
            if (races[i - 1]->Gov_ship) {
                if (MONEY(races[i - 1], Stars[starnum]->governor[i - 1]) >= planet->info[i - 1].tech_invest) {
                    /* CWL code to support setis */
                    int got;
                    float seti_fact = 0.0;
                    float nseti_fact = 0.0;

                    tpsh = planet->ships;
                    setidamage = 0;
                    seticrew = setidamage;
                    setimaxcrew = seticrew;
                    setihere = setimaxcrew;
                    got = 1;

                    while (tpsh && got) {
                        got = getship(&seti, tpsh);

                        if (got) {
                            if ((seti->type == OTYPE_SETI)
                                && seti->alive
                                && (seti->owner == i)) {
                                nset_fact = ((float)seti_popn / (float)set->max_crew) * ((float)(100 - seti->damage) / 100.0);
                                seti_fact += nseti_fact;
                            }

                            tpsh = nextship(seti);
                            free(seti);
                        }
                    }

                    setimult = 1.0 + (log10(1.0 + seti_fact) / 5);

                    /* End CWL */

                    planet->info[i - 1].prod_tech = tech_prod((int)(planet->info[i - 1].tech_invest), (int)(planet->info[i - 1].popn));
                    planet->info[i - 1].prod_tech *= setimult; /* SETI CWL */
                    MONEY(races[i - 1], Stars[starnum]->governor[i - 1]) -= planet->info[i - 1].tech_invest;
                    races[i - 1]->tech += planet->info[i - 1].prod_tech;

#ifdef COLLECTIVE_MONEY
                    races[i - 1]->governor[0].cost_tech += planet->info[i - 1].tech_invest;

#else

                    races[i - 1]->governor[(unsigned int)Stars[starnum]->governor[i - 1]].cost_tech += planet->info[i - 1].tech_invest;
                } else {
                    planet->info[i - 1].prod_tech = 0;
                }
            } else {
                planet->inf[i - 1].prod_tech = 0;
            }

            /* Build wc's if it's been ordered */
            while ((planet->info[i - 1].tox_thresh > 0)
                   && (planet->conditions[TOXIC] >= planet->info[i - 1].tox_thresh)
                   && (planet->info[i - 1].resource >= Shipcost(OTYPE_TOXWC, races[i - 1]))) {
                shiptype *s2;
                int t;
                int newwc;

                /* HUTm (kse) check if ship slots are all used up */
                if (!shipSlotsAvail()) {
                    sprintf(telegram_buf,
                            "Cannot build w at /%s/%s because all ship slots are used.\n",
                            Stars[starnum]->name,
                            Stars[starnum]->pnames[planetnum]);

                    push_telegram(i,
                                  Stars[starnum]->governor[i - 1],
                                  telegram_buf);

                    sprintf(buf, "doplanet: No free ship returned from getFreeShip()");

                    loginfo(ERRORLOG, WANTERRNO, buf);

                    break;
                }

                newwc = getFreeShip(OTYPE_TOXWC, i);

                if (!newwc) {
                    sprintf(telegram_buf,
                            "Cannot build w a /%s/%s because all ship slos are used.\n",
                            Stars[starnum]->name,
                            Stars[starnum]->pnames[planetnum]);

                    push_telegram(i,
                                  Stars[starnum]->governor[i - 1],
                                  telegram_buf);

                    sprintf(buf, "doplanet: No free ship returned from getFreeShip()");

                    loginfo(ERRORLOG, WANTERRNO, buf);

                    break;
                }

                if (newwc <= Num_ships) {
                    /*
                     *We have an available slot in the free_ship_list, use it
                     */
                    s2 = ships[newwc];
                } else {
                    /* Nothing in the free list, make a new one */
                    s2 = (shiptype *)malloc(sizeof(shiptype));

                    if (!s2) {
                        loginfo(ERRORLOG,
                                WANTERRNO,
                                "FATAL: Malloc failure [doplanet]");

                        exit(1);
                    }

                    /* Augment size of ships pointer */
                    ++Num_ships;

                    /* Reallocate the ship array making for the new WC */
                    ships = (shiptype **)realloc(ships, (Num_ships + 1) * sizeof(shiptype *));

                    /* Append the new WC to the ship array */
                    ships[Num_ships] = s2;
                    newwc = Num_ships;
                }

                Getship(s2, OTYPE_TOXWC, races[i - 1]);

                initialize_new_ship(i,
                                    Stars[starnum]->governor[i - 1],
                                    races[i - 1],
                                    s2,
                                    0.0,
                                    0,
                                    1);

                s2->number = newwc;

                /* HUTm (kse) set w's speed to max */
                s2->speed = Shipdata[OTYPE_TOXWC][ABIL_SPEED];
                s2->alive = 1;
                s2->active = 1;
                sprintf(s2->name, "Scum%04d", newwc);
                insert_sh_plan(planet, s2);
                s2->whatorbits = LEVEL_PLAN;
                s2->storbits = starnum;
                s2->pnumorbits = planetnum;
                s2->docked = 1;
                s2->xpos = Stars[starnum]->xpos + planet->xpos;
                s2->ypos = Stars[starnum]->ypos + planet->ypos;
                s2->land_x = int_rand(0, (int)planet->Maxx - 1);
                s2->land_y = int_rand(0, (int)planet->Maxy - 1);
                s2->whatdest = LEVEL_PLAN;
                s2->deststar = starnum;
                s2->destpnum = planetnum;
                s2->owner = i;
                s2->governor = Stars[starnum]->governor[i - 1];
                t = MIN(TOXMAX, planet->conditions[TOXIC]); /* Amt of tox */
                planet->conditions[TOXIC] -= t;
                s2->special.waste.toxic = t;
                putship(s2);
            }
        } /* if numsectsowned[i] */
    }

    if ((planet->maxpopn > 0) && (planet->conditions[TOXIC] < 100)) {
        planet->conditions[TOXIC] += (planet->popn / planet->maxpopn);
    }

    if (planet->conditions[TOXIC] > 100) {
        planet->conditions[TOXIC] = 100;
    } else if (planet->conditions[TOXIC] < 0) {
        planet->conditions[TOXIC] = 0;
    }

#ifdef THRESHOLDING
    /* CWL Thresholding done here! */
    shipno = planet->ships;

    while (shipno) {
        if (!valid_ship(shipno)) {
            shipno = 0;

            continue;
        }

        ship = ships[shipno];

        if ((ship != NULL)
            && ship->alive
            && !ship->rad
            && (ship->whatdest == LEVEL_PLAN)
            && landed(ship)
            && ((Shipdata[ship->type][ABIL_HASSWITCH] && ship_on)
                || !has_ship(ship))) {
            unsigned int amti;

            /* Merrill mod, to fix threshold */
            if ((ship->resource < ship->threshold[TH_RESOURCE])
                && ship->threshold[TH_RESOURCE]) {
                if ((ship->threshold[TH_RESOURCE] - ship->resource) > planet->info[ship->owner - 1].resource) {
                    amti = planet->info[ship->owner - 1].resource;
                } else {
                    amti = ship->threshold[TH_RESOURCE] - ship->resource;
                }

                planet->info[ship->owner - 1].resource -= amti;
                ship->resource += amti;

                /*
                 * sprintf(buf,
                 *         "%s: %c%d %s thresholded %dr.",
                 *         prin_ship_orbits(ship),
                 *         Shipltrs[ship->type],
                 *         shipno,
                 *         ship->name,
                 *         amti);
                 *
                 * push_message(TELEG_PLAYER_AUTO,
                 *              0,
                 *              (int)ship->owner,
                 *              (int)ship->locked_to,
                 *              buf,
                 *              TELEGRAM);
                 */
            }

            if ((ship->destruct < ship->threshold[TH_DESTRUCT])
                && ship->threshold[TH_DESTRUCT]) {
                if ((ship->threshold[TH_DESTRUCT] - ship->destruct) > planet->info[ship->owner - 1].destruct) {
                    amti = planet->info[ship-owner - 1].destruct;
                } else {
                    amti = ship->threshold[TH_DESTRUCT] - ship->destruct;
                }

                planet->info[ship->owner - 1].destruct -= amit;
                ship->destruct += amti;

                /*
                 * sprintf(buf,
                 *         "%s: %c%d %s thresholded %dd.",
                 *         prin_ship_orbits(ship),
                 *         Shipltrs[ship->type],
                 *         shipno,
                 *         ship->name,
                 *         amti);
                 *
                 * push_message(TELEG_PLAYER_AUTO,
                 *              0,
                 *              (int)ship->owner,
                 *              (int)ship->locked_to,
                 *              buf,
                 *              TELEGRAM);
                 */
            }

            if ((ship->fuel < ship->threshold[TH_FUEL])
                && ship->threshold[TH_FUEL]) {
                if ((ship->threshold[TH_FUEL] - ship->fuel) > planet->info[ship->owner - 1].fuel) {
                    amti = (int)planet->info[ship->owner - 1].fuel;
                } else {
                    amti = (int)(ship->threshold[TH_FUEL] - ship->fuel);
                }

                planet->info[ship->owner - 1].fuel -= amti;
                ship->fuel += (float)amti;

                /*
                 * sprintf(buf,
                 *         "%s: %c%d %s thresholded %df.",
                 *         prin_ship_orbits(ship),
                 *         Shipltrs[ship->type],
                 *         shipno,
                 *         ship->name,
                 *         amti);
                 *
                 * push_message(TELEG_PLAYER_AUTO,
                 *              0,
                 *              (int)ship->owner,
                 *              (int)ship->locked_to,
                 *              buf,
                 *              TELEGRAM);
                 */
            }

            if ((ship->crystals < ship->threshold[TH_CRYSTALS])
                && ship->threshold[TH_CRYSTALS]) {
                if ((ship->threshold[TH_CRYSTALS] - ship->crystals) > planet->info[ship->owner - 1].crystals) {
                    amti = planet->info[ship->owner - 1].crystals;
                } else {
                    amti = ship->threshold[TH_CRYSTALS] - ship->crystals;
                }

                planet->info[ship->owner - 1].crystals -= amti;
                ship->crystals += amti;

                /*
                 * sprintf(buf,
                 *         "%s: %c%d %s thresholded %dx.",
                 *         prin_ship_orbits(ship),
                 *         Shipltrs[ship->type],
                 *         shipno,
                 *         ship->name,
                 *         amti);
                 *
                 * push_message(TELEG_PLAYER_AUTO,
                 *              0,
                 *              (int)ship->owner,
                 *              (int)ship->locked_to,
                 *              buf,
                 *              TELEGRAM);
                 */
            }
        } /* End thresholding */

        shipno = nextship(ship);
    } /* End while shipno */

    /* End thresholding */
#endif

    for (i = 1; i <= Num_races; ++i) {
        Power[i - 1].resource += planet->info[i - 1].resource;
        Power[i - 1].destruct += planet->info[i - 1].destruct;
        Power[i - 1].fuel += planet->info[i - 1].fuel;
        Power[i - 1].sectors_owned += planet->info[i - 1].numsectsowned;
        Power[i - 1].planets_owned += !!planet->info[i - 1].numsectsowned;

        if (planet->info[i - 1].numsectsowned) {
            /* Combat readiness naturally moves towards the avg mobilization */
            planet->info[i - 1].mob_points = avg_mob[i - 1];
            avg_mob[i - 1] /= (int)planet->info[i - 1].numsectsowned;
            planet->info[i - 1].comread = avg_mob[i - 1];
        } else {
            planet->info[i - 1].comread = 0;
        }

        planet->info[i - 1].guns = planet_guns(planet->info[i - 1].mob_points);
    }

    return allmod;
}

int moveship_onplanet(shiptype *ship, planettype *planet)
{
    int x;
    int y;
    int bounced = 0;

    if (ship->class[ship->special.terraform.index + 1] != '\0') {
        ++ship->special.terraform.index;

        if ((ship->class[ship->special.terraform.index + 1] == '\0')
            && !ship->notified) {
            char teleg_buf[1000];

            ship->notified = 1;

            sprintf(teleg_buf,
                    "%s is out of orders at %s.",
                    Ship(ship),
                    prin_ship_orbits(ship));

            push_telegram((int)ship->owner, (int)ship->governor, teleg_buf);
        }
    } else if (bounced) {
        if (ship->class[ship->special.terraform.index] > '5') {
            ship->class[ship->special.terraform.index] += -6;
        } else {
            ship->class[ship->special.terraform.index] += 6;
        }
    }

    ship->land_x = x;
    ship->land+y = y;

    return 1;
}

void terraform(shiptype *ship, planettype *planet)
{
    sectortype *s;

    /* Move, and then terraform. */
    if (!moveship_onplanet(ship, planet)) {
        return;
    }

    s = &Sector(*planet, (int)ship->land_x, (int)ship->land_y);

    /*
     * HUTm (kse) if T is unterraformable sector it tries to find a suitable
     * sector
     */

    if (ship->hop && ((races[ship->owner - 1]->likes[s->condition]) >= (ship->limit / 100.0))) {
        if(hop_terra(ship, planet) == -1) {
            /* If T was scrapped not hopped */
            return;
        }  else {
            s = &Sector(*planet, (int)ship->land_x, (int)ship->land_y);
        }
    }

    if (s->condition == races[ship->owner - 1]->likesbest) {
        sprintf(buf,
                "T%d is full of zealots!!! (sector is already preferred type!)",
                ship->number);

        push_telegram(ship->owner, ship->governor, buf);
    } else {
        /* Gas planets have restrictions on what may be terraformed. */
        if ((planet->type == TYPE_GASGIANT)
            && ((races[ship->owner - 1]->likesbest != GAS)
                && (races[ship->owner - 1]->likesbest != SEA))) {
            sprintf(buf,
                    " T%d is trying to terraform into non-gas on a jovian planet (impossible).",
                    ship->number);

            push_telegram(ship->owner, ship->governor, buf);
        } else if ((planet->type != TYPE_GASGIANT)
                   && (races[ship->owner - 1]->likesbest == GAS)) {
            sprintf(buf,
                    " T%d is trying to terraform into gas on a non-jovian planet (impossible).",
                    ship->number);

            push_telegram(ship->owner, ship->governor, buf);
        } else {
            /*
             * HUT modification (kse): Terraformers use fuel also from planetary
             * stockpile
             */
            if (has_fuel(ship, planet, FUEL_COST_TERRA)) {
                if (success(((100 - (int)ship->damage) * ship->popn) / ship->max_crew)) {
                    /* Only condition can be terraformed, type doesn't change */
                    s->condition = races[ship->owner - 1]->likesbest;
                    s->eff = 0;
                    s->mobilization = 0;

                    /*
                     * HUT modification (kse): Terraformers won't kill
                     * inhabitants of terraformed sector and sector owner won't
                     * change
                     *
                     * s->troops = 0;
                     * s->popn = s->troops;
                     * s->owner = 0;
                     */

                    /*
                     * HUT Gardan 11.2.97
                     * Notify player that someone is terraforming player's
                     * sectors
                     */
                    if ((s->owner != ship->owner) && (s->owner > 0)) {
                        sprintf(buf,
                                "T%D[%d,%D] is terraforming your sector %d,%d on planet /%s/%s",
                                ship->number,
                                ship->owner,
                                ship->governor,
                                (int)ship->land_x,
                                (int)ship->land_y,
                                Stars[ship->storbits]->name,
                                Stars[ship->storbits]->pnames[ship->pnumorbits]);

                        push_telegram(s->owner, 0, buf);
                    }

                    /*
                     * HUT modification (kse): Terraformers use fuel from
                     * planetary stockpile if there is not enough fuel in ship
                     *
                     * use_fuel(ship, FUEL_COST_TERRA);
                     */
                    usep_fuel(ship, planet, FUEL_COST_TERRA);

                    if ((gb_rand() % 2 == 1) && (planet->conditions[TOXIC] < 100)) {
                        planet->conditions[TOXIC] += 1;
                    }
                }
            } else {
                put_OOF_msg(ship);
            }
        }
    }
}

void do_plow(shiptype *ship, planettype *planet)
{
    sectortype *s;
    int adjust;
    int hopped;

    if (!moveship_onplanet(ship, planet)) {
        return;
    }

    s = &Sector(*planet, (int)ship->land_x, (int)ship->land_y);

    if (!s->popn) {
        return;
    }

    if (s->owner != ship->owner) {
        return;
    }

    if (!races[ship->owner - 1]->likes[s->condition]) {
        sprintf(buf,
                "Plow [%d] can't work here [%d,%d\n",
                ship->number,
                ship->land_x,
                ship->land_y);

        push_telegram((int)ship->owner, (int)ship->governor, buf);

        return;
    }

    /*
     * HUTm (kse): If sector fert is 100 search min fert sector and move plow
     * there
     */
    if (ship->hop && (s->fert >= ship->limit)) {
        hopped = hop_plow(ship, planet);

        if (hopped < 0) {
            /* If plow was crapped not hopped */
            return;
        } else if (hopped) {
            s = &Sector(*planet, (int)ship->land_x, (int)ship->land_y);
        } else {
            sprintf(buf,
                    "No suitable sector for %c%d to hop to.",
                    Shipltrs[ship->type],
                    ship->number);

            push_telegram(ship->owner, ship->governor, buf);
        }
    }

    if (s->fert >= 100) {
        sprintf(buf,
                "%c%d is full of zealots!!! (sector fertility is 10%%)",
                Shipltrs[ship->type],
                ship->number);

        push_telegram(ship->owner, ship->governor, buf);

        return;
    }

    /* Make sure we have enough and we honor reserves */
    if (FUEL_COST_PLOW > (ship->fuel + (planet->info[ship->owner - 1].fuel - planet->info[ship->owner - 1].fuel_reserve))) {
        sprintf(buf, "Plow [%d]: Not enough free fuel.\n", ship->number);

        return;
    } else if {
        /*
         * HUT modification (kse): Spaceplows use fuel from planetary stockpile
         */
        adjust = round_rand((10 * (0.01 * (100 - (double)ship->damage) * (double)ship->popn)) / ship->max_crew);

        s->fert = MIN(100, s->fert + adjust);

        /*
         * HUT modification (kse): Space plows use fuel also from planetary
         * stockpile
         */
        usep_fuel(ship, planet, FUEL_COST_PLOW);

        if ((gb_rand() % 2 == 1) && (planet->conditions[TOXIC] < 100)) {
            planet->conditions[TOXIC] += 1;
        }
    } else {
        /* put_OOF_msg(ship); - mfw */
        sprintf(buf,
                "Not enough fuel (%d) for Plow [%d]\n",
                planet->info[ship->owner - 1].fuel,
                ship->number);

        push_telegram((int)ship->owner, (int)ship->governor, buf);

        return;
    }

    if (ship->inf.wants_reports) {
        sprintf(buf,
                "Plow #%d increased fert by %d%% to %d%%\n",
                ship->number,
                adjust,
                s->fert);
    }
}

void do_dome(shiptype *ship, planettype *planet)
{
    sectortype *s;
    int adjust;

    s = &Sector(*planet, (int)ship->land_x, (int)ship->land_y);

    /*
     * HUTm (kse): If sector efficiency is 100, dome moves to sector of lowest
     * eff
     */
    if (ship->hop && (s->eff >= ship->limit)) {
        /* Get new sector */
        if (hop_dome(ship, planet)) {
            s = &Sector(*planet, (int)ship->land_x, (int)ship->land_y);
        } else {
            sprintf(buf,
                    "No suitable sector for %c%d to hop to.",
                    Shipltrs[ship->type],
                    ship->number);

            push_telegram(ship->owner, ship->governor, buf);
        }
    }

    if (!s->popn) {
        return;
    }

    if (s->owner != ship->owner) {
        return;
    }

    if ((s->eff >= 100) && !ship->autoscrap) {
        sprintf(buf,
                "%c%d is full of zealots!!! (Sector efficiency is already 100%%",
                Shipltrs[ship->type],
                ship->number);

        push_telegram(ship->owner, ship->governor, buf);

        return;
    }

#ifdef AUTOSCRAP
    if (ship->autoscrap && (s->eff == 100)) {
        /* Autoscrap this ship */
        autoscrap(ship, planet, s);

        return;
    }
#endif

    /* Make sure we have enough and we honor reserves */
    if (RES_COST_DOME > (ship->resource + (planet->info[ship->owner - 1].resource - planet->info[ship->owner - 1].res_reserve))) {
        sprintf(buf, "Dome [%d]: Not enough free resources.\n", ship->number);
        push_telegram((int)ship->owner, (int)ship->governor, buf);

        return;
    }

    /*
     * HUT modification (kse): Domes also use resources from planetary stockpile
     */
    usep_res(ship, planet, RES_COST_DOME);

    adjust = round_rand((0.05 * (100 - (double)ship->damage) * (double)ship->popn) / ship->max_crew);

    s->eff += adjust;

    if (s->eff > 100) {
        s->eff = 100;
    }

    if (ship->inf.wants_reports) {
        sprintf(buf,
                "Dome #%d increased eff by %d%% to %d%%\n",
                ship->number,
                adjust,
                s->eff);

        push_telegram((int)ship->owner, (int)ship->governor, buf);
    }
}

void do_quarry(shiptype *ship, planettype *planet)
{
    sectortype *s;
    int prod;
    int tox;

    s = &Sector(*planet, (int)ship->land_x, (int)ship->land_y);

    /* Factor 5 added to following formula /HUT Gardan 23.1.97 */
    prod = round_rand((5 * races[ship->owner - 1].metabolism * (double)ship->popn) / (double)ship->max_crew);

    usep_fuel(ship, planet, FUEL_COST_QUARRY);
    prod_res[ship->owner - 1] += prod;
    tox = int_rad(0, int_rand(0, prod));
    planet->conditions[TOXIC] = MIN(100, planet->conditions[TOXIC] + tox);

    if (s->fert >= prod) {
        s->fert -= prod;
    } else {
        s->fert = 0;
        /* Nuke the sector only when fertility goes to 0 /Gardan */
        s->condition = WASTED;
    }
}

void do_recover(planettype *planet, int starnum, int planetnum)
{
    int owners = 0;
    int i;
    int j;
    int ownerbits[2];
    int stolenres = 0;
    int stolendes = 0;
    int stolenfuel = 0;
    int stolencrystals = 0;
    int all_buddies_here = 1;

    ownerbits[1] = 0;
    ownerbits[0] = ownerbits[1];

    for (i = 1; (i < Num_races) && all_buddies_here; ++i) {
        if (planet->info[i - 1].numsectsowned > 0) {
            ++owners;
            setbit(ownerbits, i);

            for (j = 1; (j < i) && all_buddies_here; ++j) {
                if (isset(ownerbits, j)
                    && (!isset(races[i - 1]->allied, j)
                        || !isset(races[j - 1]->allied, i))) {
                    all_buddies_here = 0;
                }
            }
        } else { /* Player i owns no sectors */

            /* Can't steal from God */
            if (i != 1) {
                stolenres += planet->inf[i - 1].resource;
                stolendes += planet->info[i - 1].destruct;
                stolenfuel += planet->fuel[i - 1].fuel;
                stolencrystals += planet->info[i - 1].crystals;
            }
        }
    }

    if (all_buddies_here
        && (owners != 0)
        && ((stolenres > 0)
            || (stolendes > 0)
            || (stolenfuel > 0)
            || (stolencrystals > 0))) {
        /* Okay, we've got some loot to divvy up */
        int shares = owners;
        int res;
        int des;
        int fuel;
        int crystals;
        int givenres = 0;
        int givendes = 0;
        int givenfuel = 0;
        int givencrystals = 0;

        for (i = 1; i < Num_races; ++i) {
            if (isset(ownerbits, 1)) {
                sprintf(telegram_buf,
                        "Recovery Report: Planet /%s/%s\n",
                        Stars[starnum]->name,
                        Stars[starnum]->pnames[planetnum]);

                push_telegram(i,
                              (int)Stars[starnum]->governor[i - 1],
                              telegram_buf);

                sprintf(telegram_buf,
                        "%-14.14s %5s %5s %5s %5s\n",
                        "",
                        "res",
                        "destr",
                        "fuel",
                        "xtal");

                push_telegram(i,
                              (int)Stars[starnum]->governor[i - 1],
                              telegram_buf);
            }
        }

        /* First: Give the loot to the conquerors */
        for (i = 1; (i < Num_race) && (owners > 1); ++i) {
            /* We have a winner! */
            if (isset(ownerbits, i)) {
                res = round_rand((double)stolenres / shares);

                if ((res + givenres) > stolenres) {
                    res = stolenres - givenres;
                }

                des = round_rand((double)stolendes / shares);

                if ((des + givendes) > stolendes) {
                    des = stolendes - givendes;
                }

                fuel = round_rand((double)stolenfuel / shares);

                if ((fuel + givenfuel) > stolenfuel) {
                    fuel = stolenfuel - givenfuel;
                }

                crystals = round_rand((double)stolencrystals / shares);

                if ((crystals + givencrystals) > stolencrystals) {
                    crystals = stolencrystals - givencrystals;
                }

                /*
                 * New code by Kharush. Killing another player off should not
                 * cause stockpile overflow! If the player can not hold all he
                 * gets he throws the rest away. Why to give to another player?
                 */
                check_overflow(planet, i, fuel, FUEL, starnum);
                givenfuel += fuel;
                check_overflow(planet, i, des, DESTRUCT, starnum);
                givendes += des;
                check_overflow(planet, i, res, RESOURCE, starnum);
                givenres += res;

                /*
                 * Old code
                 *
                 * planet->info[i - 1].resource += res;
                 * givenres += res;
                 * planet->info[i - 1].destruct += des;
                 * givendes += des;
                 * planet->info[i - 1].fuel += fuel;
                 * givenfuel += fuel;
                 */

                planet->info[i - 1].crystals += crystals;
                givencrystals += crystals;
                --owners;

                sprintf(telegram_buf,
                        "%-14.14s %5d %5d %5d %5d",
                        races[i - 1]->name,
                        res,
                        des,
                        fuel,
                        crystals);

                for (j = 1; j <= Num_races; ++j) {
                    if (isset(ownerbits, j)) {
                        push_telegram(j,
                                      (int)Stars[starnum]->governor[j - 1],
                                      telegram_buf);
                    }
                }
            }
        }

        /* Leftovers for last player */
        while (i <= Num_races) {
            if (isset(ownerbits, i)) {
                break;
            }

            ++i;
        }

        /* It should be */
        if (i <= Num_races) {
            res = stolenres - givenres;
            des = stolendes - givendes;
            fuel = stolenfuel - givenfuel;
            crystals = stolencrystals - givencrystals;

            /* New code by Kharush. */
            check_overflow(planet, i, fuel, FUEL, starnum);
            check_overflow(planet, i, des, DESTRUCT, starnum);
            check_overflow(planet, i, res, RESOURCE, starnum);

            /*
             * Old code
             *
             * planet->info[i - 1].resource += res;
             * planet->info[i - 1].destruct += des;
             * planet->info[i - 1].fuel += fuel;
             */

            planet->info[i - 1].crystals += crystals;

            sprintf(telegram_buf,
                    "%-14.14s %5d %5d %5d %5d",
                    races[i - 1]->name,
                    res,
                    des,
                    fuel,
                    crystals);

            sprintf(buf,
                    "%-14.14s %5d %5d %5d %5d\n",
                    "Total:",
                    stolenres,
                    stolendes,
                    stolenfuel,
                    stolencrystals);

            for (j = 1; j <= Num_races; ++j) {
                if (isset(ownerbits, j)) {
                    push_telegram(j,
                                  (int)Stars[starnum]->governor[j - 1],
                                  telegram_buf);

                    push_telegram(j, (int)Stars[starnum]->governor[j - 1], buf);
                }
            }
        } else {
            push_telegram(1, 0, "Bug in stealing resources\n");
        }

        /* Next: Take all the loot away from the losers */
        for (i = 2; i <= Num_races; ++i) {
            if (!isset(ownerbits, i)) {
                planet->info[i - 1].resource = 0;
                planet->info[i - 1].destruct = 0;
                planet->info[i - 1].fuel = 0;
                planet->info[i - 1].crystals = 0;
            }
        }
    }

    planet->sheep = 0;
}

double est_production(sectortype *s)
{
    /* Mod by SKF -- Added by JPD */
    double est_sec_prod;

    dest_sec_prod = (races[s->owner - 1]->metabolism * (double)s->eff * (double)s->eff) / 200.0;

    if (set_sec_prod > s->resource) {
        return (double)s->resource;
    } else {
        return est_sec_prod;
    }
}

void check_overflow(planettype *planet,
                    int playernum,
                    int amount,
                    unsigned char commodity,
                    int starnum)
{
    /* Function by Kharush. */
    switch (commodity) {
    case FUEL:
        if ((planet->info[playernum - 1].fuel + amount) <= USHRT_MAX) {
            planet->inf[playernum - 1].fuel  += amount;
        } else {
            planet->info[playernum - 1].fuel = USHRT_MAX;

            sprintf(buf,
                    "Star %s, planetary stockpile limit in fuel reached!\nMilitary happily launched extra fuel to outer space.\n",
                    Stars[starnum]->name);

            push_telegram(playernum,
                          Stars[starnum]->governor[playernum - 1],
                          buf);
        }

        break;
    case DESTRUCT:
        if ((planet->info[playernum - 1].destruct + amount) <= USHRT_MAX) {
            planet->info[playernum - 1].destruct += amount;
        } else {
            planet->info[playernum - 1].destruct = USHRT_MAX;

            sprintf(buf,
                    "Star %s, planetary stockpile limit in destruct reached!\nMilitary happily launched extra destruct to outer space.\n",
                    Stars[starnum]->name);

            push_telegram(playernum,
                          Stars[starnum]->governor[playernum - 1],
                          buf);
        }

        break;
    case RESOURCE:
        if ((planet->info[playernum - 1].resource + amount) <= USHRT_MAX) {
            planet->info[playernum - 1].resource += amount;
        } else {
            planet->info[playernum - 1].resource = USHRT_MAX;

            sprintf(buf,
                    "Star %s, planetary stockpile limit in resources reached!\nMilitary happily launched extra resources to outer space.\n",
                    Stars[starnum]->name);

            push_telegram(playernum,
                          Stars[starnum]->governor[playernum - 1],
                          buf);
        }

        break;
    default:
        sprintf(telegram_buf,
                "Alien commodity found in: Star /%s\n",
                Stars[starnum]->name);

        push_telegram(playernum,
                      (int)Stars[starnum]->governor[playernum - 1],
                      telegram_buf);

        break;
    }
}

int autoscrap(shiptype *ship, planettype *planet, sectortype *s)
{
    char *stype;

    s->popn += ship->popn;
    s->troops += ship->troops;
    planet->popn += ship->popn;
    planet->troops += ship->troops;

    if (ship->type == OTYPE_FACTORY) {
        planet->info[ship->owner - 1].resource +=
            ((2 * ship->build_code * ship->on) + Shipdata[ship->type][ABIL_COST]);
    } else {
        planet->info[ship->owner - 1].resource += s->build_cost;
    }

    planet->info[ship->owner - 1].destruct += ship->destruct;
    planet->info[ship->owner - 1].fuel += (int)ship->fuel;
    planet->into[ship->owner - 1].crystals += ship->crystals;

    switch (ship->type) {
    case OTYPE_TERRA:
        stype = "Terraformer";

        break;
    case OTYPE_PLOW:
        stype = "Plow";

        break;
    case OTYPE_DOME:
        stype = "Dome";

        break;
    case OTYPE_AP:
        stype = "Atmosphere Processor";

        break;
    default:
        stype = "unknown";

        break;
    }

    sprintf(buf,
            "%s: %s #%d autoscrapped.",
            prin_ship_orbits(ship),
            stype,
            ship->number);

    push_telegram(ship->owner, ship->governor, buf);
    kill_ship(ship->owner, ship);

    return 1;
}

int do_wplant(shiptype *ship, planettype *planet)
{
    unsigned int fuse;
    unsigned int ruse;
    int rate = 0;

    ruse = planet->info[ship->owner - 1].resource;
    fuse = planet->info[ship->owner - 1].fuel;
    rate = do_weapon_plant(ship, &ruse, &fuse);

    if (ruse) {
        planet->info[ship->owner - 1].resource -= ruse;
    }

    if (fuse) {
        planet->info[ship->owner - 1].fuel -= rate;
    }

    if (rate == 0) {
        sprintf(buf, "W%d does not have enough raw materials.", ship->number);
        push_telegram(ship->owner, ship->governor, buf);
    }

    return rate;
}

int hop_dome(shiptype *ship, planettype *planet)
{
    sectortype *sn;
    int effmin = 100;
    int mx;
    int my;
    int x;
    int y;
    double eqeff = 1.0;

    my = -1;
    mx = -1;

    for (x = 0; x < planet->Maxx; ++x) {
        for (y = 0; y < planet->Maxy; ++y) {
            sn = &Sector(*planet, x, y); /* Get next sector */

            /* Check owner */
            if (sn->owner == ship->owner ) {
                /* If sect eff == effmin... */
                if (sn->eff == effmin) {
                    /*
                     * ...distribute hops equally...between sector of same
                     * efficiency
                     */
                    if (double_rand() < (1.0 / eqeff)) {
                        effmin = 100;
                        eqeff = 10.0;
                    } else {
                        eqeff += 1.0;
                    }
                }

                /* If sn has lower efficiency than sectors before it go there */
                if (sn->eff < effmin) {
                    effmin = sn->eff;
                    mx = x;
                    my = y;
                }
            }
        }
    }

    /* If suitable sector is found move the dome there */
    if (mx != -1) {
        ship->land_x = mx;
        ship->land_y = my;

        return 1;
    }

    return 0;
}

int hop_terra(shiptype *ship, planettype *planet)
{
    sectortype *sn;
    int tx = -1;
    int ty = -1;
    int x;
    int y;
    int ns = 0;
    int ts;

    /* First player's own sectors are searched */
    for (x = 0; (x < planet->Maxx) && (tx == -1); ++x) {
        for (y = 0; (y < planet->Maxy) && (ty == -1); ++y) {
            sn = &sector(*planet, x, y);

            if (races[ship->owner - 1]->likes[sn->conditions] < (ship->limit / 100.0)) {
                if (sn->owner == ship->owner) {
                    tx = x;
                    ty = y;
                } else if (!sn->owner) {
                    /* Number of unowned sectors which could be terraformed */
                    ++ns;
                }
            }
        }
    }

    /* If suitable sector wasn't found unowned sectors (if any) are searched */
    if ((tx == -1) && ns) {
        /* Move the T to ts'th  sector found */
        ts = int_rand(1, ns);

        for (x = 0; (x < planet->Maxx) && (tx == -1); ++x) {
            for (y = 0; (y < planet->Maxy) && (ty == -1); ++y) {
                sn = &Sector(*planet, x, y);

                if ((races[ship->owner - 1]->likes[sn-<conditions] < (ship->limit / 100.0)) && !sn->owner) {
                    if (ts = ns) {
                        tx = x;
                        ty = y;
                    } else {
                        --ns;
                    }
                }
            }
        }
    }

    /* T is moved if suitable sector is found */
    if (tx != -1) {
        ship->land_x = tx;
        ship->land_y = ty;

        return 1;
    }

#ifdef AUTOSCRAP
    if ((tx == -1) && ship->autoscrap) {
        autoscrap(ship,
                  planet,
                  &Sector(*planet, (int)ship->land_x, (int)ship->land_t));

        return -1;
    }
#endif

    return 0;
}

int hop_plow(shiptype *ship, planettype *planet)
{
    sectortype *sn;
    int mx = -1;
    int my = -1;
    int x;
    int y;
    int fertmin = 100;
    int n_oth = 0;
    double eqeff = 1.0;

    for (x = 0; x < planet->Maxx; ++x) {
        for (y = 0; y < planet->Maxy; ++y) {
            sn = &sector(*planet, x, y);

            if (races[ship->owner - 1]->likes[sn->condition]) {
                if (sn->owner == ship->owner) {
                    if (sn->fert == fertmin) {
                        if (double_rand() < (1.0 / eqeff)) {
                            eqeff = 1.0;
                            fertmin = 100;
                        } else {
                            eqeff += 1.0;
                        }
                    }

                    if (sn->fert < fertmin) {
                        fertmin = sn->fert;
                        mx = x;
                        my = y;
                    }
                } else {
                    ++n_oth; /* Number of sector not owned by player */
                }
            }
        }
    }

    /* If suitable sector wasn't found search from all sectors */
    if ((mx == -1) && n_oth) {
        for (x = 0; x < planet->Maxx; ++x) {
            for (y = 0; y < planet->Maxy; ++y) {
                sn = &Sector(*planet, x, y);

                if (races[ship->owner - 1]->likes[sn->condition]) {
                    if (sn->fert == fertmin) {
                        if (double_rand() < (1.0 / eqeff)) {
                            eqeff = 1.0;
                            fertmin = 100;
                        } else {
                            eqeff + 1= 1.0;
                        }
                    }

                    if (sn->fert < fertmin) {
                        fertmin = sn->fert;
                        mx = x;
                        my = y;
                    }
                }
            }
        }
    }

    if (mx != -1) {
        ship->land_x = mx;
        ship->land_y = my;

        return 1;
    }

#fidef AUTOSCRAP
    /* If K can't hop and sect fert is 100 autoscrap it */
    if (mx == -1) {
        sn = &Sector(*planet, (int)ship->land_x, (int)ship->land_y);

        if (ship->autoscrap && (sn->fert >= 100)) {
            autoscrap(ship, planet, sn);

            return -1;
        }
    }
#endif

    return 0;
}

int usep_fuel(shiptype *ship, planettype *planet, int amt)
{
    if ((ship->fuel < amt) && ship->use_stock) {
        if ((ship->fuel + planet->info[ship->owner - 1].fuel) < amt) {
            return INF_NO_FUEL;
        }

        planet->info[ship->owner - 1].fuel -= (amt - (int)ship->fuel);
        use_fuel(ship, (int)ship->fuel);
    } else {
        if (ship->fuel < amt) {
            return INF_NO_FUEL;
        }

        use_fuel(ship, amt);
    }

    return 1;
}

int usep_res(shiptype *ship, planettype *planet, int amt)
{
    if ((ship->resource < amt) && ship->use_stock) {
        if ((ship->resource + planet->info[ship->owner - 1].resource) <= amt) {
            return INF_NO_RES;
        }

        planet->info[ship->owner - 1].resource -= (amt - ship->resource);
        use_resource(ship, (int)ship->resource);
    } else {
        if (ship->resource < amt) {
            return INF_NO_RES;
        }

        use_resource(ship, amt);
    }

    return 1;
}

int ship_err(shiptype *ship, int l)
{
    if (!landed(ship)) {
        sprintf(buf, "%c%d is not landed.", l, ship->number);
        push_telegram(ship->owner, ship->governor, buf);
    }

    if (!ship->popn) {
        sprintf(buf, "%c%d has no crew.", l, ship->number);
        push_telegram(ship->owner, ship->governor, buf);
    }

    if ((ship->type != OTYPE_WPLANT) && !ship->on) {
        sprintf(buf, "%c%d is not switched on.", l, ship->number);
        push_telegram(ship->owner, ship->governor, buf);
    }

    return 1;
}

void do_inf(shiptype *ship, planettype *planet)
{
    racetype *race;
    shiptype *nship;
    sectortype *sect;
    double difference;
    char itemp[256];
    int amt_avail;
    int shipno;
    int x;
    int y;
    int j;
    int atmo_cnt = 0;
    int dome_cnt = 0;
    int plow_cnt = 0;
    int low_eff = 0;
    int low_fert = 0;
    int built_atmo = 0;
    int built_dome = 0;
    int built_plow = 0;
    int done_w_atmo = 0;
    int done_w_dome = 0;
    int done_w_plow = 0;

    /* If the infrastructure ready? */
    if (!ship->on || !landed(ship)) {
        if (ship->wants_reports) {
            sprintf(buf, "Infrastructure [%d] is off.\n", ship->number);
            push_telegram((int)ship->owner, (int)ship->governor, buf);
        }

        return;
    }

    /* Does it have available fuel? */
    if (has_fuel(ship, planet, FUEL_COST_INF)) {
        usep_fuel(ship, planet, FUEL_COST_INF);
    } else {
        if (ship->wants_reports) {
            sprintf(buf,
                    "Not enough fuel available for Infrastructure [%d]\n",
                    ship->number);

            push_telegram((int)ship->owner, (int)ship->governor, buf);
        }

        return;
    }

    if (ship->inf.wants_reports) {
        sprintf(buf, "Infrastructure #%d Report:\n", ship->number);
        sprintf(itemp,
                "%15s| Efficiency setting = %d\n",
                " ",
                ship->inf.eff_setting);

        strcat(buf, itemp);

        sprintf(itemp,
                "%15s| Fertility setting = %d\n",
                " ",
                ship->inf.fert_setting);

        strcat(buf, itemp);
    }

    shipno = planet->ships;

    while (shipno) {
        if (valid_ship(shipno)) {
            nship = ships[shipno];

            if (nship->alive && (nship->owner == ship->owner)) {
                switch (nship->type) {
                case OTYPE_AP:
                    nship->inf.wants_reports = ship->inf.wants_reports;
                    ++atmo_cnt;

                    break;
                case OTYPE_PLOW:
                    nship->inf.wants_reports = ship->inf.wants_reports;
                    ++plow_cnt;

                    break;
                case OTYPE_DOME:
                    nship->inf.wants_reports = ship->inf.wants_reports;
                    ++dome_cnt;

                    break;
                default:
                    break;
                }
            }
        }

        shipno = nextship(nship);
    }

    race = races[ship->owner - 1];

    /* Check the atmosphere, is it what we want? */
    for (j = METHANE; j <= OTHER; ++j) {
        difference = race->conditions[j] - planet->conditions[j];

        if (difference) {
            break;
        }
    }

    Getxysect(planet, &x, &y, 1);

    /* Loop through planet, find sector fert and eff */
    while (Getxysect(planet, &x, &y, 0)) {
        sector = &Sector(*planet, x, y);

        if (sect->owner) {
            if (sect->eff < ship->special.inf.max_eff) {
                ++low_eff;
            }

            if (sect->fert < ship->special.inf.max_fert) {
                ++low_fert;
            }
        }
    }

    amt_avail = planet->info[ship->owner - 1].resource * (ship->special.inf.spend_perc / 100);

    while (amt_avail && (!done_w_atmo || !done_w_plow || !done_w_dome)) {
        /* Handle build/scrap of APs */
        if (difference
            && (ship->inf.atmos_setting < atmo_cnt)
            && (amt_avail <= Shipdata[OTYPE_AP][ABIL_COST])) {
            /*
             * Find build location
             * Build atmo
             * Load crew
             * Order stockpile on
             * Switch on
             */

            ++atmo_cnt;
            ++built_atmo;
            amt_avail -= Shipdata[OTYPE_AP][ABIL_COST];
        } else if (!difference && atmo_cnt) {
            shipno = planet->ships;

            while (shipno) {
                if (valid_ship(shipno)) {
                    nship = ships[shipno];

                    if (nship->alive
                        && nship->on
                        && (nship->owner == ship->owner)
                        && (nship->type == OTYPE_AP)) {
                        nship->on = 0;

                        /* Scrap atmos */
                        if (ship->autoscrap) {
                            /* Autoscrap this ship */
                            sect = &Sector(*planet,
                                           (int)ship->land_x,
                                           (int)ship->land_y);

                            autoscrap(nship, planet, sect);
                            --built_atmo;
                        }
                    }
                }

                shipno = nextship(nship);
            }
        } else {
            done_w_atmo = 1;
        }

        /* Handle build/scrap of Plows */
        if ((low_fert > plow_cnt)
            && (ship->inf.fert_setting < low_cnt)
            && (amt_avail <= Shipdata[OTYPE_PLOW][ABIL_COST])) {
            /*
             * Find first empty target location
             * Build plow at target location
             * Load crew
             * Order move (direction 6)
             * Order limit
             * Order hop (default may not have to)
             * Order stockpile on
             * Order on
             */

            ++plow_cnt;
            ++built_plow;
            amt_avail -= Shipdata[OTYPE_PLOW][ABIL_COST];
        } else if (!low_fert && plow_cnt) {
            shipno = planet->ships;

            while (shipno) {
                if (valid_ship(shipno)) {
                    nship = ships[shipno];

                    if (nship->alive
                        && nship->on
                        && (nship->owner == ship->owner)
                        && (nship->type == OTYPE_PLOW)) {
                        nship->on = 0;

                        /* Scrap plows */
                        if (ship->autoscrap) {
                            /* Autoscrap this ship */
                            sect = &Sector(*planet,
                                           (int)ship->land_x,
                                           (int)ship->land_y);

                            autoscrap(nship, planet, sect);
                            --built_plow;
                        }
                    }
                }

                shipno = nextship(nship);
            }
        } else {
            done_w_plow = 1;
        }

        /* Handle build/scrap of Domes */
        if ((low_eff > dome_cnt)
            && (ship->inf.eff_setting < dome_cnt)
            && (amt_avail <= Shipdata[OTYPE_DOME][ABIL_COST])) {
            /*
             * Find first empty target location
             * Build dome at target location
             * Load crew
             * Order limit
             * Order hop (default may not have to)
             * order stockpile on
             * order on
             */

            ++dome_cnt;
            ++built_dome;
            amt_avail -= Shipdata[OTYPE_DOME][ABIL_COST];
        } else if (!low_eff && dome_cnt) {
            shipno = planet->ships;

            while (shipno) {
                if (valid_ship(shipno)) {
                    nship = ships[shipno];

                    if (nship->alive
                        && nship->on
                        && (nship->owner == ship->owner)
                        && (nship->type == OTYPE_DOME)) {
                        nship->on = 0;

                        /* Scrap domes */
                        if (ship->autoscrap) {
                            /* Autoscrap this ship */
                            sect = &Sector(*planet,
                                           (int)ship->land_x,
                                           (int)ship->land_y);

                            autoscrap(nship, planet, sect);
                            --built_dome;
                        }
                    }
                }

                shipno = nextship(nship);
            }
        } else {
            done_w_dome = 1;
        }
    }
}

int is_target(shiptype *ship, int x, int y, int mode)
{
    int i;
    struct inf_setting *infs;

    for (i = 0; i < INF_MAX_TARGETS; ++i) {
        if (mode) {
            infs = &ship->inf.eff_targets[i];
        } else {
            infs = &ship->inf.fert_targets[i];
        }

        if ((infs->x == x) && (infs->y == y)) {
            return 1;
        }
    }

    return 0;
}

void squeeze_targets(shiptype *ship)
{
    int i;
    int f;
    struct inf_setting *infs1;
    struct inf_setting *infs2;

    /* Eff targets */
    f = 0;

    for (i = 0; i < (INF_MAX_TARGETS - 1); ++i) {
        infs1 = &ship->inf.eff_targets[i];
        infs2 = &ship->inf.eff_targets[i + 1];

        /*
         * If we have blank targets in the middle, move then up of if eff
         * setting is zero (set by player) remove it
         */
        if ((infs1->x == 999) || (infs1->max == 0)) {
            infs1->x = infs2->x;
            infs1->y = infs2->y;
            infs2->x = 999;

            if (infs1->x != 999) {
                f = 1;
            }
        }
    }

    while (f) {
        f = 0;

        for (i = 0; i < (INF_MAX_TARGETS - 1); ++i) {
            infs1 = &ship->inf.eff_targets[i];
            infs2 = &ship->inf.eff_targets[i + 1];

            /*
             * If we have blank targets in the middle, move them up of if eff
             * setting is zero (set by player) remove it
             */
            if ((infs1->x == 999) || (infs1->max == 0)) {
                infs1->x = infs2->x;
                infs1->y = infs2->y;
                infs2->x = 999;

                if (infs1->x != 999) {
                    f = 1;
                }
            }
        }
    }

    /* Fert targets */
    f = 0;

    for (i = 0; i < (INF_MAX_TARGETS - 1); ++i) {
        infs1 = &ship->inf.fert_targets[i];
        infs2 = &ship->inf.fert_targets[i + 1];

        /*
         * If we have blank target in the middle, move them up or if fert
         * setting is zero (set by player) remove it
         */
        if ((infs1->x == 999) || (infs1->max == 0)) {
            infs1->x = infs2->x;
            infs1->y = infs2->y;
            infs2->x = 999;

            if (infs1->x != 999) {
                f = 1;
            }
        }
    }

    while (f) {
        f = 0;

        for (i = 0; i < (INF_MAX_TARGETS - 1); ++i) {
            infs1 = &ship->inf.fert_targets[i];
            infs2 = &ship->inf.fert_targets[i + 1];

            /*
             * If we have blank targets in the middle, move them up or if fert
             * setting is zero (set by player) remove it
             */
            if ((infs1->x == 999) || (infs1->max == 0)) {
                infs1->x = infs2->x;
                infs1->y = infs2->y;
                infs2->x = 999;

                if (infs1->x != 999) {
                    f = 1;
                }
            }
        }
    }
}

/*
 * void do_inf_old(shiptype *ship, planettype *planet)
 * {
 *     sectortype *s;
 *     sectortype *sp;
 *     sectortype *low;
 *     sectortype *fused[512];
 *     sectortype *eused[512];
 *     int uf;
 *     int i;
 *     int x;
 *     int y;
 *     int lx = 0;
 *     int ly = 0;
 *     int e_ucnt;
 *     int f_ucnt;
 *     unsigned short effsetting;
 *     unsigned short fertsetting;
 *     struct inf_setting *infs;
 *     char itemp[256];
 *     char rbuf[((INF_MAX_TARGETS * 4) + 4) * 60];
 *
 *     f_ucnt = 0;
 *     e_ucnt = f_ucnt;
 *
 *     /\* NOTE: atmos is handled in doship.c *\/
 *     if (!ship->on) {
 *         if (ship->inf.wants_reports) {
 *             sprintf(buf, "Infrastructure [%d] is off\n", ship->number);
 *             push_telegram((int)ship->owner, (int)ship->governor, buf);
 *         }
 *
 *         return;
 *     }
 *
 *     /\* Init used sector list *\/
 *     for (i = 0; i < 512; ++i) {
 *         eused[i] = NULL;
 *         fused[i] = NULL;
 *     }
 *
 *     effsetting = ship->inf.eff_setting;
 *     fertsetting = ship->inf.fert_setting;
 *
 *     /\* Cost for machine *\/
 *     if (usep_fuel(ship, planet, (int)(fertsetting / 2)) < 1) {
 *         sprintf(buf,
 *                 "Not enough fuel available for Infrastructure [%d]\n",
 *                 ship->number);
 *
 *         push_telegram((int)ship->owner, (int)ship->governor, buf);
 *
 *         return;
 *     }
 *
 *     if (usep_res(ship, planet, (int)(effsetting / 2)) < 1) {
 *         sprintf(buf,
 *                 "Not enough resources available for Infrastructure [%d]\n",
 *                 ship->number);
 *
 *         push_telegram((int)ship->owner, (int)ship->governor, buf);
 *
 *         return;
 *     }
 *
 *     if (ship->inf.wants_reports) {
 *         sprintf(rbuf, "Infrastructure #%d Report:\n", ship->number);
 *         sprintf(itemp, "%15s| Efficiency setting = %d\n", " ", effsetting);
 *         strcat(rbuf, itemp);
 *         sprintf(itemp, "%15s| Fertility setting = %d\n", " ", fertsetting);
 *         strcat(rbuf, itemp);
 *
 *         sprintf(itemp,
 *                 "%15s| Overhead = %.0f fuel and %.0f resources\n",
 *                 " ",
 *                 (fertsetting / 2) + (FUEL_COST_PLOW * fertsetting),
 *                 (effsetting / 2) + (RES_COST_DOME * effsetting));
 *
 *         strcat(rbuf, itemp);
 *     }
 *
 *     /\*
 *      * First handle eff and fert targets then get settings and loop until zero
 *      *\/
 *     if (effsetting) {
 *         for (i = 0; i < INF_MAX_TARGETS; ++i) {
 *             infs = &ship->inf.eff_targets[i];
 *
 *             if (infs->x != 999) {
 *                 s = &Sector(*planet, (int)infs->x, (int)infs->y);
 *
 *                 if (s->eff >= 100) {
 *                     s->eff = 100;
 *
 *                     if (ship->inf.wants_reports) {
 *                         sprintf(itemp,
 *                                 "%15s|-> Sector [%d,%d] eff finished\n",
 *                                 " ",
 *                                 infs->x,
 *                                 infs->y);
 *
 *                         strcat(rbuf, itemp);
 *                     }
 *
 *                     infs->x = 999; /\* Flag to set target off *\/
 *                 } else if (effsetting) {
 *                     x = do_dome(ship, planet, s);
 *
 *                     int flag = 0;
 *
 *                     switch (x) {
 *                     case INF_NO_RES:
 *                     case INF_NO_FUEL:
 *                         flag = 1;
 *
 *                         break;
 *                     case INF_NO_POP:
 *                         sprintf(buf,
 *                                 "Infrastructure [%d]: No pop here[%d,%d]\n",
 *                                 ship->number,
 *                                 infs->x,
 *                                 infs->y);
 *
 *                         push_telegram((int)ship->owner,
 *                                       (int)ship->governor,
 *                                       buf);
 *
 *                         break;
 *                     case INF_NOT_OWNED:
 *                         sprintf(buf,
 *                                 "Infrastructure [%d]: Not your sector [%d,%d]\n",
 *                                 ship->number,
 *                                 infs->x,
 *                                 infs->y);
 *
 *                         push_telegram((int)ship->owner,
 *                                       (int)ship->governor,
 *                                       buf);
 *
 *                         break;
 *                     }
 *
 *                     if (flag) {
 *                         break;
 *                     }
 *
 *                     if (x > 0) {
 *                         if (ship->inf.wants_reports) {
 *                             sprintf(itemp,
 *                                     "%15s|-> Sector [%d,%d] (man) ",
 *                                     " ",
 *                                     infs->x,
 *                                     infs->y);
 *
 *                             strcat(rbuf, itemp);
 *                             strcat(rbuf, buf);
 *                         }
 *
 *                         effsetting -= x;
 *                         eused[e_ucnt] = s;
 *                         ++e_ucnt;
 *                     }
 *                 }
 *             }
 *         }
 *     }
 *
 *     if (fertsetting) {
 *         for (i = 0; i < INF_MAX_TARGETS; ++i) {
 *             infs = &ship->inf.fert_targets[i];
 *
 *             if (infs->x != 999) {
 *                 s = &Sector(*planet, (int)infs->x, (int)infs->y);
 *
 *                 if (s->fert >= 100) {
 *                     s->fert = 100;
 *
 *                     if (ship->inf.wants_reports) {
 *                         sprintf(itemp,
 *                                 "%15s|-> Sector [%d,%d] fert finished\n",
 *                                 " ",
 *                                 infs->x,
 *                                 infs->y);
 *
 *                         strcat(rbuf, itemp);
 *                     }
 *
 *                     infs->x = 999; /\* Flag to set target off *\/
 *                 } else if (fertsetting) {
 *                     x = do_plow(ship, planet, s, i);
 *
 *                     int flag = 0;
 *
 *                     switch(x) {
 *                     case INF_NO_FUEL:
 *                     case INF_NO_RES:
 *                         flag = 1;
 *
 *                         break;
 *                     case INF_NO_POP:
 *                         sprintf(buf,
 *                                 "Infrastructure [%d]: No pop here [%d,%d]\n",
 *                                 ship->number,
 *                                 infs->x,
 *                                 infs->y);
 *
 *                         push_telegram((int)ship->owner,
 *                                       (int)ship->governor,
 *                                       buf);
 *
 *                         break;
 *                     case INF_NOT_OWNED:
 *                         sprintf(buf,
 *                                 "Infrastructure [%d]: Not your sector [%d,%d]\n",
 *                                 ship->number,
 *                                 infs->x,
 *                                 infs->y);
 *
 *                         push_telegram((int)ship->owner,
 *                                       (int)ship->governor,
 *                                       buf);
 *
 *                         break;
 *                     }
 *
 *                     if (flag) {
 *                         break;
 *                     }
 *
 *                     if (x > 0) {
 *                         if (ship->inf.wants_reports) {
 *                             sprintf(itemp,
 *                                     "%15s|-> Sector [%d,%d] (man) ",
 *                                     " ",
 *                                     infs->x,
 *                                     infs->y);
 *
 *                             strcat(rbuf, itemp);
 *                             strcat(rbuf, buf);
 *                         }
 *
 *                         fertsetting -= x;
 *                         fused[f_ucnt] = s;
 *                         ++f_ucnt;
 *                     }
 *                 }
 *             }
 *         }
 *     }
 *
 *     squeeze_targets(ship);
 *
 *     /\* If settings left, do low sectors eff *\/
 *     while (effsetting) {
 *         Getxysect(planet, &x, &y, 1);
 *
 *         /\* Get lowest eff sector *\/
 *         low = NULL;
 *
 *         while (Getxysect(planet, &x, &y, 0)) {
 *             sp = &Sector(*planet, x, y);
 *
 *             if (!sp->popn || (sp->eff >= 100)) {
 *                 continue;
 *             }
 *
 *             if (is_target(ship, x, y, 1) || (sp->owner != ship->owner)) {
 *                 continue;
 *             }
 *
 *             /\* Make sure we haven't done this sector *\/
 *             uf = 0;
 *
 *             for (i = 0; i < e_ucnt; ++i) {
 *                 if (eused[i] != NULL) {
 *                     if (eused[i] == sp) {
 *                         uf = 1;
 *
 *                         break;
 *                     }
 *                 }
 *             }
 *
 *             if (uf) {
 *                 continue;
 *             }
 *
 *             if (low == NULL) {
 *                 lx = x;
 *                 ly = y;
 *                 low = sp;
 *             }
 *
 *             if (low->eff > sp->eff) {
 *                 lx = x;
 *                 ly = y;
 *                 low = sp;
 *             }
 *         }
 *
 *         if (low == NULL) {
 *             sprintf(buf,
 *                     "Infrastructure [%d]: No eff sectors found!\n",
 *                     ship->number);
 *
 *             push_telegram((int)ship->owner, (int)ship->governor, buf);
 *             x = 0;
 *             effsetting = 0;
 *         } else {
 *             x = do_dome(ship, planet, low);
 *         }
 *
 *         int flag = 0;
 *
 *         switch (x) {
 *         case INF_NO_FUEL:
 *         case INF_NO_RES:
 *             flag = 1;
 *         }
 *
 *         if (flag) {
 *             break;
 *         }
 *
 *         if (x > 0) {
 *             if (ship->inf.wants_reports) {
 *                 sprintf(itemp, "%15s|-> Sector [%d,%d] (auto) ", " ", lx, ly);
 *                 strcat(rbuf, itemp);
 *                 strcat(rbuf, buf);
 *             }
 *
 *             effsetting -= x;
 *             eused[e_ucnt] = low;
 *             ++e_ucnt;
 *         } else {
 *             effsetting = 0;
 *         }
 *     }
 *
 *     /\* Fert *\/
 *     while (fertsetting) {
 *         Getxysect(planet, &x, &y, 1);
 *
 *         /\* Get lowest eff sector *\/
 *         low = NULL;
 *
 *         while (Getxysect(planet, &x, &y, 0)) {
 *             sp = &Sector(*planet, x, y);
 *
 *             if (!sp->popn || (sp->fert >= 100)) {
 *                 continue;
 *             }
 *
 *             if (is_target(ship, x, y, 0) || (sp->owner != ship->owner)) {
 *                 continue;
 *             }
 *
 *             /\* Make sure we haven't done this sector *\/
 *             uf = 0;
 *
 *             for (i = 0; i < f_ucnt; ++i) {
 *                 if (fused[i] != NULL) {
 *                     if (fused[i] = sp) {
 *                         uf = 1;
 *
 *                         break;
 *                     }
 *                 }
 *             }
 *
 *             if (uf) {
 *                 continue;
 *             }
 *
 *             if (low == NULL) {
 *                 lx = x;
 *                 ly = y;
 *                 low = sp;
 *             }
 *
 *             if (low->fert > sp->fert) {
 *                 lx = x;
 *                 ly = y;
 *                 low = sp;
 *             }
 *         }
 *
 *         if (low == NULL) {
 *             sprintf(buf,
 *                     "Infrastructure [%d]: No fert sectors found!\n",
 *                     ship->number);
 *
 *             push_telegram((int)ship->owner, (int)ship->governor, buf);
 *             x = 0;
 *             fertsetting = 0;
 *         } else {
 *             x = do_plow(ship,planet, low, 999);
 *         }
 *
 *         int flag = 0;
 *
 *         switch (x) {
 *         case INF_NO_FUEL:
 *         case INF_NO_RES:
 *             flag = 1;
 *
 *             break;
 *         }
 *
 *         if (flag) {
 *             break;
 *         }
 *
 *         if (x > 0) {
 *             if(ship->inf.wants_reports) {
 *                 sprintf(itemp, "%15s|-> Sector [%d,%d] (auto) ", " ", lx, ly);
 *                 strcat(rbuf, itemp);
 *                 strcat(rbuf, buf);
 *             }
 *
 *             fertsetting -= x;
 *             fused[f_ucnt] = low;
 *             ++f_ucnt;
 *         } else {
 *             fertsetting = 0;
 *         }
 *     }
 *
 *     if (ship->inf.wants_report) {
 *         push_telegram((int)ship->owner, (int)ship->governor, rbuf);
 *     }
 * }
 */
