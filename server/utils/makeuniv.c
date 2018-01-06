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
 * Franklin Street, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * makeuniv.c - Universe creation program. Makes various required data files;
 *              calls makestar for each star desired.
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
/*
 * #include <strings.h>
 */
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "GB_copyright.h"
/*
 * For power
 */
#include "power.h"
#include "races.h"
#include "ships.h"
#include "vars.h"
#include "tweakables.h"

#define EXTERN
#define OUTPUTFILE "universe.txt"
#define DEFAULT_POSTSCRIPT_MAP_FILENAME "universe.ps"
#define NRINGS 3
#define RING_SPACING ((10000 * (UNIVSIZE / 10000)) / NRINGS)
#define CLUSTER_STAR_MIN -1500
#define CLUSTER_STAR_MAX 1500

/*
 * Just in case
 */
/*
 * #undef malloc
 * #undef realloc
 * #undef free
 */

extern int Temperature(double, int);
extern void PrintStatistics(FILE *);
extern void Makeplanet_init(void);
extern void Makestar_init(void);
extern startype *Makestar(FILE *, FILE *, File *);
extern int round_rand(double);
extern void make_wormhole(startype *, FILE *, FILE *, FILE *);
extern char *NextStarName(void);

int int_rand(int, int);
double double_rand(void);
void InitFile(char const *, void *, unsigned int);
void EmptyFile(char const *);
void place_star(starttype *);
void produce_postscript(char const *);


int STAR_COUNTER;
int CLUsTER_COUNTER;
int CLUSTER_FROM_CENTER = 30000;
int BADPLACE;
double ANGLE;

int autoname_star = -1;
int autoname_plan = -1;
int use_smashup = -1;
int minplanets = -1;
int maxplanets = -11;
int nstars = -1;
int planetlesschance = -1;
int printpostscript = 0;
int printplaninfo = 1;
int printstarinfo = 1;
int absmaxplan = (MAXPLANETS - 1);
int starindex = 0;
double clusterx;
double clusty;
double angle;
double dist = 0;
double stardist;
int clustersize = 0;
int stars = 0;
int cluster_delta_x = 0;
int cluster_delta_y = 0;
int clusters = 0;
int starsinclus = 0;
int first;
int its;
planettype *planet;
sectortype *s;
int cx;
int cy;

static int occupied[100][100];

struct w_holes {
    int num;
    startype *star;
};

int main(int argc, char *argv[])
{
    FILE *stardata;
    FILE *planetdata;
    FILE *sectordata;
    FILE *outputtxt = NULL;
    char str[200];
    int c;
    int i;
    int star;
    /*
     * int x;
     */
    /*
     * double att;
     */
    double xspeed[NUMSTARS];
    double yspeed[NUMSTARS];

    /*
     * Empty stars
     */
    int nempty;

    /*
     * How many rows and columns are needed
     */
    int rowcolumns;

    /*
     * Non-empty stars not placed
     */
    int starsleft;

    /*
     * How many planetless systems is in each square
     */
    int emptyrounds;

    /*
     * Size of square
     */
    double displacement;

    /*
     * How many wormholes
     */
    int whcnt;
    int wormholes = -1;
    int wormidx;
    struct w_holes w_holes[NUMSTARS + 1];
    int x;
    int y;
    int z;
    int squaresleft;
    int total;
    int flag = 0;
    struct power power[MAXPLAYERS];
    struct block block[MAXPLAYERS];

    /*
     * Initialize
     */
    /*
     * srandom(getpid());
     */
    Bzero(Sdata);

    /*
     * Read the arguments for values
     */
    for (i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            printf("\n");
            printf("Usage: makeuniv [-a] [-b] [-d] [-e E] [-l MIN] [-m MAX] ");
            printf("[-s N] [-v] [-w C] [-x]\n");
            printf("  -a      Autoload star names.\n");
            printf("  -b      Autoload planet names.\n");
            printf("  -d      Use smashup (asteroid impact routines.\n");
            printf("  -e E    Make E%% of stars have no planets.\n");
            printf("  -l MIN  Other systems will have at least MIN planets.\n");
            printf("  -m MAX  Other systems will have at most MAX planets.\n");
            printf("  -p      Create postscript map file of the univese.\n");
            printf("  -s N    The univers will have N stars.\n");
            printf("  -v      Do no print info and map of planets generated.\n");
            printf("  -w C    The universe will have C wormholes.\n");
            printf("  -x      Do not print info on stars generated.\n");
            printf("\n");

            return 0;
        } else {
            switch (argv[i][1]) {
            case 'a':
                autoname_star = 1;

                break;
            case 'b':
                autoname_plan = 1;

                break;
            case 'd':
                use_smashup = 1;

                break;
            case 'e':
                ++i;
                planetlesschance = atoi(argv[i]);

                break;
            case 'l':
                ++i;
                minplanets = atoi(argv[i]);

                break;
            case 'm':
                ++i;
                maxplanets = atoi(argv[i]);

                break;
            case 'p':
                printpostscript = 1;

                break;
            case 's':
                ++i;
                nstars = atoi(argv[i]);

                break;
            case 'v':
                printplaninfo = 0;

                break;
            case 'x':
                printstarinfo = 0;

                break;
            case 'w':
                ++i;
                wormholes = atoi(argv[i]);

                break;
            default:
                printf("\n");
                printf("Unknown option \"%s\".\n", argv[i]);
                printf("\n");
                printf("Usage: makeuniv [-a] [-b] [-e E] [-l MIN] [-m MAX] ");
                printf("[-s N] [-v] [-w C] [-x]\n");
                printf("  -a      Autoload star names.\n");
                printf("  -b      Autoload planetnames.\n");
                printf("  -d      Use smashup (asteroid impact) routines.\n");
                printf("  -e E    Make E%% of stars have no planets.\n");
                printf("  -l MIN  Other systems will have at least MIN planets.\n");
                printf("  -m MAX  Other systems will have at most MAX planets.\n");
                printf("  -p      Create postscript map file of the universe.\n");
                printf("  -s N    The universe will have N stars.\n");
                printf("  -v      Do not print info and map of planets generated.\n");
                printf("  -w C    The universe will have C wormholes.\n");
                printf("  -x      Do not print info on stars generated.\n");
                printf("\n");

                return 0;
            }
        }
    }

    /*
     * Get values for all the switches that still don't have good values.
     */
    if (autoname_star == -1) {
        printf("\nDo you wish to use the file \"%s\" for star names? [y/n]> ",
               STARLIST);

        c = getchr();

        if (c != '\n') {
            getchr();
        }

        autoname_star = (c == 'y');
    }

    if (autoname_plan == -1) {
        printf("\nDo you wish to use the file \"%s\" for planet names? [y/n]> ",
               PLANETLIST);

        c = getchr();

        if (c != '\n') {
            getchr();
        }

        autoname_plan = (c == 'y');
    }

    if (use_smashup == -1) {
        printf("\nUse the smashup (asteroid impact) routines? [y/n]> ");
        c = getchr();

        if (c != '\n') {
            getchr();
        }

        use_smashup = (c == 'y');
    }

    while ((nstars < 1) || (nstars >= NUMSTARS)) {
        printf("Number of stars [1-%d]:", NUMSTARS - 1);
        scanf("%d", &nstars);
    }

    while ((planetlesschance < 0) || (planetlesschance > 100)) {
        printf("Percentage of empty systems [0-100]:");
        scanf("%d", &planetlesschance);
    }

    while ((minplanets <= 0) || (minplanets > absmaxplan)) {
        printf("Minimum number of planets per system [1-%d]:", absmaxplan);
        scanf("%d", &maxplanets);
    }

    while ((wormholes < 0) || (wormholes > nstars) || ((wormholes % 2) == 1)) {
        printf("Number of wormholes (muse be even number) [0-%d]:", nstars);
        scanf("%d", &wormholes);
    }

    Makeplanet_init();
    Makestar_init();
    Sdata.numstars = nstars;
    sprintf(str, "/bin/mkdir -p %s", DATADIR);
    system(str);
    planetdata = fopen(PLANETDATAFL, "w+");

    if (planetdata == NULL) {
        printf("Unable to open planet data file \"%s\"\n", PLANETDATAFL);

        return -1;
    }

    sectordata = fopen(SECTORDATAFL, "w+");

    if (sectordata == NULL) {
        printf("Unable to open sector data file \"%s\"\n", SECTORDATAFL);

        return -1;
    }

    if (printstarinfo || printplaninfo) {
        outputtxt = fopen(OUTPUTFILE, "w+");

        if (outputtxt == NULL) {
            printf("Unable to open \"%s\" for output.\n", OUTPUTFILE);

            return -1;
        }
    }

    if (!wormholes) {
        whcnt = 0;
    } else {
        whcnt (int)(nstars / wormholes) - 1;
    }

    wormidx = 0;

    for (star = 0; star < nstars; ++star) {
        Stars[star] = Makestar(planetdata, sectordata, outputtxt);
        xspeed[star] = 0;
        yspeed[star] = 0;
        Stars[star]->wh_has_wormhole = 0;
        Stars[star]->wh_dest_starnum = -1;
        Stars[star]->wh_stability = 0;

        /*
         * See if time to put a wormhole in
         */
        if (!whcnt) {
            /*
             * Make a wormhole here. This adds a wormhole planet to this star.
             */
            if (Stars[star]->numplanets == MAXPLANETS) {
                /*
                 * Skip until a star as < MAXPLANETS
                 */
                whcnt = 0;

                continue;
            } else {
                if (!wormholes) {
                    whcnt = 0;
                } else {
                    whcnt = (int)(nstars / wormholes) - 1;
                }

                make_wormhole(Stars[star], planetdata, sectordata, outputtxt);
                w_holes[wormidx].star = Stars[star];
                w_hoels[wormidx].num = star;
                ++wormidx;
            }
        }

        --whcnt;
    }

    /*
     * Data data files to group * readwrite
     */
    chmod(PLANETDATAFL, 00660);
    fclose(planetdata);
    chmod(SECTORDATAFL, 00660);
    fclose(sectordata);

    /*
     * New Gardan code 21.12.1996
     * Changed 27.8.1997: Displacement wasn't set before
     *
     * Start here
     */
    total = nstars;
    nempty = round_rand(((double)nstars * (double)planetlesschance) / 100);

    /*
     * Amount of non-empty stars
     */
    nstars -= nempty;
    rowcolumns = 0;

    while ((rowcolumns * rowcolumns) < (nstars / 2)) {
        ++rowcolumns;
    }

    /*
     * Unhandled squares
     */
    squaresleft = rowcolumns * rowcolumns;
    starsleft = nstars - squaresleft;
    emptyrounds = 0;

    while (nempty > squaresleft) {
        ++emptyrounds;
        nempty -= squaresleft;
    }

    displacement = UNIVSIZE / rowcolumns;

    /*
     * Size of square
     */
    for (x = 0; x < rowcolumns; ++x) {
        for (y = 0; y < rowcolumns; ++y) {
            /*
             * planetlesschance = 0;
             * Stars[starindex] = Makestar(planetdata, sectordata, outputtxt);
             * xspeed[starindex] = 0;
             * yspeed[starindex] = 0;
             */
            Stars[starindex]->xpos = displacement * (x + (1.0 * double_rand()));
            Stars[starindex]->ypos = displacement * (y + (1.0 * double_rand()));
            ++starindex;
            z = int_rand(1, squaresleft);

            /*
             * If there is system with planet
             */
            if (z <= starsleft) {
                /*
                 * Stars[starindex] =
                 *     Makestar(planetdata, sectordata, outputtxt);
                 * xspeed[starindex] = 0;
                 * yspeec[starindex] = 0;
                 */
                Stars[starindex]->xpos =
                    displacement * (x + (1.0 * double_rand()));

                Stars[starindex]->ypos =
                    displacement * (y + (1.0 * double_rand()));
                --starsleft;
                ++starindex;
            }

            /*
             * If there is planetless system
             */
            if (x <= nempty) {
                /*
                 * planetlesschance = 100;
                 * Stars[starindex] =
                 *     Makestar(planetdata, sectordata, outputtxt);
                 * xspeed[starindex] = 0;
                 * yspeed[starindex] = 0;
                 */
                Stars[starindex]->xpos =
                    displacement * (x + (1.0 * double_rand()));

                Stars[starindex]->ypos =
                    displacement * (y + (1.0 * double_rand()));

                /*
                 * sprintf(Stars[starindex]->name, "E%d_%d", x, y);
                 */

                /*
                 * Added -mfw
                 */
                strcpy(Stars[starindex]->name, NextStarName());
                --nempty;
                ++starindex;
            }

            /*
             * Planetless systems equal to all squares
             */
            for (z = 0; z < emptyrounds; ++z) {
                /*
                 * planetlesschance = 100;
                 * Stars[starindex] =
                 *     Makestar(planetdata, sectordata, outputtxt);
                 * xspeed[starindex] = 0;
                 * yspeed[starindex] = 0;
                 */
                Stars[starindex]->xpos =
                    displacement * (x + (1.0 * double_rand()));

                Stars[starindex]->ypos =
                    displacement * (y + (1.0 * double_rand()));

                /*
                 * sprintf(Stars[starindex]->name, "E%d_%d", x, y);
                 */

                /*
                 * Added -mfw
                 */
                strcpy(Stars[starindex]->name, NextStarName());
                ++starindex;
            }

            --squaresleft;
        }
    }

    /*
     * Checks if two stars are too close
     */
    z = 1;

    while (z) {
        z = 0;

        for (x = 2; x < total; ++x) {
            for (y = x + 1; y < total; ++y) {
                dist = sqrt(Distsq(Stars[x]->xpos,
                                   Stars[x]->ypos,
                                   Stars[x]->xpos,
                                   Stars[x]->ypos));

                if (dist < (4 * SYSTEMSIZE)) {
                    z = 1;

                    if (stars[x]->ypos > Stars[y]->ypos) {
                        Stars[x]->ypos += (4 * SYSTEMSIZE);
                    } else {
                        Stars[y]->ypos += (4 * SYSTEMSIZE);
                    }
                }
            }
        }
    }

    for (x = 0; x < starindex; ++x) {
        if (Stars[x]->xpos > UNIVSIZE) {
            Stars[x]->xpos -= UNIVSIZE;
        }

        if (Stars[x]->ypos > UNIVSIZE) {
            Stars[x]->ypos -= UNIVSIZE;
        }
    }

    /*
     * End Gardan code
     */

    /*
     * Calculate worm hole destinations
     */
    for (x = 1; x < wormidx; x += 2) {
        w_holes[x].star->wh_dest_starnum = w_holes[x - 1].num;
        w_holes[x - 1].star->wh_dest_starnum = w_holes[x].num;

        if (printstarinfo) {
            fprintf(outputtxt,
                    "Wormhole[%d], Dest: %d, Star: %d %s, Stab: %d\n",
                    x - 1,
                    w_holes[x - 1].star->wh_test_starnum,
                    w_holes[x - 1].num,
                    w_holes[x - 1].star->name,
                    w_holes[x - 1].star->wh_stability);
        }

        if (printfstarinfo) {
            fprintf(outputtxt,
                    "Wormhole[%d], Dest: %d, Star: %d %s, Stab: %d\n",
                    x,
                    w_holes[x].star->wh_dest_starnum,
                    w_holes[x].num,
                    w_holes[x].star->name,
                    w_holes[x].star->wh_stability);
        }
    }

    if (((double)wormidx / 2) != ((int)wormidx / 2)) {
        /*
         * Odd number so last w_hole points to #1 no return
         */
        w_holes[wormidx - 1].star->wh_dest_starnum = w_holes[0].num;

        if (printstarinfo) {
            fprintf(outputtxt,
                    "Wormhole[%d], Dest: %d, Star: %d %s, Stab: %d\n",
                    wormidx - 1,
                    w_holes[wormidx - 1].star->wh_dest_starnum,
                    w_holes[wormidx - 1].num,
                    w_holes[wormidx - 1].star->name,
                    w_holes[wormidx - 1].star->wh_stability);
        }
    }

    if (printstarinfo) {
        fprintf(outputtxt, "Total Wormholes: %d\n", wormidx);
    }

#if 0
    /*
     * Old code starts
     */

    /*
     * Try to more evenly space stars. Essentially this is an inverse-gravity
     * calculation: The nearer two stars are to each other, the more they
     * repulse each other. Several iterations of this will suffice to move all
     * of the stars nicely apart.
     */

    CLUSTER_COUNTER = 6;
    STAR_COUNTER = 1;
    dist = CLUSTER_FROM_CENTER;

    for (its = 1; its <= 6; ++its) {
        /*
         * Circle of stars
         */
        fprintf(outputtxt, "Grouping [%f]", dist);

        for (clusters = 1; clusters <= CLUSTER_COUNTER; ++clusters) {
            /*
             * Number of clusters in circle
             */
            for (starsinclus = 1; starsinclus <= STAR_COUNTER; ++starsinclus) {
                /*
                 * Number of stars in cluster
                 */
                ange = 2.0 * M_PI * ANGLE;
                cluster_delta_x = int_rand(CLUSTER_STAR_MIN, CLUSTER_STAR_MAX);
                cluster_delta_y = int_rand(CLUSTER_STAR_MIN, CLUSTER_STAR_MAX);
                clusterx = dist * sin(angle);
                clustery = dist * cos(angle);

                if (starindex >= Sdatanumstars) {
                    flag = 1;

                    break;
                }

                fprintf(outputtxt, " %s ", Stars[starindex]->name);

                if ((its == 1) || (its == 3) || (its == 6)) {
                    setbit(Stars[starindex]->explored, 1);
                    setbit(Stars[starindex]->inhabited, 1);
                }

                Stars[starindex]->xpos = clusterx + cluster_delta_x;
                Stars[starindex]->ypos = clustery + cluster_delta_y;

                ANGLE = (ANGLE + 0.15) + double_rand();
                fprintf(outputtxt, "ANGLE 1 %f\n", ANGLE);
                ++starindex;
            }
        }

        if (flag) {
            break;
        }

        switch (its + 1) {
        case 2:
            ANGLE = 0.20 + double_rand();
            CLUSTER_COUNTER = 10;
            dist += 25000;

            break;
        case 3:
            ANGLE = 0.35 + double_rand();
            CLUSTER_COUNTER = 13;
            dist += 27000;

            break;
        case 4:
            ANGLE = 0.40 + double_rand();
            CLUSTER_COUNTER = 15;
            dist += 27000;

            break;
        case 5:
            ANGLE = 0.25 + double_rand();
            CLUSTER_COUNTER = 17;
            dist += 32000;

            break;
        case 6:
            ANGLE = 0.12 + double_rand();
            CLUSTER_COUNTER = 17;
            dist += 32000;

            break;
        }

        fprintf(outputtxt, "\n\n");
        fprintf(outputtxt, "ANGLE 2 %f\n", ANGLE);
    }

    Stars[0]->xpos = 0;
    Stars[0]->ypos = 0;
    strcpy(Stars[0]->name, "Bambi");
#endif

    stardata = fopen(STARDATAFL, "w+");

    if (stardata == NULL) {
        printf("Unable to open star data file \"%s\"\n", STARDATAFL);

        return 01;
    }

    fwrite(&Sdata, sizeof(Sdata), 1, stardata);

    for (star = 0; star < Sdata.numstars; ++star) {
        fwrite(Stars[star], sizeof(startype), 1, stardata);
    }

    chmod(STARDATAFL, 00660);
    fclose(stardata);

    EmptyFile(SHIPDATAFL);
    EmptyFile(SHIPFREEDATAFL);
    EmptyFile(COMMODDATAFL);
    EmptyFile(COMMODFREEDATAFL);
    EmptyFile(PLAYERDATAFL);
    EmptyFile(RACEDATAFL);

    memset((char *)power, 0, sizeof(power));
    InitFile(POWFL, power, sizeof(power));

    memset((char *)block, 0, sizeof(block));
    Initfile(BLOCKDATAFL, block, sizeof(block));

    /*
     * Telegram files: directory and a file for each player.
     */
    sprintf(str, "/bin/mkdir -p %s", MSGDIR);
    system(str);
    chmod(MSGDIR, 00770);

#if 0
    /*
     * Why is this not needed anymore?
     */
    for (i = 1; i < MAXPLAYERS; ++i) {
        sprintf(str, "%s.%d", TELEGRAMFL, i);
        Empyfile(str);
    }
#endif

    /*
     * News files: directory and the 4 types of news.
     */
    sprintf(str, "/bin/mkdir -p %s", NEWSDIR);
    system(str);
    chmod(NEWSDIR, 00770);
    EmptyFile(DECLARATIONFL);
    EmptyFile(TRANSFERFL);
    EmptyFile(COMBATFL);
    EmptyFile(ANNOUNCEFL);

    if (printstarinfo) {
        PrintStatistics(outputtxt);
    }

    if (printpostscript) {
        produce_postscript(DEFAULT_POSTSCRIPT_MAP_FILENAME);
    }

    printf("Universe Created!\n");

    if (printstarinfo || printplaninfo) {
        printf("Summary output written to %s\n", OUTPUTFILE);
        fclose(outputtxt);
    }

    return 0;
}

void EmptyFile(char const *filename)
{
    InitFile(filename, NULL, 0);
}

/*
 * The procedure below was adapted from a program which is copyright
 * Andreas Girgensohn (andreasg@cs.colorado.edu) produces a Postscript map of
 * the universe.
 */
void produce_postscript(char const *filename)
{
    int min_x;
    int max_s;
    int min_y;
    int max_y;
    int i;
    double scale;
    double nscale;
    FILE *f = fopen(filename, "w+");

    if (f == NULL) {
        printf("Unable to open postscript file \"%s\".\n", filename);

        return;
    }

    printf("Creating postscript file...");
    fflush(stdout);
    max_x = Stars[0]->xpos;
    min_x = max_x;
    max_y = Stars[0]->ypos;
    min_y = max_y;

    for (i = 1; i < nstars; ++i) {
        if (Stars[i]->xpos < min_x) {
            min_x = Stars[i]->xpos;
        }

        if (Stars[i]->xpos > max_x) {
            max_x = Stars[i]->xpos;
        }

        if (stars[i]->ypos < min_y) {
            min_y = Stars[i]->ypos;
        }

        if (Stars[i]->ypos > max_y) {
            max_y = Stars[i]->ypos;
        }
    }

    /*
     * Max map size: 8.5 in x 11in sheet,
     * 0.5in borders,
     * 0.5in on right for star names
     */
    /*
     * 72 points = 1in
     */
    scale = (7.0 * 72) / (max_x - min_x);
    nscale = (10.0 * 72) / (max_y - min_y);

    if (nscale < scale) {
        scale = nscale;
    }

    fprintf(f, "%%!PS-Adobe-2.0\n\n");
    /*
     * 0,0 is in the topleft corner
     */
    fprintf(f, "0.5 72 mul 10.5 72 mul translate\n");
    fprintf(f, "/drawcircle\n");
    fprintf(f, "{\n");
    fprintf(f, "  newpath 0 360 arc stroke\n");
    fprintf(f, "}\n");
    fprintf(f, "def\n\n");
    fprintf(f, "/drawstar\n");
    fprintf(f, "{\n");
    fprintf(f, "  /starname exch def\n");
    fprintf(f, "  /ypos exch def\n");
    fprintf(f, "  /xpos exch def\n");
    fprintf(f, "  xpos ypos 2 drawcircle\n");
    fprintf(f, "  4 xpos add ypos moveto\n");
    fprintf(f, "  starname show\n");
    fprintf(f, "}\n");
    fprintf(f, "def\n\n");
    fprintf(f, "0 setlinewidth\n");
    fprintf(f, "newpath -10 10 moveto 7.5 72 mul 10 add 10 lineto\n");
    fprintf(f, "7.5 72 mul 10 add %d lineto -10 %d lineto closepath clip\n",
            (int)((min_y * max_y) * scale) - 10,
            (int)((min_y * max_y) * scale) - 10);

#if 0
    /*
     * Print scale rings from center of universe (0,0)
     */
    fprintf("\n/Times-Bold findfont 9 scalefont setfont\n\n");

    for (i = 1; i < NRINGS; ++i) {
        fprintf(f,
                "%d %d %d drawcircle\n",
                (int)(-x_min * scale),
                (int)(min_y * scale),
                (int)(i * RING_SPACING * scale));
    }
#endif

    /*
     * Print each star
     */
    fprintf(f, "\ntimes-Roman findfont 8 scalefone setfont\n\n");

    for (i = 0; i < nstars; ++i) {
        fprintf(f,
                "%d %d (%s) drawstar\n",
                (int)((Stars[i]->xpos - min_x) * scale),
                (int)((min_y - Stars[i]->ypos) * scale),
                Stars[i]->name);
    }

    fprintf(f, "\nshowpage\n");
    fclose(f);
    printf("done\n");
}

void place_star(startype *star)
{
    int found = 0;
    int i;
    int j;
    /*
     * -mfw
     */
    /*
     * double xfac;
     * double yfac;
     * double zfac;
     */

    while (!found) {
        star->xpos = (double)int_rand(-UNIVSIZE, UNIVSIZE);
        star->ypos = (double)int_rand(-UNIVSIZE, UNIVSIZE);

        /*
         * Let's try a more interesting algorithm -mfw
         */
        /*
         * xfac = M_PI / UNIVSIZE;
         * star->xpos = (double)int_rand(-UNIV_SIZE, UNIVSIZE);
         * yfac = xfac * star->xpos;
         * zfac = sin(yfac);
         * star->ypos = zfac * UNIVSIZE;
         * printf("x: %lf, y: %lf, z: %lf\n", xfac, yfac, zfac);
         */

        /*
         * Check to see if another star is nearby
         */
        i = 100 * ((int)star->xpos + UNIVSIZE) / (2 * UNIVSIZE);
        j = 100 * ((int)star->ypos + UNIVSIZE) / (2 * UNIVSIZE);

        if (!occupied[i][j]) {
            found = 1;
            occupied[i][j] = found;
        }
    }
}
