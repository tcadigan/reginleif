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

extern int errno;

racetype *Race;
int planet_translate[N_HOME_PLANET_TYPES] = { 0, 6, 7, 5, 2, 3, 4 };

int getpid(void);
void modify_print_loop(int level);
void found_planet(int playernum, int star, int pnum);

int notify(int who, int gov, const char *msg)
{
    /* This is a dummy routine */
    return 1;
}

void warn(int who, int gov, char *msg)
{
    /* This is a dummy routine */
}

void push_message(int what, int who, char *msg)
{
    /* This is a dummy routing */
}

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

    if ((playernum == 1) && (race.priv_type != P_GOD)) {
        close_data_files();
        sprintf(race.rejection,
                "The first race enrolled must have God privileges.\n");

        return 1;
    }

    if (playernum >= MAXPLAYERS) {
        close_data_files();
        sprintf(race.rejection,
                "There are already %d players; No more allowed.\n",
                MAXPLAYERS - 1);
        race.status = STATUS_UNENROLLABLE;

        return 1;
    }

    getsdata(&Sdata);
    star_arena = (startype *)malloc(Sdata.numstars * sizeof(startype));

    for (star = 0; star < Sdata.numstars; ++star) {
        Stars[star] = &star_arena[star];
        getstar(&Stars[star], star);
    }

    printf("Looking for %s...", planet_print_name[race.home_planet_type]);
    fflush(stdout);

    ppref = planet_translate[race.home_planet_type];

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
    sprintf(race.rejection,
            "Didn't find any free %s. Choose another home planet type.\n",
            planet_print_name[race.home_planet_type]);
    close_data_files();
    race.status = STATUS_UNENROLLABLE;

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
    Race = Malloc(racetype);
    Bzero(*Race);

    Race->Playernum = playernum;
    Race->God = (race.priv_type == P_GOD);
    Race->Guest = (race.priv_type == P_GUEST);
    Race->governors = 0;

    strcpy(Race->name, race.name);
    strcpy(Race->password, race.password);
    strcpy(Race->governor[0].name, race.leader);
    strcpy(Race->governor[0].password, race.ldrpw);

    Race->governor[0].deflevel = LEVEL_PLAN;
    Race->governor[0].homelevel = Race->governor[0].deflevel;
    Race->governor[0].defsystem = star;
    Race->governor[0].homesystem = Race->governor[0].defsystem;
    Race->governor[0].defplanetnum = pnum;
    Race->governor[0].homeplanetnum = Race->governor[0].defplanetnum;

    /*
     * Display options
     */
    Race->governor[0].toggle.highlight = playernum;
    Race->governor[0].toggle.inverse = 1;
    Race->governor[0].toggle.color = 0;
    Race->governor[0].active = 1;

    for (i = 0; i <= OTHER; ++i) {
        Race->conditions[i] = planet->conditions[i];
    }

#if 0
    /*
     * Make conditions preferred by your people set to (more or less) those of
     * the planet: higher the concentration of gas, the higher percentage
     * difference between planet and race
     */
    for (i = 0; i <= OTHER; ++i) {
        Race->conditions[j] = planet->conditions[i]
            + int_rand(round_rand(-planet->conditions[i] * 2.0),
                       round_rand(planet->conditions[i] * 2.0));
    }
#endif

    for (i = 0; i < MAXPLAYERS; ++i) {
        /*
         * Message from autoreport, player #1 are decodable
         */
        if ((i == playernum) || (playernum == 1) || Race->God) {
            /*
             * You can talk to own race
             */
            Race->translate[i - 1] = 100;
        }
        else {
            Race->translate[i - 1] = 1;
        }
    }

#if 0
    /*
     * All of the following zeros are not really needed, because the race was
     * memset to 0 prior.
     */
    for (i = 0; i < 80; ++i) {
        Race->discoverables[i] = 0;
    }

    Race->tech = 0.0;
    Race->morale = 0;
    Race->turn = 0;
    Race->allied[1] = 0;
    Race->allied[0] = Race->allied[1];
    Race->atwar[1] = 0;
    Race->atwar[0] = Race->atwar[1];

    for (i = 0; i < MAXPLAYERS; ++i) {
        Race->points[i] = 0;
    }
#endif

    /*
     * Assign racial characteristics.
     */
    Race->absorb = race.attr[ABSORB];
    Race->collective_iq = race.attr[COL_IQ];
    Race->Metamorph = (race.race_type == R_METAMORPH);
    Race->pods = race.attr[PODS];
    Race->fighters = race.attr[FIGHT];

    if (race.attr[COL_IQ] == 1.0) {
        Race->IQ_limit = race.attr[A_IQ];
    } else {
        Race->IQ = race.attr[A_IQ];
    }

    Race->number_sexes = race.attr[SEXES];
    Race->fertilize = race.attr[FERT] * 100;
    Race->adventurism = race.attr[ADVENT];
    Race->birthrate = race.attr[BIRTH];
    Race->mass = race.attr[MASS];
    Race->metabolism = race.attr[METAB];

    /*
     * Assign sector compatibilities and determine a primary sector type.
     */
    for (i = FIRST_SECTOR_TYPE; i <= LAST_SECTOR_TYPE; ++i) {
        Race->likes[i] = race.compat[i] / 100.0;

        if ((race.compat[i] == 100)
            && (planet_compat_cov[race.home_planet_type][i] == 1.0)) {
            Race->likesbest = i;
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
        if (Sector(*planet, x, y).condition == Race->likesbest) {
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
        sect->popn = Race->number_sexes;
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
    s.mass = s.base_mass + (s.max_crew * Race->mass);
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

    Race->Gov_ship = shipno;

    planet->troops = sect->troops;
    planet->ships = shipno;
    planet->info[playernum - 1].numsectsowned = 1;
    planet->explored = 0;
    planet->info[playernum - 1].explored = 1;
    /*
     * planet->info[playernum - 1].autorep = 1;
     */
    planet->maxpopn =
        ((maxsupport(Race, sect, 100.0, 0) * planet->Maxx) * planet->Maxy) / 2;
    planet->popn = sect->popn;

#ifdef STARTING_INVENTROY
    if (Race->Metamorph) {
        planet->info[playernum - 1].resource +=
            (START_REST - START_MESO_RES_DIFF);
    } else {
        planet->info[playernum - 1].resource += START_REST;
    }

    planet->info[playernum - 1].fuel += START_FUEL;
    planet->info[playernum - 1].destruct += START_DES;
#endif

    putrace(Race);
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
           race.name,
           x,
           y,
           Stars[star]->name,
           Stars[star]->pnames[pnum]);
    race.status = STATUS_ENROLLED;

    free(planet);
}

int loginfo(int i, ...)
{
    return 1;
}
