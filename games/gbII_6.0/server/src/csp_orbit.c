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
 * orbit.c -- Displays orbits of planets (graphic representation)
 *
 * #ident  "@(#)csp_orbit.c  1.2 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/csp_orbit.c,v 1.4 2007/07/06 18:09:34 gbp Exp
 * $
 */
#include "csp_orbit.h"

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "csp.h"
#include "csp_types.h"
#include "files_shl.h"
#include "GB_server.h"
#include "getplace.h"
#include "max.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "shipdata.h"
#include "ships.h"
#include "vars.h"

#include "fire.h"

void csp_DispStar(int, int, int, int, startype *, racetype *, char *);
void csp_DispPlanet(int, int, int, int, int, planettype *, char *, racetype *, char *);
void csp_DispShip(int, int, placetype *, shiptype *, planettype *, racetype *, char *);

void csp_orbit(int playernum, int governor, int unused3, int unused4, orbitinfo *oi)
{
    int sh;
    int i;
    int iq;
    int showit;
    planettype *p;
    shiptype *s;
    placetype where;

    if (strncmp(args[0], "orbit", 5) && strncmp(args[0], "map", 3)) {
        if (argn < 2) {
            sprintf(buf,
                    "%c %d %d\n",
                    CSP_CLIENT,
                    CSP_ERR,
                    CSP_ERR_TOO_FEW_ARGS);

            notify(playernum, governor, buf);

            return;
        }

        if (argn > 3) {
            sprintf(buf,
                    "%c %d %d\n",
                    CSP_CLIENT,
                    CSP_ERR,
                    CSP_ERR_TOO_MANY_ARGS);

            notify(playernum, governor, buf);

            return;
        }

        if (argn == 2) {
            where = Getplace(playernum, governor, ":", 0);
        } else {
            where = Getplace(playernum, governor, args[2], 0);
        }
    } else {
        if (argn == 1) {
            where = Getplace(playernum, governor, ":", 0);
        } else {
            where = Getplace(playernum, governor, args[2], 0);
        }

        /*
         * Going to need to parse other orbit args here too (see orbit.hlp) -mfw
         */
    }

    if (where.err) {
        sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_ERR, CSP_ERR_NOSUCH_PLACE);
        notify(playernum, governor, buf);

        return;
    }

    racetype *race = races[playernum - 1];

    sprintf(buf,
            "%c %d %d %d %d %d %d %lf %lf %lf %d %d %d %d %d %d %s %s\n",
            CSP_CLIENT,
            CSP_ORBIT_OUTPUT_INTRO,
            where.level,
            UNIVSIZE,
            SYSTEMSIZE,
            PLORBITSIZE,
            ORBIT_SCALE,
            oi->Lastx,
            oi->Lasty,
            oi->Zoom,
            oi->DontDispStars,
            oi->DontDispPlanets,
            oi->DontDispShips,
            race->governor[governor].toggle.inverse,
            race->governor[governor].toggle.color,
            Sdata.numstars,
            GAL_TYPE,
            GAL_NAME);

    notify(playernum, governor, buf);

    switch (where.level) {
    case LEVEL_UNIV:
        if (!oi->DontDispStars) {
            for (i = 0; i < Sdata.numstars; ++i) {
                csp_DispStar(playernum,
                             governor,
                             LEVEL_UNIV,
                             i,
                             Stars[i],
                             race,
                             buf);

                notify(playernum, governor, buf);
            }
        }

        if (!oi->DontDispShips) {
            sh = Sdata.ships;

            while (sh) {
                getship(&s, sh);

                if (sh != oi->DontDispNum) {
                    csp_DispShip(playernum,
                                 governor,
                                 &where,
                                 s,
                                 NULL,
                                 race,
                                 buf);

                    notify(playernum, governor, buf);
                }

                sh = nextship(s);
                free(s);
            }
        }

        break;
    case LEVEL_STAR:
        if (!oi->DontDispStars) {
            csp_DispStar(playernum,
                         governor,
                         LEVEL_STAR,
                         where.snum,
                         Stars[where.snum],
                         race,
                         buf);

            notify(playernum, governor, buf);
        }

        if (!oi->DontDispShips) {
            for (i = 0; i < Stars[where.snum]->numplanets; ++i) {
                getplanet(&p, (int)where.snum, i);

                csp_DispPlanet(playernum,
                               governor,
                               LEVEL_STAR,
                               where.snum,
                               i,
                               p,
                               Stars[where.snum]->pnames[i],
                               race,
                               buf);

                notify(playernum, governor, buf);
                free(p);
            }
        }

        if (!oi->DontDispShips) {
            /*
             * Check to see if you have ships orbiting at the star, if so you
             * can see enemy ships
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

            sh = Stars[where.snum]->ships;

            while (sh) {
                getship(&s, sh);

                /* The statement from orbit.c: */
#ifdef notdef
                /*
                 * Ick...what a horrible statement. As close as I can figure, it
                 * says to never display MINEs unless they are yours. And only
                 * display other player's ships if iq == 1. So that's what I'll
                 * code...
                 */
                if ((oi->DontDispNum != sh)
                    && ((s->owner == playernum)
                        || (s->type != STYPE_MINEF))) {
                    if ((s->owner == playernum) || (iq == 1)) {
                        csp_DispShip(playernum,
                                     governor,
                                     &where,
                                     s,
                                     NULL,
                                     race,
                                     buf);

                        notify(playernum, governor, buf);
                    }
                }
#endif
                if (sh == oi->DontDispNum) {
                    showit = 0;
                } else if (s->owner == playernum) {
                    showit = 1; /* Always display our ships */
                } else if (s->type != STYPE_MINEF) {
                    /* Never display mines */
                    showit = iq; /* showit if we can see it */
                } else {
                    showit = 0;
                }

                if (showit) {
                    csp_DispShip(playernum,
                                 governor,
                                 &where,
                                 s,
                                 NULL,
                                 race,
                                 buf);

                    notify(playernum, governor, buf);
                }

                sh = nextship(s);
                free(s);
            }
        }

        break;
    case LEVEL_PLAN:
        getplanet(&p, (int)where.snum, (int)where.pnum);

        if (!oi->DontDispPlanets) {
            csp_DispPlanet(playernum,
                           governor,
                           LEVEL_PLAN,
                           where.snum,
                           where.pnum,
                           p,
                           Stars[where.snum]->pnames[where.pnum],
                           race,
                           buf);

            notify(playernum, governor, buf);
        }

        if (!oi->DontDispShips) {
            /*
             * Check to see if you have ships landed or orbiting the planet, if
             * so you can see orbiting enemy ships
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
            sh = p->ships;

            while (sh) {
                getship(&s, sh);

                if (!landed(s)) {
                    if (((s->owner == playernum) || (iq == 1))
                        && (sh != oi->DontDispNum)) {
                        csp_DispShip(playernum,
                                     governor,
                                     &where,
                                     s,
                                     p,
                                     race,
                                     buf);

                        notify(playernum, governor, buf);
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

        break;
    }

    sprintf(buf, "%c %d\n", CSP_CLIENT, CSP_ORBIT_OUTPUT_END);
    notify(playernum, governor, buf);
}

void csp_DispStar(int playernum,
                  int governor,
                  int level,
                  int snum,
                  startype *star,
                  racetype *race,
                  char *string)
{
    int stability;
    int explored;
    double x = -1.0;
    double y = -1.0;
    int god = race->God;

    if (level == LEVEL_UNIV) {
        x = star->xpos;
        y = star->ypos;
    } else if (level == LEVEL_STAR) {
        x = 0.0;
        y = 0.0;
    }

    /*
     * if (star->nova_stage) {
     *     DispArray(x, y, 11, 7, Novae[star->nova_stage - 1], fac);
     * }
     */

    if (isset(star->explored, playernum) || god) {
        explored = 1;
    } else {
        explored = 0;
    }

    if ((race->tech >= TECH_SEE_STABILITY) || god) {
        stability = star->stability;
    } else {
        stability = -1;
    }

    if (explored) {
        sprintf(string,
                "%c %d %d %f %f %d %ld %d %d %d %d %f %s\n",
                CSP_CLIENT,
                CSP_ORBIT_STAR_DATA,
                snum,
                x,
                y,
                explored,
                isset(star->inhabited, playernum),
                star->numplanets,
                stability,
                star->nova_stage,
                star->temperature,
                star->gravity,
                star->name);
    } else {
        sprintf(string,
                "%c %d %d %f %f %d %ld -1 -1 -1 -1 0.0 %s\n",
                CSP_CLIENT,
                CSP_ORBIT_STAR_DATA,
                snum,
                x,
                y,
                explored,
                isset(star->inhabited, playernum),
                star->name);
    }
}

void csp_DispPlanet(int playernum,
                    int governor,
                    int level,
                    int snum,
                    int pnum,
                    planettype *p,
                    char *name,
                    racetype *r,
                    char *string)
{
    double x = -1.0;
    double y = -1.0;

    if (level == LEVEL_STAR) {
        y = p->ypos;
        x = p->xpos;
    } else if (level == LEVEL_PLAN) {
        y = 0.0;
        x = 0.0;
    }

    if (!p->info[playernum - 1].explored && !r->God) {
        sprintf(string,
                "%c %d %d %d %f %f %s\n",
                CSP_CLIENT,
                CSP_ORBIT_UNEXP_PL_DATA,
                snum,
                pnum,
                x,
                y,
                name);
    } else {
        if (p->info[playernum - 1].numsectsowned) {
            sprintf(string,
                    "%c %d %d %d %f %f %d %f 1 %s\n",
                    CSP_CLIENT,
                    CSP_ORBIT_EXP_PL_DATA,
                    snum,
                    pnum,
                    x,
                    y,
                    p->type,
                    compatibility(p, r),
                    name);
        } else {
            sprintf(string,
                    "%c %d %d %d %f %f %d %f 0 %s\n",
                    CSP_CLIENT,
                    CSP_ORBIT_EXP_PL_DATA,
                    snum,
                    pnum,
                    x,
                    y,
                    p->type,
                    compatibility(p, r),
                    name);
        }
    }
}

void csp_DispShip(int playernum,
                  int governor,
                  placetype *where,
                  shiptype *ship,
                  planettype *pl,
                  racetype *race,
                  char *string)
{
    double x;
    double y;
    shiptype *aship;
    planettype *apl;
    double xt;
    double yt;
    int god = race->God;

    *string = '\0';

    if (!ship->alive) {
        return;
    }

    switch (where->level) {
    case LEVEL_PLAN:
        x = ship->xpos - (Stars[where->snum]->xpos + pl->xpos);
        y = ship->ypos - (Stars[where->snum]->ypos + pl->ypos);

        break;
    case LEVEL_STAR:
        x = ship->xpos - Stars[where->snum]->xpos;
        y = ship->ypos - Stars[where->snum]->ypos;

        break;
    case LEVEL_UNIV:
        x = ship->xpos;
        y = ship->ypos;

        break;
    default:
        notify(playernum, governor, "WHOA! Error in DispShip.\n");

        return;
    }

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

        break;
    default:
        xt = 0.0;
        yt = 0.0;

        break;
    }

    if ((ship->whatorbits != LEVEL_UNIV)
        || ((ship->owner == playernum) || god)) {
        if (race->governor[governor].toggle.color) {
            sprintf(string,
                    "%c %d %d %d %c %f %f %f %f\n",
                    CSP_CLIENT,
                    CSP_ORBIT_SHIP_DATA,
                    ship->number,
                    ship->owner + '?',
                    Shipltrs[ship->type],
                    x,
                    y,
                    xt,
                    yt);
        } else {
            sprintf(string,
                    "%c %d %d %d %c %f %f %f %f\n",
                    CSP_CLIENT,
                    CSP_ORBIT_SHIP_DATA,
                    ship->number,
                    ship->owner,
                    Shipltrs[ship->type],
                    x,
                    y,
                    xt,
                    yt);
        }
    }
}
