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
 * #ident  "@(#)exship.c  1.4 3/21/93 "
 * exship.c
 *
 * Created: Fri Jan   1 19:34:23 EST 1993
 * Author:  J. Deragon (deragon@jethro.nyu.edu)
 *
 * Version: 1.4 14:23:29
 */

/*
 * Includes
 */
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "files.h"
#include "shipdata.h"
#include "ships.h"
#include "vars.h"

#define EXTERN extern
#define DATAFILE DATA(ship)

/*
 * Prototypes
 */
void check_ship(int);
void check_size(void);
void display_ship(void);
void summarize_ship(void);

#ifdef __STDC__

void readship(shiptype **, int);

#else

void main();
void readship();

#endif

/*
 * Global variables
 */

/*
 * The file descriptor of the datafile
 */
static int ship_fd;

/*
 * Used for getting the size of the file
 */
struct stat buffer;

/*
 * Number of total ships in database
 */
int num_ships;

/*
 * The number of bad ships in the database
 */
int bad_ship_count = 0;

/*
 * This ship we are currently working on
 */
shiptype *display;

/*
 * main
 *
 * Description: If invoked with zero arguments, it will open the ship datafile
 *              and go through looking for obvious errors. If invoked with a
 *              ship number it will print out a very verbose listing of the
 *              requested ship.
 *
 * Arguments: shipnumber
 *
 * Called by:
 */
int main(int argc, char *argv[])
{
    int i;

    ship_fd = open(DATA_FILE, 000, 0777);

    if (ship_fd < 0) {
	perror("main");
	printf("Unable to open %s\n", DATAFILE);

	exit(-1);
    }

    check_size();

    if (argc == 1) {
	/*
	 * Check the whole database for errors
	 */
	for (i = 1; i <= num_ships; ++i) {
	    readship(&display, i);
	    check_ship(i);
	    free(display);
	}

	printf("I found a total of %d bad ships out of %d\n",
	       bad_ship_count,
	       num_ships);
    } else if (!atoi(argv[1])) {
	/*
	 * A summary of all ships
	 */
	for (i = 1; i <= num_ships; ++i) {
	    readship(&display, i);
	    summarize_ship();
	    free(display);
	}
    } else {
	/*
	 * We want a specific ship display
	 */
	readship(&display, atoi(argv[1]));
	display_ship();
    }

    printf("All done\n");

    return 0;
}

/*
 * readship
 *
 * Description: This function reads the actual data from the file.
 *
 * Arguments: shiptype structure, shipnumber
 *
 * Called by:
 */
void readship(shiptype **s, int shipnum)
{
    int n;

    if (shipnum <= 0) {
	exit(1);
    }

    *s = (shiptype *)malloc(sizeof(shiptype));

    if (*s == 0) {
	printf("readship: malloc() error\n");

	exit(0);
    }

    if (lseek(ship_fd, (shipnum - 1) * sizeof(shiptype), L_SET) < 0) {
	perror("lseek");

	exit(1);
    }

    n = read(ship_fd, (char *)*s, sizeof(shiptype));

    if (n != sizeof(shiptype)) {
	perror("read");
    }
}

/*
 * check_size
 *
 * Description: Gets the number of ships in the current database
 *
 * Arguments: none
 *
 * Called by: main
 */
void check_size(void)
{
    fstat(ship_fd, &buffer);
    num_ships = buffer.st_size / sizeof(shiptype);
    printf("Number of ships in database is %d\n", num_ships);
}

/*
 * check_ship
 *
 * Description: Checks basic cargo to make sure it's within limits of ship's
 *              ability.
 *
 * Arguments: index
 *
 * Called by: main
 */
void check_ship(int idx)
{
    int ship_ok = 1;
    int pop = 1;
    int troops = 1;
    int res = 1;
    int des = 1;
    int fu = 1;
    int speed = 1;
    int hanger = 1;

    if ((display->owner == 0)
	&& (display->governor == 0)
	&& (display->type == 0)
	&& (display->number == 0)) {
	++bad_ship_count;
	printf("Uninitialized ship in position %d\n", idx);

	return;
    }

    if (display->type == OTYPE_FACTORY) {
	if (display->popn > Shipdata[display->type][ABIL_MAXCREW]) {
	    pop = 0;
	    ship_ok = pop;
	}

	if (dsiplay->troops > Shipdata[display->type][ABIL_MAXCREW]) {
	    troops = 0;
	    ship_ok = troops;
	}

	if ((display->popn + display->troops) > Shipdata[display-type][ABIL_MAXCREW]) {
	    troops = 0;
	    pop = troops;
	    ship_ok = pop;
	}
    } else {
	if (display->popn > display->max_crew) {
	    pop = 0;
	    ship_ok = pop;
	}

	if (display->troops > display->max_crew) {
	    troops = 0;
	    ship_ok = troops;
	}

	if ((display->popn + display->troops) > display->max_crew) {
	    troops = 0;
	    pop = troops;
	    ship_ok = pop;
	}
    }

    if ((display->resource > Max_resource(display))
	&& (display->type != STYPE_SHUTTLE)) {
	res = 0;
	ship_ok = res;
    }

    if (display->destruct > Max_destruct(display)) {
	des = 0;
	ship_ok = des;
    }

    if ((int)display->fuel > Max_fuel(display)) {
	fu = 0;
	ship_ok = fu;
    }

    if (display->speed > Max_speed(display)) {
	speed = 0;
	ship_ok = speed;
    }

    if (display->hanger > display->max_hanger) {
	hanger = 0;
	ship_ok = hanger;
    }

    if (!ship_ok) {
	++bad_ship_count;

	printf("Problem with ship number %d\n", display->number);
	printf("\t\tOwner: %d\n", display->owner);
	printf("\t\tGovernor: %d\n", display->governor);
	printf("\t\tName: %s\n", display->name);
	printf("\t\tType: %c\n", Shipltrs[display->type]);
	printf("\n");

	if (pop) {
	    printf("\t        popn: %d\t max_popn: %d\n",
		   display->popn,
		   display->max_crew);
	} else {
	    printf("\t -----> popn: %d\t max_pop: %d\n",
		   display->popn,
		   display->max_crew);
	}

	if (troops) {
	    printf("\t        troops: %d\t max_troops: %d\n",
		   display->troops,
		   display->max_crew);
	} else {
	    printf("\t -----> troops: %d\t max_troops: %d\n",
		   display->troops,
		   display->max_crew);
	}

	if (res) {
	    printf("\t        resources: %d\t max_resources: %d\n",
		   display->resource,
		   display->max_resources);
	} else {
	    printf("\t -----> resources: %d\t max_resources: %d\n",
		   display->resource,
		   display->max_resources);
	}

	if (des) {
	    printf("\t        destruct: %d\t max_destruct: %d\n",
		   display->destruct,
		   display->max_destruct);
	} else {
	    printf("\t -----> destruct: %d\t max_destruct: %d\n",
		   display->destruct,
		   display->max_destruct);
	}

	if (fu) {
	    printf("\t        fuel: %d\t max_fuel: %d\n",
		   (int)display->fuel,
		   (int)display->max_fuel);
	} else {
	    printf("\t -----> fuel: %d\t max_fuel: %d\n",
		   (int)display->fuel,
		   (int)display->max_fuel);
	}

	if (speed) {
	    printf("\t        speed: %d\t max_speed: %d\n",
		   display->speed,
		   display->max_speed);
	} else {
	    printf("\t -----> speed: %d\t max_speed: %d\n",
		   display->speed,
		   display->max_speed);
	}

	if (hanger) {
	    printf("\t        hanger: %d\t max_hanger: %d\n",
		   display->hanger,
		   display->max_hanger);
	} else {
	    printf("\t -----> hanger: %d\t max_hanger: %d\n",
		   display->hanger,
		   display->max_hanger);
	}
    }
}

/*
 * display_ship
 *
 * Description: Prints a _long_ description of a specific ship.
 *
 * Arguments: none
 *
 * Called by: main
 */
void display_ship(void)
{
    printf("Ship Number: %d", display->number);
    printf("\tShip Type: %c", Shipltrs[display->type]);
    printf("\tShip Owner: %d", display->owner);
    printf("\tship Governor: %d", display->governor);
    printf("\n");
    printf("Ship Name: %s", display->name);
    printf("\tAge: %d", display->age);
    printf("\n");
    printf("\n");
    printf("Crew: %-9d", display->popn);
    printf("\t  Troops: %-6d", display->troops);
    printf("\tArmor: %d", display->armor);
    printf("\n");
    printf("Size: %-9d", display->size);
    printf("\t  Base Mass: %-4.1f", display->base_mass);
    printf("\tBase Tech: %4.1f", display->tech);
    printf("\n");
    printf("Destruct: %-6d", display->destruct);
    printf("  Resources: %-6d", display->resource);
    printf("\tCrystals: %d", display->crystals);
    printf("\n");
    printf("Fuel: %4.1f", display->fuel);
    printf("\t  Max Fuel: %-6d", display->max_fuel);
    printf("\tOn: %d", display->on);
    printf("\n");
    printf("\n");
    printf("Whatorbits: %d", display->whatorbits);
    printf("  Pos X: %lf", display->xpos);
    printf("  Pos Y: %lf", display->ypos);
    printf("  Sorbit: %d", display->storbits);
    printf("  Porbit: %d", display->pnumorbits);
    printf("\n");
    printf("whatdest: %d", display->whatdest);
    printf("  deststart: %d", display->deststar);
    printf("  destpnum: %d", display->destpnum);
    printf("\n");
    printf("Loc X: %d", display->land_x);
    printf("  Loc Y: %d", display->land_y);
    printf("  Docked: %d", display->docked);
    printf("\n");
    printf("Guns:\tPrimary: %-3d", display->primary);

    if (display->primtype == LIGHT) {
	printf("L");
    } else if (display->primtype == MEDIUM) {
	printf("M");
    } else if (display->primtype == HEAVY) {
	printf("H");
    } else {
	printf("N");
    }

    printf("\n");
    printf("     \tSecondary:  %-3d", display->secondary);

    if (display->sectype == LIGHT) {
	printf("L");
    } else if (display->sectype == MEDIUM) {
	printf("M");
    } else if (display->sectype == HEAVY) {
	printf("H");
    } else {
	printf("N");
    }

    printf("\n");

    /*
     * Fleet stuff -mfw
     */
    printf("\n");
    printf("Fleet: %d (%c)  Next in Fleet: %d\n",
	   display->fleetmember,
	   display->fleetmember + ('A' - 1),
	   display->nextinfleet);

#ifdef USE_VN
    /*
     * VN mind output -mfw
     */
    if((display->type == OTYPE_VN) || (display->type == OTYPE_BERS)) {
	printf("\nVon Neumann Mind:\n");
	printf("\tProgenitor: %-3u  Generation: %-3u\n",
	       display->special.mind.progenitor,
	       display->special.mind.generation);
	printf("\tBusy: %-3u        Target: %-3u\n",
	       display->special.mind.busy,
	       display->special.mind.target);
	printf("\tWho Killed: %-3u  Tampered: %-3u\n",
	       display->special.mind.who_killed,
	       display->special.mind.tampered);
    }
#endif

    printf("\nShips: %-6d\n", display->ships);

    if (display->alive) {
	printf("Nextship: %-6d\t (ALIVE) Reuse=%d\n",
	       display->nextship,
	       display->reuse);
    } else {
	printf("Nextship: %-6d\t (DEAD) Reuse=%d\n",
	       display->nextship,
	       display->reuse);
    }
}

/*
 * summarize_ship
 *
 * Description: Prints a _short_ description of a specific ship.
 *
 * Arguments: none
 *
 * Called by: main
 */
void summarize_ship(void)
{
    printf("#%d %c [%d,%d] ",
	   display->number,
	   Shipltrs[display->type],
	   display->owner,
	   display->governor);

    if (display->alive) {
	printf("alive ");
    } else {
	printf("dead ");
    }

    if (display->reuse) {
	printf("reuse %d", display->age);
    } else {
	printf("%d", display->age);
    }

    printf("\n");
}
