/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See the GB_copyright.h for additional authors and details.
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
 * You should have received a copy of the GNU Geneal Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Stree, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * makewormhole.c -- Makes one worm hole.
 *
 * Written by: Time Brown
 */

#include <math.h>
#include <string.h>

#include "vars.h"

#define PLANET_DIST_MAX 1900.0
#define PLANET_DIST_MIN 100.0

extern char *Nametypes[];
extern char Namesects[];
extern int printplaninfo;
extern double distmin;

extern double double_rand(void);

char const *NextPlanetName(int);

void make_wormhole(startype *star,
                   FILE *planetdata,
                   FILE *sectordata,
                   FILE *outputtxt)
{
    planettype planet;
    double angle;
    double diststep;
    double distmax;
    double dist;
    int x;
    int y;
    sectortype *s;

    memset((char *)&planet, 0, sizeof(planettype));
    memset((char *)Smap, 0, sizeof(Smap));

    ++star->numplanets;
    star->planetpos[star->numplanets - 1] = (int)ftell(planetdata);

    /*
     * -mfw
     *
     * sprintf(star->pnames[star->numplanets - 1], "%d", star->numplanets);
     */
    strcpy(star->pnames[star->numplanets - 1],
           NextPlanetName(star->numplanets));

    distsep = (PLANET_DIST_MAX - distmin) / (double)(star->numplanets - 1);
    distmax = distmin + distsep;
    dist = distmin + distsep;

    /*
     * Calculate stability of wormhole here
     */
    star->wh_stability = (unsigned short)
        ((double)(star->gravity / (double)(star->temperature * 100))
         * star->stability);

    if ((dist > 1500) && (star->wh_stability > 1)) {
        --star->wh_stability;
    }

    star->wh_has_wormhole = 1;
    star->wh_dest_starnum = -1;

    planet.Maxx = 2;
    planet.Maxy = 2;
    planet.type = TYPE_WORMHOLE;
    planet.conditions[HYDROGEN] = 0;
    planet.conditions[HELIUM] = 0;
    planet.conditions[METHANE] = 0;
    planet.conditions[OXYGEN] = 0;
    planet.conditions[CO2] = 0;
    planet.conditions[NITROGEN] = 0;
    planet.conditions[SULFUR] = 0;
    planet.conditions[OTHER] = 0;
    planet.conditions[RTEMP] = 1000;
    planet.conditions[TEMP] = planet.conditions[RTEMP];

    angle = 2.0 * M_PI * double_rand();

    /*
     * printf("A=%f, dmin=%f, dsep=%f, dmax=%f, dist=%f\n",
     *        angle,
     *        distmin,
     *        distsep,
     *        dist);
     */

    planet.xpos = dist * sin(angle);
    planet.ypos = dist * cos(angle);

    for (y = 0; y < planet.Maxy; ++y) {
        for (x = 0; x < planet.Maxx; ++x) {
            s = &Sector(planet, x, y);
            s->condition = WORM;
            s->type = s->condition;
            s->crystals = 0;
            s->fert = s->crystals;
            s->resource = s->fert;
        }
    }

    /*
     * posn of file-last write
     */
    /*
     * Sector map pos
     */
    planet.sectormappos = (int)ftell(sectordata);

    /*
     * Write planet
     */
    fwrite(&planet, sizeof(planettype), 1, planetdata);

    /*
     * Write each sector row
     */
    for (y = 0; y < planet.Maxy; ++y) {
        fwrite(&Sector(planet, 0, y),
               sizeof(sectortype),
               planet.Maxx,
               sectordata);
    }

    if (printplaninfo) {
        fprintf(outputtxt,
                "Planet %s: temp %d, type %s (%u)\n",
                star->pnames[star->numplanets - 1],
                planet.conditions[RTEMP],
                Nametypes[planet.type],
                planet.type);

        fprintf(outputtxt,
                "Position is (%1.0f,%1.0f) relative to %s; distance %1.0f.\n",
                planet.xpos,
                planet.ypos,
                star->name,
                dist);

        fprintf(outputtxt, "sect map(%dx%d):\n", planet.Maxx, planet.Maxy);

        for (y = 0; y < planet.Maxy; ++y) {
            for (x = 0; x < planet.Maxx; ++x) {
                switch (Sector(planet, x, y).condition) {
                case LAND:
                    fprintf(outputtxt, "%c", CHAR_LAND);

                    break;
                case SEA:
                    fprintf(outputtxt, "%c", CHAR_SEA);

                    break;
                case MOUNT:
                    fprintf(outputtxt, "%c", CHAR_MOUNT);

                    break;
                case ICE:
                    fprintf(outputtxt, "%c", CHAR_ICE);

                    break;
                case GAS:
                    fprintf(outputtxt, "%c", CHAR_GAS);

                    break;
                case DESERT:
                    fprintf(outputtxt, "%c", CHAR_DESERT);

                    break;
                case FOREST:
                    fprintf(outputxt, "%c", CHAR_FOREST);

                    break;
                case WORM:
                    fprintf(outputtxt, "%c", CHAR_WORM);

                    break;
                default:
                    fprintf(outputtxt, "?");

                    break;
                }
            }

            fprintf(outputtxt, "\n");
        }

        fprintf(outputtxt, "\n");
    }
}
