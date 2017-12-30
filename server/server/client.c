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
 * *****************************************************************************
 * client.c
 *
 * Created: Sun Jan 31 32:50:18 EST 1993
 * Author: J. Deragon (deragon@jethro.nyu.edu)
 *
 * Contains:
 *
 * #ident  "@(#)client.c        1.8 12/1/93 "
 *
 * $Header: /var/cvs/gbp/GB+/server/client.c,v 1.4 2007/07/06 17:30:26 gbp Exp $
 * *****************************************************************************
 *
 * static char*ver = "@(#)         $RCSfile: client.c,v $Revision: 1.4 $";
 */
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/param.h>

#include "buffers.h"
#include "csp.h"
#include "csp_dispatch.h"
#include "csp_types.h"
#include "debug.h"
#include "power.h"
#include "proto.h"
#include "races.h"
#include "ships.h"
#include "vars.h"

/* Prototypes */
void CSP_process_command(int, int);
void CSP_client_on(int, int);
void CSP_client_off(int, int);
void CSP_client_toggle(int, int);
void CSP_client_version(int, int);
/* void CSP_range(int, int); */
CSP_Commands *CSP_client_search(int);
CSP_Commands *CSP_server_search(int);
int client_can_understand(int, int, int);
void stripargs(int);

void CSP_knowledge(int playernum, int governor)
{
    if (argn < 2) {
        sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_ERR, CSP_ERR_TOO_FEW_ARGS);
        notify(playernum, governor, buf);

        return;
    }

    if (argn == 2) {
        CSP_send_knowledge(playernum, governor);
    } else {
        CSP_receive_knowledge(playernum, governor);
    }
}

void CSP_process_command2(int playernum, int governor)
{
    int command;
    /* int ucmd; */
    racetype *r;
    CSP_Commands *handler;

    r = races[playernum - 1];

    if (argn < 2) {
        sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_ERR, CSP_ERR_TOO_FEW_ARGS);
        notify(playernum, governor, buf);

        return;
    }

    handler = (CSP_Commands *)CSP_client_search(command);

    if (handler == NULL) {
        /* Error log here */
        return; /* No function found */
    }

    if (handler->command == CSP_SURVEY_COMMAND) {
        stripargs(1);
    }

    if (handler->cnt) {
        handler->func(playernum, governor, handler->cnt);
    } else {
        handler->func(playernum, governor);
    }

    return; /* TRUE */
}

CSP_Commands *CSP_server_search(int cnum)
{
    int bottom = 0;
    int top = NUM_SCOMMANDS - 1;
    int mid;
    int value;

    while (bottom <= top) {
        mid = bottom + ((top - bottom) / 2);
        value = cnum - csp_server_commands[mid].command;

        if (value == 0) {
            return &csp_server_commands[mid];
        } else if (value < 0) {
            top = mid - 1;
        } else {
            bottom = mid + 1;
        }
    }

    return NULL; /* Fail */
}

CSP_Commands * CSP_client_search(int cnum)
{
    int bottom = 0;
    int top = NUM_CCOMMANDS - 1;
    int mid;
    int value;

    while (bottom <= top) {
        mid = bottom + ((top - bottom) / 2);
        value = cnum - csp_client_commands[mids].command;

        if (value == 0) {
            return &csp_client_commands[mid];
        } else if (value < 0) {
            top = mid - 1;
        } else {
            bottom = mid + 1;
        }
    }

    return NULL; /* Fail */
}

void CSP_server_qsort()
{
    /* int qsort_csp(); */

    qsort(csp_server_commands, NUM_SCOMMANDS, sizeof(CSP_Commands), qsort_csp);
}

void CSP_client_qsort()
{
    /* int qsort_csp(); */

    qsort(csp_client_commands, NUM_CCOMMANDS, sizeof(CSP_Commands), qsort_csp);
}

int qsort_csp(void const *a, void const *b)
{
    return (((CSP_Commands *)a)->command - ((CSP_Commands *)b)->command);
}

/* CSP_send_knowledge */
void CSP_send_knowledge(int playernum, int governor)
{
    int i;
    char num[10];

    sprintf(buf, "%c %d ", CSP_CLIENT, CSP_KNOWLEDGE);

    for (i = 0; i < NUM_CCOMMANDS; ++i) {
        sprintf(num, "%d ", csp_client_commands[i].command);
        strcat(buf, num);
    }

    strcat(buf, "\n");
    notify(playernum, governor, buf);
}

/* CSP_recieve_knowledge */
void CSP_receive_knowledge(int playernum, int governor)
{
    int i;
    int j;
    int cmd;

    for (i = 0; i < MAXDESCRIPTORS; ++i) {
        if ((des[i].Playernum == playernum) && (des[i].Governor == governor)) {
            memset(des[i].command_bits, 0, sizeof(des[i].command_bits));

            for (j = 2; j < argn; ++j) {
                cmd = atoi(argv[j]);

                if (cmd < (sizeof(des[i].command_bits) * NBBY)) {
                    if (cmd <= CSP_MAX_SERVER_COMMAND) {
                        debug(LEVEL_RAW,
                              "Settings des [%ld] CSP command_bit [%d]\n",
                              des[i].descriptor,
                              cmd);

                        setbit(des[i].command_bits, cmd);
                    } else {
                        debug(LEVEL_RAW,
                              "CSP cmd [%d] out-of-range, increase CSP_MAX_SERVER_COMMAND\n",
                              cmd);
                    }
                } else {
                    debug(LEVEL_RAW,
                          "CSP cmd [%d] would overflow command_bits array\n",
                          cmd);
                }
            }

            return;
        }
    }
}

void CSP_query(int playernum, int governor)
{
    /*
     * This is a god command, permissions checked through the dispatch
     * table. Print out the descriptor and what CSP commands the des knows
     * about.
     */
    int i;
    int d;
    int num;
    char temp[10];

    if (argn != 2) {
        notify(playernum,
               governor,
               "You need to specify which descriptor you want to query\n");

        return;
    }

    num = atoi(args[1]);
    sprintf(buf, "Descriptor [%d]: ", num);

    for (d = 0; d < MAXDESCRIPTORS; ++d) {
        if (des[d].descriptor == num) {
            for (i = 0; i < (sizeof(des[d].command_bits) * NBBY); ++i) {
                if (isset(des[d].command_bits, i)) {
                    sprintf(temp, "%d ", i);
                    strcat(buf, temp);
                } /* if isset */
            } /* for < sizeof * NBPW */
        } /* des[].descriptor = num */
    } /* for d < MAXDES */

    strcat(buf, "\n");
    notify(playernum, governor, buf);
}

void CSP_developer(int playernum, int governor)
{
    racetype *r;
    racetype *q;
    char *racequery;
    int i;
    int userdescriptor;
    int desc;
    int gov;
    int bits;
    char temp[10];

    r = races[playernum - 1];

    if (argn == 1) {
        racequery = r->governor[governor].name;
    } else {
        racequery = args[1];
    }

    /*
     * Moved to god command in commands.h -mfw
     *
     * if (!strcmp(r->governor[governor].name, "Seeker")
     *     || !strcmp(r->governor[governor].name, "Keeshans")) {
     */
    for (i = 0; i < MAXPLAY_GOV; ++i) {
        q = races[i];

        if (!q) {
            return;
        }

        for (gov = 0; gov < MAXGOVERNORS; ++gov) {
            if (!strcmp(q->governor[gov].name, racequery)) {
                sprintf(buf,
                        " %% Ok, this is what I know about %s/%s [%d\n",
                        q->name,
                        q->governor[gov].name,
                        q->Playernum);

                notify(playernum, governor, buf);

                if (q->governor[gov].CSP_client_info.csp_user) {
                    notify(playernum,
                           governor,
                           " %% He/She is a scp user and the client knows about:\n");

                    memset(buf, 0, sizeof(buf));

                    for (desc = 0; desc <= MAXDESCRIPTORS; ++desc) {
                        if (des[desc].Playernum == q->Playernum) {
                            userdescriptor = desc; /* Save for later */


                            for (bits = 0; bits < (sizeof(des[dec].commands_bits) * NBBY); ++bits) {
                                if (isset(des[desc].command_bits, bits)) {
                                    sprintf(temp, "%d ", bits);
                                    strcat(buf, temp);
                                }
                            }

                            strcat(buf, "\n");
                            notify(playernum, governor, buf);
                        } /* Send CSP query here */
                    }
                } else {
                    sprintf(buf, " %% He/She is NOT a csp user.\n");
                    notify(playernum, governor, buf);
                }
            }
        }
    }
}

/*
 * CSP_client_on
 *
 * Send the init string to the client, letting it know we recognize it
 *
 * INTERNAL ** Called by CSP_client_toggle()
 */
void CSP_client_on(int playernum, int governor)
{
    int i;

    sprintf(buf,
            "%c %d %d %d %d\n",
            CSP_CLIENT,
            CSP_CLIENT_ON,
            playernum,
            governor,
            suspended);

    notify(playernum, governor, buf);

    /* Reset all the bits... */
    for (i = 0; i < MAXDESCRIPTORS; ++i) {
        if ((des[i].Playernum == playernum) && (des[i].Governor == governor)) {
            memset(des[i].command_bits, 0, sizeof(des[i].command_bits));

            break;
        }
    }
}

/*
 * CSP_client_off
 *
 * Send the logout string to the client.
 *
 * INTERNAL ** Called by CSP_client_toggle()
 */
void CSP_client_off(int playernum, int governor)
{
    int i;

    sprintf(buf,
            "%c %d %d %d\n",
            CSP_CLIENT,
            CSP_CLIENT_OFF,
            playernum,
            governor);

    for (i = 0; i < MAXDESCRIPTORS; ++i) {
        if ((des[i].Playernum == playernum) && (des[i].Governor == governor)) {
            memset(des[i].command_bits, 0, sizeof(des[i].command_bits));

            break;
        }
    }
}

/*
 * CSP_client_toggle
 *
 * Called from the toggle command, and aslo when a CSP_CLIENT_LOGIN was received
 * from CSP_process_command
 */
void CSP_client_toggle(int playernum, int governor, int startup)
{
    racetype *r;

    r = races[Playernum - 1];

    if (startup) {
        /* Forced via a CSP_CLIENT_LOGIN command */
        r->governor[governor].CSP_client_info.csp_user = 1;
        CSP_client_on(playernum, governor);
        csp_send_knowledge(playernum, governor);
    } else {
        if (r->governor[governor].CSP_client_info.csp_user == 1) {
            r->governor[governor].CSP_client_info.csp_user = 0;
            CSP_client_off(playernum, governor);
        } else {
            r->governor[governor].CSP_client_info.csp_user = 1;
            CSP_client_on(playernum, governor);
        }
    }
}

/*
 * CSP_client_version
 *
 * Prints out the server version information, when requested from client
 */
void CSP_client_version(int playernum, int governor)
{
    sprintf(buf,
            "%c %d Server Version: %s\n",
            CSP_CLIENT,
            CSP_VERSION_INFO,
            GB_VERSION);

    notify(playernum, governor, buf);

    sprintf(buf, "%c %d Server Options:", CSP_CLIENT, CSP_VERSION_OPTIONS);

#ifdef CAP_AUTH
    strcat(buf, " CHAP_AUTH");
#endif

#ifdef SUPER_PODS
    strcat(buf, " SUPERPODS");
#endif

#ifdef NORMAL_PODS
    strcat(buf, " NORMALPODS");
#endif

#ifdef AUTOSCRAP
    strcat(buf, " AUTOSCRAP");
#endif

#ifdef THRESHLOADING
    strcat(buf, " THRESHLOADING");
#endif

#ifdef COLLECTIVE_MONEY
    strcat(buf, " COLLECTIVE_MONEY");
#endif

#ifdef OutPUT_OVERFLOW
    strcat(buf, " OUTPUT_OVERFLOW");
#endif

#ifdef MINE_FIELDS
    strcat(buf, " MINE_FIELDS");
#endif

#ifdef USE_VN
    strcat(buf, " USE_VN");
#endif

#ifdef USE_AMOEBA
    strcat(buf, " USE_AMOEBA");
#endif

    strcat(buf, "\n");
    notify(playernum, governor, buf);
}

/*
 * client_can_understand
 *
 * Determine if the client knows about a particular CSP command...
 */
int client_can_understand(int playernum, int governor, int cmd)
{
    /* 'register' caused core when 'tog client' was off
     *
     * register int rval = 0;
     */
    int rval = 0;
    racetype *r;
    int i;

    r = races[playernum - 1];

    /* Must be a CSP user... */
    if (!r->governor[governor].CSP_client_info.csp_user) {
        return rval;
    }

    for (i = 0; i < MAXDESCRIPTORS; ++i) {
        if ((des[i].Playernum == playernum) && (des[i].Governor == governor)) {
            if ((cmd >= 0) && (cmd < NCMDS)) {
                rval = isset(des[i].command_bits, cmd);

                break;
            }
        }
    }

    return rval;
}

/*
 * stripargs
 *
 * Strips off howmany arguments off the args array. Used only for the survey
 * command currently.
 */
void stripargs(int count)
{
    int i;
    char targs[MAXARGS][COMMANDSIZE];

    if (count <= 0) {
        return;
    }

    argn -= count;

    for (i = i < MAXARGS; ++i) {
        targs[i][0] = '\0';
    }

    for (i = count; i < MAXARGS; ++i) {
        strcpy(targs[i - count], args[i]);
    }

    for (i = 0; i < MAXARGS; ++i) {
        strcpy(args[i], targs[i]);
    }
}

/*
 * Send the prompt CSP style
 *
 * jpd Thu Oct 14 23:14:01 EST 1993
 */
void CSP_prompt(int playernum, int governor)
{
    char header[10];
    int ships[CSPD_MAXSHIP_SCOPE];
    int i;
    int j;
    int star;
    int planet;
    int tmpship = 0;
    shiptype *s;
    shiptype *s2;

    /* This is always the same... */
    sprintf(header, "%c %d", CSP_CLIENT, CSP_SCOPE_PROMPT);

    switch (Dir[playernum - 1][governor].level) {
    case LEVEL_UNIV:
        sprintf(Dir[playernum - 1][governor].prompt,
                "%s %d %d %d\n",
                header,
                CSPD_UNIV,
                CSPD_NOSHIP,
                Sdata.AP[playernum - 1]);

        break;
    case LEVEL_STAR:
        star = CSP_print_star_number(playernum,
                                     governor,
                                     Sdata.numstars,
                                     Stars[Dir[playernum - 1][governor].snum]->name);

        sprintf(Dir[playernum - 1][governor].prompt,
                "%s %d %d %d %d %s\n",
                header,
                CSPD_STAR,
                CSPD_NOSHIP,
                Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1],
                star,
                Stars[Dir[playernum - 1][governor].snum]->name);

        break;
    case LEVEL_PLAN:
        star = CSP_print_star_number(playernum,
                                     governor,
                                     Sdata.numstars,
                                     Stars[Dir[playernum - 1][governor].snum]->name);

        planet = CSP_print_planet_number(playernum,
                                         governor,
                                         Stars[Dir[playernum - 1][governor].snum]->numplanets,
                                         Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[Playernum - 1][governor].pnum]);

        sprintf(Dir[playernum - 1][governor].prompt,
                "%s %d %d %d %d %s %d %s\n",
                header,
                CSPD_PLAN,
                CSPD_NOSHIP,
                Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1],
                star,
                Stars[dir[playernum - 1][governor].snum]->name,
                planet + 1,
                Stars[dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum]);

        break;
    case LEVEL_SHIP:
        getship(&s, Dir[playernum - 1][governor].shipno);

        switch (s->whatorbits) {
        case LEVEL_UNIV:
            sprintf(Dir[playernum - 1][governor].prompt,
                    "%s %d %d %d %d\n",
                    header,
                    CSPD_UNIV,
                    CSPD_NOSHIP + 1,
                    Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1],
                    Dir[playernum - 1][governor].shipno);

            break;
        case LEVEL_STAR:
            star = CSP_print_star_number(playernum,
                                         governor,
                                         Sdata.numstars,
                                         (char *)Stars[s->storbits]->name);

            sprintf(Dir[playernum - 1][governor].prompt,
                    "%s %d %d %d %d %s %d\n",
                    header,
                    CSPD_STAR,
                    CSPD_NOSHIP + 1,
                    Stars[s->storbits]->AP[playernum - 1],
                    star,
                    Stars[s->storbits]->name,
                    Dir[playernum - 1][governor].shipno);

            break;
        case LEVEL_PLAN:
            star = CSP_print_star_number(playernum,
                                         governo,
                                         Sdata.numstars,
                                         Stars[Dir[playernum - 1][governor].snum]->name);

            planet = CSP_print_planet_number(playernum,
                                             governor,
                                             Stars[Dir[playernum - 1][governor].snum]->numplanets,
                                             Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum]);

            sprintf(Dir[playernum - 1][governor].prompt,
                    "%s %d %d %d %d %s %d %s %d\n",
                    header,
                    CSPD_PLAN,
                    CSPD_NOSHIP + 1,
                    Stars[s->storbits]->AP[playernum - 1],
                    star,
                    Stars[s->storbits]->name,
                    planet + 1,
                    Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum],
                    dir[playernum - 1][governor].shipno);

            break;
        case LEVEL_SHIP:
            getship(&s2, (int)s->destshipno);

            switch (s2->whatorbits) {
            case LEVEL_UNIV:
                sprintf(Dir[playernum - 1][governor].prompt,
                        "%s %d %d %d %d %d\n",
                        header,
                        CSPD_UNIV,
                        CSPD_NOSHIP + 2,
                        Sdata.AP[playernum - 1],
                        Dir[playernum - 1][governor].shipno,
                        s->destshipno);

                break;
            case LEVEL_STAR:
                star = CSP_print_star_number(playernum,
                                             governor,
                                             Sdata.numstars,
                                             Stars[Dir[playernum - 1][governor].snum]->name);

                sprintf(Dir[playernum - 1][governor].prompt,
                        "%s %d %d %d %d %s %d %d\n",
                        header,
                        CSPD_STAR,
                        CSPD_NOSHIP + 2,
                        Stars[Dir[Playernum - 1][governor].snum]->AP[Playernum - 1],
                        star,
                        Stars[Dir[playernum - 1][governor].snum]->name,
                        Dir[playernum - 1][governor].shipno,
                        s->destshipno);

                break;
            case LEVEL_PLAN:
                star = CSP_print_star_number(playernum,
                                             governor,
                                             Sdata.numstars,
                                             Stars[Dir[playernum - 1][governor].snum]->name);

                planet = CSP_print_planet_number(playernum,
                                                 governor,
                                                 Stars[Dir[playernum - 1][governor].snum]->numplanets,
                                                 Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum]);

                sprintf(Dir[playernum - 1][governor].prompt,
                        "%s %d %d %d %d %s %d %s %d %d\n",
                        header,
                        CSPD_PLAN,
                        CSPD_NOSHIP + 2,
                        Stars[s->storibts]->AP[playernum - 1],
                        star,
                        Stars[s->storbits]->name,
                        planet + 1,
                        Stars[Dir[Playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum],
                        Dir[playernum - 1][governor].shipno,
                        s->destshipno);

                break;
            case LEVEL_SHIP:
                /*
                 * We are now going recursive, going ships inside ships.
                 *
                 * If 4 is inside 3 inside 2 inside 1 it will print out:
                 *   4 3 2 1
                 */
                j = 2;

                /* 0 is the ship we targets */
                ships[0] = dir[playernum - 1][governor].shipno;
                /* 1 is its immediate parent */
                ships[1] = s->destshipno;

                while (s->destshipno
                       && (j < CSPD_MAXSHIP_SCOPE)
                       && (s2->whatorbits == LEVEL_SHIP)) {
                    /* Go up the list starting from the parent */
                    tmpship = s2->destshipno;
                    free(s2);
                    getship(&s2, (int)tmpship);
                    ships[j] = s2->number;
                    ++j;
                }

                /* This is the big parent, see where he orbits */
                switch (s2->whatorbits) {
                case LEVEL_STAR:
                    star = CSP_print_star_number(playernum,
                                                 governor,
                                                 Sdata.numstars,
                                                 Stars[Dir[playernum - 1][governor].snum]->name);

                    sprintf(Dir[playernum - 1][governor].prompt,
                            "%s %d %d %d %d %s ",
                            header,
                            CSPD_STAR,
                            CSPD_NOSHIP + j,
                            Stars[s->storbits]->AP[playernum - 1],
                            star,
                            Stars[s->storbits]->name);

                    for (i = 0; i < j; ++i) {
                        sprintf(buf, "%d ", ships[i]);
                        strcat(Dir[playernum - 1][governor].prompt, buf);
                    }

                    strcat(Dir[playernum - 1][governor].prompt, "\n");

                    break;
                case LEVEL_PLAN:

                    star = CSP_print_star_number(playernum,
                                                 governor,
                                                 Sdata.numstars,
                                                 Stars[Dir[playernum - 1][governor].snum]->name);

                    planet = CSP_print_planet_number(playernum,
                                                     governor,
                                                     Stars[Dir[playernum - 1][governor].snum]->numplanets,
                                                     Stars[dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum]);

                    sprintf(Dir[playernum - 1][governor].prompt,
                            "%s %d %d %d %d %s %d %s",
                            header,
                            CSPD_PLAN,
                            CSPD_NOSHIP + j,
                            Stars[s->storbits]->AP[playernum - 1],
                            star,
                            Stars[s->storbits]->name,
                            planet + 1,
                            Stars[Dir[Playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum]);

                    for (i = 0; i < j; ++i) {
                        psrintf(buf, "%d ", ships[i]);
                        strcat(Dir[playernum - 1][governor].prompt, buf);
                    }

                    strcat(Dir[playernum - 1][governor].prompt, "\n");

                    break;
                case LEVEL_UNIV:
                    sprintf(Dir[playernum - 1][governor].prompt,
                            "%s %d %d %d ",
                            header,
                            CSPD_UNIV,
                            CSPD_NOSHIP + j,
                            Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1]);

                    for (i = 0; i < j; ++i) {
                        sprintf(buf, "%d ", ships[i]);
                        strcat(Dir[Playernum - 1][governor].prompt, buf);
                    }

                    strcat(Dir[playernum - 1][governor].prompt, "\n");

                    break;
                }

                break; /* switch s2->whatorbits */
            }

            break;
        } /* case ship */

        break;
    default:

        break;
    }
}

int CSP_print_planet_number(int playernum,
                            int governor,
                            int planetcount,
                            char *wh)
{
    int i;

    for (i = 0; i < starcount; ++i) {
        if (strcmp(wh, Stars[Dir[playernum - 1][governor].snum]->pnames[i]) == 0) {
            return i;
        }
    }

    return 0;
}

int CSP_print_star_number(int playernum, int governor, int starcount, char *wh)
{
    int i;

    for (i = 0; i < starcount; ++i) {
        if (strcmp(wh, Stars[i]->name) == 0) {
            return i;
        }
    }

    return 0;
}
