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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "buffers.h"
#include "files_shl.h"
#include "max.h"
#include "perm.h"
#include "power.h"
#include "races.h"
#include "rand.h"
#include "ranks.h"
#include "shipdata.h"
#include "ships.h"
#include "vars.h"

#define RACIAL_TYPES 10

char en_desshow(planettype *p, int x, int y);

struct stardata Sdata;
startype *Stars[NUMSTARS];
sectortype Smap[((MAX_X + 1) * (MAX_Y + 1)) + 1];
planettype *planets[NUMSTARS][MAXPLANETS];
unsigned long Num_ships;

char buf[4096];
unsigned short free_ship_list;

struct stype {
  char here;
  char x;
  char y;
  int count;
};

/*
 * Racial types (10 racial types)
 */
int Thing[RACIAL_TYPES] = {1, 1, 1, 0, 0, 0, 0, 0, 0, 0};

double db_Mass[RACIAL_TYPES] = {0.1,   0.15,  0.2,   0.125, 0.125,
                                0.125, 0.125, 0.125, 0.125, 0.125};

double db_Birthrate[RACIAL_TYPES] = {0.9, 0.85, 0.8, 0.5,  0.55,
                                     0.6, 0.65, 0.7, 0.75, 0.8};

int db_Fighters[RACIAL_TYPES] = {9, 10, 11, 2, 3, 4, 5, 6, 7, 8};

int db_Intelligence[RACIAL_TYPES] = {0,   0,   0,   190, 180,
                                     170, 160, 150, 140, 130};

double db_Adventurism[RACIAL_TYPES] = {0.89, 0.89, 0.89, 0.6,  0.65,
                                       0.7,  0.7,  0.75, 0.75, 0.8};

int Min_Sexes[RACIAL_TYPES] = {1, 1, 1, 2, 2, 2, 2, 2, 2, 2};

int Max_Sexes[RACIAL_TYPES] = {1, 1, 1, 2, 2, 4, 4, 4, 4, 4};

double db_Metabolism[RACIAL_TYPES] = {3.0,  2.7,  2.4, 1.0,  1.15,
                                      1.30, 1.45, 1.6, 1.75, 1.9};

/*
 * Compatibility schematic for sectors. Note that plated sectors are compatible
 * with everything.
 */
double Likes[15] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.2,
                    0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};

const char *Desnames[] = {"ocean", "land", "mountainous", "gaseous", "ice",
                          "forest", "desert", "plated",
                          /*
                           * Illegal values
                           */
                          "error in des type!", "err in des type!"};

int main(int argc, char *argv[]) {
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
  char str[100];
  char c;
  /*
   * char racepass[MAXCOMMSTRSIZE];
   * char govpass[MAXCOMMSTRSIZE];
   */
  sectortype *sect;
  struct stype secttypes[WASTED + 1];
  planettype *planet;
  unsigned char not_found[TYPE_GASGIANT + 1];
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
    printf("There are already %d players; no more allowed.\n", MAXPLAYERS - 1);

    exit(-1);
  }

  printf("Enter racial type to be created (1-%d):", RACIAL_TYPES);
  scanf("%d", &idx);
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
    bool check = true;

    /*
     * Skip over inhabited stars - or stars with just one planet!
     */
    if ((Stars[star]->inhabited[0] + Stars[star]->inhabited[1]) ||
        (Stars[star]->numplanets < 2)) {
      check = false;
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

          if (vacant && (planet->conditions[RTEMP] >= -50) &&
              (planet->conditions[RTEMP] <= 50)) {
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
      bool check = true;

      /*
       * Skip over inhabited stars - or stars with just one planet!
       */
      if ((Stars[star]->inhabited[0] + Stars[star]->inhabited[1]) ||
          (Stars[star]->numplanets < 2)) {
        check = false;
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

            if (vacant && (planet->conditions[RTEMP] >= -50) &&
                (planet->conditions[RTEMP] <= 50)) {
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

  racetype *race = Malloc(racetype);
  Bzero(*race);

  printf("\n\tDeity/Guest/Normal (d/g/n)?");
  c = getchr();
  getchr();

  if (c == 'd') {
    race->God = 1;
  } else {
    race->God = 0;
  }

  if (c == 'g') {
    race->Guest = 1;
  } else {
    race->Guest = 0;
  }

  strcpy(race->name, "Unknown");

  for (i = 0; i <= MAXGOVERNORS; ++i) {
    race->governor[0].money = 0;
  }

  race->governor[0].deflevel = LEVEL_PLAN;
  race->governor[0].homelevel = race->governor[0].deflevel;
  race->governor[0].defsystem = star;
  race->governor[0].homesystem = race->governor[0].defsystem;
  race->governor[0].defplanetnum = pnum;
  race->governor[0].homeplanetnum = race->governor[0].defplanetnum;

  /*
   * Display options
   */
  race->governor[0].toggle.highlight = Playernum;
  race->governor[0].toggle.inverse = 1;
  race->governor[0].toggle.color = 0;
  race->governor[0].active = 1;

  printf("Enter the password for this race:");
  scanf("%s", race->password);
  getchr();

  printf("Enter the password for thise leader:");
  scanf("%s", race->governor[0].password);
  getchr();

  /*
   * Make conditions preferred by your people set to (more or less) those of
   * the planet: higher the concentraion of gas, the higher percentage
   * difference between planet and race (commented out)
   */
  for (j = 0; j <= OTHER; ++j) {
    race->conditions[j] = planet->conditions[j];

    /*
     * + int_rand(round_rand(-planet->conditions[j] * 2.0),
     *            round_rand(planet->conditions[j] * 2.0));
     */
  }

  for (i = 0; i < MAXPLAYERS; ++i) {
    /*
     * Messages from autoreport, player #1 are decodable
     */
    if ((i == Playernum) || (Playernum == 1) || race->God) {
      /*
       * You can talk to your own race
       */
      race->translate[i - 1] = 100;
    } else {
      race->translate[i - 1] = 1;
    }
  }

  /*
   * Assign racial characteristics
   */
  for (i = 0; i < 100; ++i) {
    race->discoveries[i] = 0;
  }

  race->tech = 0.0;
  race->morale = 0;
  race->turn = 0;
  race->allied[1] = 0;
  race->allied[0] = race->allied[1];
  race->atwar[1] = 0;
  race->atwar[0] = race->atwar[1];

  race->mass = db_Mass[idx] + (0.001 * (double)int_rand(-25, 25));
  race->birthrate = db_Birthrate[idx] + (0.01 * (double)int_rand(-10, 10));
  race->fighters = db_Fighters[idx] + int_rand(-1, 1);

  if (Thing[idx]) {
    race->IQ = 0;
    race->pods = 1;
    race->collective_iq = race->pods;
    race->absorb = race->collective_iq;
    race->Metamorph = race->absorb;
  } else {
    race->IQ = db_Intelligence[idx] + int_rand(-10, 10);
    race->pods = 0;
    race->collective_iq = race->pods;
    race->absorb = race->collective_iq;
    race->Metamorph = race->absorb;
  }

  race->adventurism = db_Adventurism[idx] + (0.01 * (double)int_rand(-10, 10));
  race->number_sexes =
      int_rand(Min_Sexes[idx], int_rand(Min_Sexes[idx], Max_Sexes[idx]));
  race->metabolism = db_Metabolism[idx] + (0.01 * (double)int_rand(-15, 15));

  if (race->Metamorph) {
    printf("METAMORPHIC\n");
  } else {
    printf("\n");
  }

  printf("       Birthrate: %.3f\n", race->birthrate);
  printf("Fighting ability: %d\n", race->fighters);
  printf("              IQ: %d\n", race->IQ);
  printf("      Metabolism: %2.f\n", race->metabolism);
  printf("     Adventurism: %.2f\n", race->adventurism);
  printf("            Mass: %.2f\n", race->mass);
  printf(" Number of sexes: %d (minimum required for colonization)\n",
         race->number_sexes);

  printf("\n\nLook OK (y/n)?");

  /*
   * if(gets(str) == NULL) gcc says this is dangerouse -mfw
   */
  if (fgets(str, 100, stdin) == NULL) {
    exit(1);
  }

  while (str[0] != 'y') {
    race->mass = db_Mass[idx] + (0.001 * (double)int_rand(-25, 25));
    race->birthrate = db_Birthrate[idx] + (0.01 * (double)int_rand(-10, 10));
    race->fighters = db_Fighters[idx] + int_rand(-1, 1);

    if (Thing[idx]) {
      race->IQ = 0;
      race->pods = 1;
      race->collective_iq = race->pods;
      race->absorb = race->collective_iq;
      race->Metamorph = race->absorb;
    } else {
      race->IQ = db_Intelligence[idx] + int_rand(-10, 10);
      race->pods = 0;
      race->collective_iq = race->pods;
      race->absorb = race->collective_iq;
      race->Metamorph = race->absorb;
    }

    race->adventurism =
        db_Adventurism[idx] + (0.01 * (double)int_rand(-10, 10));
    race->number_sexes =
        int_rand(Min_Sexes[idx], int_rand(Min_Sexes[idx], Max_Sexes[idx]));
    race->metabolism = db_Metabolism[idx] + (0.01 * (double)int_rand(-15, 15));

    if (race->Metamorph) {
      printf("METAMORPHIC\n");
    } else {
      printf("\n");
    }

    printf("       Birthrate: %.3f\n", race->birthrate);
    printf("Fighting ability: %d\n", race->fighters);
    printf("              IQ: %d\n", race->IQ);
    printf("      Metabolism: %.2f\n", race->metabolism);
    printf("     Adventurism: %.2f\n", race->adventurism);
    printf("            Mass: %.2f\n", race->mass);
    printf(" Number of sexes: %d (minimum required for colonization)\n",
           race->number_sexes);

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
    ++secttypes[Sector(*planet, x, y).condition].count;

    if (!secttypes[Sector(*planet, x, y).condition].here) {
      secttypes[Sector(*planet, x, y).condition].here = 1;
      secttypes[Sector(*planet, x, y).condition].x = x;
      secttypes[Sector(*planet, x, y).condition].y = y;
    }
  }

  planet->explored = 1;

  for (i = SEA; i <= WASTED; ++i) {
    if (secttypes[i].here) {
      printf("(%2d): %c (%d, %d) (%s, %d sectors)\n", i,
             en_desshow(planet, secttypes[i].x, secttypes[i].y), secttypes[i].x,
             secttypes[i].y, Desnames[i], secttypes[i].count);
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
  race->likesbest = i;
  race->likes[i] = 1.0;
  race->likes[PLATED] = 1.0;
  race->likes[WASTED] = 0.0;

  printf("\nEnter compatibilities of other sectors -\n");

  for (j = SEA; j <= PLATED; ++j) {
    if (i != j) {
      printf("%6s (%3d sectors): ", Desnames[j], secttypes[j].count);
      scanf("%d", &k);
      race->likes[j] = (double)k / 100.0;
    }
  }

  printf("Numraces = %d\n", Numraces());
  race->Playernum = Numraces() + 1;
  Playernum = race->Playernum;

  /*
   * mask = sigblock(SIGBLOCKS);
   */

  /*
   * Build a capital ship to run the government
   */
  shiptype capital_ship;
  Bzero(capital_ship);
  int shipno = Numships() + 1;

  printf("Creating government ship %d...\n", shipno);

  race->Gov_ship = shipno;
  planet->ships = shipno;
  capital_ship.nextship = 0;

  capital_ship.type = OTYPE_GOV;
  capital_ship.xpos = Stars[star]->xpos + planet->xpos;
  capital_ship.ypos = Stars[star]->ypos + planet->ypos;
  capital_ship.land_x = (char)secttypes[i].x;
  capital_ship.land_y = (char)secttypes[i].y;
  capital_ship.speed = 0;
  capital_ship.owner = Playernum;
  capital_ship.race = Playernum;
  capital_ship.governor = 0;
  capital_ship.tech = 100.0;
  capital_ship.build_type = capital_ship.type;
  capital_ship.armor = Shipdata[capital_ship.type][ABIL_ARMOR];
  capital_ship.guns = PRIMARY;
  capital_ship.primary = Shipdata[capital_ship.type][ABIL_GUNS];
  capital_ship.primtype = Shipdata[capital_ship.type][ABIL_PRIMARY];
  capital_ship.secondary = Shipdata[capital_ship.type][ABIL_GUNS];
  capital_ship.sectype = Shipdata[capital_ship.type][ABIL_SECONDARY];
  capital_ship.max_crew = Shipdata[capital_ship.type][ABIL_MAXCREW];
  capital_ship.max_destruct = Shipdata[capital_ship.type][ABIL_DESTCAP];
  capital_ship.max_resource = Shipdata[capital_ship.type][ABIL_CARGO];
  capital_ship.max_fuel = Shipdata[capital_ship.type][ABIL_FUELCAP];
  capital_ship.max_speed = Shipdata[capital_ship.type][ABIL_SPEED];
  capital_ship.build_cost = Shipdata[capital_ship.type][ABIL_COST];
  capital_ship.size = 100;
  capital_ship.base_mass = 100.0;
  sprintf(capital_ship.class, "Standard");
  capital_ship.fuel = 0.0;
  capital_ship.popn = capital_ship.max_crew;
  capital_ship.troops = 0;
  capital_ship.mass = capital_ship.base_mass + (capital_ship.popn * race->mass);
  capital_ship.resource = 0;
  capital_ship.destruct = capital_ship.resource;
  capital_ship.alive = 1;
  capital_ship.active = 1;
  capital_ship.protect.self = 1;
  capital_ship.docked = 1;
  /*
   * Docked on the planet
   */
  capital_ship.whatorbits = LEVEL_PLAN;
  capital_ship.whatdest = LEVEL_PLAN;
  capital_ship.deststar = star;
  capital_ship.destpnum = pnum;
  capital_ship.storbits = star;
  capital_ship.pnumorbits = pnum;
  capital_ship.rad = 0;
  /*
   * Shipdata[capital_ship.type][ABIL_DAMAGE];
   */
  capital_ship.damage = 0;
  /*
   * First capital is 100% efficient
   */
  capital_ship.retaliate = 0;
  capital_ship.ships = 0;
  capital_ship.on = 1;
  capital_ship.limit = 100;
  capital_ship.use_stock = 1;
  capital_ship.hop = 1;
  capital_ship.name[0] = '\0';
  capital_ship.number = shipno;

  printf("Created on sector %d,%d on /%s/%s\n", capital_ship.land_x,
         capital_ship.land_y, Stars[capital_ship.storbits]->name,
         Stars[capital_ship.storbits]->pnames[capital_ship.pnumorbits]);

  putship(&capital_ship);

  for (j = 0; j < MAXPLAYERS; ++j) {
    race->points[j] = 0;
  }

  putrace(race);

  sect->owner = Playernum;
  sect->race = Playernum;

  if (ADAM_AND_EVE) {
    planet->popn = race->number_sexes;
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
      ((maxsupport(race, sect, 100.0, 0) * planet->Maxx) * planet->Maxy) / 2;
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
  printf("Your race has been created on sector %d,%d on\n", secttypes[i].x,
         secttypes[i].y);
  printf("%s/%s.\n\n", Stars[star]->name, Stars[star]->pnames[pnum]);

  return 0;
}

/*
 * Copied from map.c
 */
char en_desshow(planettype *p, int x, int y) {
  sectortype *s = &Sector(*p, x, y);

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
    return CHAR_GAS;
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
