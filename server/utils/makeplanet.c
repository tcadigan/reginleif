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
 * ANY WARRANTY; without even the implied warrenty of MERCHANTABILITY or FITNESS
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
 * makeplanet.c - Makes one planet.
 *
 * #ident "@(#)makeplanet.c 1.3 2/17/93 "
 *
 * Modified Feb 13, 1992 by Shawn Fox: skf5055@tamsun.tamu.edu
 */

#include <math.h>
#include <string.h>
/*
 * #include <strings.h>
 */

/*
 * For random()
 */
#include <stdlib.h>

#include "GB_copyright.h"
#include "vars.h"

/*
 * # of beginning islands for makeuniv
 */
#define MAP_ISLANDS 3

/*
 * Percentage of mountain areas
 */
#define MAP_MOUNT_PERCENT 0.22

/*
 * Percent of desert areas
 */
#define MAP_DESERT_PERCENT 0.10

/*
 * Min size gasgiants that have bands
 */
#define MAP_GASGIANT_BANDMIN 20

/*
 * Percent chance jovian has liquid secs too
 */
#define JOVIAN_WITH_WATER 50
#define LANDPERCENTAGE int_rand(20, 70) / 100
#define POLEFUDGE 10
#define TFAC 10

extern int use_smashup;
extern double double_rand(void);
extern int int_rad(int, int);
extern int round_rand(double);
extern in rposneg(void);
extern int Temperature(double, int);
double DistmapSq(int, int, int, int);
int gb_rand(void);
planettype Makeplanet(double, short, int);
void MakeEarthAtmosphere(planettype *, int);
int neighbors(planettype *, int, int, int);
int SectorTemp(planettype *, int, int);
void Makesurface(planettype *);
short SectTemp(planettype *, int);
void seed(planettype *, int, int);
void grow(planettype *, int, int, int);
void jovian_with_water(planettype *);
void smashup(planettype *, int, char, int, int, int);
int num_neighbors(planettype *, int, int, int);

/*
 * New stats by Gardan
 *
 * @ o O # ~ . ( -
 */
int xmin[] = { 19, 1, 5, 5, 30, 15, 15, 17 };
int xmax[] = { 25, 3, 8, 8, 37, 20, 20, 22 };

/*
 * Min and max sizes for planet types (RATIOXY is 3.7)
 *
 * @ o O # ~ . ( -
 * min 114 1 15 15 270 75 75 85
 * max 200 6 40 40 407 140 140 161
 */

/*
 * Old stats
 *
 * int xmin[] = { 17, 2, 4, 5, 28, 14, 14, 14 };
 * int xmax[] = { 24, 4, 8, 8, 32, 22, 22, 22 };
 */

/*
 * f_min, f_max, r_min, r_max are now all based on the sector types as well as
 * the planet type.
 */

/*
 * Changes by Gardan: Desert planet's r_min and r_max raised by 50. Asteroid's
 *                    r_min raised by 150, r_max raised to double value. Class M
 *                    r_min and r_max reduced by 25. Waterball mountain r_min
 *                    and r_max raised by 100. 14/1/1997
 */
/*
 * . * ^ ~ # ( -
 */
int x_chance[] = { 5, 15, 10, 4, 5, 7, 6 };
int f_min[][8] = {
    /*
     * @
     */
    { 25, 20, 10, 0, 20, 45, 4 },
    /*
     * o
     */
    { 0, 1, 2, 0, 0, 0, 1 },
    /*
     * O
     */
    { 0, 3, 2, 0, 0, 0, 2 },
    /*
     * #
     */
    { 0, 0, 8, 0, 25, 0, 0 },
    /*
     * ~
     */
    { 0, 0, 0, 35, 0, 0, 0 },
    /*
     * .
     */
    { 30, 0, 5, 0, 20, 0, 0 },
    /*
     * (
     */
    { 30, 0, 0, 0, 30, 60, 0 },
    /*
     * -
     */
    { 0, 5, 2, 0, 0, 0, 2 }
};

/*
 * . * ^ ~ # ( -
 */
int f_max[][8] = {
    /*
     * @
     */
    { 40, 35, 20, 0, 40, 65, 15 },
    /*
     * o
     */
    { 0, 2, 3, 0, 0, 0, 2 },
    /*
     * O
     */
    { 0, 5, 4, 0, 0, 0, 3 },
    /*
     * #
     */
    { 0, 0, 15, 0, 35, 0, 0 },
    /*
     * ~
     */
    { 0, 0, 0, 55, 0, 0, 0 },
    /*
     * .
     */
    { 50, 0, 15, 0, 40, 0, 0 },
    /*
     * (
     */
    { 60, 0, 0, 0, 50, 90, 0 },
    /*
     * -
     */
    { 0, 10, 6, 0, 0, 0, 8 }
};

/*
 * . * ^ ~ # ( -
 */
int r_min[][8] = {
    /*
     * @
     */
    { 175, 200, 275, 0, 175, 175, 225 },
    /*
     * o
     */
    { 0, 400, 500, 0, 0, 0, 450 },
    /*
     * O
     */
    { 0, 225, 275, 0, 0, 0, 250 },
    /*
     * ~
     */
    /*
     * JH value -mfw
     * { 0, 0, 0, 60, 0, 0, 0 },
     */
    { 0, 0, 0, 30, 0, 0, 0 },
    /*
     * .
     */
    { 175, 0, 350, 0, 200, 0, 0 },
    /*
     * (
     */
    { 150, 0, 0, 0, 150, 150, 0 },
    /*
     * -
     */
    { 0, 250, 350, 0, 0, 0, 300 }
};

/*
 * . * ^ ~ # ( -
 */
int r_max[][8] = {
    /*
     * @
     */
    { 225, 300, 375, 0, 225, 200, 275 },
    /*
     * o
     */
    { 0, 600, 1200, 0, 0, 0, 800 },
    /*
     * O
     */
    { 0, 350, 550, 0, 0, 0, 350 },
    /*
     * ~
     */
    /*
     * JH value -mfw
     * { 0, 0, 0, 120, 0, 0, 0 },
     */
    { 0, 0, 0, 60, 0, 0, 0 },
    /*
     * .
     */
    { 225, 0, 500, 0, 250, 0, 0 },
    /*
     * (
     */
    { 250, 0, 0, 0, 250, 200, 0 },
    /*
     * -
     */
    { 0, 250, 550, 0, 0, 0, 400 }
};

/*
 * The starting conditions of the sectors given a planet type
 */
/*
 * @ o O # ~ . ( _
 */
int cond[] = { SEA, MOUNT, LAND, ICE, GAS, SEA, FOREST, DESERT };

planettype Makeplanet(double dist, short stemp, int type)
{
    int x;
    int y;
    sectortype *s;
    planettype planet;
    int atmos;
    int total_sects;
    char c;
    char t;
    double f;

    Bzero(planet);
    memset((char *)Smap, 0, sizeof(Smap));
    planet.type = type;
    planet.expltimer = 5;
    planet.conditions[RTEMP] = Temperature(dist, stemp);
    planet.conditions[TEMP] = planet.conditions[RTEMP];
    planet.Maxx = int_rand(xmin[type], xmax[type]);
    f = (double)planet.Maxx / RATIOXY;
    planet.Maxy = round_rand(f) + 1.0;

    /*
     * Make odd number of latitud bands
     */
    /*
     * This is commented away to make size calculus easier -- Gardan
     *
     * if ((planet.Maxy % 2) == 0) {
     *     ++planet.Maxy;
     * }
     */

    /*
     * Maxy for asteroids is reduced from 3 to 2 -- Gardan
     */
    if (type == TYPE_ASTEROID) {
        /*
         * Asteroids have funny shapes.
         */
        planet.Maxy = int_rand(1, 2);
    }

    /*
     * These two are for making ball class planets a little bigger and more
     * like balls -- Gardan
     */
    if (type == TYPE_ICEBALL) {
        ++planet.Maxy;
    }

    if (type == TYPE_MARS) {
        ++planet.Maxy;
    }

    c = cond[type];
    t = c;

    for (y = 0; y < planet.Maxy; ++y) {
        for (x = 0; x < planet.Maxx; ++x) {
            s = &Sector(planet, x, y);
            s->condition = t;
            s->type = s->condition;
        }
    }

    total_sects = (planet.Maxy - 1) * (planet.Maxx - 1);

    switch (type) {
    case TYPE_GASGIANT:
        /*
         * Gas giant planet
         */
        /*
         * Either lots of meth or not too much
         */
        if (int_rand(0, 1)) {
            /*
             * Methane planet
             */
            planet.conditions[METHANE] = int_rand(70, 80);
            atmos = 100 - planet.conditions[METHANE];
            planet.conditions[HYDROGEN] = int_rand(1, atmos / 2);
            atmos -= planet.conditions[HYDROGEN];
            planet.conditions[HELIUM] = 1;
            atmos -= planet.conditions[HELIUM];
            planet.conditions[OXYGEN] = 0;
            atmos -= planet.conditions[OXYGEN];
            planet.conditions[CO2] = 1;
            atmos -= planet.conditions[CO2];
            planet.conditions[NITROGEN] = int_rand(1, atmos / 2);
            planet.conditions[SULFUR] = 0;
            atmos -= planet.conditions[SULFUR];
            planet.conditions[OTHER] = atmos;
        } else {
            planet.conditions[HYDROGEN] = int_rand(30, 75);
            atmos = 100 - planet_conditions[HYDROGEN];
            planet.conditions[HELIUM] = int_rand(20, atmos / 2);
            atmos -= planet.conditions[HELIUM];
            planet.conditions[METHANE] = ((gb_rand() % 2) == 1);
            atmos -= planet.conditions[METHANE];
            planet.conditions[OXYGEN] = 0;
            atmos -= planet.conditions[OXYGEN];
            planet.conditions[CO2] = ((gb_rand() % 2) == 1);
            atmos -= planet.conditions[CO2];
            planet.conditions[NITROGEN] = int_rand(1, atmos / 2);
            atmos -= planet.conditions[NITROGEN];
            planet.conditions[SULFUR] = 0;
            atmos -= planet.conditions[SULFUR];
            palnet.conditions[OTHER] = atmos;
        }

        if (int_rand(1, 100) < JOVIAN_WITH_WATER) {
            /*
             * -mfw
             */
            jovian_with_water(&planet);
        }

        if (use_smashup) {
            smashup(&planet, int_rand(2, 4), SEA, 0, 0, 0);
            smashup(&planet, int_rand(1, 2), LAND, 2, 3, 1);
        }

        break;
    case TYPE_MARS:
        planet.conditions[HYDROGEN] = 0;
        planet.conditions[HELIUM] = 0;
        planet.conditions[METHANE] = 0;
        planet.conditions[OXYGEN] = 0;

        /*
         * Some have an atmosphere, some don't
         */
        if ((gb_rand() % 2) == 1) {
            planet.conditions[CO2] = int_rand(30, 45);
            atmos = 100 - planet.conditions[CO2];
            planet.conditions[NITROGEN] = int_rand(10, atmos / 2);
            atmos -= planet.conditions[NITROGEN];

            if ((gb_rand() % 2) == 1) {
                planet.conditions[SULFUR] = 0;
            } else {
                planet.conditions[SULFUR] = int_rand(20, atmos / 2);
            }

            atmos -= planet.conditions[SULFUR];
            planet.conditions[OTHER] = atmos;
        } else {
            planet.conditions[CO2] = 0;
            planet.conditions[NITROGEN] = 0;
            planet.conditions[SULFUR] = 0;
            planet.conditions[OTHER] = 0;
        }

        seed(&planet, DESERT, int_rand(1, total_sects));
        seed(&palnet, MOUNT, int_rand(1, total_sects));

        if (use_smashup) {
            if (int_rand(0, 4)) {
                smashup(&planet, 6, LAND, 5, 1, 0);
            } else {
                smashup(&planet, 6, DESERT, 5, 1, 0);
            }
        }

        break;
    case TYPE_ASTEROID:
        /*
         * Asteroid
         */
        /*
         * No atmosphere
         */
        for (y = 0; y < planet.Maxy; ++y) {
            for (x = 0; x < planet.Maxxl ++x) {
                if (!int_rand(0, 3)) {
                    s = &Sector(planet,
                                int_rand(1, planet.Maxx),
                                int_rand(1, planet.Maxy));
                    s->condition = LAND;
                    s->type = s->condition;
                }
            }
        }

        seed(&planet, DESERT, int_rand(1, total_sects));

        break;
    case TYPE_ICEBALL:
        /*
         * Ball of ice
         */
        /*
         * No atmosphere
         */
        planet.conditions[HYDROGEN] = 0;
        planet.conditions[HELIUM] = 0;
        planet.conditions[METHANE] = 0;
        planet.conditions[OXYGEN] = 0;

        if ((planet.Maxx * planet.Maxy) > int_rand(0, 20)) {
            planet.conditions[CO2] = int_rand(30, 45);
            atmos = 100 - planet.conditions[CO2];
            planet.conditions[NITROGEN] = int_rand(10, atmos / 2);
            atmos -= planet.conditions[NITROGEN];

            if ((gb_rand() % 2) == 1) {
                planet.conditions[SULFUR] = 0;
            } else {
                planet.conditions[SULFUR] = int_rand(20, atmos / 2);
            }

            atmos -= planet.conditions[SULFUR];
            planet.conditions[OTHER] = atmos;
        } else {
            planet.conditions[CO2] = 0;
            planet.conditions[NITROGEN] = 0;
            planet.conditions[SULFUR] = 0;
            planet.conditions[OTHER] = 0;
        }

        seed(&planet, MOUNT, int_rand(1, total_sects / 2));

        if (use_smashup) {
            smashup(&planet, int_rand(0, 3), MOUNT, 20, 0, 0);
        }

        break;
    case TYPE_EARTH:
        /*
         * Was 33 -- Gardan
         */
        MakeEarthAtmosphere(&planet, 50);
        seed(&planet, LAND, int_rand(total_sects / 30, total_sects / 20));
        grow(&planet, LAND, 1, 1);
        grow(&planet, LAND, 1, 2);
        grow(&planet, LAND, 2, 3);

        /*
         * Start of new Gardan code 20/12/1996
         * Modified Gardan 8/1/1997
         * Randomize by Scott's suggestion
         */
        switch (int_rand(0, 5)) {
        case 0:
            seed(&planet, MOUNT, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, MOUNT, 1, 1);
            grow(&planet, MOUNT, 1, 2);
            seed(&planet, FOREST, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, FOREST, 1, 1);
            grow(&planet, FOREST, 1, 2);
            seed(&planet, DESERT, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, DESERT, 1, 1);
            grow(*planet, DESERT, 1, 2);

            break;
        case 1:
            seed(&planet, MOUNT, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, MOUNT, 1, 1);
            grow(&planet, MOUNT, 1, 2);
            seed(&planet, DESERT, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, DESERT, 1, 1);
            grow(&planet, DESERT, 1, 2);
            seed(&planet, FOREST, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, FOREST, 1, 1);
            grow(&planet, FOREST, 1, 2);

            break;
        case 2:
            seed(&planet, FOREST, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, FOREST, 1, 1);
            gros(&planet, FOREST, 1, 2);
            seed(&planet, MOUNT, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, MOUNT, 1, 1);
            grow(&planet, MOUNT, 1, 2);
            seed(&planet, DESERT, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, DESERT, 1, 1);
            grow(&planet, DESERT, 1, 2);

            break;
        case 3:
            seed(&planet, FOREST, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, FOREST, 1, 1);
            grow(&planet, FOREST, 1, 2);
            seed(&planet, DESERT, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, DESERT, 1, 1);
            grow(&planet, DESERT, 1, 2);
            seed(&planet, MOUNT, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, MOUNT, 1, 1);
            grow(&planet, MOUNT, 1, 2);

            break;
        case 4:
            seed(&planet, DESERT, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, DESERT, 1, 1);
            grow(&planet, DESERT, 1, 2);
            seed(&planet, FOREST, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, FOREST, 1, 1);
            grow(&planet, FOREST, 1, 2);
            seed(&planet, MOUNT, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, MOUNT, 1, 1);
            grow(&planet, MOUNT, 1, 2);

            break;
        case 5:
            seed(&planet, DESERT, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, DESERT, 1, 1);
            grow(&planet, DESERT, 1, 2);
            seed(&planet, MOUNT, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, MOUNT, 1, 1);
            grow(&planet, MOUNT, 1, 2);
            seed(&planet, FOREST, int_rand(total_sects / 65, total_sects / 45));
            grow(&planet, FOREST, 1, 1);
            grow(&planet, FOREST, 1, 2);

            break;
        }

        grow(&planet, SEA, 1, 3);
        grow(&planet, SEA, 1, 5);

        /*
         * End of new Gardan code
         */

        grow(&planet, SEA, 1, 4);

        if (use_smashup) {
            smashup(&planet, int_rand(0, 2), DESERT, 0, 1, 0);
            smashup(&planet, int_rand(0, 2), FOREST, 50, 60, 0);
        }

        break;
    case TYPE_FOREST:
        /*
         * Was 0 -- Gardan
         */
        MakeEarthAtmosphere(&planet, 50);
        seed(&planet, SEA, int_rand(total_sects / 30, total_sects / 20));
        grow(&planet, SEA, 1, 1);
        grow(&planet, SEA, 1, 3);
        grow(&planet, FOREST, 1, 3);

        if (use_smashup) {
            smashup(&planet, int_rand(0, 2), SEA, 20, 40, 0);
        }

        break;
    case TYPE_WATER:
        /*
         * Was 25 -- Gardan
         */

        /*
         * Start of new Gardan code 20/12/1996
         * Modified Gardan 8/1/1997
         */
        seed(&planet, MOUNT, int_rand(total_sects / 30, total_sects / 20));
        grow(&planet, MOUNT, 1, 1);
        grow(&planet, MOUNT, 2, 3);
        grow(&planet, SEA, 1, 3);

        /*
         * End of new Gardan code
         */

        if (use_smashup) {
            smashup(&planet, int_rand(0, 1), LAND, 50, 50, 0);
        }

        break;
    case TYPE_DESERT:
        MakeEarthAtmosphere(&planet, 50);

        /*
         * Start of new Gardan code 20/12/1996
         * Modified Gardan 8/1/1997
         * Randomize by Scott's suggestion
         */
        switch (int_rand(0, 1)) {
        case 0:
            seed(&planet, MOUNT, int_rand(total_sects / 50, total_sects / 25));
            grow(&planet, MOUNT, 1, 1);
            grow(&planet, MOUNT, 2, 3);
            seed(&planet, LAND, int_rand(total_sects / 50, total_sects / 25));
            grow(&planet, LAND, 1, 1);
            grow(&planet, LAND, 2, 3);

            break;
        case 1:
            seed(&planet, LAND, int_rand(total_sects / 50, total_sects / 25));
            grow(&planet, LAND, 1, 1);
            grow(&planet, LAND, 2, 3);
            seed(&planet, MOUNT, int_rand(total_sects / 50, total_sects / 25));
            grow(&planet, MOUNT, 1, 1);
            grow(&planet, MOUNT, 2, 3);

            break;
        }

        /*
         * End of new Gardan code
         */

        grow(&planet, DESERT, 1, 3);

        if (use_smashup) {
            smashup(&planet, int_rand(0, 2), MOUNT, 200, 10, 0);
            smashup(&planet, int_rand(0, 1), LAND, 100, 20, 0);
        }

        break;
    }

    /*
     * Determine surface geology based on environment
     */
    Makesurface(&planet);

    return planet;
}

void MakeEarthAtmosphere(planettype *pptr, int chance)
{
    int atmos = 100;

    if (int_rand(0, 99) > chance) {
        /*
         * Oxygen-reducing atmosphere
         */
        pptr->conditions[OXYGEN] = int_rand(10, 25);
        atmos -= pptr->conditions[OXYGEN];
        pptr->conditions[NITROGEN] = int_rand(20, atmos - 20);
        atmos -= pptr->conditions[NITROGEN];
        pptr->conditions[CO2] = int_rand(10, atmos / 2);
        atmos -= pptr->conditions[CO2];
        pptr->conditions[HELIUM] = int_rand(2, (atmos / 8) + 1);
        atmos -= pptr->conditions[HELIUM];
        pptr->conditions[METHANE] = ((gb_rand() % 2) == 1);
        atmos -= pptr->conditions[METHANE];
        pptr->conditions[SULFUR] = 0;
        atmos -= pptr->conditions[SULFUR];
        pptr->conditions[HYDROGEN] = 0;
        atmos -= pptr->conditions[HYDROGEN];
        pptr->conditions[OTHER] = atmos;
    } else {
        /*
         * Methane atmosphere
         */
        pptr->conditions[METHANE] = int_rand(70, 80);
        atmos -= pptr->conditions[METHANE];
        pptr->conditions[HYDROGEN] = int_rand(1, atmos / 2);
        atmos -= pptr->conditions[HYDROGEN];
        pptr->conditions[HELIUM] = 1 + ((gb_rand() % 2) == 1);
        atmos -= pptr->conditions[HELIUM];
        pptr->conditions[OXYGEN] = 0;
        atmos -= pptr->conditions[OXYGEN];
        pptr->conditions[CO2] = 1 + ((gb_rand() % 2) == 1);
        atmos -= pptr->conditions[CO2];
        pptr->conditions[SULFUR] = ((gb_rand() % 2) == 1);
        atmos -= pptr->conditions[SULFUR];
        pptr->conditions[NITROGEN] = int_rand(1, atmos / 2);
        atmos -= pptr->conditions[NITROGEN];
        pptr->conditions[OTHER] = atmos;
    }
}

double DistmapSq(int x, int y, int x2, int y2)
{
#if 0

    return ((fabs((double)(x - x2)) / RATIOXY) + fabs((double)(y - y2)));

#else

    return (((0.8 * (x - x2)) * (x - x2)) + ((y - y2) * (y - y2)));

#endif
}

int SectorTemp(planettype *pptr, int x, int y)
{
    /*
     * X of the pole, and the ghost pole
     */
    int p_x;
    int p_xg;

    /*
     * Y of the nearest pole
     */
    int p_y;

    /*
     * "Distance" to pole
     */
    double f;
    double d;
    static double renorm[] = {
        0,
        1.0 / 1.0,
        2.0 / 2.0,
        4.0 / 3.0,
        6.0 / 4.0,
        9.0 / 5.0,
        12.0 / 6.0,
        16.0 / 7.0,
        20.0 / 8.0,
        25.0 / 9.0,
        30.0 / 10.0,
        36.0 / 11.0,
        42.0 / 12.0,
        49.0 / 13.0
    };

    /*
     * @ o O # ~ . ( -
     */
    static int variance[] = { 30, 40, 40, 40, 10, 25, 30, 30 };

    /*
     * I use pptr->sectormappos to calculate the pole position from. This in
     * spite of the fact that the two have nothing to do with each other. I did
     * it because:
     *  (a) I don't want the pole to move and sectormappos will also not change
     *  (b) sectormappos will not show up to the player in any other fashion
     */
    p_x = pptr->sectormappos % pptr->Maxx;

    if (y < (pptr->Maxy / 2.0)) {
        p_y = -1;
    } else {
        p_y = pptr->Maxy;
        p_x = p_x + (pptr->Maxx / 2.0);

        if (p_x >= pptr->Maxx) {
            p_x -= pptr->Maxx;
        }
    }

    if (p_x <= (pptr->Maxy / 2)) {
        p_xg = p_x + pptr->Maxy;
    } else {
        p_xg = p_x - pptr->Maxy;
    }

    d = (y - p_y) * (y - p_y);
    f = (x - p_x + 0.2) / pptr->Maxx;

    if (f < 0.0) {
        f = -f;
    }

    if (f > 0.85) {
        f = 1.0 - f;
    }

    d = sqrt(d + f - 0.5);

    return (pptr->conditions[RTEMP] +
            (variance[pptr->type] * (d - renorm[pptr->Maxy])));
}

/*
 * Returns # of neighors of a given desgination that a sector has
 */
int nieghbors(planettype *p, int x, int y, int type)
{
    /*
     * Left and right columns
     */
    int l = x - 1;
    int r = x + 1;

    /*
     * Number of neighbors so far
     */
    int n = 0;

    if (x == 0) {
        l = p->Maxx - 1;
    } else if (r == p->Maxx) {
        r = 0;
    }

    if (y > 0) {
        n += ((Sector(*p, x, y - 1).type == type)
              + (Sector(*p, l, y - 1).type == type)
              + (Sector(*p, r, y - 1).type == type));
    }

    n += ((Sector(*p, l, y).type == type)
          + (Sector(*p, r, y).type == type));

    if (y < (p->Maxy - 1)) {
        n += ((Sector(*p, x, y + 1).type == type)
              + (Sector(*p, l, y + 1).type == type)
              + (Sector(*p, r, y + 1).type == type));
    }

    return n;
}

/*
 * Randomly places n sector of designation type on a planet
 */
void seed(planettype *p, int type, int n)
{
    int x;
    int y;
    sectortype *s;

    while (n > 0) {
        x = int_rand(0, p->Maxx - 1);
        y = int_rand(0, p->Maxy - 1);
        s = &Sector(*p, x, y);
        s->condition = type;
        s->type = s->condition;
        --n;
    }
}

/*
 * Spread out a sector of a certain type over the planet. Rate is the number of
 * adjacent sectors of the same type that must bye found for the sector to
 * become type.
 */
void grow(planettype *p, int type, int n, int rate)
{
    int x;
    int y;
    sectortype *s;
    sectortype Smap2[((MAX_X + 1) * (MAX_Y + 1)) + 1];

    while (n > 0) {
        memcpy(Smap2, Smap, sizeof(Smap));

        for (x = 0; x < p->Maxx; ++x) {
            for (y = 0; y < p->Maxy; ++y) {
                if (neighbors(p, x, y, type) > rate) {
                    s = &Smap2[x + (y * p->Maxx) + 1];
                    s->type = type;
                    s->condition = s->type;
                }
            }
        }

        memcpy(Smap, Smap2, sizeof(Smap));
        --n;
    }
}

void Makesurface(planettype *p)
{
    int x;
    int y;
    /*
     * int x2;
     * int y2;
     * int xx;
     */
    int temp;
    sectortype *s;

    for (x = 0; x < p->Maxx; ++x) {
        for (y = 0; y < p->Maxy; ++y) {
            s = &Sector(*p, x, y);
            temp = SectTemp(p, y);

            switch (s->type) {
            case SEA:
                if (success(-temp) && ((y == 0) || (y == (p->Maxy - 1)))) {
                    s->condition = ICE;
                }

                break;
            case LAND:
                if (p->type == TYPE_EARTH) {
                    if (success(-temp) && ((y == 0) || (y == (p->Maxy - 1)))) {
                        s->condition = ICE;
                    }
                }

                break;
            case FOREST:
                if (p->type == TYPE_FOREST) {
                    if (success(-temp) && ((y == 0) || (y == (p->Maxy - 1)))) {
                        s->condition = ICE;
                    }
                }

                break;
            }

            s->type = s->condition;
            s->resource = int_rand(r_min[p->type][s->type],
                                   r_max[p->type][s->type]);
            s->fert = int_rand(f_min[p->type][s->type],
                               f_max[p->type][s->type]);

            if (int_rand(0, 1000) < x_chance[s->type]) {
                s->crystals = int_rand(4, 8);
            } else {
                s->crystals = 0;
            }
        }
    }
}

short SectTemp(planettype *p, int y)
{
    int dy;
    int mid;
    int temp;

    temp = p->conditions[TEMP];
    mid = ((p->Maxy + 1) / 2) - 1;
    dy = abs(y - mid);
    temp -= (TFAC * dy * dy);

    /*
     * return(p->conditions[TEMP]);
     */
    return temp;
}

void jovian_with_water(planettype *p)
{
    int nbands;
    int i;
    int x;
    int y;
    sectortype *s;

    if (p->Maxx > MAP_GASGIANT_BANDMIN) {
        nbands = MAX(1, p->Maxy / 5);

        for (i = 1; i <= nbands; ++i) {
            y = int_rand(1, p->Maxy - 2);

            for (x = 0; x < p->Maxx; ++x) {
                s = &Sector(*p, x, y);

                /*
                 * Make random bands of water
                 *
                 * Let's keep the type gas and just modify the condition -mfw
                 * s->resource = SEA;
                 * s->type = s->resource;
                 * s->condition = s->type;
                 */
                s->resource = SEA;
                s->condition = s->resource;
                s->fert += int_rand(20, 30);
            }
        }

        for (i = 0; i <= (MAP_ISLANDS * 2); ++i) {
            y = int_rand(1, p->Maxy - 2);
            x = int_rand(1, p->Maxx - 2);

            s = &Sector(*p, x, y);

            /*
             * Make random spots of water
             */
            s->resource = SEA;
            s->type = s->resource;
            s->condition = s->type;
            s->fert += int_rand(60, 90);
        }
    } else {
        for (i = 0; i < (MAP_ISLANDS * 2); ++i) {
            s = &Sector(*p, int_rand(1, p->Maxx - 2), int_rand(1, p->Maxy - 2));
            s->resource = SEA;
            s->type = s->resource;
            s->condition = s->type;
            s->fert = int_rand(60, 90);
        }

        /*
         * -mfw
         *
         * Smashup(&planet, int_rand(2, 4), SEA, 0, 0, 0);
         * Smashup(&planet, int_rand(1, 2), LAND, 2, 3, 1);
         */
    }
}

/*
 * Smash psuedo-meteors into the planet
 *
* I pulled this over from original GB and ported it -mfw
*/
void smashup(planettype *pptr, int n, char desig, int res, int fert, int waste)
{
    /*
     * n: # iterations
     * desig: des to make target sectors
     * res: resource
     * fert: fert to add
     * waste: whether to waste
     */
    int q;
    int r;
    int x;
    int y;
    int x2;
    int y2;
    sectortype *s;

    while (n) {
        x = int_rand(1, pptr->Maxx - 2);
        y = int_rand(1, pptr->Maxy - 2);
        r = int_rand(2, int_rand(2, (pptr->Maxy / 3) + 1));

        for (y2 = y - r - 1; y2 <= (y + r + 1); ++y2) {
            for (x2 = x - r - 1; x2 <= (x + r+ 1); ++x2) {
                q = DistmapSq(x, y, x2, y2);

                if (q < r) {
                    if ((x2 < pptr->Maxx)
                        && (x2 >= 0)
                        && (y2 < pptr->Maxy)
                        && (y2 >= 0)) {
                        s = &Smap[(y2 * pptr->Maxx) + x2];

                        if (!int_rand(0, q)) {
                            s->type = desig;
                        }

                        if (waste) {
                            s->condition = WASTED;
                            s->type = WASTEd;
                        }

                        if (res) {
                            s->resource += int_rand(res / 2, res * 2);
                        }

                        if (fert) {
                            s->fert += int_rand(fert / 2, fert * 2);
                        }
                    }
                }
            }
        }

        --n;
    }
}

/*
 * I pulled this over from original GB, but it seems like it is much like the
 * grow() function, so it's not currently being used. -mfw
 */
int Volcano(planettype *pptr,
            int landsectors,
            int numlandsects,
            int continent,
            int type)
{
    /*
     * type=MOUNT or DESERT for example
     */
    int x;
    int y;
    sectortype *s;

    x = int_rand(0, pptr->Maxx - 1);
    y = int_rand(0, pptr->Maxy - 1);
    s = &Sector(*pptr, x, y);

    if (y > (pptr->Maxy / 2)) {
        if (y == (pptr->maxy - 1)) {
            s->type = ICE;

            return ((random() % 2) == 1);
        } else {
            if ((Sector(*pptr, x, y +1) == ICE)
                && (int_rand(-50, 20) > pptr->conditions[RTEMP])) {
                s->type = ICE;

                return ((random() % 2) == 1);
            }
        }
    } else {
        if (y == 0) {
            s->type = ICE;

            return ((random() % 2) == 1);
        } else if ((Sector(*pptr, x, y - 1).type == ICE)
                   && (int_rand(-50, 20) > pptr->conditions[RTEMP])) {
            s->type = ICE;

            return ((random() % 2) == 1);
        }
    }

    if (continent) {
        if (num_neighbors(pptr, x, y, LAND)
            || num_neighbors(pptr, x, y, type)) {
            /*
             * if (landsectors > (MAP_MOUNT_PERCENT * numlandsects)) {
             *     s->type = LAND;
             * } else {
             *     s->type = type;
             * }
             */
            if (int_rand(0, 1)) {
                s->type = LAND;
            } else {
                s->type = type;
            }

            s->resource = round_rand(
                ((float)(numlandsects - landsectors - 5) / (float)numlandsects)
                * 100);

            /*
             * Min content prop to dist from sea
             */
            if ((s->type == SEA) || (s->type == FOREST)) {
                s->fert =
                    ((float)(landsectors + 5) / (float)numlandsects) * 100;
            }

            /*
             * fert content prop to dist from center of continent
             */
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

int TemperatureFOO(float dist, short stemp)
{
    float factor;

    factor = 20.0 * (((float)stemp * (1000.0 / dist)) - 6.0);

    return (int)factor;
}

/*
 * Return # of neighbots of a given desgination that a sector has
 */
int num_niehgbors(planettype *p, int x, int y, int type)
{
    int d;
    int count = 0;

    count = (Sector(*p, mod(x - 1, p->Maxx, d), y).type == type);
    count += (Sector(*p, mod(x + 1, p->Maxx, d), y).type == type);

    if (y == 0) {
        count += 0;
    } else {
        count += (Sector(*p, x, y - 1).type == type);

        if (y == (p->Maxy - 1)) {
            count += 0;
        } else {
            count += (Sector(*p, x, y + 1).type == type);
        }
    }

    return count;
}
