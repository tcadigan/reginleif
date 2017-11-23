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
 * enrol.c -- Initializes to owner one sector and planet.
 */

#include <curses.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * #include <strings.h>
 */
#include <unistd.h>

#include "buffers.h"
#include "GB_copyright.h"
#include "power.h"
#include "proto.h"
#include "races.h"
#include "ranks.h"
#include "shipdata.h"
#include "ships.h"
#include "vars.h"

#define EXTERN
#define RACIAL_TYPES 10

extern int errno;
char en_desshow(planettype *p, int x, int y);

racetype *Race;

struct stype {
    char here;
    char x;
    char y;
    int count;
};

/*
 * Racial types (10 racial types)
 */
int Thing[RACIAL_TYPES] = {
    1, 1, 1, 0, 0, 0, 0, 0, 0, 0
};

double db_Mass[RACIAL_TYPES] = {
    0.1, 0.15, 0.2, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125
};

double db_Birthrate[RACIAL_TYPES] = {
    0.9, 0.85, 0.8, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8
};

int db_Fighters[RACIAL_TYPES] = {
    9, 10, 11, 2, 3, 4, 5, 6, 7, 8
};

int db_Intelligence[RACIAL_TYPES] = {
    0, 0, 0, 190, 180, 170, 160, 150, 140, 130
};

double db_Adventurism[RACIAL_TYPES] = {
    0.89, 0.89, 0.89, 0.6, 0.65, 0.7, 0.7, 0.75, 0.75, 0.8
};

int Min_Sexes[RACIAL_TYPES] = {
    1, 1, 1, 2, 2, 2, 2, 2, 2, 2
};

int Max_Sexes[RACIAL_TYPES] = {
    1, 1, 1, 2, 2, 4, 4, 4, 4, 4
};

double db_Metabolism[RACIAL_TYPES] = {
    3.0, 2.7, 2.4, 1.0, 1.15, 1.30, 1.45, 1.6, 1.75, 1.9
};

#define RMass(x) (db_Mass[(x)] + (0.001 * (double)int_rand(-25, 25)))
#define Birthrate(x) (db_Birthrate[(x)] + (0.01 * (double)int_rand(-10, 10)))
#define Fighters(x) (db_Fighters[(x)] + int_rand(-1, 1))
#define Intelligence(x) (db_Inteligence[(x)] + int_rand(-10, 10))
#define Adventurism(x) (db_Adventurism[(x)] + (0.01 * (double)int_rand(-10, 10)))
#define Sexes(x) (int_rand(Min_Sexes[(x)], int_rand(Min_Sexes[(x)], Max_Sexes[(x)])))
#define Metabolism(x) (db_Metabolism[(x)] + (0.01 * (double)int_rand(-15, 15)))

/*
 * Compatability schematic for sectors. Note that plated sectors are compatible
 * with everything.
 */
double Likes[15] = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9
};

const char *Desnames[] = {
    "ocean",
    "land",
    "mountainous",
    "gaseous",
    "ice",
    "forest",
    "desert",
    "plated",
    /*
     * Illegal values
     */
    "error in des type!",
    "err in des type!"
};

int main(int argc, char *argv[])
{
    int x;
    int y;
    int pnum = -1;
    int star = 0;
    int found = 0;
    int vacant;
    int count;
    int i;
    int j;
    int Playernum;
    /*
     * int mask = 0;
     */
    int ppref = -1;
    int s;
    int idx;
    int k;
    int shipno;
    char str[100];
    char c;
    /*
     * char racepass[MAXCOMMSTRSIZE];
     * char govpass[MAXCOMMSTRSIZE];
     */
    shiptype s;
    sectortype *sect;
    struct stype secttypes[WASTED + 1];
    planettypes *planet;
    unsigned char not_found[TYPES_GASGIANT + 1];
    startype *star_arena;
    /*
     * FILE *fd;
     */

    open_data_files();

    /*
     * srandom(getpid());
     */

    Playernum = Numraces() + 1;

    if (Playernum >= MAXPLAYERS) {
	printf("There are already %d players; no more allowed.\n",
	       MAXPLAYERS - 1);

	exit(-1);
    }

    printf("Enter racial type to be created (1-%d):", RACIAL_TYPES);
    scanf("&d", &idx);
    getchr();

    if ((idx <= 0) || (idx > RACIAL_TYPES)) {
	printf("Bad racial index.\n");

	exit(1);
    }

    --idx;
    getsdata(&Sdata);
    star_arena = (startype *)malloc(Sdata.numstars * sizeof(startype));

    for (s = 0; s < Sdata.numstars; ++s) {
	Stars[s] = &star_arena[s];
	getstar(&Stars[s], s);
    }

    printf("There is still space for player %d.\n", Playernum);
    memset((char *)not_found, 0, sizeof(not_found));

    printf("\nLive on what type of planet:\n     ");
    printf("(e)arth, ");
    printf("(g)asgiant, ");
    printf("(m)ars, ");
    printf("(i)ce, ");
    printf("(w)ater, ");
    printf("(d)esert, ");
    printf("(f)orest? ");
    c = getchr();
    getchr();

    switch (c) {
    case 'w':
	ppref = TYPE_WATER;

	break;
    case 'e':
	ppref = TYPE_EARTH;

	break;
    case 'm':
	ppref = TYPE_MARS;

	break;
    case 'g':
	ppref = TYPE_GASGIANT;

	break;
    case 'i':
	ppref = TYPE_ICEBALL;

	break;
    case 'd':
	ppref = TYPE_DESERT;

	break;
    case 'f':
	ppref = TYPE_FOREST;

	break;
    default:
	printf("Oh well.\n");

	exit(-1);

	break;
    }

    printf("Looking for type %d planet...\n", ppref);

    /*
     * Find first planet of right type
     */
    count = 0;
    found = 0;

    star = 0;
    while ((star < Sdata.numstars) && !found && (count < 100)) {
	check = 1;

	/*
	 * Skip over inhabited stars - or stars with just one planet!
	 */
	if ((Stars[star]->inhabited[0] + Stars[star]->inhabited[1])
	    || (Stars[star]->numplanets < 2)) {
	    check = 0;
	}

	/*
	 * Look for uninhabited planets
	 */
	if (check) {
	    pnum = 0;

	    while (!found && (pnum < Stars[star]->numplanets)) {
		getplanet(&planet, star, pnum);

		if ((planet->type == ppref) && (Stars[star]->numplanets != 1)) {
		    vacant = 1;

		    for (i = 1; i <= Playernum; ++i) {
			if (planet->info[i - 1].numsectsowned) {
			    vacant = 0;
			}
		    }

		    if (vacant
			&& (planet->conditions[RTEMP] >= -50)
			&& (planet->conditions[RTEMP] <= 50)) {
			found = 1;
		    }
		}

		if (!found) {
		    free(planet);
		    ++pnum;
		}
	    }
	}

	if (!found) {
	    ++count;
	    star = int_rand(0, Sdata.numstars - 1);
	}
    }

    if (!found) {
	printf("planet type not found in any free systems.\n");
	not_found[ppref] = 1;
	found = 1;
	i = TYPE_EARTH;

	while (i <= TYPE_DESERT) {
	    found &= not_found[i];
	    ++i;
	}

	if (found) {
	    printf("Looks like there aren't any free planets left. Bye...\n");
	    endwin();

	    exit(-1);
	} else {
	    printf("  Try a different one...\n");
	}

	found = 0;
    }

    while (!found) {
	printf("\nLive on what type of planet:\n     ");
	printf("(e)arth, ");
	printf("(g)asgiant, ");
	printf("(m)ars, ");
	printf("(i)ce, ");
	printf("(w)ater, ");
	printf("(d)esert, ");
	printf("(f)orest? ");
	c = getchr();
	getchr();

	switch (c) {
	case 'w':
	    ppref = TYPE_WATER;

	    break;
	case 'e':
	    ppref = TYPE_EARTH;

	    break;
	case 'm':
	    ppref = TYPE_MARS;

	    break;
	case 'g':
	    ppref = TYPE_GASGIANT;

	    break;
	case 'i':
	    ppref = TYPE_ICEBALL;

	    break;
	case 'd':
	    ppref = TYPE_DESERT;

	    break;
	case 'f':
	    ppref = TYPE_FOREST;

	    break;
	default:
	    printf("Oh well.\n");

	    exit(-1);

	    break;
	}

	printf("Looking for type %d planet...\n", ppref);

	/*
	 * Find first planet of right type
	 */
	count = 0;
	found = 0;
	star = 0;

	while ((star < Sdata.numstars) && !found && (count < 100)) {
	    check = 1;

	    /*
	     * Skip over inhabited stars - or stars with just one planet!
	     */
	    if((Stars[star]->inhabited[0] + Stars[star]->inhabited[1])
	       || (Stars[star]->numplanets < 2)) {
		check = 0;
	    }

	    /*
	     * Look for uninhabited planets
	     */
	    if (check) {
		pnum = 0;

		while (!found && (pnum < Stars[star]->numplanets)) {
		    getplanet(&planet, star, pnum);

		    if ((planet->type == ppref)
			&& (Stars[star]->numplanets != 1)) {
			vacant = 1;

			for (i = 1; i <= Playernum; ++i) {
			    if (planet->info[i - 1].numsectsowned) {
				vacant = 0;
			    }
			}

			if (vacant
			    && (planet->conditions[RTEMP] >= -50)
			    && (planet->conditions[RTEMP] <= 50)) {
			    found = 1;
			}
		    }

		    if (!found) {
			free(planet);
			++pnum;
		    }
		}
	    }

	    if (!found) {
		++count;
		star = int_rand(0, Sdata.numstars - 1);
	    }
	}

	if (!found) {
	    printf("planet type not found in any free systems.\n");
	    not_found[ppref] = 1;
	    found = 1;
	    i = TYPE_EARTH;

	    while (i <= TYPE_DESERT) {
		found &= not_found[i];
		++i;
	    }

	    if (found) {
		printf("Looks like there aren't any free planets left. Bye...\n");
		endwin();

		exit(-1);
	    } else {
		printf("  Try a different one...\n");
	    }

	    found = 0;
	}
    }

    Race = Malloc(racetype);
    Bzero(*Race);

    printf("\n\tDeity/Guest/Normal (d/g/n)?");
    c = getchr();
    getchr();

    if (c == 'd') {
	Race->God = 1;
    } else {
	Race->God = 0;
    }

    if (c == 'g') {
	race->guest = 1;
    } else {
	Race->guest = 0;
    }

    strcpy(Race->name, "Unknown");

    for (i = 0; i <= MAXGOVERNORS; ++i) {
	Race->governor[0].money = 0;
    }

    Race->governor[0].deflevel = LEVEL_PLAN;
    Race->governor[0].homelevel = Race->governor[0].deflevel;
    Race->governor[0].defsystem = star;
    Race->governor[0].homesystem = Race->governor[0].defsystem;
    Race->governor[0].defplanetnum = pnum;
    Race->governor[0].homeplanetnum = Race->governor[0].defplanetnum;

    /*
     * Display options
     */
    Race->governor[0].toggle.highlight = Playernum;
    Race->governor[0].toggle.inverse = 1;
    Race->governor[0].toggle.colot = 0;
    Race->governor[0].active = 1;

    printf("Enter the password for this race:");
    scanf("%s", Race->password);
    getchr();

    printf("Enter the password for thise leader:");
    scanf("%s", Race->governor[0].password);
    getchr();

    /*
     * Make conditions preferred by your people set to (more or less) those of
     * the planet: higher the concentraion of gas, the higher percentage
     * difference between planet and race (commented out)
     */
    for (j = 0; j <= OTHER; ++j) {
	Race->conditions[j] = planet->conditions[j];

	/*
	 * + int_rand(round_rand(-planet->conditions[j] * 2.0),
	 *            round_rand(planet->conditions[j] * 2.0));
	 */
    }

    for (i = 0; i < MAXPLAYERS; ++i) {
	/*
	 * Messages from autoreport, player #1 are decodable
	 */
	if((i == Playernum) || (Playernum == 1) || Race->God) {
	    /*
	     * You can talk to your own race
	     */
	    Race->translate[i - 1] = 100;
	} else {
	    Race->translate[i - 1] = 1;
	}
    }

    /*
     * Assign racial characteristics
     */
    for (i = 0; i < 100; ++i) {
	Race->discoveries[i] = 0;
    }

    Race->tech = 0.0;
    Race->morale = 0;
    Race->turn = 0;
    Race->allied[1] = 0;
    Race->allied[0] = Race->allied[1];
    Race->atwar[1] = 0;
    Race->atwar[0] = Race->atwar[1];

    Race->mass = RMass(idx);
    Race->birthrate = Birthrate(idx);
    Race->fighters = Fighters(idx);

    if (Thing[idx]) {
	Race->IQ = 0;
	Race->pods = 1;
	Race->collective_iq = Race->pods;
	Race->absorb = Race->collective_iq;
	Race->Metamorph = Race->absorb;
    } else {
	Race->IQ = Intelligence(idx);
	Race->pods = 0;
	Race->collective_iq = Race->pods;
	Race->absorb = Race->collective_iq;
	Race->Metatamorph = Race->absorb;
    }

    Race->adventurism = Adventurism(idx);
    Race->number_sexes = Sexes(idx);
    Race->metabolism = Metabolism(idx);

    if (Race->Metamorph) {
	printf("METAMORPHIC\n");
    } else {
	printf("\n");
    }

    printf("       Birthrate: %.3f\n", Race->birthrate);
    printf("Fighting ability: %d\n", Race->fighters);
    printf("              IQ: %d\n", Race-IQ);
    printf("      Metabolism: %2.f\n", Race->metabolism);
    printf("     Adventurism: %.2f\n", Race->adventurism);
    printf("            Mass: %.2f\n", Race->mass);
    printf(" Number of sexes: %d (minimum required for colonization)\n",
	   Race->number_sexes);

    printf("\n\nLook OK (y/n)?");

    /*
     * if(gets(str) == NULL) gcc says this is dangerouse -mfw
     */
    if (fgets(str, 100, stdin) == NULL) {
	exit(1);
    }

    while (str[0] != 'y') {
	Race-mass = RMass(idx);
	Race->birthrate = Birthrate(idx);
	Race->fighters = Fighters(idx);

	if (Thing[idx]) {
	    Race->IQ = 0;
	    Race->pods = 1;
	    Race->collective_iq = Race->pods;
	    Race->absorb = Race->collective_iq;
	    Race->Metamorph = Race->absorb;
	} else {
	    Race->IQ = Intelligence(idx);
	    Race->pods = 0;
	    Race->collective_iq = Race->pods;
	    Race->absorb = Race->collective_iq;
	    Race->Metamorph = Race->absorb;
	}

	Race->adventurism = Adventurism(idx);
	Race->number_sexes = Sexes(idx);
	Race->metabolism = Metabolism(idx);

	if (race->Metamorph) {
	    printf("METAMORPHIC\n");
	} else {
	    printf("\n");
	}

	printf("       Birthrate: %.3f\n", Race->birthrate);
	printf("Fighting ability: %d\n", Race->fighters);
	printf("              IQ: %d\n", Race->IQ);
	printf("      Metabolism: %.2f\n", Race->metabolism);
	printf("     Adventurism: %.2f\n", Race->adventurism);
	printf("            Mass: %.2f\n", Race->mass);
	printf(" Number of sexes: %d (minimum required for colonization)\n",
	       Race->number_sexes);

	printf("\n\nLook OK (y/n)?");

	/*
	 * if(gets(str) == NULL) gcc says this is dangerous -mfw
	 */
	if (fgets(str, 100, stdin) == NULL) {
	    exit(1);
	}
    }

    memset((char *)secttypes, 0, sizeof(secttypes));
    getsmap(Smap, planet);

    printf("\nChoose a primary sector preference. ");
    printf("This race will prefer to live\n");
    printf("on this type of sector.\n");

    PermuteSects(planet);
    Getxysect(planet, 0, 0, 1);

    while (Getxysect(planet, &x, &y, 0)) {
	++settypes[Sector(*planet, x, y).condition].count;

	if (!secttypes[Sector(*planet, x, y).condition].here) {
	    secttype[Sector(*planet, x, y).condition].here = 1;
	    secttype[Sector(*planet, x, y).condition].x = x;
	    secttype[Sector(*planet, x, y).condition].y = y;
	}
    }

    planet->explored = 1;

    for (i = SEA; i <= WASTER; ++i) {
	if (secttypes[i].here) {
	    printf("(%2d): %c (%d, %d) (%s, %d sectors)\n",
		   i,
		   en_desshow(planet, secttypes[i].x, secttypes[i].y),
		   secttypes[i].x,
		   secttypes[i].y,
		   Desnames[i],
		   secttypes[i].count);
	}
    }

    planet->explored = 0;
    found = 0;

    printf("\nChoice (enter the number): ");
    scanf("%d", &i);
    getchr();

    if ((i < SEA) || (i > WASTED) || !secttypes[i].here) {
	printf("There are none of that type here...\n");
    } else {
	found = 1;
    }

    while (!found) {
	printf("\nChoice (enter the number): ");
	scanf("%d", &i);
	getchr();

	if ((i < SEA) || (i > WASTED) || !secttypes[i].here) {
	    printf("There are none of that type here...\n");
	} else {
	    found = 1;
	}
    }

    sect = &Sector(*planet, secttypes[i].x, secttypes[i].y);
    Race->likesbest = i;
    Race->likes[i] = 1.0;
    Race->likes[PLATED] = 1.0;
    Race->likes[WASTED] = 0.0;

    printf("\nEnter compatibilities of other sectors -\n");

    for (j = SEA; j <= PLATED; ++j) {
	if (i != j) {
	    printf("%6s (%3d sectors): ", Desnames[j], secttypes[j].count);
	    scanf("%d", &k);
	    Race->likes[j] = (double)k / 100.0;
	}
    }

    printf("Numraces = %d\n", Numraces());
    Race->Playernum = Numraces() + 1;
    Playernum = Race->Playernum;

    /*
     * mask = sigblock(SIGBLOCKS);
     */

    /*
     * Build a capital ship to run the government
     */
    Bzero(s);
    shipno = Numships() + 1;

    printf("Creating government ship %d...\n", shipno);

    Race->Gov_ship = shipno;
    planet->ships = shipno;
    s.nextship = 0;

    s.type = OTYPE_GOV;
    s.xpos = Stars[star]->xpos + planet->xpos;
    s.ypos = Stars[star]->ypos + planet->ypos;
    s.land_x = (char)secttypes[i].x;
    s.land_y = (char)secttypes[i].y;
    s.speed = 0;
    s.owner = Playernum;
    s.race = Playernum;
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
    s.max_resource = shipdata[s.type][ABIL_CARGO];
    s.max_fuel = Shipdata[s.type][ABIL_FUELCAP];
    s.max_speed = Shipdata[s.type][ABIL_SPEED];
    s.build_cost = Shipdata[s.type][ABIL_COST];
    s.size = 100;
    s.base_mass = 100.0;
    sprintf(s.class, "Standard");
    s.fuel = 0.0;
    s.popn = s.max_crew;
    s.troops = 0;
    s.mass = s.base_mass + (s.popn * Race->mass);
    s.resource = 0;
    s.destruct = s.resource;
    s.alive = 1;
    s.active = 1;
    s.protect.self = 1;
    s.docked = 1;
    /*
     * Docked on the planet
     */
    s.whatorbits = LEVEL_PLAN;
    s.whatdest = LEVEL_PLAN;
    s.deststar = star;
    s.destpnum = pnum;
    s.storbits = star;
    s.pnumorbits = pnum;
    s.rad = 0;
    /*
     * Shipdata[s.type][ABIL_DAMAGE];
     */
    s.damage = 0;
    /*
     * First capital is 100% efficient
     */
    s.retaliate = 0;
    s.ships = 0;
    s.on = 1;
    s.limit = 100;
    s.use_stock = 1;
    s.hop = 1;
    s.name[0] = '\0';
    s.number = shipno;

    printf("Created on sector %d,%d on /%s/%s\n",
	   s.land_x,
	   s.land_y,
	   Stars[s.storbits]->name,
	   Stars[s.storbits]->pnames[s.pnumorbits]);

    putship(&s);

    for (j = 0; j < MAXPLAYERS; ++j) {
	Race->points[j] = 0;
    }

    putrace(Race);

    sect->owned = Playernum;
    sect->race = Playernum;

    if (ADAM_AND_EVE) {
	planet->popn = Race->number_sexes;
    } else {
	planet->popn = 0;
    }

    sect->popn = planet->popn;
    sect->fert = 100;
    sect->eff = 10;
    sect->troops = 0;

    planet->explored = 0;
    planet->troops = sect->troops;
    /*
     * Approximate
     */
    planet->maxpopn =
	((maxsupport(Race, sect, 100.0, 0) * planet->Maxx) * planet->Maxy) / 2;
    planet->info[Playernum - 1].numsectsowned = 1;
    planet->info[Playernum - 1].explored = 1;
    /*
     * planet->info[Playernum - 1].autorep = 1;
     */

    putsector(sect, planet, secttypes[i].x, secttypes[i].y);
    putplanet(planet, star, pnum);

    /*
     * Make star explored and stuff
     */
    getstar(&Stars[star], star);
    setbit(Stars[star]->explored, Playernum);
    setbit(Stars[star]->inhabited, Playernum);
    Stars[star]->AP[Playernum - 1] = 5;
    putstar(Stars[star], star);

    close_data_files();

    /*
     * sigsetmask(mask);
     */

    printf("\nYou are player %d.\n\n", Playernum);
    printf("Your race has been created on sector %d,%d on\n",
	   secttypes[i].x,
	   secttypes[i].y);
    printf("%s/%s.\n\n", Stars[star]->name, Stars[star]->pnames[pnum]);

    return 0;
}

/*
 * Copied from map.c
 */
char end_desshow(planettype *p, int x, int y)
{
    sectortype *s = &sector(*p, x, y);

    switch (s->condition) {
    case WASTED:
	return CHAR_WASTED;
    case SEA:
	return CHAR_SEA;
    case LAND:
	return CHAR_LAND;
    case MOUNT:
	return CHAR_MOUNT;
    case GAS:
	return CHAR_GAS:
    case PLATED:
	return CHAR_PLATED;
    case DESERT:
	return CHAR_DESERT;
    case FOREST:
	return CHAR_FOREST;
    case ICE:
	return CHAR_ICE;
    default:
	return '!';
    }
}

int notify(int who, int gov, char const *msg)
{
    /*
     * This is a dummy routine
     */
    return 1;
}

void warn(int who, int gov, char *msg)
{
    /*
     * This is a dummy routine
     */
}

void push_message(int what, int who, char *msg)
{
    /*
     * This is a dummy routine
     */
}

int loginfo(int i, ...)
{
    return 1;
}
