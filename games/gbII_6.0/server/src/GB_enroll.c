/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the Free
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
 * Franklin Street, 5th floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * GB_enroll.c -- GB file interface for GB race enrollment program.
 */
#include "GB_enroll.h"

#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "config.h"
#include "files_shl.h"
#include "max.h"
#include "perm.h"
#include "power.h"
#include "racegen.h"
#include "races.h"
#include "rand.h"
#include "shipdata.h"
#include "ships.h"
#include "vars.h"

#define START_RES 100;
#define START_MESO_RES_DIFF 40
#define START_FUEL 100
#define START_DES 100

int planet_translate[N_HOME_PLANET_TYPES] = { 0, 6, 7, 5, 2, 3, 4 };

int getpid(void);
void modify_print_loop(int level);
void found_planet(int playernum, int star, int pnum);

void init_enroll(void)
{
    /* srandom(getpid()); */
}

/*
 * Return 0 if successfully enrolled, or 1 if failure.
 */
int enroll_valid_race(void)
{
    /* int mask = 0; */
    int star;
    int pnum;
    int i;
    int ppref;
    int playernum;
    int last_star_left;
    int indirect[NUMSTARS];
    startype *star_arena;

    /*
     * if (race.status == STATUS_ENROLLED) {
     *   sprintf(race.rejection, "This race has already been enrolled!\n");
     *   return 1;
     * }
     */
    open_data_files();
    playernum = Numraces() + 1;

    if ((playernum == 1) && (gen_race.priv_type != P_GOD)) {
        close_data_files();
        sprintf(gen_race.rejection,
                "The first race enrolled must have God privileges.\n");

        return 1;
    }

    if (playernum >= MAXPLAYERS) {
        close_data_files();
        sprintf(gen_race.rejection,
                "There are already %d players; No more allowed.\n",
                MAXPLAYERS - 1);
        gen_race.status = STATUS_UNENROLLABLE;

        return 1;
    }

    getsdata(&Sdata);
    star_arena = (startype *)malloc(Sdata.numstars * sizeof(startype));

    for (star = 0; star < Sdata.numstars; ++star) {
        Stars[star] = &star_arena[star];
        getstar(&Stars[star], star);
    }

    printf("Looking for %s...", planet_print_name[gen_race.home_planet_type]);
    fflush(stdout);

    ppref = planet_translate[gen_race.home_planet_type];

    for (i = 0; i < Sdata.numstars; ++i) {
        indirect[i] = i;
    }

    last_star_left = Sdata.numstars - 1;

    while (last_star_left >= 0) {
        i = int_rand(0, last_star_left);
        star = indirect[i];

        /*
         * printf(".", star);
         */
        printf(".");
        fflush(stdout);

        /*
         * Skip over inhabited stars and stars with few planets.
         */
        if ((Stars[star]->numplanets >= 6)
            && !Stars[star]->inhabited[0]
            && !Stars[star]->inhabited[1]) {
            /* Look for uninhabited planets */
            for (pnum = 0; pnum < Stars[star]->numplanets; ++pnum) {
                planettype *planet;

                getplanet(&planet, star, pnum);

                if ((planet->type == ppref)
                    && (planet->conditions[RTEMP] >= -200)
                    && (planet->conditions[RTEMP] <= 100)) {
                    found_planet(playernum, star, pnum);
                }

                free(planet);

                return 0;
            }
        }

        /*
         * Since we get here, this star didn't work out
         */
        indirect[i] = indirect[last_star_left];
        --last_star_left;
    }

    /*
     * If we get here, then we did not find any good planet.
     */
    printf(" Failed!\n");
    sprintf(gen_race.rejection,
            "Didn't find any free %s. Choose another home planet type.\n",
            planet_print_name[gen_race.home_planet_type]);
    close_data_files();
    gen_race.status = STATUS_UNENROLLABLE;

    return 1;
}

void found_planet(int playernum, int star, int pnum)
{
    int i;
    int x;
    int y;
    sectortype *sect;
    planettype *planet;
    shiptype s;
    int shipno;

    getplanet(&planet, star, pnum);

    // ===
    printf(" Found!\n");
    racetype *race = Malloc(racetype);
    Bzero(*race);

    race->Playernum = playernum;
    race->God = (gen_race.priv_type == P_GOD);
    race->Guest = (gen_race.priv_type == P_GUEST);
    race->governors = 0;

    strcpy(race->name, gen_race.name);
    strcpy(race->password, gen_race.password);
    strcpy(race->governor[0].name, gen_race.leader);
    strcpy(race->governor[0].password, gen_race.ldrpw);

    race->governor[0].deflevel = LEVEL_PLAN;
    race->governor[0].homelevel = race->governor[0].deflevel;
    race->governor[0].defsystem = star;
    race->governor[0].homesystem = race->governor[0].defsystem;
    race->governor[0].defplanetnum = pnum;
    race->governor[0].homeplanetnum = race->governor[0].defplanetnum;

    /*
     * Display options
     */
    race->governor[0].toggle.highlight = playernum;
    race->governor[0].toggle.inverse = 1;
    race->governor[0].toggle.color = 0;
    race->governor[0].active = 1;

    for (i = 0; i <= OTHER; ++i) {
        race->conditions[i] = planet->conditions[i];
    }

#if 0
    /*
     * Make conditions preferred by your people set to (more or less) those of
     * the planet: higher the concentration of gas, the higher percentage
     * difference between planet and race
     */
    for (i = 0; i <= OTHER; ++i) {
        race->conditions[j] = planet->conditions[i]
            + int_rand(round_rand(-planet->conditions[i] * 2.0),
                       round_rand(planet->conditions[i] * 2.0));
    }
#endif

    for (i = 0; i < MAXPLAYERS; ++i) {
        /*
         * Message from autoreport, player #1 are decodable
         */
        if ((i == playernum) || (playernum == 1) || race->God) {
            /*
             * You can talk to own race
             */
            race->translate[i - 1] = 100;
        }
        else {
            race->translate[i - 1] = 1;
        }
    }

#if 0
    /*
     * All of the following zeros are not really needed, because the race was
     * memset to 0 prior.
     */
    for (i = 0; i < 80; ++i) {
        race->discoverables[i] = 0;
    }

    race->tech = 0.0;
    race->morale = 0;
    race->turn = 0;
    race->allied[1] = 0;
    race->allied[0] = race->allied[1];
    race->atwar[1] = 0;
    race->atwar[0] = race->atwar[1];

    for (i = 0; i < MAXPLAYERS; ++i) {
        race->points[i] = 0;
    }
#endif

    /*
     * Assign racial characteristics.
     */
    race->absorb = gen_race.attr[ABSORB];
    race->collective_iq = gen_race.attr[COL_IQ];
    race->Metamorph = (gen_race.race_type == R_METAMORPH);
    race->pods = gen_race.attr[PODS];
    race->fighters = gen_race.attr[FIGHT];

    if (gen_race.attr[COL_IQ] == 1.0) {
        race->IQ_limit = gen_race.attr[A_IQ];
    } else {
        race->IQ = gen_race.attr[A_IQ];
    }

    race->number_sexes = gen_race.attr[SEXES];
    race->fertilize = gen_race.attr[FERT] * 100;
    race->adventurism = gen_race.attr[ADVENT];
    race->birthrate = gen_race.attr[BIRTH];
    race->mass = gen_race.attr[MASS];
    race->metabolism = gen_race.attr[METAB];

    /*
     * Assign sector compatibilities and determine a primary sector type.
     */
    for (i = FIRST_SECTOR_TYPE; i <= LAST_SECTOR_TYPE; ++i) {
        race->likes[i] = gen_race.compat[i] / 100.0;

        if ((gen_race.compat[i] == 100)
            && (planet_compat_cov[gen_race.home_planet_type][i] == 1.0)) {
            race->likesbest = i;
        }
    }

    /*
     * Find sector to build capitol on, and populate it.
     */
    getsmap(Smap, planet);
    PermuteSects(planet);
    Getxysect(planet, 0, 0, 1);
    i = Getxysect(planet, &x, &y, 0);

    while (i) {
        if (Sector(*planet, x, y).condition == race->likesbest) {
            break;
        }

        i = Getxysect(planet, &x, &y, 0);
    }

    if (!i) {
        y = 0;
        x = 0;
    }

    sect = &Sector(*planet, x, y);
    sect->owner = playernum;
    sect->race = playernum;
    sect->fert = 100;
    sect->eff = 10;
    sect->troops = 0;

    if (ADAM_AND_EVE) {
        sect->popn = race->number_sexes;
    } else {
        sect->popn = 0;
    }

    /*
     * mask = sigblocks(SIGBLOCKS);
     */

    /*
     * Build a capital ship to run the government
     */
    Bzero(s);
    shipno = Numships() + 1;
    s.nextship = 0;

    s.type = OTYPE_GOV;
    s.xpos = Stars[star]->xpos + planet->xpos;
    s.ypos = Stars[star]->ypos + planet->ypos;
    s.land_x = x;
    s.land_y = y;

    s.speed = 0;
    s.owner = playernum;
    s.race = playernum;
    s.governor = 0;
    s.tech = 100.0;
    s.build_type = s.type;
    s.armor = Shipdata[s.type][ABIL_ARMOR];
    s.guns = PRIMARY;
    s.primary = Shipdata[s.type][ABIL_GUNS];
    s.primtype = Shipdata[s.type][ABIL_PRIMARY];
    s.secondary = Shipdata[s.type][ABIL_GUNS];
    s.sectype = Shipdata[s.type][ABIL_SECONDARY];
    s.max_crew = Shipdata[s.type][ABIL_MAXCREW];
    s.max_destruct = Shipdata[s.type][ABIL_DESTCAP];
    s.max_resource = Shipdata[s.type][ABIL_CARGO];
    s.max_fuel = Shipdata[s.type][ABIL_FUELCAP];
    s.max_speed = Shipdata[s.type][ABIL_SPEED];
    s.build_cost = Shipdata[s.type][ABIL_COST];
    s.size = 100;
    s.base_mass = 100.0;
    sprintf(s.class, "Standard");
    s.fuel = 0.0;
    s.popn = s.max_crew;
    s.troops = 0;
    s.mass = s.base_mass + (s.max_crew * race->mass);
    s.resource = 0;
    s.destruct = 0;
    s.alive = 1;
    s.active = 1;
    s.protect.self = 1;
    /*
     * Docked on the planet.
     */
    s.docked = 1;
    s.whatorbits = LEVEL_PLAN;
    s.whatdest = s.whatorbits;
    s.deststar = star;
    s.destpnum = pnum;
    s.storbits = s.deststar;
    s.pnumorbits = s.destpnum;
    s.rad = 0;
    s.damage = 0;
    /*
     * Shipdata[s.type][ABIL_DAMAGE];
     */

    /*
     * First capital is 100% efficient
     */
    s.retaliate = 0;
    s.ships = 0;
    s.on = 1;
    s.name[0] = '\0';
    s.number = shipno;
    putship(&s);

    race->Gov_ship = shipno;

    planet->troops = sect->troops;
    planet->ships = shipno;
    planet->info[playernum - 1].numsectsowned = 1;
    planet->explored = 0;
    planet->info[playernum - 1].explored = 1;
    /*
     * planet->info[playernum - 1].autorep = 1;
     */
    planet->maxpopn =
        ((maxsupport(race, sect, 100.0, 0) * planet->Maxx) * planet->Maxy) / 2;
    planet->popn = sect->popn;

#ifdef STARTING_INVENTROY
    if (race->Metamorph) {
        planet->info[playernum - 1].resource +=
            (START_REST - START_MESO_RES_DIFF);
    } else {
        planet->info[playernum - 1].resource += START_REST;
    }

    planet->info[playernum - 1].fuel += START_FUEL;
    planet->info[playernum - 1].destruct += START_DES;
#endif

    putrace(race);
    putsector(sect, planet, x, y);
    putplanet(planet, star, pnum);

    /*
     * Make start expored and stuff.
     */
    getstar(&Stars[star], star);
    setbit(Stars[star]->explored, playernum);
    setbit(Stars[star]->inhabited, playernum);
    Stars[star]->AP[playernum - 1] = 5;
    putstar(Stars[star], star);
    close_data_files();

    /*
     * sigsetmask(mask);
     */

    printf("Player %d (%s) created on sector %d,%d on %s/%s.\n",
           playernum,
           gen_race.name,
           x,
           y,
           Stars[star]->name,
           Stars[star]->pnames[pnum]);
    gen_race.status = STATUS_ENROLLED;

    free(planet);
}
