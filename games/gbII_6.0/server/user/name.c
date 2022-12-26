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
 * name.c -- Rename something to something else
 * announce.c -- Make announcements in the system you are currently int. You
 *               must be inhabiting that system for your message to be sent. You
 *               must also be in that system (and inhabiting) to receive
 *               announcements.
 * page.c -- Send a message to a player requesting his presence in a system.
 *
 * #ident  "@(#)name.c        1.13 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/name.c,v 1.6 2007/07/06 18:09:34 gbp Exp $
 */
#include "name.h"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../server/buffers.h"
#include "../server/config.h"
#include "../server/dispatch.h"
#include "../server/files_shl.h"
#include "../server/GB_server.h"
#include "../server/getplace.h"
#include "../server/lists.h"
#include "../server/max.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/rand.h"
#include "../server/ranks.h"
#include "../server/ships.h"
#include "../server/shlmisc.h"
#include "../server/tweakables.h"
#include "../server/vars.h"

#include "capture.h"
#include "dissolve.h"
#include "tele.h"

static char msg[1024];
static char head[1024];

/* For watching next update */
/* static struct tm *current_tm; */
/* static long clk; */

extern void personal(int, int, char *);
extern void motto(int, int, int, char *);
extern int MostAPs(int, startype *);
extern void announce(int, int, char *, int, int);
extern char *garble_msg(char *, int, int, int);

/* Garble code variables from HAP by CWL -mfw */
#define BUFFER_LEN ((MAX_COMMAND_LEN) * 8)

extern int chat_flag;
extern int chat_static;
extern long size_of_words;
extern FILE *garble_file;

/* Buffer used by garble */
char gbuf[BUFFER_LEN];

/* end CWL */

#define mypunct(c) index("[]<>;:,.?!", (c))

void personal(int playernum, int governor, char *message)
{
    racetype *race;

    race = races[playernum - 1];
    strncpy(race->info, message, PERSONALSIZE - 1);
    putrace(race);
}

void bless(int playernum, int governor, int apcount, int unused3, orbitinfo *unused4)
{
    planettype *planet;
    racetype *race;
    int who;
    int amount;
    int mod;
    char commod;

    race = races[playernum - 1];

    if (!race->God || (playernum != 1)) {
        notify(playernum,
               governor,
               "You are not privileged to use this command.\n");

        return;
    }

    if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
        notify(playernum, governor, "Please cs to the planet in question.\n");

        return;
    }

    who = atoi(args[1]);

    if ((who < 1) || (who > Num_races)) {
        notify(playernum, governor, "No such player number.\n");

        return;
    }

    if (argn < 3) {
        notify(playernum,
               governor,
               "Syntax: bless <player> <what> <+amount>\n");

        return;
    }

    amount = atoi(args[3]);
    race = races[who - 1];

    /* Race characteristics? */
    mod = 1;

    if (match(args[2], "money")) {
        MONEY(race, 0) += amount;
        sprintf(buf, "deity gave you %d money.\n", amount);
    } else if (match(args[2], "password")) {
        strcat(race->password, args[3]);
        sprintf(buf, "Deity changed your race password to '%s'\n", args[3]);
    } else if (match(args[2], "morale")) {
        race->morale += amount;
        sprintf(buf, "Deity gave you %d morale.\n", amount);
    } else if (match(args[2], "pods")) {
        race->pods = 1;
        sprintf(buf, "Deity gave you pod ability.\n");
    } else if (match(args[2], "nopods")) {
        race->pods = 0;
        sprintf(buf, "Deity took away pod ability.\n");
    } else if (match(args[2], "collectiveiq")) {
        race->collective_iq = 1;
        sprintf(buf, "Deity gave you collective intelligence.\n");
    } else if (match(args[2], "nocollectiveiq")) {
        race->collective_iq = 0;
        sprintf(buf, "Deity took away collective intelligence.\n");
    } else if (match(args[2], "maxiq")) {
        race->IQ_limit = atoi(args[3]);
        sprintf(buf, "Deity gave you a maximum IQ of %d.\n", race->IQ_limit);
    } else if (match(args[2], "mass")) {
        race->mass = atof(args[3]);
        sprintf(buf, "Deity gave you %.2f mass.\n", race->mass);
    } else if (match(args[2], "metabolism")) {
        race->metabolism = atof(args[3]);
        sprintf(buf, "Deity gave you %.2f metabolism.\n", race->metabolism);
    } else if (match(args[2], "adventurism")) {
        race->adventurism = atof(args[3]);
        sprintf(buf,
                "Deity gave you %-3.0f%% adventurism.\n",
                race->adventurism * 100.0);
    } else if (match(args[2], "birthrate")) {
        race->birthrate = atof(args[3]);
        sprintf(buf, "Deity gave you %.2f birthrate.\n", race->birthrate);
    } else if (match(args[2], "fertility")) {
        race->fertilize = amount;
        sprintf(buf, "Deity gave you a fertilization ability of %d.\n", amount);
    } else if (match(args[2], "IQ")) {
        race->IQ = amount;
        sprintf(buf, "Deity ave you %d IQ.\n", amount);
    } else if (match(args[2], "fight")) {
        race->fighters = amount;
        sprintf(buf, "Deity set your fighting ability to %d.\n", amount);
    } else if (match(args[2], "technology")) {
        race->tech += (double)amount;
        sprintf(buf, "Deity gave you %d technology.\n", amount);
    } else if (match(args[2], "guest")) {
        race->Guest = 1;
        sprintf(buf, "Deity turned you into a guest race.\n");
    } else if (match(args[2], "god")) {
        race->God = 1;
        sprintf(buf, "Deity turned you into a deity race.\n");
    } else if (match(args[2], "mortal")) {
        race->God = 0;
        race->Guest = 0;
        sprintf(buf, "Deity turned you into a mortal race.\n");
    } else if (match(args[2], "water")) { /* Sector preferences */
        race->likes[SEA] = 0.01 * (double) amount;
        sprintf(buf, "Deity set your water preference to %d%%\n", amount);
    } else if (match(args[2], "land")) {
        race->likes[LAND] = 0.01 * (double)amount;
        sprintf(buf, "Deity set your land preference to %d%%\n", amount);
    } else if (match(args[2], "mountain")) {
        race->likes[MOUNT] = 0.01 * (double)amount;
        sprintf(buf, "Deity set your mountain preference to %d%%\n", amount);
    } else if (match(args[2], "gas")) {
        race->likes[GAS] = 0.01 * (double)amount;
        sprintf(buf, "Deity set your gas preference to %d%%\n", amount);
    } else if (match(args[2], "ice")) {
        race->likes[ICE] = 0.01 * (double)amount;
        sprintf(buf, "Deity set your ice preference to %d%%\n", amount);
    } else if (match(args[2], "forest")) {
        race->likes[FOREST] = 0.01 * (double)amount;
        sprintf(buf, "Deity set your forest preference to %d%%\n", amount);
    } else if (match(args[2], "desert")) {
        race->likes[DESERT] = 0.01 * (double)amount;
        sprintf(buf, "Deity set your desert preference to %d%%\n", amount);
    } else if (match(args[2], "plated")) {
        race->likes[PLATED] = 0.01 * (double)amount;
        sprintf(buf, "Deity set your plated preference to %d%%\n", amount);
    } else if (match(args[2], "activate")) {
        if ((amount >= 0) && (amount <= 5)) {
            race->governor[amount].active = 1;
            sprintf(buf, "Deity activated governor %d\n", amount);
        }
    } else if (match(args[2], "deactivate")) {
        if ((amount >= 0) && (amount <= 5)) {
            race->governor[amount].active = 0;
            sprintf(buf, "Deity deactivated governor %d\n", amount);
        }
    } else if (match(args[2], "homesystem")) {
        race->governor[0].homesystem = amount;
        sprintf(buf, "Deity set home system.\n");
    } else if (match(args[2], "homeplanet")) {
        race->governor[0].homeplanetnum = amount;
        sprintf(buf, "Deity set home planet.\n");
    } else if (match(args[2], "dissolve")) {
        race->dissolved = 1;
        sprintf(buf, "Deity dissolved race.\n");
    } else {
        mod = 0;
    }

    if (mod) {
        putrace(race);
        warn(who, 0, buf);

        return;
    }

    /* Ok, must be the planet then */
    commod = args[2][0];

    getplanet(&planet,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    if (match(args[2], "explorebit")) {
        planet->info[who - 1].explored = 1;

        getstar(&Stars[Dir[playernum - 1][governor].snum],
                Dir[playernum - 1][governor].snum);

        setbit(Stars[Dir[playernum - 1][governor].snum]->explored, who);

        putstar(Stars[Dir[playernum - 1][governor].snum],
                Dir[playernum - 1][governor].snum);

        sprintf(buf,
                "Deity set your explored bit at /%s/%s.\n",
                Stars[Dir[playernum - 1][governor].snum]->name,
                Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum]);
    } else if (match(args[2], "noexploredbit")) {
        planet->info[who - 1].explored = 0;

        sprintf(buf,
                "Deity reset your explored bit at /%s/%s.\n",
                Stars[Dir[playernum - 1][governor].snum]->name,
                Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum]);

        if (match(args[3], "system")) {
            clrbit(Stars[Dir[playernum - 1][governor].snum]->explored, who);
        }
    } else if (match(args[2], "planetpopulation"))  {
        long was;
        long now;

        was = planet->info[who - 1].popn;
        now = atoi(args[3]);
        planet->info[who - 1].popn = now;
        planet->popn += (now - was);

        sprintf(buf,
                "Deity set your population variable to %ld at /%s/%s.\n",
                planet->info[who - 1].popn,
                Stars[Dir[playernum - 1][governor].snum]->name,
                Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum]);
    } else if (match(args[2], "inhabited")) {
        getstar(&Stars[Dir[playernum - 1][governor].snum],
                Dir[playernum - 1][governor].snum);

        setbit(Stars[Dir[playernum - 1][governor].snum]->inhabited, playernum);

        putstar(Stars[Dir[playernum - 1][governor].snum],
                Dir[playernum - 1][governor].snum);

        sprintf(buf,
                "Deity has set your inhabited bit for /%s/%s.\n",
                Stars[Dir[playernum - 1][governor].snum]->name,
                Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum]);
    } else if (match(args[2], "numsectsowned")) {
        planet->info[who - 1].numsectsowned = atoi(args[3]);

        sprintf(buf,
                "Deity set your \"numsectsowned\" variable at /%s/%s to %d.\n",
                Stars[Dir[playernum - 1][governor].snum]->name,
                Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum],
                planet->info[who - 1].numsectsowned);
    } else {
        switch (commod) {
        case 'r':
            planet->info[who - 1].resource += amount;

            sprintf(buf,
                    "Deity gave you %d resources at /%s/%s.\n",
                    amount,
                    Stars[Dir[playernum - 1][governor].snum]->name,
                    Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum]);

            break;
        case 'd':
            planet->info[who - 1].destruct += amount;

            sprintf(buf,
                    "Deity gave you %d destruct at /%s/%s.\n",
                    amount,
                    Stars[Dir[playernum - 1][governor].snum]->name,
                    Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum]);

            break;
        case 'f':
            planet->info[who - 1].fuel += amount;

            sprintf(buf,
                    "Deity gave you %d fuel at /%s/%s.\n",
                    amount,
                    Stars[Dir[playernum - 1][governor].snum]->name,
                    Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum]);

            break;
        case 'x':
            planet->info[who - 1].crystals += amount;

            sprintf(buf,
                    "Deity gave you %d crystals at /%s/%s.\n",
                    amount,
                    Stars[Dir[playernum - 1][governor].snum]->name,
                    Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum]);

            break;
        case 'a':
            getstar(&Stars[Dir[playernum - 1][governor].snum],
                    Dir[playernum - 1][governor].snum);

            Stars[Dir[playernum - 1][governor].snum]->AP[who - 1] += amount;

            putstar(Stars[Dir[playernum - 1][governor].snum],
                    Dir[playernum - 1][governor].snum);

            sprintf(buf,
                    "Deity gave you %d action points at %s.\n",
                    amount,
                    Stars[Dir[playernum - 1][governor].snum]->name);

            break;
        default:
            notify(playernum, governor, "No such commodity.\n");
            free(planet);

            return;
        }
    }

    putplanet(planet,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    warn_race(who, buf);
    free(planet);
}

void insurgency(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    int who;
    int amount;
    int eligible;
    int them = 0;
    racetype *race;
    racetype *alien;
    planettype *p;
    double x;
    int changed_hands;
    int chance;
    int i;

    if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
        notify(playernum,
               governor,
               "You must 'cs' to the planet you wish to try it on.\n");

        return;
    }

    /*
     * if (argn < 3) {
     *     notify(playernum,
     *            governor,
     *            "The correct syntax is 'insurgency <race> <money>'\n");
     *
     *     return;
     * }
     */

    if (!enufAP(playernum, governor, Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1], apcount)) {
        return;
    }

    who = GetPlayer(args[1]);

    if (!who) {
        sprintf(buf, "No such player.\n");
        notify(playernum, governor, buf);

        return;
    }

    race = races[playernum - 1];
    alien = races[who - 1];

    if (alien->Guest) {
        notify(playernum, governor, "Don't be such a dickweed.\n");

        return;
    }

    if (who == playernum) {
        notify(playernum, governor, "You can't revolt against yourself!\n");

        return;
    }

    eligible = 0;
    them = 0;

    for (i = 0; i < Stars[Dir[playernum - 1][governor].snum]->numplanets; ++i) {
        getplanet(&p, Dir[playernum - 1][governor].snum, i);
        eligible += p->info[playernum - 1].popn;
        them += p->info[who - 1].popn;
        free(p);
    }

    if (!eligible) {
        notify(playernum,
               governor,
               "You must have population in the star system to attempt insurgency.\n");

        return;
    }

    getplanet(&p,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    if (!p->info[who - 1].popn) {
        notify(playernum,
               governor,
               "This player does not occupy this planet.\n");

        free(p);

        return;
    }

    sscanf(args[2], "%d", &amount);

    if (amount < 0) {
        notify(playernum,
               governor,
               "You have to use a positive amount of money.\n");

        free(p);

        return;
    }

    if (MONEY(race, governor) < amount) {
        notify(playernum, governor, "Nice try.\n");
        free(p);

        return;
    }

    /*
     * HUT Gardan 11.2.1997
     *
     * x = eligible / (eligible + them);
     * x *= ((0.8 * (double)p->info[who - 1].tax) + (0.2 * (them / (them + 2000))));
     * x *= (((double)amount * 2) / (them + (double)amount));
     * x *= ((double)amount / ((double)amount + 4000));
     * chance = x * 100.0;
     */

    /* old code */
    x = (INSURG_FACTOR * (double)amount * (double)p->info[who - 1].tax) / (double)p->info[who - 1].popn;
    x *= morale_factor((double)(race->morale - alien->morale));
    x *= morale_factor((double)(eligible - them) / 50.0);
    x *= morale_factor(10.0 * (double)((race->fighters * p->info[playernum - 1].troops) - (alien->fighters * p->info[who - 1].troops))) / 50.0;

    sprintf(buf, "x = %f\n", x);
    notify(playernum, governor, buf);
    chance = round_rand((200.0 * atan((double)x)) / 3.14159265);

    sprintf(long_buf,
            "/%s/%s: %s [%d] tries insurgency vs %s [%d]\n",
            Stars[Dir[playernum - 1][governor].snum]->name,
            Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum],
            race->name,
            playernum,
            alien->name,
            who);

    sprintf(buf,
            "\t%s: %d total civs [%d]  opposing %d total civs [%d\n",
            Stars[Dir[playernum - 1][governor].snum]->name,
            eligible,
            playernum,
            them,
            who);

    strcat(long_buf, buf);

    sprintf(buf,
            "\t\t %ld morale [%d] vs %ld morale [%d]\n",
            race->morale,
            playernum,
            alien->morale,
            who);

    strcat(long_buf, buf);

    sprintf(buf,
            "\t\t %d money against %ld population at tax rate %d%%\n",
            amount,
            p->info[who - 1].popn,
            p->info[who - 1].tax);

    strcat(long_buf, buf);
    sprintf(buf, "Success chance is %d%%\n", chance);
    strcat(long_buf, buf);

    if (success(chance)) {
        changed_hands = revolt(p, who, playernum);
        notify(playernum, governor, long_buf);

        if (changed_hands == 1) {
            sprintf(buf, "Success! You liberate %d sector.\n", changed_hands);
        } else {
            sprintf(buf, "Success! You liberate %d sectors.\n", changed_hands);
        }

        notify(playernum, governor, buf);

        if (changed_hands == 1) {
            sprintf(buf,
                    "A revolt on /%s/%s instigated by %s [%d] costs you %d sector\n",
                    Stars[Dir[playernum - 1][governor].snum]->name,
                    Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum],
                    race->name,
                    playernum,
                    changed_hands);
        } else {
            sprintf(buf,
                    "A revolt on /%s/%s instigated by %s [%d] costs you %d sectors\n",
                    Stars[Dir[playernum - 1][governor].snum]->name,
                    Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum],
                    race->name,
                    playernum,
                    changed_hands);
        }

        strcat(long_buf, buf);

        warn(who,
             (int)Stars[Dir[playernum - 1][governor].snum]->governor[who - 1],
             long_buf);

        p->info[playernum - 1].tax = p->info[who - 1].tax;

        /* You inherit their tax rate (insurgency wars hehe) */
        sprintf(buf,
                "/%s/%s: Successful insurgency by %s [%d] against %s [%d]\n",
                Stars[Dir[playernum - 1][governor].snum]->name,
                Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum],
                race->name,
                playernum,
                alien->name,
                who);

        post(buf, DECLARATION);
    } else {
        notify(playernum, governor, long_buf);
        notify(playernum, governor, "The insurgency failed!\n");

        sprintf(buf,
                "A revolt on /%s/%s instigated by %s [%d] fails\n",
                Stars[Dir[playernum - 1][governor].snum]->name,
                Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum],
                race->name,
                playernum);

        strcat(long_buf, buf);

        warn(who,
             (int)Stars[Dir[playernum - 1][governor].snum]->governor[who - 1],
             long_buf);

        sprintf(buf,
                "/%s/%s: Failed insurgency by %s [%d] against %s [%d]\n",
                Stars[Dir[playernum - 1][governor].snum]->name,
                Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum],
                race->name,
                playernum,
                alien->name,
                who);

        post(buf, DECLARATION);
    }

    deductAPs(playernum,
              governor,
              apcount,
              Dir[playernum - 1][governor].snum,
              0);

    MONEY(race, governor) -= amount;
    putrace(race);
    free(p);
}

void pay(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    int who;
    int amount;
    racetype *race;
    racetype *alien;

    who = GetPlayer(args[1]);

    if (!who) {
        sprintf(buf, "No such player.\n");
        notify(playernum, governor, buf);

        return;
    }

    if (!enufAP(playernum, governor, Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1], apcount)) {
        return;
    }

    race = races[playernum - 1];
    alien = races[who - 1];

    sscanf(args[2], "%d", &amount);

    if (amount < 0) {
        notify(playernum,
               governor,
               "You have to give a player a positive amount of money.\n");

        return;
    }

    if (race->Guest) {
        notify(playernum,
               governor,
               "Nice try. Your attempt has been duly noted.\n");

        return;
    }

    if (MONEY(race, governor) < amount) {
        notify(playernum,
               governor,
               "You don't have that much money to give!\n");

        return;
    }

    MONEY(race, governor) -= amount;
    MONEY(alien, 0) += amount;
    sprintf(buf, "%s [%d] payed you %d.\n", race->name, playernum, amount);
    warn(who, 0, buf);
    sprintf(buf, "%d payed to %s [%d].\n", amount, alien->name, who);
    notify(playernum, governor, buf);

    sprintf(buf,
            "%s [%d] pays %s [%d].\n",
            race->name,
            playernum,
            alien->name,
            who);

    post(buf, TRANSFER);
    deductAPs(playernum,
              governor,
              apcount,
              Dir[playernum - 1][governor].snum,
              0);

    putrace(alien);
    putrace(race);
}

void give(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    int who;
    int sh;
    shiptype *ship;
    planettype *planet;
    racetype *race;
    racetype *alien;

    who = GetPlayer(args[1]);

    if (!who) {
        sprintf(buf, "No such player.\n");
        notify(playernum, governor, buf);

        return;
    }

    alien = races[who - 1];
    race = races[playernum - 1];

    if (alien->Guest && !race->God) {
        notify(playernum, governor, "You can't give this player anything.\n");

        return;
    }

    if (race->Guest) {
        notify(playernum, governor, "You can't give anyone anything.\n");

        return;
    }

    /* Check to see if both players are mutually allied */
    if (!race->God
        && (!isset(race->allied, who)) && isset(race->allied, playernum)) {
        notify(playernum, governor, "You two are not mutually allied\n");

        return;
    }

    if (args[2][0] == '#') {
        sscanf(args[2] + 1, "%d", &sh);
    } else {
        sscanf(args[2], "%d", &sh);
    }

    if (!getship(&ship, sh)) {
        notify(playernum, governor, "Illegal ship number.\n");

        return;
    }

    if ((ship->owner != playernum) || !ship->alive) {
        DontOwnErr(playernum, governor, sh);
        free(ship);

        return;
    }

    /* HUTm (kse) Ships can be given only in current scope */
    if (((Dir[playernum - 1][governor].level == LEVEL_UNIV)
         && (ship->whatorbits != LEVEL_UNIV))
        || (Dir[playernum - 1][governor].snum != ship->storbits)) {
        notify(playernum,
               governor,
               "You cannot give ship from different system.\n");

        free(ship);

        return;
    }

    if (ship->type) {
        notify(playernum,
               governor,
               "You cannot change the ownership of spore pods.\n");

        free(ship);

        return;
    }

    if ((ship->popn + ship->troops) && !race->God) {
        notify(playernum,
               governor,
               "You can't give this ship away while it has crew/mil on board.\n");

        free(ship);

        return;
    }

    if (ship->ships && !race->God) {
        notify(playernum,
               governor,
               "You can't give away this ship, it has other ships loaded on it.\n");

        free(ship);

        return;
    }

    switch (ship->whatorbits) {
    case LEVEL_UNIV:
        if (!enufAP(playernum, governor, Sdata.AP[playernum - 1], apcount)) {
            free(ship);

            return;
        }

        break;
    default:
        if (!enufAP(playernum, governor, Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1], apcount)) {
            free(ship);

            return;
        }

        break;
    }

    /* Remove its fleet association -mfw */
    if (ship->fleetmember || ship->nextinfleet) {
        remove_sh_fleet(playernum, governor, ship);
    }

    ship->owner = who;
    /* Give to the leader */
    ship->governor = 0;
    capture_stuff(ship);

    putship(ship);

    /* Set inhabited/explored bits */
    switch (ship->whatorbits) {
    case LEVEL_UNIV:

        break;
    case LEVEL_STAR:
        getstar(&Stars[ship->storbits], (int)ship->storbits);
        setbit(Stars[ship->storbits]->explored, who);
        putstar(Stars[ship->storbits], (int)ship->storbits);

        break;
    case LEVEL_PLAN:
        getstar(&Stars[ship->storbits], (int)ship->storbits);
        setbit(Stars[ship->storbits]->explored, who);
        putstar(Stars[ship->storbits], (int)ship->storbits);

        getplanet(&planet, (int)ship->storbits, (int)ship->pnumorbits);
        planet->info[who - 1].explored = 1;
        putplanet(planet, (int)ship->storbits, (int)ship->pnumorbits);
        free(planet);

        break;
    default:
        notify(playernum,
               governor,
               "Something wrong with this ship's scope.\n");

        free(ship);

        return;
    }

    switch (ship->whatorbits) {
    case LEVEL_UNIV:
        deductAPs(playernum, governor, apcount, 0, 1);
        free(ship);

        return;
    default:
        deductAPs(playernum,
                  governor,
                  apcount,
                  Dir[playernum - 1][governor].snum,
                  0);

        break;
    }

    notify(playernum, governor, "Owner changed.\n");

    sprintf(buf,
            "%s [%d] gave you %s at %s.\n",
            race->name,
            playernum,
            Ship(ship),
            prin_ship_orbits(ship));

    warn(who, 0, buf);

    if (!race->God) {
        sprintf(buf,
                "%s [%d] gives %s [%d] a ship.\n",
                race->name,
                playernum,
                alien->name,
                who);

        post(buf, TRANSFER);
        free(ship);
    }
}

void page(int playernum, int governor, int apcount0, int unused4, orbitinfo *unused5)
{
    int i;
    int who = -1;
    int gov = -1;
    int to_block;
    int dummy[2];
    int apcount;
    racetype *race;
    racetype *alien;

    apcount = apcount0;

    if (!enufAP(playernum, governor, Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1], apcount)) {
        return;
    }

    to_block = 0;

    if (match(args[1], "block")) {
        to_block = 1;
        notify(playernum, governor, "Paging alliance block.\n");
    } else {
        who = GetPlayer(args[1]);

        if (!who) {
            sprintf(buf, "No such player.\n");
            notify(playernum, governor, buf);

            return;
        }

        alien = races[who - 1];
        apcount *= !alien->God;

        if (argn > 1) {
            gov = atoi(args[2]);
        }
    }

    switch (Dir[playernum - 1][governor].level) {
    case LEVEL_UNIV:
        sprintf(buf, "You can't make pages at universal scope.\n");
        notify(playernum, governor, buf);

        break;
    default:
        getstar(&Stars[Dir[playernum - 1][governor].snum],
                Dir[playernum - 1][governor].snum);

        if (!enufAP(playernum, governor, Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1], apcount)) {
            return;
        }

        race = races[playernum - 1];

        sprintf(buf,
                "%s \"%s\" page(s) you from the %s star system.\n",
                race->name,
                race->governor[governor].name,
                Stars[Dir[playernum - 1][governor].snum]->name);

        if (to_block) {
            dummy[0] = Blocks[playernum - 1].invite[0] & Blocks[playernum - 1].pledge[0];
            dummy[1] = Blocks[playernum - 1].invite[1] & Blocks[playernum - 1].pledge[1];

            for (i = 1; i <= Num_races; ++i) {
                if (isset(dummy, i) && (i != playernum)) {
                    notify_race(i, buf);
                }
            }
        } else {
            if (argn > 1) {
                notify(who, gov, buf);
            } else {
                notify_race(who, buf);
            }
        }



        notify(playernum, governor, "Request sent.\n");

        break;
    }

    deductAPs(playernum, governor,
              apcount,
              Dir[playernum - 1][governor].snum,
              0);
}

void send_message(int playernum, int governor, int apcount0, int postit, orbitinfo *unused5)
{
    int who = -1;
    int i;
    int j;
    int to_block;
    int dummy[2];
    int apcount;
    int to_star;
    int star = -1;
    int start;
    int to;
    int what;
    placetype where;
    racetype *race;
    racetype *alien;

    apcount = apcount0;
    to_block = 0;
    to_star = to_block;

    /* Clear the message buffer out */
    strcpy(msg, "");

    if (argn < 2) {
        notify(playernum, governor, "Send what?\n");

        return;
    }

    if (postit) {
        race = races[playernum - 1];

        sprintf(msg,
                "%s \"%s\" [%d,%d]: ",
                race->name,
                race->governor[governor].name,
                playernum,
                governor);

        /* Put the message together */
        for (j = 1; j < argn; ++j) {
            sprintf(buf, "%s ", args[j]);
            strcat(msg, buf);
        }
        strcat(msg, "\n");
        post(msg, ANNOUNCE);

        return;
    }

    if (match(args[1], "block")) {
        to_block = 1;
        notify(playernum, governor, "Sending message to alliance block.\n");

        who = GetPlayer(args[2]);

        if (!who) {
            sprintf(buf, "No such alliance block.\n");
            notify(playernum, governor, buf);

            return;
        }

        alien = races[who - 1];
        apcount *= !alien->God;
    } else if (match(args[1], "star")) {
        to_star = 1;
        notify(playernum, governor, "Sending message to star system.\n");
        where = Getplace(playernum, governor, args[2], 1);

        if (where.err || (where.level != LEVEL_STAR)) {
            sprintf(buf, "No such star.\n");
            notify(playernum, governor, buf);

            return;
        }

        star = where.snum;
        getstar(&Stars[star], star);
    } else {
        who = GetPlayer(args[1]);

        if (!who) {
            sprintf(buf, "No such player.\n");
            notify(playernum, governor, buf);

            return;
        }

        alien = races[who - 1];
        apcount *= (!alien->God && (who != playernum));
    }

    switch (Dir[playernum - 1][governor].level) {
    case LEVEL_UNIV:
        sprintf(buf, "You can't send message from universal scope.\n");
        notify(playernum, governor, buf);

        return;
    case LEVEL_SHIP:
        sprintf(buf, "You can't send message from ship scope.\n");
        notify(playernum, governor, buf);

        return;
    default:
        getstar(&Stars[Dir[playernum - 1][governor].snum],
                Dir[playernum - 1][governor].snum);

        if (!enufAP(playernum, governor, Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1], apcount)) {
            return;
        }

        break;
    }

    race = races[playernum - 1];

    /* Send the message */
    if (to_block) {
        to = TO_BLOCK;
        what = who;
    } else if (to_star) {
        to = TO_STAR;
        what = star;
    } else {
        to = TO_PLAYER;
        what = who;
    }

    if (to_star || to_block || isdigit((unsigned char)*args[2])) {
        start = 3;
    } else if (postit) {
        start = 1;
    } else {
        to = TO_RACE;
        what = who;
        start = 2;
    }

    /* Put the message together */
    for (j = start; j < argn; ++j) {
        sprintf(buf, "%s ", args[j]);
        strcat(msg, buf);
    }

    /* Post it */
    sprintf(buf,
            "%s \"%s\" [%d,%d] has sent you a dispatch.\nUse 'read dispatches' to access it.\n",
            race->name,
            race->governor[governor].name,
            playernum,
            governor);

    if (to_block) {
        dummy[0] = Blocks[who - 1].invite[0] & Blocks[who - 1].pledge[0];
        dummy[1] = Blocks[who - 1].invite[1] & Blocks[who - 1].pledge[1];

        sprintf(buf,
                "%s \"%s\" [%d,%d sends a dispatch to %s [%d] alliance block.\n",
                race->name,
                race->governor[governor].name,
                playernum,
                governor,
                Blocks[who - 1].name,
                who);

        for (i = 1; i <= Num_races; ++i) {
            if (isset(dummy, i)) {
                /* Determine alien from 'i' then garble message -mfw */
                alien = races[i - 1];

                if (!race->God && !alien->God && (chat_flag == TRANS_CHAT)) {
                    garble_msg(msg, alien->translate[playernum - 1],
                               1,
                               playernum);
                }

                notify_race(i, buf);
                send_race_dispatch(playernum, governor, i, to, what, msg);
            }
        }
    } else if (to_star) {
        sprintf(buf,
                "%s \"%s\" [%d,%d] sends a stargram to %s.\n",
                race->name,
                race->governor[governor].name,
                playernum,
                governor,
                Stars[star]->name);

        notify_star(playernum, governor, 0, star, buf);

        /*
         * I've basically copied the warn_star function here so we can better
         * deal with garbling the message -mfw
         */
        for (i = 1; i <= Num_races; ++i) {
            if ((i != playernum)
                && (i != 0)
                && isset(Stars[star]->inhabited, i)) {
                alien = races[i - 1];

                if (!race->God && !alien->God && (chat_flag == TRANS_CHAT)) {
                    garble_msg(msg,
                               alien->translate[playernum - 1],
                               1,
                               playernum);
                }

                send_race_dispatch(playernum, governor, i, to , what, msg);
            }
        }
    } else {
        int gov;

        if (who == playernum ) {
            apcount = 0;
        }

        alien = races[who - 1];

        if (!race->God && !alien->God && (chat_flag == TRANS_CHAT)) {
            garble_msg(msg, alien->translate[playernum - 1], 1, playernum);
        }

        if (isdigit((unsigned char)*args[2])) {
            gov = atoi(args[2]);

            if ((gov >= 0) && (gov <= MAXGOVERNORS)) {
                notify(who, gov, buf);
                send_dispatch(playernum, governor, who, gov, to, what, msg);
            } else {
                notify_race(who, buf);
                send_race_dispatch(playernum, governor, who, to, what, msg);
            }
        } else {
            notify_race(who, buf);
            send_race_dispatch(playernum, governor, who, to, what, msg);
        }

        /* Translation modifier increases */
        alien->translate[playernum - 1] = MIN(alien->translate[playernum - 1] + int_rand(1, 3), 100);

        putrace(alien);
    }

    notify(playernum, governor, "Dispatch send.\n");
    deductAPs(playernum,
              governor,
              apcount,
              Dir[playernum - 1][governor].snum,
              0);
}

void read_messages(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    if (!strncmp(args[1], "telegrams", 1)) {
        teleg_read(playernum, governor);
    } else if (!strncmp(args[1], "dispatches", 1)) {
        if (argn == 2) {
            read_dispatch(playernum, governor, 0);
        } else if (argn == 3) {
            read_dispatch(playernum, governor, atoi(args[2]));
        } else {
            notify(playernum,
                   governor,
                   "Wrong number of arguments to 'read dispatches'.\n");
        }
    } else if (!strncmp(args[1], "news", 1)) {
        notify(playernum, governor, CUTE_MESSAGE);
        notify(playernum,
               governor,
               "\n----------        Declarations        ----------\n");

        news_read(playernum, governor, DECLARATION);
        notify(playernum,
               governor,
               "\n----------           Combat           ----------\n");

        news_read(playernum, governor, COMBAT);
        notify(playernum,
               governor,
               "\n----------          Business          ----------\n");

        news_read(playernum, governor, TRANSFER);
        notify(playernum,
               governor,
               "\n----------          Bulletins         ----------\n");

        news_read(playernum, governor, ANNOUNCE);
    } else if (!strncmp(args[1], "declarations", 2)) {
        notify(playernum, governor, CUTE_MESSAGE);
        notify(playernum,
               governor,
               "\n----------        Declarations        ----------\n");

        news_read(playernum, governor, DECLARATION);
    } else if (!strncmp(args[1], "combat", 1)) {
        notify(playernum, governor, CUTE_MESSAGE);
        notify(playernum,
               governor,
               "\n----------           Combat           ----------\n");

        news_read(playernum, governor, COMBAT);
    } else if (!strncmp(args[1], "business", 1)) {
        notify(playernum, governor, CUTE_MESSAGE);
        notify(playernum,
               governor,
               "\n----------          Business          ----------\n");

        news_read(playernum, governor, TRANSFER);
    } else if (!strncmp(args[1], "postings", 1)) {
        notify(playernum, governor, CUTE_MESSAGE);
        notify(playernum,
               governor,
               "\n----------          Bulletins         ----------\n");

        news_read(playernum, governor, ANNOUNCE);
    } else {
        notify(playernum, governor, "Read what?\n");
    }
}

void purge_messages(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    if (argn != 2) {
        notify(playernum, governor, "Purge what?\n");

        return;
    }

    if (!strncmp(args[1], "telegrams", 1)) {
        purge_telegrams(playernum, governor);
    } else if (!strncmp(args[1], "dispatches", 1)) {
        purge_dispatch(playernum, governor, apcount);
    } else if (!strncmp(args[1], "all", 1)) {
        purge_telegrams(playernum, governor);
        purge_dispatch(playernum, governor, apcount);
    } else {
        notify(playernum,
               governor,
               "Improper usage, see 'help command purge'\n");
    }
}

void motto(int playernum, int governor, int apcount, char *message)
{
    strncpy(Blocks[playernum - 1].motto, message, MOTTOSIZE - 1);
    Putblock(Blocks);
    notify(playernum, governor, "Done.\n");
}

void name(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    char *ch;
    int i;
    int spaces;
    int len;
    int s;
    unsigned char check = 0;
    shiptype *ship;
    char string[1024];
    char temp[128];
    racetype *race;
    planettype *p;

    if (!isalnum((unsigned char) args[2][0]) || (argn < 3)) {
        notify(playernum, governor, "Illegal name format.\n");

        return;
    }

    sprintf(buf, "%s", args[2]);

    for (i = 3; i < argn; ++i) {
        sprintf(temp, " %s", args[i]);
        strcat(buf, temp);
    }

    sprintf(string, "%s", buf);
    i = strlen(args[0]);

    /*
     * Make sure there are no ^'s or '/' in name, also make sure the name has at
     * least 1 character in it
     */
    ch = string;
    spaces = 0;

    while (*ch != '\0') {
        /* Added underscore character -mfw */
        /*
         * check |= ((!isalnum(*ch) && (*ch != ' ') && (*ch != '.'))
         *           || (*ch == '/'));
         */
        check |= ((!isalnum((unsigned char)*ch) && (*ch != ' ') && (*ch != '.') && (*ch != '_')) || (*ch == '/'));

        ++ch;

        if (*ch == ' ') {
            ++spaces;
        }
    }

    len = strlen(buf);

    /* Not allowing whitespaces -mfw */
    /* if (spaces == strlen(buf)) { */
    if (spaces > 0) {
        notify(playernum,
               governor,
               "Illegal name, no whitespaces allowed.\n");

        return;
    }

    if ((strlen(buf) < 1) || check) {
        if (check) {
            sprintf(buf, "Illegal name form.\n");
        } else {
            sprintf(buf, "Illegal name length.\n");
        }

        notify(playernum, governor, buf);

        return;
    }

    if (match(args[1], "ship")) {
        if (Dir[playernum - 1][governor].level == LEVEL_SHIP) {
            getship(&ship, Dir[playernum - 1][governor].shipno);
            strncpy(ship->name, buf, SHIP_NAMESIZE);
            putship(ship);
            notify(playernum, governor, "Name set.\n");
            free(ship);

            return;
        } else {
            notify(playernum,
                   governor,
                   "You have to 'cs' to a ship to name it.\n");

            return;
        }
    } else if (match(args[1], "class")) {
        if (Dir[playernum - 1][governor].level == LEVEL_SHIP) {
            getship(&ship, Dir[playernum - 1][governor].shipno);

            if (ship->type != OTYPE_FACTORY) {
                notify(playernum, governor, "You are not at a factory!\n");
                free(ship);

                return;
            }

            if (ship->on) {
                notify(playernum,
                       governor,
                       "This factory is already on line.\n");

                free(ship);

                return;
            }

            strncpy(ship->class, buf, SHIP_NAMESIZE - 1);
            putship(ship);
            notify(playernum, governor, "Class set.\n");
            free(ship);

            return;
        } else {
            notify(playernum,
                   governor,
                   "You have to 'cs' to a factory to name the ship class.\n");

            return;
        }
    } else if (match(args[1], "block")) {
        /* Name your alliance block */
        strncpy(Blocks[playernum - 1].name, buf, RNAMESIZE - 1);
        Putblock(Blocks);
        notify(playernum, governor, "Done.\n");
    } else if (match(args[1], "star")) {
        if (Dir[playernum - 1][governor].level == LEVEL_STAR) {
            race = races[playernum - 1];

#ifndef NAME_STARS
            if (!race->God) {
                notify(playernum,
                       governor,
                       "Only dieties may name a star.\n");

                return;
            }

#else
            if (!race->God && !MostAPs(Playernum, Stars[Dir[playernum - 1][governor].snum])) {
                notify(playernum,
                       governor,
                       "You don't have the most AP's in that system.\n");

                return;
            }
#endif

            /* Got to re-get all stars, to stop cheaters -mfw */
            for (s = 0; s < Sdata.numstars; ++s) {
                getstar(&Stars[s], s);
            }

            /* To prevent cheating or naming conflicts -mfw */
            for (s = 0; s < Sdata.numstars; ++s) {
                if (s != Dir[playernum - 1][governor].snum) {
                    if (!strncmp(Stars[s]->name, buf, MIN(len, strlen(Stars[s]->name)))) {
                        notify(playernum,
                               governor,
                               "Unacceptable star name.\n");

                        return;
                    }
                }
            }

            strncpy(Stars[Dir[playernum - 1][governor].snum]->name,
                    buf,
                    NAMESIZE - 1);

            putstar(Stars[Dir[playernum - 1][governor].snum],
                    Dir[playernum - 1][governor].snum);
        } else {
            notify(playernum,
                   governor,
                   "You have to 'cs' to a star to name it.\n");

            return;
        }
    } else if (match(args[1], "planet")) {
        if (Dir[playernum - 1][governor].level == LEVEL_PLAN) {
            getstar(&Stars[Dir[playernum - 1][governor].snum],
                    Dir[playernum - 1][governor].snum);

            race = races[playernum - 1];

            getplanet(&p,
                      Dir[playernum - 1][governor].snum,
                      Dir[playernum - 1][governor].pnum);

            if ((((p->Maxx * p->Maxy) / 2) >= p->info[playernum - 1].numsectsowned)
                && !race->God) {
                notify(playernum,
                       governor,
                       "You must own more than half the planet to rename it...\n");

                free(p);

                return;
            }

            free(p);

            /* To prevent cheating or naming conflicts -mfw */
            for (s = 0; s < Stars[Dir[playernum - 1][governor].snum]->numplanets; ++s) {
                if (!strncmp(Stars[Dir[playernum - 1][governor].snum]->pnames[s], buf, MIN(len, strlen(Stars[Dir[playernum - 1][governor].snum]->pnames[s])))) {
                    notify(playernum,
                           governor,
                           "Sorry, that name is already taken.\n");

                    return;
                }
            }

            strncpy(Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum],
                    buf,
                    NAMESIZE - 1);

            putstar(Stars[Dir[playernum - 1][governor].snum],
                    Dir[playernum - 1][governor].snum);

            deductAPs(playernum,
                      governor,
                      apcount,
                      Dir[playernum - 1][governor].snum,
                      0);
        } else {
            notify(playernum,
                   governor,
                   "You have to 'cs' to a planet to name it.\n");

            return;
        }
    } else if (match(args[1], "race")) {
        if (governor != 0) {
            notify(playernum,
                   governor,
                   "Only your leader can name your race.\n");

            return;
        }

        race = races[playernum - 1];

        if (race->governor[governor].toggle.standby & LOCK_RNAME) {
            notify(playernum,
                   governor,
                   "You can't change your race name again.\n");
        } else {
            /* CWL's name changes cost APs -mfw */
            if (!strncmp(race->name, "Unknown", 7)) {
                apcount = 0;
            } else {
                apcount = 5;
            }

            if (!enufAP(playernum, governor, Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1], apcount)) {
                notify(playernum, governor, "You don't have enough APs\n");

                return;
            } else {
                deductAPs(playernum,
                          governor,
                          apcount,
                          Dir[playernum - 1][governor].snum,
                          0);
            }

            strncpy(race->name, buf, RNAMESIZE - 1);
            sprintf(buf, "Race name changed to '%s'.\n", race->name);
            notify(playernum, governor, buf);

            if (chat_flag != FREE_CHAT) {
                race->governor[governor].toggle.standby |= LOCK_RNAME;
            }

            putrace(race);
        }
    } else if (match(args[1], "governor")) {
        race = races[playernum - 1];

        if (race->governor[governor].toggle.standby & LOCK_GNAME) {
            notify(playernum,
                   governor,
                   "You can't change your governor name again.\n");
        } else {
            strncpy(race->governor[governor].name,
                    buf,
                    RNAMESIZE - 1);

            sprintf(buf,
                    "Governor name changed to '%s'.\n",
                    race->governor[governor].name);

            notify(playernum, governor, buf);

            if (chat_flag != FREE_CHAT) {
                race->governor[governor].toggle.standby |= LOCK_GNAME;
            }

            putrace(race);
        }
    } else if (match(args[1], "fleet")) {
        /* Allow player to name fleet -mfw */
        if (Dir[playernum - -1][governor].level == LEVEL_SHIP) {
            int fl;

            getship(&ship, Dir[playernum - 1][governor].shipno);
            race = races[playernum - 1];
            fl = ship->fleetmember;

            if (fl) {
                if ((fl > 0)
                    && (race->fleet[fl].admiral != governor)
                    && (race->fleet[fl].flagship != 0)) {
                    notify(playernum,
                           governor,
                           "You're not admiral of this fleet.\n");

                    return;
                }

                strncpy(race->fleet[ship->fleetmember].name,
                        buf,
                        FLEET_NAMESIZE);

                notify(playernum, governor, "Fleet name set.\n");
                putrace(race);
            } else {
                notify(playernum, governor, "This ship is not in a fleet.\n");
            }

            free(ship);

            return;
        } else {
            notify(playernum,
                   governor,
                   "You have to 'cs' to a ship in the fleet to name your fleet.\n");

            return;
        }
    } else {
        notify(playernum, governor, "I don't know what you mean.\n");

        return;
    }

    /* Not allowing whitespaces - mfw */
    /* } */
}

int MostAPs(int playernum, startype *s)
{
    int i;
    int t = 0;

    for (i = 0; i < MAXPLAYERS; ++i) {
        if (s->AP[i] >= t) {
            t = s->AP[i];
        }
    }

    return (s->AP[playernum - 1] == t);
}

void announce(int playernum, int governor, char *message, int mode, int override)
{
    racetype *race;
    char symbol = '\a';

    race = races[playernum - 1];

    if ((mode == SHOUT) && !race->God) {
        notify(playernum,
               governor,
               "You are not privileged to use this command.\n");

        return;
    }

    if ((chat_flag == NO_CHAT) && !race->God) {
        notify(playernum,
               governor,
               "That mode of communication is currently disabled.\n");

        return;
    }

    switch (Dir[playernum - 1][governor].level) {
    case LEVEL_UNIV:
        if (mode == ANN) {
            mode = BROADCAST;
        }

        break;
    default:
        if ((mode == ANN)
            && (!isset(Stars[Dir[playernum - 1][governor].snum]->inhabited, playernum)
                && !race->God)) {
            sprintf(buf,
                    "You do not inhabit this system or have diety privileges.\n");

            notify(playernum, governor, buf);

            return;
        }
    }

    switch (mode) {
    case ANN:
        symbol = ':';

        break;
    case BROADCAST:
        symbol = '>';

        break;
    case SHOUT:
        symbol = '!';

        break;
    case THINK:
        symbol = '=';

        break;
    }

    message++;
    sprintf(msg, "%s\n", message);

    if (mode == EMOTE) {
        sprintf(head,
                "%s [%d,%d] %s ",
                race->name,
                playernum,
                governor,
                race->governor[governor].name);
    } else {
        sprintf(head,
                "%s \"%s\" [%d,%d %c ",
                race->name,
                race->governor[governor].name,
                playernum,
                governor,
                symbol);
    }

    switch (mode) {
    case ANN:
        d_announce(playernum,
                   governor,
                   Dir[playernum - 1][governor].snum,
                   head,
                   msg);

        break;
    case BROADCAST:
        d_broadcast(playernum, governor, head, msg, 0);

        break;
    case SHOUT:
        d_shout(playernum, governor, head, msg);

        break;
    case THINK:
        d_think(playernum, governor, head, msg);

        break;
    case EMOTE:
        d_broadcast(playernum, governor, head, msg, 1);

        break;
    default:

        break;
    }
}

/* Garble -CWL */
char *garble_msg(char *s, int pcnt, int diff, int playernum)
{
    int l;
    int i;
    int over;
    int rand;
    char word[130];
    char wbuf[130];
    char *wp;
    char *wbp;
    char *sp;
    /* long fsize; -mfw */
    long lookup;
    long lp;
    int csum;
    int wascap;

    l = strlen(s);
    over = (int)(102.0 * sqrt((double)diff));

    if (USE_GARBLEFILE && size_of_words) {
        rewind(garble_file);

        if (!garble_file) {
            return s;
        }

        gbuf[0] = 0;
        word[0] = 0;
        sp = s;

        while (*sp) {
            wp = word;

            while (*sp && !isspace(*sp) && !mypunct(*sp) && !isdigit(*sp)) {
                *wp = *sp;
                ++wp;
                ++sp;
            }

            *wp = 0;

            if (word[0]) {
                wascap = isupper(word[0]);
                csum = 0;
                wp = word;

                while (*wp) {
                    csum += (int)*wp;
                    ++wp;
                }

                rand = int_rand(1, over);

                if (rand > pcnt) {
                    if (rand > 100) {
                        /* This word will be garbled by static -mfw */
                        for (lp = 0; lp < strlen(word); ++lp) {
                            word[lp] = '*';
                        }
                    } else {
                        /*
                         * This word will be garbled by language barrier
                         * (translated)
                         */
                        lookup = (playernum * pcnt * csum * strlen(word)) % size_of_words;

                        if (UNIFORM_WORDS) {
                            fseek(garble_file, lookup * UNIFORM_WORDS, 0);
                            fgets(wbuf, UNIFORM_WORDS, garble_file);
                        } else {
                            for (lp = 0; lp < lookup; ++lp) {
                                fgets(wbuf, 80, garble_file);
                            }
                        }

                        if (wascap && islower(wbuf[0])) {
                            wbuf[0] = toupper(wbuf[0]);
                        } else if (isupper(wbuf[0]) && !wascap) {
                            wbuf[0] = tolower(wbuf[0]);
                        }

                        wbp = wbuf;
                        wp = word;

                        while (*wbp
                               && (*wbp != '\n')
                               && (*wbp != UNIFORM_FILLER)) {
                            *wp = *wbp;
                            ++wp;
                            ++wbp;
                        }

                        *wp = 0;
                    }
                }

                if ((strlen(gbuf) + strlen(word)) < BUFFER_LEN) {
                    strcat(gbuf, word);
                }

                rewind(garble_file);
            }

            wp = word;

            if (isdigit(*sp) && (int_rand(1, over) > pcnt)) {
                *wp = (char)(int_rand(0, 9) + (int)'0');
                ++wp;
                ++sp;
            } else {
                *wp = *sp;
                ++wp;
                ++sp;
            }

            *wp = 0;

            strcat(gbuf, word);
        } /* while */

        strcpy(s, gbuf);

        return gbuf;
    } else {
        for (i = 0; i < l; ++i) {
            if ((int_rand(1, over) > pcnt)
                && (s[i] != '\n')
                && (s[i] != ' ')
                && (s[i] != '[')
                && (s[i] != ']')
                && (s[i] != ':')
                && (s[i] != '>')) {
                s[i] = ' ';
            }
        }

        return s;
    }
} /* End garble_msg */

void garble_chat(int playernum, int governor, int unused3, int unused4, orbitinfo *unused5)
{
    int temp1;
    int temp2;

    if ((argn < 2) || (argn > 3)) {
        sprintf(buf, "You need to specify the chat level.\n");
        notify(playernum, governor, buf);

        return;
    }

    temp1 = atoi(args[1]);

    if ((temp1 < 0) || (temp1 > 2)) {
        sprintf(buf, "Invalid chat level, use 0-2.\n");
        notify(playernum, governor, buf);

        return;
    }

    if (temp1 == 1) {
        temp2 = atoi(args[2]);

        if ((temp2 < 1) || (temp2 > 4)) {
            sprintf(buf, "Invalid static level, use 1-4.\n");
            notify(playernum, governor, buf);

            return;
        }

        chat_static = temp2;
    } else {
        chat_static = 1;
    }

    chat_flag = temp1;

    if (chat_flag == 1) {
        sprintf(buf,
                "Chat now at level %d with static level at %d\n",
                chat_flag,
                chat_static);
    } else {
        sprintf(buf, "chat now at level %d.\n", chat_flag);
    }

    notify(playernum, governor, buf);

    return;
}
