/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
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
 * orbit.c -- Display orbits of planets (graphic representation)
 *
 * OPTIONS:
 *   -p : If this option is set, ''orbit'' will not display planet names.
 *   -S : Do not display star names.
 *   -s : Do not display ships.
 *   -(number) : Do not display that #'d ship or planet (in case it obstructs
 *               the view of another object)
 *
 * #ident  "@(#)orbit.c  1.9 12/3/93 "
 *
 * Header: /var/cvs/gbp/GB+/user/orbit.c,v 1.3 2007/07/06 18:09:34 gbp Exp $
 */
#include "orbit.h"

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../server/buffers.h"
#include "../server/client.h"
#include "../server/files_shl.h"
#include "../server/GB_server.h"
#include "../server/getplace.h"
#include "../server/max.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/rand.h"
#include "../server/ranks.h"
#include "../server/ships.h"
#include "../server/vars.h"

#include "csp_orbit.h"
#include "fire.h"

extern char Shipltrs[];

static racetype *race;

/* Global definitions here...Should possibly move them */
char Psymbol[] =
    { '@', 'o', 'O', '#', '~', '.', '"', '-', '0', '(' };

/* Note: Swamp is not implemented */
char const *Planet_types[] = {
    "Class M",
    "Asteroid",
    "Airless",
    "Iceball",
    "Jovian",
    "Waterball",
    "Forest",
    "Desert",
    "Wormhole",
    "Swamp"
};

extern void DispStar(int, int, int, startype *, int, char *, orbitinfo *);
extern void DispPlanet(int,
                       int,
                       int,
                       planettype *,
                       char *,
                       racetype *,
                       char *,
                       orbitinfo *);

extern void DispShip(int,
                     int,
                     placetype *,
                     shiptype *,
                     planettype *,
                     int,
                     char *,
                     orbitinfo *);

void orbit(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    int sh;
    int i;
    int iq;
    planettype *p;
    shiptype *s;
    placetype where;
    orbitinfo oi;

    oi.DontDispStars = 0;
    oi.DontDispShips = oi.DontDispStars;
    oi.DontDispPlanets = oi.DontDispShips;
    oi.DontDispNum = -1;

    /* Find options, set flags accordingly */
    if (optn) {
        if (opts['s']) {
            oi.DontDispShips = 1;
        }

        if (opts['S']) {
            oi.DontDispStars = 1;
        }

        if (opts['p']) {
            oi.DontDispPlanets = 1;
        }

        if (opts['d']) {
            oi.DontDispNum = opts['d'];

            if (oi.DontDispNum) {
                /* Make a '1' into a '0' */
                --oi.DontDispNum;
            } else {
                sprintf(buf, "Bad number %d.\n", oi.DontDispNum);
                notify(playernum, governor, buf);
                oi.DontDispNum = -1;
            }
        }
    }

    if (argn == 1) {
        where = Getplace(playernum, governor, ":", 0);

        if (Dir[playernum - 1][governor].level == LEVEL_UNIV) {
            i = 1;
        } else {
            i = 0;
        }

        oi.Lastx = Dir[playernum - 1][governor].lastx[i];
        oi.Lasty = Dir[playernum - 1][governor].lasty[i];
        oi.Zoom = Dir[playernum - 1][governor].zoom[i];
    } else {
        where = Getplace(playernum, governor, args[argn - 1], 0);
        oi.Lasty = 0.0;
        oi.Lastx = oi.Lasty;
        oi.Zoom = 1.0;
    }

    if (where.err) {
        notify(playernum, governor, "Orbit: Error in args.\n");

        return;
    }

    /* Display CSP orbit instead, if the client can understand it */
    if (client_can_understand(playernum, governor, CSP_ORBIT_OUTPUT_INTRO)) {
        csp_orbit(playernum, governor, -1, -1, &oi);

        return;
    }

    /* Orbit type of map */
    notify(playernum, governor, "#");
    race = races[playernum - 1];

    switch (where.level) {
    case LEVEL_UNIV:
        for (i = 0; i < Sdata.numstars; ++i) {
            if (oi.DontDispNum != i) {
                DispStar(playernum,
                         governor,
                         LEVEL_UNIV,
                         Stars[i],
                         (int)race->God,
                         buf,
                         &oi);

                notify(playernum, governor, buf);
            }
        }

        if (!oi.DontDispShips) {
            sh = Sdata.ships;

            while (sh) {
                getship(&s, sh);

                if (oi.DontDispNum != sh) {
                    DispShip(playernum,
                             governor,
                             &where,
                             s,
                             NULL,
                             (int)race->God,
                             buf,
                             &oi);

                    notify(playernum, governor, buf);
                }

                sh = nextship(s);
                free(s);
            }
        }

        break;
    case LEVEL_STAR:
        DispStar(playernum,
                 governor,
                 LEVEL_STAR,
                 Stars[where.snum],
                 (int)race->God,
                 buf,
                 &oi);

        notify(playernum, governor, buf);

        for (i = 0; i < Stars[where.snum]->numplanets; ++i) {
            if (oi.DontDispNum != i) {
                getplanet(&p, (int)where.snum, i);

                DispPlanet(playernum,
                           governor,
                           LEVEL_STAR,
                           p,
                           Stars[where.snum]->pnames[i],
                           race,
                           buf,
                           &oi);

                notify(playernum, governor, buf);
                free(p);
            }
        }

        /*
         * Check to see if you have ships orbiting the star, if so you can see
         * enemy ships
         */
        iq = 0;

        if (race->God) {
            iq = 1;
        } else {
            sh = Stars[where.snum]->ships;

            while (sh && !iq) {
                getship(&s, sh);

                if ((s->owner == playernum)
                    && ((s->type == OTYPE_PROBE) || s->popn)) {
                    /* You are there to sight, need a crew */
                    iq = 1;
                }

                sh = nextship(s);
                free(s);
            }
        }

        if (!oi.DontDispShips) {
            sh = Stars[where.snum]->ships;

            while (sh) {
                getship(&s, sh);

                if ((oi.DontDispNum != sh)
                    && ((s->owner == playernum) || (s->type == STYPE_MINEF))) {
                    if ((s->owner == playernum) || (iq == 1)) {
                        DispShip(playernum,
                                 governor,
                                 &where,
                                 s,
                                 NULL,
                                 (int)race->God,
                                 buf,
                                 &oi);

                        notify(playernum, governor, buf);
                    }
                }

                sh = nextship(s);
                free(s);
            }
        }

        break;
    case LEVEL_PLAN:
        getplanet(&p, (int)where.snum, (int)where.pnum);

        DispPlanet(playernum,
                   governor,
                   LEVEL_PLAN,
                   p,
                   Stars[where.snum]->pnames[where.pnum],
                   race,
                   buf,
                   &oi);

        notify(playernum, governor, buf);

        /*
         * Check to see if you have ships landed or orbiting the planet, if so
         * you can see orbiting enemy ships
         */
        iq = 0;
        sh = p->ships;

        while (sh && !iq) {
            getship(&s, sh);

            if ((s->owner == playernum)
                && ((s->type == OTYPE_PROBE) || s->popn)) {
                /* You are there to sight, need a crew */
                iq = 1;
            }

            sh = nextship(s);
            free(s);
        }

        /* End check */
        if (!oi.DontDispShips) {
            sh = p->ships;

            while (sh) {
                getship(&s, sh);

                if (oi.DontDispNum != sh) {
                    if (!landed(s)) {
                        if ((s->owner == playernum) || (iq == 1)) {
                            DispShip(playernum,
                                     governor,
                                     &where,
                                     s,
                                     p,
                                     (int)race->God,
                                     buf,
                                     &oi);

                            notify(playernum, governor, buf);
                        }
                    }
                }

                sh = nextship(s);
                free(s);
            }
        }

        free(p);

        break;
    default:
        notify(playernum, governor, "Bad scope.\n");

        return;
    }

    notify(playernum, governor, "\n");
}

void DispStar(int playernum,
              int governor,
              int level,
              startype *star,
              int god,
              char *string,
              orbitinfo *oi)
{
    int x = -1;
    int y = -1;
    int stand;

    *string = '\0';

    if (level == LEVEL_UNIV) {
        x = (int)(ORBIT_SCALE + ((ORBIT_SCALE * (star->xpos - oi->Lastx)) / (UNIVSIZE * oi->Zoom)));
        y = (int)(ORBIT_SCALE + ((ORBIT_SCALE * (star->ypos - oi->Lasty)) / (UNIVSIZE * oi->Zoom)));
    } else if (level == LEVEL_STAR) {
        x = (int)(ORBIT_SCALE + (ORBIT_SCALE * (-oi->Lastx)) / (SYSTEMSIZE * oi->Zoom));
        y = (int)(ORBIT_SCALE + (ORBIT_SCALE * (-oi->Lasty)) / (SYSTEMSIZE * oi->Zoom));
    }

    /*
     * if (star->nova_stage) {
     *     DispArray(x, y, 11, 7, Novae[star->nova_stage - 1], fac);
     * }
     */

    if ((y >= 0) && (x >= 0)) {
        if (race->governor[governor].toggle.color) {
            if (isset(star->explored, playernum)) {
                stand = playernum + '?';
            } else {
                stand = 0 + '?';
            }

            sprintf(temp, "%c %d %d %d * ", (char)stand, x, y, star->nova_stage);
            strcat(string, temp);

            if (isset(star->inhabited, playernum)) {
                stand = playernum + '?';
            } else {
                stand = 0 + '?';
            }

            sprintf(temp, "%c %s;", (char)stand, star->name);
            strcat(string, temp);
        } else if (race->governor[governor].toggle.inverse) {
            if (isset(star->explored, playernum)) {
                stand = 1;
            } else {
                stand = 0;
            }

            sprintf(temp, "%d %d %d %d * ", stand, x, y, star->nova_stage);
            strcat(string, temp);

            if (isset(star->inhabited, playernum)) {
                stand = 1;
            } else {
                stand = 0;
            }

            sprintf(temp, "%d %s;", stand, star->name);
            strcat(string, temp);
        } else {
            sprintf(temp, "0 %d %d %d * ", x, y, star->nova_stage);
            strcat(string, temp);
            sprintf(temp, "0 %s;", star->name);
            strcat(string, temp);
        }
    }
}

void DispPlanet(int playernum,
                int governor,
                int level,
                planettype *p,
                char *name,
                racetype *r,
                char *string,
                orbitinfo *oi)
{
    int x = -1;
    int y = -1;
    int stand;

    *string = '\0';

    if (level == LEVEL_STAR) {
        y = (int)(ORBIT_SCALE + (ORBIT_SCALE * (p->ypos - oi->Lasty)) / (SYSTEMSIZE * oi->Zoom));
        x = (int)(ORBIT_SCALE * (ORBIT_SCALE * (p->xpos - oi->Lastx)) / (SYSTEMSIZE * oi->Zoom));
    } else if (level == LEVEL_PLAN) {
        y = (int)(ORBIT_SCALE + (ORBIT_SCALE * (-oi->Lasty)) / (PLORBITSIZE * oi->Zoom));
        x = (int)(ORBIT_SCALE + (ORBIT_SCALE * (-oi->Lastx)) / (PLORBITSIZE * oi->Zoom));
    }

    if ((x >= 0) && (y >= 0)) {
        if (r->governor[governor].toggle.color) {
            if (p->info[playernum - 1].explored) {
                stand = playernum + '?';
            } else {
                stand = 0 + '?';
            }

            if ((stand > '?') || r->God) {
                sprintf(temp,
                        "%c %d %d 0 %c ",
                        (char)stand,
                        x,
                        y,
                        Psymbol[p->type]);
            } else {
                sprintf(temp,
                        "%c %d %d 0 %c ",
                        (char)stand,
                        x,
                        y,
                        '?');
            }

            strcat(string, temp);

            if (p->info[playernum - 1].numsectsowned) {
                stand = playernum + '?';
            } else {
                stand = 0 + '?';
            }

            sprintf(temp, "%c %s", (char)stand, name);
            strcat(string, temp);
        } else if (r->governor[governor].toggle.inverse) {
            if (p->info[playernum - 1].explored) {
                stand = 1;
            } else {
                stand = 0;
            }

            if (stand || r->God) {
                sprintf(temp, "%d %d %d 0 %c ", stand, x, y, Psymbol[p->type]);
            } else {
                sprintf(temp, "%d %d %d 0 %c ", stand, x, y, '?');
            }

            strcat(string, temp);

            if (p->info[playernum - 1].numsectsowned) {
                stand = 1;
            } else {
                stand = 0;
            }

            sprintf(temp, "%d %s", stand, name);
            strcat(string, temp);
        } else {
            if (p->info[playernum - 1].explored) {
                stand = 1;
            } else {
                stand = 0;
            }

            if (stand || r->God) {
                sprintf(temp, "0 %d %d 0 %c ", x, y, Psymbol[p->type]);
            } else {
                sprintf(temp, "0 %d %d 0 %c ", x, y, '?');
            }

            strcat(string, temp);
            sprintf(temp, "0 %s", name);
            strcat(string, temp);
        }

        if (r->governor[governor].toggle.compat
            && p->info[playernum - 1].explored) {
            sprintf(temp, "(%d)", (int)compatibility(p, r));
            strcat(string, temp);
        }

        strcat(string, ";");
    }
}

void DispShip(int playernum,
              int governor,
              placetype *where,
              shiptype *ship,
              planettype *pl,
              int god,
              char *string,
              orbitinfo *oi)
{
    int x;
    int y;
    int wm;
    int stand;
    shiptype *aship;
    planettype *apl;
    double xt;
    double yt;
    double slope;

    if (!ship->alive) {
        return;
    }

    *string = '\0';

    switch (where->level) {
    case LEVEL_PLAN:
        x = (int)(ORBIT_SCALE + ((ORBIT_SCALE * (ship->xpos - (Stars[where->snum]->xpos + pl->xpos) - oi->Lastx)) / (PLORBITSIZE * oi->Zoom)));
        y = (int)(ORBIT_SCALE + ((ORBIT_SCALE * (ship->ypos - (Stars[where->snum]->ypos + pl->ypos) - oi->Lasty)) / (PLORBITSIZE * oi->Zoom)));

        break;
    case LEVEL_STAR:
        x = (int)(ORBIT_SCALE + ((ORBIT_SCALE * (ship->xpos - Stars[where->snum]->xpos - oi->Lastx)) / (SYSTEMSIZE * oi->Zoom)));
        y = (int)(ORBIT_SCALE + ((ORBIT_SCALE * (ship->ypos - Stars[where->snum]->ypos - oi->Lasty)) / (SYSTEMSIZE * oi->Zoom)));

        break;
    case LEVEL_UNIV:
        x = (int)(ORBIT_SCALE + ((ORBIT_SCALE * (ship->xpos - oi->Lastx)) / (UNIVSIZE * oi->Zoom)));
        y = (int)(ORBIT_SCALE + ((ORBIT_SCALE * (ship->ypos - oi->Lasty)) / (UNIVSIZE * oi->Zoom)));

        break;
    default:
        notify(playernum, governor, "WHOA! Error in DispShip.\n");

        return;
    }

#ifdef USE_VN
    int n;
    int xa;
    int ya;
    float fac;
#endif

    switch (ship->type) {
    case STYPE_MIRROR:
        if (ship->special.aimed_at.level == LEVEL_STAR) {
            xt = Stars[ship->special.aimed_at.snum]->xpos;
            yt = Stars[ship->special.aimed_at.snum]->ypos;
        } else if (ship->special.aimed_at.level == LEVEL_PLAN) {
            if ((where->level == LEVEL_PLAN)
                && (ship->special.aimed_at.pnum == where->pnum)) {
                /* Same planet */
                xt = Stars[ship->special.aimed_at.snum]->xpos + pl->xpos;
                yt = Stars[ship->special.aimed_at.snum]->ypos + pl->ypos;
            } else {
                /* Different planet */
                getplanet(&apl, (int)where->snum, (int)where->pnum);
                xt = Stars[ship->special.aimed_at.snum]->xpos + apl->xpos;
                yt = Stars[ship->special.aimed_at.snum]->ypos + apl->ypos;
                free(apl);
            }
        } else if (ship->special.aimed_at.level == LEVEL_SHIP) {
            if (getship(&aship, (int)ship->special.aimed_at.shipno)) {
                xt = aship->xpos;
                yt = aship->ypos;
                free(aship);
            } else {
                yt = 0.0;
                xt = yt;
            }
        } else {
            yt = 0.0;
            xt = yt;
        }

        wm = 0;

        if (xt == ship->xpos) {
            if (yt > ship->ypos) {
                wm = 5;
            } else {
                wm = 1;
            }
        } else {
            slope = (yt - ship->ypos) / (xt - ship->xpos);

            if (yt == ship->ypos) {
                if (xt > ship->xpos) {
                    wm = 3;
                } else {
                    wm = 7;
                }
            } else if (yt > ship->ypos) {
                if (slope < -2.414) {
                    wm = 5;
                }

                if (slope > -2.414) {
                    wm = 6;
                }

                if (slope > -0.414) {
                    wm = 7;
                }

                if (slope > 0.000) {
                    wm = 3;
                }

                if (slope > 0.414) {
                    wm = 4;
                }

                if (slope > 2.414) {
                    wm = 5;
                }
            } else if (yt < ship->ypos) {
                if (slope < -2.414) {
                    wm = 1;
                }

                if (slope > -2.414) {
                    wm = 2;
                }

                if (slope > -0.414) {
                    wm = 3;
                }

                if (slope > 0.000) {
                    wm = 7;
                }

                if (slope > 0.414) {
                    wm = 8;
                }

                if (slope > 2.414) {
                    wm = 1;
                }
            }
        }

        /* Magnification */
        if ((x >= 0) && (y >= 0)) {
            if (race->governor[governor].toggle.color) {
                sprintf(string,
                        "%c %d %d %d %c %c %d;",
                        (char)(ship->owner + '?'),
                        x,
                        y,
                        wm,
                        Shipltrs[ship->type],
                        (char)(ship->owner + '?'),
                        ship->number);
            } else {
                if (ship->owner == race->governor[governor].toggle.highlight) {
                    stand = 1;
                } else {
                    stand = 0;
                }

                sprintf(string,
                        "%d %d %d %d %c %d %d;",
                        stand,
                        x,
                        y,
                        wm,
                        Shipltrs[ship->type],
                        stand,
                        ship->number);
            }
        }

        break;
    case OTYPE_CANIST:
    case OTYPE_GREEN:

        break;

#ifdef USE_VN
    case OTYPE_VN:
        wm = 0;

        /* Make a cloud of Von Neumann machines */
        if ((ship->whatorbits != LEVEL_UNIV)
            || (ship->owner == playernum)
            || god) {
            fac = ship->number /
                ((ship->whatorbits == LEVEL_UNIV ? 100.0
                  : (ship->whatorbits == LEVEL_STAR ? 30.0
                     : 4.0)) * oi->Zoom);

            for (n = 1; (n <= ship->number) && (n < 267); ++n) {
                xa = int_rand(x - (int)fac, x + (int)fac);
                ya = int_rand(y - (int)fac, y + (int)fac);

                if ((xa >= 0) && (ya >= 0)) {
                    if (race->governor[governor].toggle.color) {
                        sprintf(temp,
                                "%c %d %d %d %c %c %d;",
                                (char)(ship->owner + 48),
                                xa,
                                ya,
                                wm,
                                Shipltrs[ship->type],
                                (char)(ship->owner + 48),
                                ship->number);
                    } else {
                        if (ship->owner == race->governor[governor].toggle.highlight) {
                            stand = 1;
                        } else {
                            stand = 0;
                        }

                        sprintf(temp,
                                "%d %d %d %d %c %d %d;",
                                stand,
                                xa,
                                ya,
                                wm,
                                Shipltrs[ship->type],
                                stand,
                                ship->number);
                    }

                    strcat(string, temp);
                }
            }
        }

        break;
#endif
    default:
        /* Make sure ship is not cloaked if not ours */
        if ((ship->owner != playernum) && ship->cloaked) {
            return;
        }

        /* Other ships can only be seen when in system */
        wm = 0;

        if ((ship->whatorbits != LEVEL_UNIV)
            || ((ship->owner == playernum) || god)) {
            if ((x >= 0) && (y >= 0)) {
                if (race->governor[governor].toggle.color) {
                    sprintf(string,
                            "%c %d %d %d %c %c %d;",
                            (char)(ship->owner + '?'),
                            x,
                            y,
                            wm,
                            Shipltrs[ship->type],
                            (char)(ship->owner + '?'),
                            ship->number);
                } else {
                    if (ship->owner == race->governor[governor].toggle.highlight) {
                        stand = 1;
                    } else {
                        stand = 0;
                    }

                    sprintf(string,
                            "%d %d %d %d %c %d %d;",
                            stand,
                            x,
                            y,
                            wm,
                            Shipltrs[ship->type],
                            stand,
                            ship->number);
                }
            }
        }

        break;
    }
}
