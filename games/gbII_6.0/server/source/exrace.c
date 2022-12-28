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
 * exrace.c - Based on John Deregon's exship.c. By Michael Wilkinson (3/1/05).
 */

/*
 * Includes
 */
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "files.h"
#include "races.h"
#include "vars.h"

#define DATAFILE DATA(race)

/*
 * Prototypes
 */
void check_size(void);
void check_race(void);
void display_race(void);
void readrace(racetype **, int);

/*
 * Global variables
 */

/*
 * The file descriptor of the datafile
 */
static int race_fd;

/*
 * Used for getting the size of the file
 */
struct stat buffer;

/*
 * Number of total ships in the database
 */
int num_races;

/*
 * This ship we are currently working on
 */
racetype *display;

/*
 * The number of bad races in the database
 */
int bad_race_count = 0;

/*
 * main
 *
 * Description: If invoked with zero arguments, it will open the race datafile
 *              and count the number of races stored in the db. If invoked with
 *              a ship number it will print out a very verbose listing of the
 *              requested ship.
 *
 * Arguments: racenumber
 */
int main(int argc, char *argv[])
{
    int i;

    race_fd = open(DATAFILE, 000, 0777);

    if (race_fd < 0) {
        perror("main");
        printf("Unable to open %s\n", DATAFILE);

        exit(-1);
    }

    check_size();

    if (argc == 1) {
        /*
         * Check the whole database for errors
         */
        for (i = 1; i < num_races; ++i) {
            readrace(&display, i);
            check_race();
            free(display);
        }

        printf("I found a total of %d corrupt races out of %d\n",
               bad_race_count,
               num_races);
    } else {
        /*
         * We want a specific ship display
         */
        readrace(&display, atoi(argv[1]));
        display_race();
    }

    printf("All done\n");

    return 0;
}

/*
 * readrace
 *
 * Description: This function reads the actual data from the file.
 *
 * Arguments: racetype structure, racenumber
 *
 * Called by:
 */
void readrace(racetype **s, int racenum)
{
    int n;

    if (racenum <= 0) {
        exit(1);
    }

    *s = (racetype *)malloc(sizeof(racetype));

    if (*s == 0) {
        printf("readrace: malloc() error\n");

        exit(0);
    }

    if (lseek(race_fd, (racenum - 1) * sizeof(racetype), L_SET) < 0) {
        perror("lseek");

        exit(1);
    }

    n = read(race_fd, (char *)*s, sizeof(racetype));

    if (n != sizeof(racetype)) {
        perror("read");
    }
}

/*
 * check_size
 *
 * Description: Gets the number of races in the current database
 *
 * Arguments: none
 *
 * Called by: main
 */
void check_size(void)
{
    fstat(race_fd, &buffer);
    num_races = buffer.st_size / sizeof(racetype);
    printf("Number of races in database is %d\n", num_races);
}

/*
 * check_race
 *
 * Description: Does nothing right now
 *
 * Arguments: none
 *
 * Called by: main
 */
void check_race(void)
{
}

/*
 * display_race
 *
 * Description: Prints info on a specific race.
 *
 * Arguments: none
 *
 * Called by: main
 */
void display_race(void)
{
    int i;

    printf("Race Number: %d", display->Playernum);
    printf("\tRace Name: %s", display->name);
    printf("\tRace Password: %s\n", display->password);

    if (display->God) {
        printf("Race had GOD permissions.\n");
    }

    if (display->Guest) {
        printf("Race has GUEST permissions.\n");
    }

    if (display->Metamorph) {
        printf("Race is a Metamorph.\n");
    }

    if (display->dissolved) {
        printf("Race is DISSOLVED.\n");
    }

    printf("Gov ship: %d\n\n", display->Gov_ship);

    for (i = 0; i < 6; ++i) {
        if (display->governor[i].active) {
            printf("Gov Number: %d", i);
            printf("\tGov Name: %s", display->governor[i].name);
            printf("\tGov Password: %s", display->governor[i].password);
            printf("\tRank: %d\n", display->governor[i].rank);
        }
    }
}
