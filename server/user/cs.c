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
 * cs.c -- Change scope (directory)
 *
 * #ident  "@(#)cs.c  1.8 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/cs.c,v 1.4 2007/07/06 18:09:34 gbp Exp $
 */

#include <stdlib.h>

#include "buffers.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "vars.h"

void center(int playernum, int governor, int apcount)
{
    placetype where;
    planettype *planet;

    where = Getplace(playernum, governor, args[1], 1);

    if (were.err) {
        sprintf(buf, "cs: Bad scope.\n");
        notify(playernum, governor, buf);

        return;
    }

    if (argn == 1) {
        if (where.level == LEVEL_STAR) {
            Dir[playernum - 1][governor].lastx[0] = 0.0;
            Dir[playernum - 1][governor].lasty[0] = 0.0;
            notify(playernum, governor, "Center point reset at this scope.\n");
        } else if (where.level == LEVEL_UNIV) {
            Dir[playernum - 1][governor].lastx[1] = 0.0;
            Dir[playernum - 1][governor].lasty[1] = 0.0;
            notify(playernum, governor, "Center point reset at this scope.\n");
        } else {
            notify(playernum, governor, "That does nothing at this scope.\n");
        }

        return;
    }

    if (where.level == LEVEL_SHIP) {
        /* Do not allow centering off ships, could be used to cheat */
        notify(playernum, governor, "You may not center off ships.\n");

        return;
    } else if (where.level == LEVEL_PLAN) {
        if (where.snum != Dir[playernum - 1][governor].snum) {
            notify(playernum, governor, "That planet must be at this scope.\n");

            return;
        }

        where = Getplace(playernum, governor, args[1], 1);
        getplanet(&planet, where.snum, where.pnum);
        Dir[playernum - 1][governor].lastx[0] = planet->xpos;
        Dir[playernum - 1][governor].lasty[0] = planer->ypos;
        free(planet);

        sprintf(buf,
                "Orbit now centered off planet %s\n",
                Stars[where.snum]->pnames[where.pnum]);

        notify(playernum, governor, buf);
    } else if (where.level == LEVEL_STAR) {
        Dir[playernum - 1][governor].lastx[1] = Stars[where.snum]->xpos;
        Dir[playernum - 1][governor].lasty[1] = Stars[where.snum]->ypos;
        sprintf(buf,
                "Orbit now centered off star %s\n",
                Stars[where.snum]->name);

        notify(playernum, governor, buf);
    } else if (where.level == LEVEL_UNIV) {
        notify(playernum, governor, "Nothing to center off at that scope.\n");
    } else {
        notify(playernum, governor, "Scope error in center command.\n");
    }

    return;
}

void do_prompt(int playernum, int governor)
{
    shiptype *s;
    shiptype *s2;

    if (client_can_understand(playernum, governor, CSP_SCOPE_COMMAND)) {
        CSP_prompt(playernum, governor);
    } else {
        if (Dir[playernum - 1][governor].level == LEVEL_UNIV) {
            sprintf(Dir[playernum - 1][governor].prompt,
                    " ( [%d] / )\n",
                    Sdata.AP[playernum - 1]);
        } else if (Dir[playernum - 1][governor].level == LEVEL_STAR) {
            sprintf(Dir[playernum - 1][governor].prompt,
                    " ( [%d] /%s )\n",
                    Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1],
                    Stars[Dir[playernum - 1][governor].snum]->name);
        } else if (Dir[playernum - 1][governor].level == LEVEL_PLAN) {
            sprintf(Dir[playernum - 1][governor].prompt,
                    " ( [%d] /%s/%s )\n",
                    Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1],
                    Stars[Dir[playernum - 1][governor].snum]->name,
                    Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum]);
        } else if (Dir[playernum - 1][governor].level == LEVEL_SHIP) {
            getship(&s, Dir[playernum - 1][governor].shipno);

            switch (s->whatorbits) {
            case LEVEL_UNIV:
                sprintf(Dir[playernum - 1][governor].prompt,
                        " ( [%d] /#%d )\n",
                        Sdata.AP[playernum - 1],
                        Dir[playernum - 1][governor].shipno);

                break;
            case LEVEL_STAR:
                sprintf(Dir[playernum - 1][governor].prompt,
                        " ( [%d] /%s/#%d )\n",
                        Stars[s->storbits]->AP[playernum - 1],
                        Stars[s->storbits]->name,
                        Dir[playernum - 1][governor].shipno);

                break;
            case LEVEL_PLAN:
                sprintf(Dir[playernum - 1][governor].prompt,
                        " ( [%d] /%s/%s/#%d )\n",
                        Stars[s->storbits]->AP[playernum - 1],
                        Stars[s->storbits]->name,
                        Stars[s->storbits]->pnames[Dir[playernum - 1][governor].pnum],
                        Dir[playernum - 1][governor].shipno);

                break;
            case LEVEL_SHIP:
                /*
                 * I put this mess in because of non-functioning prompts when
                 * you are in a ship within a ship, or deeper. I am certain this
                 * can be done more elegantly (a lot more) but I don't feel like
                 * trying that right now. Right now I want it to function.
                 * Maarten
                 */
                getship(&s2, (int)s->destshipno);

                switch (s2->whatorbits) {
                case LEVEL_UNIV:
                    sprintf(Dir[playernum - 1][governor].prompt,
                            " ( [%d] /#%d/#%d )\n",
                            Sdata.AP[playernum - 1],
                            s->destshipno,
                            Dir[playernum - 1][governor].shipno);

                    break;
                case LEVEL_STAR:
                    sprintf(Dir[playernum - 1][governor].prompt,
                            " ( [%d] /%s/#%d/#%d )\n",
                            Stars[s->storbits]->AP[playernum - 1],
                            Stars[s->storbits]->name,
                            s->destshipno,
                            Dir[playernum - 1][governor].shipno);

                    break;
                case LEVEL_PLAN:
                    sprintf(Dir[playernum - 1][governor].prompt,
                            " ( [%d] /%s/%s/#%d/#%d )\n",
                            Stars[s->storbits]->AP[playernum - 1],
                            Stars[s->storbits]->name,
                            Stars[s->storbits]->pnames[Dir[playernum - 1][governor].pnum],
                            s->destshipno,
                            Dir[playernum - 1][governor].shipno);

                    break;
                case LEVEL_SHIP:
                    while (s2->whatorbits == LEVEL_SHIP) {
                        free(s2);
                        getship(&s2, (int)s2->destshipno);
                    }

                    switch (s2->whatorbits) {
                    case LEVEL_UNIV:
                        sprintf(Dir[playernum - 1][governor].prompt,
                                " ( [%d] / /../#%d/#%d )\n",
                                Sdata.AP[playernum - 1],
                                s->destshipno,
                                Dir[playernum - 1][governor].shipno);

                        break;
                    case LEVEL_STAR:
                        sprintf(Dir[playernum - 1][governor].prompt,
                                " ( [%d] /%s /../#%d/#%d )\n",
                                Stars[s->storbits]->AP[playernum - 1],
                                Stars[s->storbits]->name,
                                s->destshipno,
                                Dir[playernum - 1][governor].shipno);

                        break;
                    case LEVEL_PLAN:
                        sprintf(Dir[playernum - 1][governor].prompt,
                                " ( [%d] /%s/%s /../#%d/#%d )\n",
                                Stars[s->storbits]->AP[playernum -1],
                                Stars[s->storbits]->name,
                                Stars[s->storbits]->pnames[Dir[playernum - 1][governor].pnum],
                                s->destshipno,
                                Dir[playernum - 1][governor].shipno);

                        break;
                    default:

                        break;
                    }

                    free(s2);

                    break;
                default:

                    break;
                }
            }

            free(s);
        }
    }
}

void cs(int playernum, int governor, int apcount)
{
    placetype where;
    planettype *planet;
    shiptype *s;
    racetype *race;

    race = races[playernum - 1];

    /* Handle cs used with options */
    if (optn) {
        /* Make new def scope */
        if (!opts['d']) {
            notify(playernum, governor, "Invalid options(s).\n");

            return;
        }

        if (argn == 2) {
            where = Getplace(playernum, governor, args[1], 0);
        } else if (argn == 1) {
            where.level = Dir[playernum - 1][governor].level;
            where.snum = Dir[playernum - 1][governor].snum;
            where.pnum = Dir[playernum - 1][governor].pnum;
            where.shipno = Dir[playernum - 1][governor].shipno;
            where.err = 0;
        } else {
            notify(playernum, governor, "Invalid usage.\n");

            return;
        }

        if (!where.err && (where.level != LEVEL_SHIP)) {
            race->governor[governor].deflevel = where.level;
            race->governor[governor].defsystem = where.snum;
            race->governor[governor].defplanetnum = where.pnum;
            putrace(race);

            sprintf(buf,
                    "New home system is %s\n",
                    Dispplace(playernum, governor, &where));

            notify(playernum, governor, buf);
        } else {
            sprintf(buf, "cs: Bad home system.\n");
            notify(playernum, governor, buf);
        }

        return;
    }

    if (argn == 2) {
        /* chdir to specified scope */
        where = Getplace(playernum, governor, args[1], 0);

        if (where.err) {
            sprintf(buf, "cs: Bad scope.\n");
            notify(playernum, governor, buf);
            Dir[playernum - 1][governor].lasty[0] = 0.0;
            Dir[playernum - 1][governor].lastx[0] = Dir[playernum - 1][governor].lasty[0];

            return;
        }

        /* Fix lastx, lasty coordinates */
        switch (Dir[playernum - 1][governor].level) {
        case LEVEL_UNIV:
            Dir[playernum - 1][governor].lasty[0] = 0.0;
            Dir[playernum - 1][governor].lastx[0] = Dir[playernum - 1][governor].lasty[0];

            break;
        case LEVEL_STAR:
            if (where.level == LEVEL_UNIV) {
                Dir[playernum - 1][governor].lastx[1] = Stars[Dir[playernum - 1][governor].snum]->xpos;
                Dir[playernum - 1][governor].lasty[1] = Stars[Dir[playernum - 1][governor].snum]->ypos;
            } else {
                Dir[playernum - 1][governor].lasty[0] = 0.0;
                Dir[playernum - 1][governor].lastx[0] = Dir[playernum - 1][governor].lasty[0];
            }

            break;
        case LEVEL_PLAN:
            getplanet(&planet,
                      Dir[playernum - 1][governor].snum,
                      Dir[playernum - 1][governor].pnum);

            if ((where.level == LEVEL_STAR)
                && (where.snum == Dir[playernum - 1][governor].snum)) {
                Dir[playernum - 1][governor].lastx[0] = planet->xpos;
                Dir[playernum - 1][governor].lasty[0] = planet->ypos;
            } else if (where.level == LEVEL_UNIV) {
                Dir[playernum - 1][governor].lastx[1] = Stars[Dir[playernum - 1][governor].snum]->xpos + planet->xpos;
                Dir[playernum - 1][governor].lasty[1] = Stars[Dir[playernum - 1][governor].snum]->ypos + planet->ypos;
            } else {
                Dir[playernum - 1][governor].lasty[0] = 0.0;
                Dir[playernum - 1][governor].lastx[0] = Dir[playernum - 1][governor].lasty[0];
            }

            free(planet);

            break;
        case LEVEL_SHIP:
            getship(&s, Dir[playernum - 1][governor].shipno);

            if (!s->docked) {
                switch (where.level) {
                case LEVEL_UNIV:
                    Dir[playernum - 1][governor].lastx[1] = s->xpos;
                    Dir[playernum - 1][governor].lasty[1] = s->ypos;

                    break;
                case LEVEL_STAR:
                    if ((s->whatorbits >= LEVEL_STAR)
                        && (s->storbits == where.snum)) {
                        /* We are going UP from the ship...change last */
                        Dir[playernum - 1][governor].lastx[0] = s->xpos - Stars[s->storbits]->xpos;
                        Dir[playernum - 1][governor].lasty[0] = s->ypos - Stars[s->storbits]->ypos;
                    } else {
                        Dir[playernum - 1][governor].lasty[0] = 0.0;
                        Dir[playernum - 1][governor].lastx[0] = Dir[playernum - 1][governor].lasty[0];
                    }

                    break;
                case LEVEL_PLAN:
                    if ((s->whatorbits == LEVEL_PLAN)
                        && (s->storbits == where.snum)
                        && (s->pnumorbits == where.pnum)) {
                        /* Same */
                        getplanet(&planet, (int)s->storbits, (int)s->pnumorbits);
                        Dir[playernum - 1][governor].lastx[0] = s->xpos - Stars[s->storbits]->xpos - planet->xpos;
                        Dir[playernum - 1][governor].lasty[0] = s->ypos - Stars[s->storbits]->ypos - planet->ypos;

                        free(planet);
                    } else {
                        Dir[playernum - 1][governor].lasty[0] = 0.0;
                        Dir[playernum - 1][governor].lastx[0] = Dir[playernum - 1][governor].lasty[0];
                    }

                    break;
                case LEVEL_SHIP:
                    Dir[playernum - 1][governor].lasty[0] = 0.0;
                    Dir[playernum - 1][governor].lastx[0] = Dir[playernum - 1][governor].lasty[0];

                    break;
                default:

                    break;
                }
            } else {
                Dir[playernum - 1][governor].lasty[0] = 0.0;
                Dir[playernum - 1][governor].lastx[0] = Dir[playernum - 1][governor].lasty[0];
            }

            free(s);

            break;
        default:

            break;
        }

        Dir[playernum - 1][governor].level = where.level;
        Dir[playernum - 1][governor].snum = where.snum;
        Dir[playernum - 1][governor].pnum = where.pnum;
        Dir[playernum - 1][governor].shipno = where.shipno;
    } else if (argn == 1) {
        /* chdir to def scope */
        Dir[playernum - 1][governor].level = race->governor[governor].deflevel;
        Dir[playernum - 1][governor].snum = race->governor[governor].defsystem;

        if (Dir[playernum - 1][governor].snum >= Sdata.numstars) {
            Dir[playernum[governor].snum] = Sdata.numstars - 1;
        }

        Dir[playernum - 1][governor].pnum = race->governor[governor].defplanetnum;

        if (Dir[playernum - 1][governor].pnum >= Stars[Dir[playernum - 1][governor].snum]->numplanets) {
            Dir[playernum - 1][governor].pnum = Stars[Dir[playernum - 1][governor].snum]->numplanets - 1;
        }

        Dir[playernum - 1][governor].shipno = 0;
        Dir[playernum - 1][governor].lasty[0] = 0.0;
        Dir[playernum - 1][governor].lastx[0] = Dir[playernum - 1][governor].lasty[0];
        Dir[playernum - 1][governor].lastx[1] = Stars[Dir[playernum - 1][governor].snum]->xpos;
        Dir[playernum - 1][governor].lasty[1] = Stars[Dir[playernum - 1][governor].snum]->ypos;

        return;
    } else {
        sprintf(buf, "cs: Bad scope.\n");
        notify(playernum, governor, buf);
    }
}
