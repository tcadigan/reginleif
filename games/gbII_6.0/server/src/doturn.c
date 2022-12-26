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
 * Franklin street, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * doturn -- Does one turn.
 *
 * #ident  "@(#)doturn.c  1.10 12/1/93"
 *
 * $Header: /var/cvs/gbp/GB+/server/doturn.c,v 1.6 2007/07/06 17:30:26 gbp Exp $
 *
 * static char *ver = "@(#)  $RCSfile: doturn.c,v $ $Revision: 1.6 $";
 */
#include "doturn.h"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "buffers.h"
#include "doplanet.h"
#include "doship.h"
#include "dospace.h"
#include "doturn.h"
#include "files_shl.h"
#include "first.h"
#include "GB_server.h"
#include "lists.h"
#include "log.h"
#include "max.h"
#include "moveplanet.h"
#include "power.h"
#include "races.h"
#include "rand.h"
#include "shipdata.h"
#include "ships.h"
#include "shlmisc.h"
#include "tweakables.h"
#include "vars.h"
#include "vn.h"

#include "build.h"
#include "tele.h"

void do_turn(int);
int APadd(int, int, racetype *);
int governed(racetype *);
void fix_stability(startype *);
void do_reset(int);
void handle_victory(void);
void make_discoveries(racetype *);

#ifdef MARKET
void maintain(racetype *, int, int);
#endif

int attack_planet(shiptype *);
void output_ground_attacks(void);
int planet_points(planettype *);
int vp_planet_points(planettype *);
int learn_tech(racetype *, float);

#define ALIVE(i) ((ships[(i)] != NULL) && ships[(i)]->alive && ships[(i)]->owner)

static void destroy_ship(int no)
{
    if (no > Num_ships) {
        return;
    }

    ships[no]->owner = 0;
    ships[no]->governor = 0;
    ships[no]->alive = 0;
    ships[no]->damage = 100;

    for (no = ships[no]->ships; no && (no <= Num_ships); no = nextship(ships[no])) {
        destroy_ship(no);
    }
}

void do_turn(int update)
{
    int star;
    int i;
    int j;
    int ii;
    commodtype *c;
    int dummy[2];
    int temp;
    double dist;
    int planet_pts[MAXPLAYERS];

    struct victstruct {
        int numsects;
        int shipcost;
        int shiptech;
        int morale;
        int res;
        int des;
        int fuel;
        int money;
    } *victory;

    /* Set the garble static */
    chat_static = int_rand(1, 4); /* CWL */

    /* Make all 0 for the first iteration of doplanet */
    if (update) {
        memset(starpopns, 0, sizeof(starpopns));
        memset(starnumships, 0, sizeof(starnumships));
        memset(Sdatanumships, 0, sizeof(Sdatanumships));
        memset(Stinfo, 0, sizeof(Stinfo));
        memset(StarsInhab, 0, sizeof(StarsInhab));
        memset(Power, 0, sizeof(Power));
        memset(inhabited, 0, sizeof(inhabited));
    }

    Num_ships = Numships();
    ships = (shiptype **)malloc(sizeof(shiptype *) * (Num_ships + 1));

    if (!ships) {
        loginfo(ERRORLOG, WANTERRNO, "doturn: ships malloc error");
        exit(-11);
    }

    for (i = 1; i <= Num_ships; ++i) {
        if (getship(&ships[i], i)) {
            if (!ALIVE(i)) {
                ships[i]->reuse = 1;
            }
        }
    }

    for (i = 1; i <= Num_ships; ++i) {
        int bad;
        int owner = ships[i]->owner;
        int gov = ships[i]->governor;
        /* Increased buffer -mfw */
        /* char msg[100]; */
        char msg[2048];

        bad = 0;

        if (ships[i] == NULL) {
            sprintf(msg, "Memory Error by ship %d", i);
            loginfo(ERRORLOG, WANTERRNO, msg);
            push_telegram(1, 0, msg);
        } else if ((ships[i]->number != i) && (ships[i]->number != 0)) {
            sprintf(msg, "DATA Error: Ship %d by itself", i);
            loginfo(ERRORLOG, WANTERRNO, msg);
            push_telegram(1, 0, msg);
        } else if (owner > Num_races) {
            sprintf(msg, "DATA Error: Ship %d Owner(Race) Unknown", i);
            loginfo(ERRORLOG, WANTERRNO, msg);
            push_telegram(1, 0, msg);
            bad = 1;
        } else if (owner
                   && ((gov > MAXGOVERNORS)
                       || !races[owner - 1]->governor[gov].active)) {
            sprintf(msg, "DATA Error: Ship %d Owner(Gov) Unknown", i);
            loginfo(ERRORLOG, WANTERRNO, msg);
            push_telegram(1, 0, msg);
            bad = 1;
        }

        if (bad) {
            destroy_ship(i);
        }
    }

    for (i = 1; i <= Num_ships; ++i) {
        if ((ships[i]->type == STYPE_SWEEPER) && ALIVE(i)) {
            do_sweeper(i);
        }
    }

    for (i = 1; i <= Num_ships; ++i) {
        if ((ships[i]->type == STYPE_MINEF) && ALIVE(i)) {
            do_mine(i, 0);
        }
    }

    /* Get all stars and planets */
    getsdata(&Sdata);
    Planet_count = 0;

    for (star = 0; star <Sdata.numstars; ++star) {
        getstar(&Stars[star], star);

        if (update) {
            /* Nova */
            fix_stability(Stars[star]);
        }

        for (i = 0; i < Stars[star]->numplanets; ++i) {
            getplanet(&planets[star][i], star, i);

            /* Move planets in orbits; also sets StarsInhab[] */
            if (planets[star][i]->type != TYPE_ASTEROID) {
                /* We need the total to evaluate victory */
                ++Planet_count;
            }

            if (update) {
                moveplanet(star, planets[star][i], i);
            }

            if (Stars[star]->pnames[i] == NULL) {
                sprintf(Stars[star]->pnames[i], "NULL-%d", i);
            }
        }

        if (Stars[star]->name[0] == '\0') {
            sprintf(Stars[star]->name, "NULL-%d", star);
        }
    }

    /* If not time for combat check for ships orbiting occupied planets */
    if (get_num_updates() < CombatUpdate) {
        for (i = 1; i <= Num_ships; ++i) {
            doOrbit(i);
        }
    }

    /* Check for ships that should get lost */
    for (i = 1; i <= Num_ships; ++i) {
        doUniv(i);
    }

    for (i = 1; i <= Num_races; ++i) {
        /* Increase tech; change to something else */
        if (update) {
            int j;

            /* Reset controlled planet count */
            races[i - 1]->controlled_planets = 0;
            races[i - 1]->planet_points = 0;

            for (j = 0; j <= MAXGOVERNORS; ++j) {
                if (races[i - 1]->governor[j].active) {
#ifdef MARKET
                    races[i - 1]->governor[j].maintain = 0;
                    races[i - 1]->governor[j].cost_market = 0;
                    races[i - 1]->governor[j].profit_market = 0;
#endif

                    races[i - 1]->governor[j].cost_tech = 0;
                    races[i - 1]->governor[j].income = 0;
                }
            }
        }

#ifdef VOTING
        /* Reset their vote for Update go. */
        races[i - 1]->votes &= ~VOTE_UPDATE_GO;
#endif
    }

#ifdef USE_VN
    vn_mad();
#endif

    output_ground_attacks();

#ifdef MARKET
    if (update) {
        /* Reset market */
        Num_commods = Numcommods();
        clr_commodfree();

        for (i = Num_commods; i >= 1; --i) {
            getcommod(&c, i);

            if (!c->deliver) {
                c->deliver = 1;
                putcommod(c, i);
                free(c);

                continue;
            }

            if (c->owner
                && c->bidder
                && (MONEY(races[c->bidder - 1], c->bidder_gov) >= c->bid)) {
                MONEY(races[c->bidder - 1], c->bidder_gov) -= c->bid;
                MONEY(races[c->owner - 1], c->governor) += c->bid;
                temp = shipping_cost((int)c->star_to,
                                     (int)c->star_from,
                                     &dist,
                                     (int)c->bid);

#ifdef COLLECTIVE_MONEY
                races[c->bidder - 1]->governor[0].cost_market += (c->bid + temp);
                races[c->owner - 1]->governor[0].profit_market += c->bid;
#else

                races[c->bidder - 1]->governor[c->bidder_gov].cost_market += (c->bid + temp);
                races[(unsigned)c->owner - 1]->governor[(unsigned)c->governor].profit_market += c->bid;
#endif

                maintain(races[c->bidder - 1], (int)c->bidder_gov, temp);

                switch(c->type) {
                case RESOURCE:
                    planets[c->star_to][c->planet_to]->info[c->bidder - 1].resource += c->quantity;

                    break;
                case FUEL:
                    planets[c->star_to][c->planet_to]->info[c->bidder - 1].fuel += c->quantity;

                    break;
                case DESTRUCT:
                    planets[c->star_to][c->planet_to]->info[c->bidder - 1].destruct += c->quantity;

                    break;
                case CRYSTAL:
                    planets[c->star_to][c->planet_to]->info[c->bidder - 1].crystals += c->quantity;

                    break;
                }

                sprintf(buf,
                        "Lot %d purchased from %s [%d] at a cost of %ld.\n\t%d %s arrived at /%s/%s\n",
                        i,
                        races[c->owner - 1]->name,
                        c->owner,
                        c->bid,
                        c->quantity,
                        Commod[(unsigned int)c->type],
                        Stars[c->star_to]->name,
                        Stars[c->star_to]->pnames[c->planet_to]);

                push_telegram((int)c->bidder, (int)c->bidder_gov, buf);

                sprintf(buf,
                        "Lot %d (%d %s sold to %s [%d] at a cost of %ld.\n",
                        i,
                        c->quantity,
                        Commod[(unsigned int)c->type],
                        races[c->bidder - 1]->name,
                        c->bidder,
                        c->bid);

                push_telegram((int)c->owner, (int)c->governor, buf);
                c->governor = 0;
                c->owner = c->governor;
                c->bidder_gov = 0;
                c->bidder = c->bidder_gov;
            } else {
                c->bidder_gov = 0;
                c->bidder = c->bidder_gov;
                c->bid = 0;
            }

            if (!c->owner) {
                makecommoddead(i);
            }

            putcommod(c, i);
            free(c);
        }
    }
#endif

    /* Check ship masses - ownership */
    for (i = 1; i <= Num_ships; ++i) {
        if (ALIVE(i)) {
            domass(ships[i]);
            doown(ships[i]);
        }
    }

    /* Do all ships one turn - do slower ships first */
    for (j = 0; j <= 9; ++j) {
        for (i = 1; i <= Num_ships; ++i) {
            if (ALIVE(i) && (ships[i]->speed == j)) {
                doship(ships[i], update);

                if ((ships[i]->type == STYPE_MISSILE)
                    && !attack_planet(ships[i])) {
                    domissile(ships[i]);
                }
            }

            /* CWL sanity checking! */
            if (ships[i]->fuel < 0) {
                ships[i]->fuel = 0;
                loginfo(ERRORLOG,
                        NOERRNO,
                        "Needed fuel sanity check for ship %d.",
                        i);
            }
            /* End CWL */

            /* Clear detected bit for SETI section CWL */
            ships[i]->detected = 0;
        }
    }

    /* CWL one more time...loop through and find SETIs and give reports */
    for (i = 1; i <= Num_ships; ++i) {
        if ((ships[i]->type == OTYPE_SETI)
            && ALIVE(i)
            && ships[i]->on
            && (ships[i]->whatorbits == LEVEL_PLAN)) {
            int nsh;
            float dist2seti;
            float setiview;
            int chance2see;
            int roll;

            nsh = Stars[ships[i]->storbits]->ships;

            while (nsh) {
                if ((nsh != i)
                    && !ships[nsh]->detected
                    && (ships[nsh]->type != OTYPE_CANIST)
                    && (ships[nsh]->type != OTYPE_GREEN)
                    && (ships[nsh]->owner != ships[i]->owner)
                    && (!isset(races[ships[nsh]->owner - 1]->allied, ships[i]->owner)
                        || !isset(races[ships[i]->owner - 1]->allied, ships[nsh]->owner))) {
                    dist2seti = sqrt((double)Distsq(ships[i]->xpos,
                                                    ships[i]->ypos,
                                                    ships[nsh]->xpos,
                                                    ships[nsh]->ypos));

                    setiview = (log1p((double)ships[i]->tech) * 2000) + (SYSTEMSIZE / 3);

                    if (setiview >= dist2seti) {
                        chance2see = (int)(SETI_SEE_FACTOR * sqrt(ships[i]->tech) * cos(M_PI_2 * (dist2seti / setiview)));

                        if (ships[nsh]->cloaked) {
                            chance2see /= 2;
                        }

                        roll = int_rand(1, 100);

                        if ((roll <= chance2see)
                            && (roll <= (100 - ships[i]->damage))
                            && (roll <= (int)((ships[i]->popn / ships[i]->max_crew) * 100))) {
                            /* We see them */
                            float rxpos;
                            float rypos;

                            ships[nsh]->detected = 1;

                            if (ships[nsh]->cloaked) {
                                rxpos = ships[nsh]->xpos + (float)(int_rand(1, 1000) - 500);
                                rypos = ships[nsh]->ypos + (float)(int_rand(1, 1000) - 500);
                            } else {
                                rxpos = ships[nsh]->xpos;
                                rypos = ships[nsh]->ypos;
                            }

                            if (ships[nsh]->cloaked) {
                                sprintf(telegram_buf,
                                        "Science Station #%d detected cloaked ship #%d.\n\t%s Distance: %6.2f Position: %6.2f, %6.2f (%d, %d)",
                                        i,
                                        nsh,
                                        prin_ship_orbits(ships[i]),
                                        dist2seti,
                                        rxpos,
                                        rypos,
                                        chance2see,
                                        roll);
                            } else {
                                sprintf(telegram_buf,
                                        "Science Station #%d detected %s %c %d.\n\t%s Distance: %6.2f Position: %6.2f, %6.2f (%d, %d)",
                                        i,
                                        Shipnames[ships[nsh]->type],
                                        Shipltrs[ships[nsh]->type],
                                        nsh,
                                        prin_ship_orbits(ships[i]),
                                        dist2seti,
                                        rxpos,
                                        rypos,
                                        chance2see,
                                        roll);
                            }

                            push_telegram(ships[i]->owner,
                                          ships[i]->governor,
                                          telegram_buf);
                        } /* We saw the ship! */
                    } /* chance2see */
                } /* Found a ship */

                nsh = nextship(ships[nsh]);
            } /* while nsh */
        } /* If a SETI */
    } /* For */
    /* End CWL SETI */

#ifdef MARKET
    /* Do maintenance costs */
    if (update) {
        for (i = 1; i <= Num_ships; ++i) {
            if (ALIVE(i) && Shipdata[ships[i]->type][ABIL_MAINTAIN]) {
#ifdef COLLECTIVE_MONEY
                if (ships[i]->popn || ships[i]->troops) {
                    races[ships[i]->owner - 1]->governor[0].maintain += ships[i]->build_cost;
                }

                if (ships[i]->troops) {
                    races[ships[i]->owner - 1]->governor[0].maintain += (UPDATE_TROOP_COST * ships[i]->troops);
                }

#else

                if (ships[i]->popn || ships[i]->troops) {
                    races[ships[i]->owner - 1]->governor[ships[i]->governor].maintain += ships[i]->build_cost;
                }

                if (ships[i]->troops) {
                    race[ships[i]->owner - 1]->governor[ships[i]->governor].maintain += (UPDATE_TROOP_COST * ships[i]->troops);
                }
#endif
            }
        }
    }
#endif
    
    /* Prepare dead ships for recycling */
    clr_shipfree();

    for (i = 1; i <= Num_ships; ++i) {
        if (!ALIVE(i)) {
            makeshipdead(i);

            /* Remove it from its fleet too. -mfw */
            remove_sh_fleet(ships[i]->owner, ships[i]->governor, ships[i]);
        }
    }

    /* Erase next ship pointers - reset in insert_sh_*... */
    for (i = 1; i <= Num_ships; ++i) {
        ships[i]->nextship = 0;
        ships[i]->ships = 0;
    }

    /* Clear ship list for insertion */
    Sdata.ships = 0;

    for (star = 0; star < Sdata.numstars; ++star) {
        Stars[star]->ships = 0;

        for (i = 0; i < Stars[star]->numplanets; ++i) {
            planets[star][i]->ships = 0;
        }
    }

    /* Insert ship into the list of wherever it might be */
    for (i = Num_ships; i >= 1; --i) {
        if (ALIVE(i)) {
            switch (ships[i]->whatorbits) {
            case LEVEL_UNIV:
                insert_sh_univ(&Sdata, ships[i]);

                break;
            case LEVEL_STAR:
                insert_sh_star(Stars[ships[i]->storbits], ships[i]);

                break;
            case LEVEL_PLAN:
                insert_sh_plan(planets[ships[i]->storbits][ships[i]->pnumorbits],
                               ships[i]);

                break;
            case LEVEL_SHIP:
                insert_sh_ship(ships[i], ships[ships[i]->destshipno]);

                break;
            default:

                break;
            }
        }
    }

    /*
     * Put ABMs and surviving missiles here because ABMs need to have the
     * missile in the shiplist of the target planet.  Maarten
     */

    /* ABMs defend plant */
    for (i = 1; i <= Num_ships; ++i) {
        if ((ships[i]->type == OTYPE_ABM) && ALIVE(i)) {
            doabm(ships[i]);
        }
    }

    for (i = 1; i <= Num_ships; ++i) {
        if ((ships[i]->type == STYPE_MINEF) && ALIVE(i)) {
            do_mine(i, 0);
        }
    }

    for (i = 1; i <= Num_ships; ++i) {
        if ((ships[i]->type == STYPE_MISSILE)
            && ALIVE(i)
            && attack_planet(ships[i])) {
            domissile(ships[i]);
        }
    }

    doFreeShipList(ships, FIRST_USER_SHIP, Num_ships);

    for (i = Num_ships; i >= 1; --i) {
        putship(ships[i]);
    }

    for (j = 1; j <= Num_races; ++j) {
        planet_pts[j - 1] = 0;
    }

    for (star = 0; star < Sdata.numstars; ++star) {
        for (i = 0; i < Stars[star]->numplanets; ++i) {
            /* Store occupation for VPs */
            for (j = 1; j <= Num_races; ++j) {
                if (planets[star][i]->info[j - 1].numsectsowned) {
                    setbit(inhabited[star], j);
                    setbit(Stars[star]->inhabited, j);
                }

                if ((planets[star][i]->type != TYPE_ASTEROID)
                    && (planets[star][i]->info[j - 1].numsectsowned > ((planets[star][i]->Maxx * planets[star][i]->Maxy) / 2))) {
                    ++races[j - 1]->controlled_planets;
                }

                if (planets[star][i]->info[j - 1].numsectsowned) {
                    races[j - 1]->planet_points += planet_points(planets[star][i]);
                    planet_pts[j - 1] += ((planets[star][i]->info[j - 1].numsectsowned * (double)vp_planet_points(planets[star][i])) / (double)(planets[star][i]->Maxx * planets[star][i]->Maxy));
                }
            }

            if (update) {
                if (doplanet(star, planets[star][i], i)) {
                    /*
                     * Save smap gotten and altered by doplanet only if the
                     * planet is explored
                     */
                    putsmap(Smap, planets[star][i]);
                }
            }

            putplanet(planets[star][i], star, i);
        }

        /* Do APs for each planet */
        if (update) {
            for (i = 1; i <= Num_races; ++i) {
                if (starpopns[star][i - 1]) {
                    setbit(Stars[star]->inhabited, i);
                } else {
                    clrbit(Stars[star]->inhabited, i);
                }

                if (isset(Stars[star]->inhabited, i)) {
                    int aps;

                    aps = Stars[star]->AP[i - 1] + APadd((int)starnumships[star][i - 1], (int)starpopns[star][i - 1], races[i - 1]);

                    if (aps < LIMIT_APS) {
                        Stars[star]->AP[i - 1] = aps;
                    } else {
                        Stars[star]->AP[i - 1] = LIMIT_APS;
                    }
                }

                /* Compute victory points for the block */
                if (inhabited[star][0] + inhabited[star][1]) {
                    dummy[0] = Blocks[i - 1].invite[0] & Blocks[i - 1].pledge[0];
                    dummy[1] = Blocks[i - 1].invite[1] & Blocks[i - 1].pledge[1];

                    if (((inhabited[star][0] | dummy[0]) == dummy[0])
                        && ((inhabited[star][1] | dummy[1]) == dummy[1])) {
                        ++Blocks[i - 1].systems_owned;
                    }
                }
            }
        }

        putstar(Stars[star], star);
    }

    /* Add APs to sdata for each player */
    if (update) {
        for (i = 1; i <= Num_races; ++i) {
            /* Recount systems owned */
            /* Blocks[i - 1].systems_owned = 0; */

            if (governed(races[i - 1])) {
                int aps;

                aps = Sdata.AP[i - 1] + races[i - 1]->planet_points;

                if (aps < GLOBAL_LIMIT_APS) {
                    Sdata.AP[i - 1] = aps;
                } else {
                    Sdata.AP[i - 1] = GLOBAL_LIMIT_APS;
                }
            }
        }
    }

    putsdata(&Sdata);

    /* Here is where we do victory calculations. */
    if (update) {
        victory = (struct victstruct *)malloc(Num_races * sizeof(struct victstruct));

        if (!victory) {
            printf("victcalc: malloc error\n");

            exit(-11);
        }

        for (i = 1; i <= Num_races; ++i) {
            victory[i - 1].numsects = 0;
            victory[i - 1].shipcost = 0;
            victory[i - 1].shiptech = 0;
            victory[i - 1].morale = races[i - 1]->morale;
            victory[i - 1].res = 0;
            victory[i - 1].des = 0;
            victory[i - 1].fuel = 0;
            victory[i - 1].money = MONEY(races[i - 1], 0);

#ifndef COLLECTIVE_MONEY
            for (j = 1; j <= MAXGOVERNOR; ++j) {
                if (races[i - 1]->governor[j].active) {
                    victory[i - 1].money += MONEY(races[i - 1], j);
                }
            }
#endif
        }

        for (star = 0; star < Sdata.numstars; ++star) {
            /* Do planets in the star next */
            for (i = 0; i < Stars[star]->numplanets; ++i) {
                for (j = 0; j < Num_races; ++j) {
                    if (planets[star][i]->info[j].explored) {
                        victory[j].numsects += (int)planets[star][i]->info[j].numsectsowned;
                        victory[j].res += (int)planets[star][i]->info[j].resource;
                        victory[j].des += (int)planets[star][i]->info[j].destruct;
                        victory[j].fuel += (int)planets[star][i]->info[j].fuel;
                    }
                }
            } /* End of planet searching */
        } /* End of star searching */


        for (i = 1; i <= Num_ships; ++i) {
            if (ALIVE(i)) {
                victory[ships[i]->owner - 1].shipcost += ships[i]->build_cost;
                victory[ships[i]->owner - 1].shiptech += ships[i]->tech;
                victory[ships[i]->owner - 1].res += ships[i]->resource;
                victory[ships[i]->owner - 1].des += ships[i]->destruct;
                victory[ships[i]->owner - 1].fuel += ships[i]->fuel;
            }
        }

        /* Now that we have the info...calculate the race score */
        /* Decreased effect of population by 10. /Gardan 17.1.1997 */
        for (i = 0; i <= Num_races; ++i) {
            races[i]->victory_score =
                planet_pts[i] +
                (Power[i].popn / 100000) +
                (Power[i].troops / 5000) +
                (victory[i].money / 100000) +
                (victory[i].shipcost / 2000) +
                (victory[i].res / 200) +
                (victory[i].fuel / 10000) +
                (victory[i].des / 500);

            races[i]->victory_score = (int)(morale_factor((double)victory[i].morale) * races[i]->victory_score);
        }

        free(victory);
    } /* End of if (update) */

    for (i = 1; i <= Num_ships; ++i) {
        putship(ships[i]);
        free(ships[i]);
    }

    if (update) {
        for (i = 1; i <= Num_races; ++i) {
            /* Tech cutter by 0.3% before tech gain. /Gardan 17.1.1997 */
            races[i - 1]->tech *= 0.997;

            /* Collective intelligence */
            if (races[i - 1]->collective_iq) {
                double x = (2.0 / 3.14159265) * atan((double)Power[i - 1].popn / MESO_POP_SCALE);
                races[i - 1]->IQ = races[i - 1]->IQ_limit * x * x;
            }

            races[i - 1]->tech += ((double)(races[i - 1]->IQ) / 100.0);

            /* MORALE */
            /* Morale cutter by 10% before calculus. /Gardan 11.2.1997 */
            races[i - 1]->morale *= 0.9;

            /* Morale from sector not from planets as earlier. /Gardan 11.2.1997 */
            for (star = 0; star < Sdata.numstars; ++star) {
                for (ii = 0; ii < Stars[star]->numplanets; ++ii) {
                    if (planets[star][ii]->info[i - 1].explored) {
                        /* numsectsowned might overflow */
                        if ((planets[star][ii]->info[i - 1].numsectsowned < 400)
                            && (planets[star][ii]->info[i - 1].numsectsowned > 0)) {
                            races[i - 1]->morale += ((int)planets[star][ii]->info[i - 1].numsectsowned / 10);
                            /* races[i - 1]->morale += Power[i - 1].planets_owned; */
                        }
                    }
                }
            }

            make_discoveries(races[i - 1]);
            races[i - 1]->turn += 1;

            if (races[i - 1]->controlled_planets >= ((Planet_count * VICTORY_PERCENT) / 100)) {
                ++races[i - 1]->victory_turns;
            } else {
                races[i - 1]->victory_turns = 0;
            }

            if (races[i - 1]->controlled_planets >= ((Planet_count * VICTORY_PERCENT) / 200)) {
                for (j = 1; j <= Num_races; ++j) {
                    races[j - 1]->translate[i - 1] = 100;
                }
            }

            Blocks[i - 1].VPs = 10 * Blocks[i - 1].systems_owned;

#ifdef MARKET
#ifndef COLLECTIVE_MONEY
            for (j = 0; j <= MAXGOVERNORS; ++j) {
                if (races[i - 1]->governor[j].active) {
                    maintain(races[i - 1], j, (int)races[i - 1]->governor[j].maintain);
                }
            }

#else
            j = 0;

            if (races[i - 1]->governor[j].active) {
                maintain(races[i - 1], j, (int)races[i - 1]->governor[j].maintain);
            }
#endif
#endif
        }

        for (i = 1; i <= Num_races; ++i) {
            putrace(races[i - 1]);
        }
    }

    free(ships);

    if (update) {
        compute_power_blocks();

        for (i = 1; i <= Num_races; ++i) {
            Power[i - 1].money = 0;

#ifndef COLLECTIVE_MONEY
            for (j = 0; j <= MAXGOVERNOR; ++j) {
                if (races[i - 1]->governor[j].active) {
                    Power[i - 1].money += MONEY(races[i - 1], j);
                }
            }

#else

            Power[i - 1].money = MONEY(races[i - 1], 0);
#endif
        }

        Putpower(Power);
        Putblock(Blocks);
    }

    for (j = 1; j <= Num_races; ++j) {
        if (update) {
            notify_race(j, "Finished with update.\n");
        } else {
            notify_race(j, "Finished with movement segment.\n");
        }
    }
}

/*
 * Routine for number of APs to add to each player in each system, scaled by
 * amount of crew in their palace
 */
int APadd(int sh, int popn, racetype *race)
{
    int aps;

    aps = round_rand(((double)sh / 10.0) + (5.0 * log10(1.0 + (double)popn)));

    if (governed(race)) {
        return aps;
    } else {
        /* Don't have an active gov center */
        return round_rand((double)aps / 20.0);
    }
}

int governed(racetype *race) {
    if (!race->Gov_ship) {
        return 0;
    }

    if (race->Gov_ship > Num_ships) {
        return 0;
    }

    if (ships[race->Gov_ship] == NULL) {
        return 0;
    }

    if (!ships[race->Gov_ship]->alive) {
        return 0;
    }

    if (!ships[race->Gov_ship]->docked) {
        return 0;
    }

    if (ships[race->Gov_ship]->whatdest != LEVEL_PLAN) {
        if (ships[race->Gov_ship]->whatorbits != LEVEL_SHIP) {
            return 0;
        }

        if (ships[ships[race->Gov_ship]->destshipno]->type != STYPE_HABITAT) {
            return 0;
        }

        if (ships[ships[race->Gov_ship]->destshipno]->whatorbits != LEVEL_PLAN) {
            if (ships[ships[race->Gov_ship]->destshipno]->whatorbits != LEVEL_STAR) {
                return 0;
            }
        }
    }

    return 1;
}

/* Fix stability for stars */
void fix_stability(startype *s)
{
    int a;
    int i;

    if (s->nova_stage > 0) {
        if (s->nova_stage > MAX_NOVA) {
            s->stability = 20;
            s->nova_stage = 0;
            sprintf(telegram_buf, "Notice:\n");
            sprintf(buf, "\n\tScientists report that star %s\n", s->name);
            strcat(telegram_buf, buf);
            sprintf(buf, "\tis no longer undergoing nova...\n");
            strcat(telegram_buf, buf);

            for (i = 1; i <= Num_races; ++i) {
                push_telegram_race(i, telegram_buf);
            }

            /* Telegram everyone when nova over? */
        } else {
            ++s->nova_stage;
        }
    } else if (s->stability > 20) {
        a = int_rand(-1, 3);

        /* Nova just starting; notify everyone */
        if ((s->stability + a) > 100) {
            s->stability = 100;
            s->nova_stage = 1;
            sprintf(telegram_buf, "***** BULLETIN! *****\n");
            sprintf(buf, "\n\tScientists report that star %s\n", s->name);
            strcat(telegram_buf, buf);
            sprintf(buf, "is now undergoing nova.\n");
            strcat(telegram_buf, buf);

            for (i = 1; i <= Num_races; ++i) {
                push_telegram_race(i, telegram_buf);
            }
        } else {
            s->stability += a;
        }
    } else {
        a = int_rand(-1, 1);

        if (((int)s->stability + a) < 0) {
            s->stability = 0;
        } else {
            s->stability += a;
        }
    }
}

void do_reset(int time_reset)
{
    int star;
    int i;
    int j;

    Num_ships = Numships();
    ships = (shiptype **)malloc((Num_ships + 1) * sizeof(shiptype *));

    if (!ships) {
        printf("do_reset: malloc error\n");

        exit(-11);
    }

    for (i = 1; i <= Num_ships; ++i) {
        getship(&ships[i], i);
    }

    /* Get all stars and planets */
    getsdata(&Sdata);

    for (star = 0; star < Sdata.numstars; ++star) {
        getstar(&Stars[star], star);

        for (i = 0; i < Stars[star]->numplanets; ++i) {
            getplanet(&planets[star][i], star, i);
        }
    }

    output_ground_attacks();

    /* Erase next ship points - reset in insert_sh_*... */
    for (i = 1; i <= Num_ships; ++i) {
        ships[i]->nextship = 0;
        ships[i]->ships = 0;
    }

    /* Clear ship list for insertion */
    Sdata.ships = 0;

    for (star = 0; star < Sdata.numstars; ++star) {
        Stars[star]->ships = 0;

        for (i = 0; i < Stars[star]->numplanets; ++i) {
            planets[star][i]->ships = 0;
        }
    }

    /* Insert ship into the list of wherever it might be */
    for (i = Num_ships; i >= 1; --i) {
        if (ALIVE(i)) {
            switch (ships[i]->whatorbits) {
            case LEVEL_UNIV:
                insert_sh_univ(&Sdata, ships[i]);

                break;
            case LEVEL_STAR:
                insert_sh_star(Stars[ships[i]->storbits], ships[i]);

                break;
            case LEVEL_PLAN:
                insert_sh_plan(planets[ships[i]->storbits][ships[i]->pnumorbits], ships[i]);
                ships[i]->xpos = planets[ships[i]->storbits][ships[i]->pnumorbits]->xpos + Stars[ships[i]->storbits]->xpos;
                ships[i]->ypos = planets[ships[i]->storbits][ships[i]->pnumorbits]->ypos + Stars[ships[i]->storbits]->ypos;

                break;
            case LEVEL_SHIP:
                insert_sh_ship(ships[i], ships[ships[i]->destshipno]);

                break;
            default:

                break;
            }
        } else {
            memset(ships[i], 0, sizeof(shiptype));
            ships[i]->number = i;
        }
    }

    /* Check ship masses */
    for (i = 1; i <= Num_ships; ++i) {
        if (ALIVE(i)) {
            /*
             * Put this next check in to avoid some core dumps, see domass()
             * - mfw
             */
            if (i == ships[i]->number) {
                domass(ships[i]);
                doown(ships[i]);
            } else {
                sprintf(buf,
                        "doturn(): ship # discrepancy index = %d, number = %d",
                        i,
                        ships[i]->number);

                loginfo(ERRORLOG, WANTERRNO, buf);
            }
        }
    }

    for (star = 0; star < Sdata.numstars; ++star) {
        if (!isascii(*Stars[star]->name)) {
            sprintf(Stars[star]->name, "%d", star);
        }

        for (i = 0; i < Stars[star]->numplanets; ++i) {
            if (!isascii(*Stars[star]->pnames[i])) {
                sprintf(Stars[star]->pnames[i], "%d", i);
            }

            putplanet(planets[star][i], star, i);
        }

        putstar(Stars[star], star);
    }

    putsdata(&Sdata);

    for (i = 1; i <= Num_ships; ++i) {
        putship(ships[i]);
        free(ships[i]);
    }

    free(ships);

#ifdef COLLECTIVE_MONEY
    for (i = 1; i <= Num_races; ++i) {
        for (j = 1; j <= MAXGOVERNORS; ++j) {
            MONEY(races[i - 1], 0) += races[i - 1]->governor[j].money;
            races[i - 1]->governor[j].money = 0;
        }
    }
#endif

    for (i = 1; i <= Num_races; ++i) {
        putrace(races[i - 1]);

        if (!send_special_string(i, RESET_END)) {
            notify_race(i, "Finished with reset.\n");
        }
    }
}

#define BIG_WINNER 1
#define LITTLE_WINNER 2

void handle_victory(void)
{
    int i;
    int j;
    int game_over = 0;
    int win_category[64];

#ifndef VICTORY
    return;
#endif

    for (i = 1; i <= Num_races; ++i) {
        win_category[i - 1] = 0;

        if (races[i - 1]->controlled_planets >= ((Planet_count * VICTORY_PERCENT) / 100)) {
            win_category[i - 1] = LITTLE_WINNER;
        }

        if (races[i - 1]->victory_turns >= VICTORY_UPDATES) {
            ++game_over;
            win_category[i - 1] = BIG_WINNER;
        }
    }

    if (game_over) {
        for (i = 1; i <= Num_races; ++i) {
            strcat(telegram_buf, "*** Attention ***");
            push_telegram_race(i, telegram_buf);
            strcat(telegram_buf, "This game of Galactic Bloodshed is now *over*");
            push_telegram_race(i, telegram_buf);

            if (game_over == 1) {
                sprintf(telegram_buf, "The big winner is");
            } else {
                sprintf(telegram_buf, "the big winners are");
            }

            for (j = 1; j <= Num_races; ++j) {
                if (win_category[j - 1] == BIG_WINNER) {
                    sprintf(telegram_buf,
                            "*** [%2d] %-30.30s ***",
                            j,
                            races[j - 1]->name);

                    push_telegram_race(i, telegram_buf);
                }
            }

            strcat(telegram_buf, "Lesser winners:");
            push_telegram_race(i, telegram_buf);

            for (j = 1; j <= Num_races; ++j) {
                if (win_category[j - 1] == LITTLE_WINNER) {
                    sprintf(telegram_buf,
                            "+++ [%2d] %-30.30s +++",
                            j,
                            races[j - 1]->name);

                    push_telegram_race(i, telegram_buf);
                }
            }
        }
    }
}

void make_discoveries(racetype *r)
{
    /* Would be nicer to do this with a loop of course - but it's too late */
    if (!Hyper_drive(r) && learn_tech(r, TECH_HYPER_DRIVE)) {
        push_telegram_race(r->Playernum,
                           "You have discovered HYPERDRIVE technology.\n");

        r->discoveries[D_HYPER_DRIVE] = 1;
    }

    if (!Laser(r) && learn_tech(r, TECH_LASER)) {
        push_telegram_race(r->Playernum,
                           "You have discovered LASER technology.\n");

        r->discoveries[D_LASER] = 1;
    }

    if (!Cew(r) && learn_tech(r, TECH_CEW)) {
        push_telegram_race(r->Playernum,
                           "You have discovered CEW technology.\n");

        r->discoveries[D_CEW] = 1;
    }

#ifdef USE_VN
    if (!Vn(r) && learn_tech(r, TECH_VN)) {
        push_telegram_race(r->Playernum,
                           "You have discovered VN technology.\n");

        r->discoveries[D_VN] = 1;
    }
#endif

    if (!Tractor_beam(r) && learn_tech(r, TECH_TRACTOR_BEAM)) {
        push_telegram_race(r->Playernum,
                           "You have discovered TRACTOR BEAM technology.\n");

        r->discoveries[D_TRACTOR_BEAM] = 1;
    }

    if (!Transporter(r) && learn_tech(r, TECH_TRANSPORTER)) {
        push_telegram_race(r->Playernum,
                           "You have discovered TRANSPORTER technology.\n");

        r->discoveries[D_TRANSPORTER] = 1;
    }

    if (!Avpm(r) && learn_tech(r, TECH_AVPM)) {
        push_telegram_race(r->Playernum,
                           "You have discovered AVPM technology.\n");

        r->discoveries[D_AVPM] = 1;
    }

    if (!Cloak(r) && learn_tech(r, TECH_CLOAK)) {
        push_telegram_race(r->Playernum,
                           "You have discovered CLOAK technology.\n");

        r->discoveries[D_CLOAK] = 1;
    }

    if (!Wormhole(r) && learn_tech(r, TECH_WORMHOLE)) {
        push_telegram_race(r->Playernum,
                           "You have discovered WORMHOLE technology.\n");

        r->discoveries[D_WORMHOLE] = 1;
    }

    if (!Crystal(r) && learn_tech(r, TECH_CRYSTAL)) {
        push_telegram_race(r->Playernum,
                           "You have discovered CRYSTAL technology.\n");

        r->discoveries[D_CRYSTAL] = 1;
    }

    if (!Atmos(r) && learn_tech(r, TECH_ATMOS)) {
        push_telegram_race(r->Playernum,
                           "You have discovered ATMOSPHERIC technology.\n");

        r->discoveries[D_ATMOS] = 1;
    }
}

#ifdef MARKET
void maintain(racetype *r, int gov, int amount)
{
    int loss;

    if (MONEY(r, gov) >= amount) {
        MONEY(r, gov) -= amount;
    } else {
        loss = (amount - MONEY(r, gov)) / 10;
        r->morale -= loss;
        MONEY(r, gov) = 0;
        sprintf(buf, "Morale suffers due to lack of funds.\n");
        push_telegram(r->Playernum, gov, buf);
    }
}
#endif

int attack_planet(shiptype *ship)
{
    if (ship->whatdest == LEVEL_PLAN) {
        return 1;
    } else {
        return 0;
    }
}

void output_ground_attacks(void)
{
    int star;
    int i;
    int j;

    for (star = 0; star < Sdata.numstars; ++star) {
        for (i = 1; i <= Num_races; ++i) {
            for (j = 1; j <= Num_races; ++j) {
                if (ground_assaults[i - 1][j - 1][star]) {
                    sprintf(buf,
                            "%s: %s [%d] assaults %s [%d] %d times.\n",
                            Stars[star]->name,
                            races[i - 1]->name,
                            i,
                            races[j - 1]->name,
                            j,
                            ground_assaults[i - 1][j - 1][star]);

                    post(buf, COMBAT);
                    ground_assaults[i - 1][j - 1][star] = 0;
                }
            }
        }
    }
}

int planet_points(planettype *p)
{
    switch (p->type) {
    case TYPE_ASTEROID:

        return ASTEROID_POINTS;
    case TYPE_EARTH:

        return EARTH_POINTS;
    case TYPE_MARS:

        return MARS_POINTS;
    case TYPE_ICEBALL:

        return ICEBALL_POINTS;
    case TYPE_GASGIANT:

        return GASGIANT_POINTS;
    case TYPE_WATER:

        return WATER_POINTS;
    case TYPE_FOREST:

        return FOREST_POINTS;
    case TYPE_DESERT:

        return DESERT_POINTS;
    default:

        return 0;
    }
}

int vp_planet_points(planettype *p)
{
    switch (p->type) {
    case TYPE_ASTEROID:

        return VP_ASTEROID;
    case TYPE_EARTH:

        return VP_EARTH;
    case TYPE_MARS:

        return VP_MARS;
    case TYPE_ICEBALL:

        return VP_ICE;
    case TYPE_GASGIANT:

        return VP_GAS;
    case TYPE_WATER:

        return VP_WATER;
    case TYPE_FOREST:

        return VP_FOREST;
    case TYPE_DESERT:

        return VP_DESERT;
    default:

        return 0;
    }
}

/* Make learning a new science *not* so automatic (from HAP - mfw) */
int learn_tech(racetype *race, float learn_tech)
{
    int chance2learn;
    int techlearnfac;

    if (race->tech < learn_tech) {
        return 0;
    }

    /* To guard against divide by 0 warns */
    techlearnfac = TECH_LEARN_FACTOR;

    if (techlearnfac) {
        chance2learn = (int)((race->IQ / techlearnfac) + (((race->tech - learn_tech) / learn_tech) * 100));

        return (int_rand(1, 100) <= chance2learn);
    } else {
        return 1;
    }
}
