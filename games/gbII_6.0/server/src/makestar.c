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
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY of FITNESS
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
 * makestar.c -- Create, name, position and make planets for a star.
 *
 * #ident "@(#)makestar.c 1.3 2/17/93 "
 */

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/*
 * (for power)
 */
#include "power.h"
#include "races.h"
#include "ships.h"
#include "vars.h"

#if 0
G.O.D[1] > methane melts at -182C
G.O.D[1] > it boils at -164
G.O.D[1] > ammonia melts at -78C
G.O.D[1] > boils at -33
#endif

#define PLANET_DIST_MAX 1900.0
#define PLANET_DIST_MIN 100.0

/*
 * Just in case
 */
extern void place_star(startype *star);
extern planettype Makeplanet(double, int, int);
extern double double_rand(void);
extern int int_rand(int, int);
extern int round_rand(double);
extern int rposneg(void);


/*
 * Variables found in makeuniv.c
 */
extern int autoname_plan;
extern int autoname_star;
extern int minplanets;
extern int maxplanets;
extern int nstars;
extern int planetlesschance;
extern int printplaninfo;
extern int printstarinfo;

startype *Makestar(FILE *, FILE *, FILE *);
void Makeplanet_init(void);
char const *NextPlanetName(int);
void rand_list(int, int *);

double distmin;
int numplist;
int namepcount;
int numslist;
int namestcount;
char SNames[1000][20];
char PNames[1000][20];
int planet_list[1000];
int star_list[1000];
int Numtypes[TYPE_DESERT + 2] = { 0 };
int Resource[TYPE_DESERT + 2] = { 0 };
int Numsects[TYPE_DESERT + 2][PLATED + 1] = { {0} };
int Fertsects[TYPE_DESERT + 2][PLATED + 1] = { {0} };
char const *Nametypes[] = {
    "Earth",
    "Asteroid",
    "Airless",
    "Iceball",
    "Gaseous",
    "Water",
    "Forest",
    "Desert",
    "Wormhole",
    ">>"
};

char const *Namesects[] = {
    "sea",
    "land",
    "mountain",
    "gaseous",
    "ice",
    "desert",
    "forest",
    "plated"
};

int Temperature(double dist, int stemp)
{
    return (-269 + (((stemp * 1315) * 40) / (40 + dist)));
}

void PrintStatistics(FILE *outputtxt)
{
    int i;
    int j;
    int y;

    fprintf(outputtxt, "\nPlanet/Sector distribution -\n");
    fprintf(outputtxt, "Type ");
    fprintf(outputtxt, "NP     ");
    fprintf(outputtxt, ".    ");
    fprintf(outputtxt, "*    ");
    fprintf(outputtxt, "^    ");
    fprintf(outputtxt, "~    ");
    fprintf(outputtxt, "#    ");
    fprintf(outputtxt, "(    ");
    fprintf(outputtxt, "-    ");
    fprintf(outputtxt, "NS   ");
    fprintf(outputtxt, "Avg     ");
    fprintf(outputtxt, "Res    ");
    fprintf(outputtxt, "A/Sec\n");

    for (i = 0; i < (TYPE_DESERT + 1); ++i) {
        fprintf(outputtxt, "%3.3s%4d ", Nametypes[i], Numtypes[i]);

        if (i < (TYPE_DESERT + 1)) {
            Numtypes[TYPE_DESERT + 1] += Numtypes[i];

            for (j = 0; j < PLATED; ++j) {
                fprintf(outputtxt, "%5d", Numsects[i][j]);
                Numsects[i][PLATED] += Numsects[i][j];

                if (i <= TYPE_DESERT) {
                    Numsects[TYPE_DESERT + 1][j] += Numsects[i][j];
                }
            }
        } else {
            for (j = 0; j < PLATED; ++j) {
                Numsects[i][PLATED] += Numsects[i][j];

            }

            for (j = 0; j < PLATED; ++j) {
                fprintf(outputtxt,
                        "%5.1f",
                        (100.0 * (float)Numsects[i][j])
                        / (float)Numsects[i][PLATED]);
            }
        }

        fprintf(outputtxt,
                "%6d %5.1f",
                Numsects[i][PLATED],
                (float)Numsects[i][PLATED] / (float)Numtypes[i]);

        fprintf(outputtxt,
                "%8d %7.1f %5.1f\n",
                Resource[i],
                (double)Resource[i] / Numtypes[i],
                (double)Resource[i] / Numsects[i][PLATED]);

        Resource[TYPE_DESERT + 1] += Resource[i];
    }

    fprintf(outputtxt, "\nAverage Sector Fertility -\n");
    fprintf(outputtxt, "Type ");
    fprintf(outputtxt, "NP    ");
    fprintf(outputtxt, ".    ");
    fprintf(outputtxt, "*    ");
    fprintf(outputtxt, "^    ");
    fprintf(outputtxt, "~    ");
    fprintf(outputtxt, "#    ");
    fprintf(outputtxt, "(    ");
    fprintf(outputtxt, "-    ");
    fprintf(outputtxt, "Fert  ");
    fprintf(outputtxt, "/Plan  ");
    fprintf(outputtxt, "/Sect\n");

    for (i = 0; i <= (TYPE_DESERT + 1); ++i) {
        fprintf(outputtxt, "%3.3s%4d ", Nametypes[i], Numtypes[i]);
        y = 0;

        for (j = 0; j < PLATED; ++j) {
            if (Numsects[i][j]) {
                fprintf(outputtxt,
                        "%5.1f",
                        (double)Fertsects[i][j] / Numsects[i][j]);
            } else {
                fprintf(outputtxt, "    -");
            }

            y += Fertsects[i][j];
            Fertsects[TYPE_DESERT + 1][j] += Fertsects[i][j];
        }

        fprintf(outputtxt,
                "%8d %7.1f %5.1f\n",
                y,
                (1.0 * y) / Numtypes[i],
                (1.0 * y) / Numsects[i][PLATED]);
    }
}

int ReadNameList(char ss[1000][20], int n, int m, char const *filename)
{
    int i;
    int j;
    int flag;
    FILE *f = fopen(filename, "r");

    if (f == NULL) {
        printf("Unable to open \"%s\"\n", filename);

        return -1;
    }

    for (i = 0; i < n; ++i) {
        flag = 0;

        for (j = 0; j < m; ++j) {
            ss[i][j] = getc(f);

            if (ss[i][j] == '\n') {
                /*
                 * This check is here to prevent empty string for name. If there
                 * is an empty string, put 'Q' for name, otherwise cut the
                 * string setting last char to '\0' /Gardan 4.6.97
                 */
                if (j != 0) {
                    ss[i][j] = '\0';
                } else {
                    /*
                     * Empty text lines cause problems!
                     * scrolli
                     */
                    fclose(f);
                    printf("%d names listed in %s\n", i, filename);

                    return i;
                }

                flag = 1;

                break;
            } else if (!isprint((unsigned char)ss[i][j])) {
                fclose(f);
                printf("%d names listed in %s\n", i, filename);

                return i;
            }
        }

        if (!flag) {
            ss[i][j] = '\0';
            j = getc(f);

            while (j != '\n') {
                if (EOF == j) {
                    fclose(f);
                    printf("%d names listed in %s\n", i, filename);

                    return i;
                }

                j = getc(f);
            }
        }
    }

    fclose(f);
    printf("%d names listed in %s\n", i, filename);

    return i;
}

/*
 * Mix up the numbers 0 through n
 */
void rand_list(int n, int *list)
{
    short nums[1000];
    short i;
    short j;
    short k;
    short kk;
    short ii;

    for (i = 0; i <= n; ++i) {
        nums[i] = 0;
    }

    for (j = 0; j <= n; ++j) {
        k = int_rand(0, n);
        i = k;

        while (nums[k] != 0) {
            k += nums[k];
        }

        list[j] = k;

        if (k == n) {
            nums[k] = -n;
            kk = 0;
        } else {
            nums[k] = 1;
            kk = k + 1;
        }

        /*
         * K is now the next position in the list after the most recent number.
         * Go through the list, make each pointer point to k.
         */
        while (i != k) {
            ii = i + nums[i];
            nums[i] = kk - 1;
            i = ii;
        }
    }
}

void Makeplanet_init(void)
{
    numplist = ReadNameList(PNames, 1000, 20, PLANETLIST);
    rand_list(numplist, planet_list);

    if (numplist < 0) {
        exit(0);
    }

    namepcount = 0;
}

char const *NextPlanetName(int i)
{
    static const char *Numbers[] = {
        "1", "2", "3", "4", "5", "6", "7", "8",
        "9", "10", "11", "12", "13", "14", "15"
    };

    if (autoname_plan && (namepcount < numplist)) {
        ++namepcount;
        return PNames[planet_list[namepcount - 1]];
    } else {
        return Numbers[i];
    }
}

void Makestar_init(void)
{
    numslist = ReadNameList(SNames, 1000, 20, STARLIST);
    rand_list(numslist, star_list);

    if (numslist < 0) {
        exit(0);
    }

    namestcount = 0;
}

char *NextStarName(void)
{
    static char buf[20];
    int i;

    if (autoname_star && (namestcount <= numslist)) {
        ++namestcount;
        return SNames[star_list[namestcount - 1]];
    } else {
        printf("Next star name:");

        for (i = 0; i < (NAMESIZE - 4); ++i) {
            putchr('.');
        }

        for (i = 0; i < (NAMESIZE - 4); ++i) {
            /*
             * ^H
             */
            putchr('\010');
        }

        scanf("%14[^\n]", buf);
        getchr();
    }

    return buf;
}

startype *Makestar(FILE *planetdata, FILE *sectordata, FILE *outputtxt)
{
    planettype planet;
    int type;
    int roll;
    int temperature;
    /*
     * int empty_slots;
     */
    int i;
    int y;
    int x;
    double dist;
    double distmax;
    double distsep;
    double angle;
    double xpos;
    double ypos;
    startype *Star;

    /*
     * Get names, positions of stars first
     */
    Star = malloc(sizeof(startype));
    memset(Star, 0, sizeof(startype));

    Star->gravity = int_rand(0, int_rand(0, 300))
        + int_rand(0, 300)
        + int_rand(100, 400)
        + (int_rand(0, 9) / 10.0);

    Star->temperature = round_rand(Star->gravity / 100.0);
    /*
     * + int_rand(0, 2) - 1;
     */
    strcpy(Star->name, NextStarName());
    place_star(Star);

    if (printstarinfo) {
        fprintf(outputtxt,
                "Star %s: position (%d, %d), gravity %1.1f, temp %d\n\n",
                Star->name,
                (int)Star->xpos,
                (int)Star->ypos,
                Star->gravity,
                (int)Star->temperature);
    }

    /*
     * Generate planets for this star
     *
     * Gardan's code addon planetlesschance is modified for each call of this
     * function
     */
    if (int_rand(0, 99) > planetlesschance) {
        Star->numplanets = int_rand(minplanets, maxplanets);
    } else {
        Star->numplanets = 0;
    }

    distmin = PLANET_DIST_MIN;

    for (i = 0; i < Star->numplanets; ++i) {
        distsep = (PLANET_DIST_MAX - distmin) / (double)(Star->numplanets - i);
        distmax = distmin + distsep;
        dist = distmin + (double_rand() * (distmax - distmin));
        distmin = dist;
        temperature = Temperature(dist, Star->temperature);

        if (((temperature > 100) || (temperature < 0)) && !int_rand(0, 3)) {
            --i;

            continue;
        }

        angle = 2.0 * M_PI * double_rand();
        xpos = dist * sin(angle);
        ypos = dist = cos(angle);

        strcpy(Star->pnames[i], NextPlanetName(i));

        roll = int_rand(1, 100);

        if (temperature > 400) {
            type = TYPE_ASTEROID;
        } else if (temperature > 250) {
            if (roll <= 60) {
                type = TYPE_MARS;
            } else {
                type = TYPE_ASTEROID;
            }
        } else if (temperature > 100) {
            /*
             * Gardan: new
             */
            if (roll <= 5) {
                type = TYPE_ASTEROID;
            } else if (roll <= 60) {
                type = TYPE_MARS;
            } else {
                type = TYPE_DESERT;
            }
        } else if (temperature > 30) {
            /*
             * Gardan: new
             */
            if (roll <= 5) {
                type = TYPE_ASTEROID;
            } else if (roll <= 35) {
                /*
                 * Gardan: was 30
                 */
                type = TYPE_EARTH;
            } else if (roll <= 60) {
                /*
                 * Gardan: was 65
                 */
                type = TYPE_WATER;
            } else {
                type = TYPE_FOREST;
            }
        } else if (temperature > -1) {
            /*
             * Gardan: new
             */
            if (roll <= 5) {
                type = TYPE_ASTEROID;
            } else if (roll <= 35) {
                /*
                 * Gardan: was 45
                 */
                type = TYPE_EARTH;
            } else if (roll <= 75) {
                /*
                 * Gardan: was 60
                 */
                type = TYPE_WATER;
            } else if (roll <= 90) {
                /*
                 * Gardan: was 80
                 */
                type = TYPE_FOREST;
            } else {
                type = TYPE_DESERT;
            }
        } else if (temperature > -50) {
            /*
             * Gardan: new
             */
            if (roll <= 5) {
                type = TYPE_ASTEROID;
            } else if (roll <= 25) {
                /*
                 * Gardan: was 30
                 */
                type = TYPE_DESERT;
            } else if (roll <= 40) {
                /*
                 * Gardan: new one
                 */
                type = TYPE_WATER;
            } else if (roll <= 65) {
                /*
                 * Gardan: was 60
                 */
                type = TYPE_ICEBALL;
            } else if (roll <= 80) {
                type = TYPE_FOREST;
            } else {
                type = TYPE_MARS;
            }
        } else if (temperature > -100) {
            /*
             * Gardan: new
             */
            if (roll <= 5) {
                type = TYPE_ASTEROID;
            } else if (roll <= 40) {
                type = TYPE_GASGIANT;
            } else if (roll <= 80) {
                type = TYPE_ICEBALL;
            } else {
                type = TYPE_MARS;
            }
        } else {
            /*
             * Gardan: new
             */
            if (roll <= 5) {
                type = TYPE_ASTEROID;
            } else if (roll <= 80) {
                type = TYPE_ICEBALL;
            } else {
                type = TYPE_GASGIANT;
            }
        }

        planet = Makeplanet(dist, Star->temperature, type);
        planet.xpos = xpos;
        planet.ypos = ypos;
        planet.total_resources = 0;
        ++Numtypes[type];

        if (printplaninfo) {
            fprintf(outputtxt,
                    "Planet %s: temp %d, type %s (%u)\n",
                    Star->pnames[i],
                    planet.conditions[RTEMP],
                    Nametypes[planet.type],
                    planet.type);

            fprintf(outputtxt,
                    "Position is (%1.0f,%1.0f) relative to %s; distance %1.0f.\n",
                    planet.xpos,
                    planet.ypos,
                    Star->name,
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
                        fprintf(outputtxt, "%c", CHAR_FOREST);

                        break;
                    case PLATED:
                        fprintf(outputtxt, "%c", CHAR_PLATED);

                        break;
                    case WASTED:
                        fprintf(outputtxt, "%c", CHAR_WASTED);

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

        /*
         * Tabulate statistics for this star's planets.
         */
        for (y = 0; y < planet.Maxy; ++y) {
            for(x = 0; x < planet.Maxx; ++x) {
                char d = Sector(planet, x, y).condition;
                planet.total_resources += Sector(planet, x, y).resource;
                Resource[type] += Sector(planet, x, y).resource;
                ++Numsects[type][(unsigned int)d];
                Fertsects[type][(unsigned int)d] += Sector(planet, x, y).fert;
            }
        }

        Star->planetpos[i] = (int)ftell(planetdata);

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
    }

    return Star;
}
