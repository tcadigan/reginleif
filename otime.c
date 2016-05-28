/*
 * Omega copyright (c) 1987-1989 by Laurence Raphael Brothers
 *
 * otime.c
 *
 * This file deals with the passage of time in Omega
 */
#include "otime.h"

#include <stdlib.h>

#include "oaux3.h"
#include "ocom1.h"
#include "oglob.h"
#include "omon.h"
#include "oscr.h"
#include "outil.h"

/*
 * This function coordinates monsters and player actions, as well as random
 * events. Each tick is a second. There are therefore 60 ticks to the minute and
 * 60 minutes to the hour.
 */
void time_clock(int reset)
{
    pml ml;

    ++Tick;

    if(Tick > 60) {
        Tick = 0;

        /* See about some player statuses each minute */
        minute_status_check();

        ++Time;

        if((Time % 10) == 0) {
            tenminute_check();
        }
    }

    if(reset) {
        Player.click = 0;
        Tick = Player.click;
    }

    while((Tick == Player.click) && (Current_Environment != E_COUNTRYSIDE)) {
        if(!gamestatusp(SKIP_PLAYER)) {
            resetgamestatus(SKIP_MONSTERS);

            if(!Player.status[SLEPT] && (Current_Environment != E_COUNTRYSIDE)) {
                p_process();
            }

            while(gamestatusp(SKIP_MONSTERS) && (Current_Environment != E_COUNTRYSIDE)) {
                resetgamestatus(SKIP_MONSTERS);

                if(!Player.status[SLEPT] && (Current_Environment != E_COUNTRYSIDE)) {
                    p_process();
                }
            }
        }
        else {
            resetgamestatus(SKIP_PLAYER);
        }

        Player.click = (Player.click + Command_Duration) % 60;
    }

    /*
     * Kludgy but what the heck. Without this line, if the player caused a
     * change-environment to the country, the monsters on the old Level will
     * still act, causing all kinds of anomalies and core dumps,
     * intermittently. However, any other environment change will reset Level
     * appropriately, so only have to check for countryside.
     */
    if(Current_Environment != E_COUNTRYSIDE) {
        /*
         * No longer search for dead monsters every round -- waste of
         * time. Instead, just skip dead monsters. Eventually, the whole level
         * will be freed, getting rid of the dead 'uns
         */

        for(ml = Level->mlist; ml != NULL; ml = ml->next) {
            if(ml->m->hp > 0) {
                /* 
                 * Following is a hack until I discover the source of the
                 * phantom monsters 
                 */
                if(Level->site[ml->m->x][ml->m->y].creature != ml->m) {
                    fix_phantom(ml->m);
                }

                if(Tick == ml->m->click) {
                    ml->m->click += ml->m->speed;

                    while(ml->m->click > 60) {
                        ml->m->click -= 60;
                    }

                    m_pulse(ml->m);
                }
            }
        }
    }
}

/* Remedies occasional defective monsters */
void fix_phantom(struct monster *m)
{
    if(Level->site[m->x][m->y].creature == NULL) {
        mprint("You hear a sound like a sigh of relief...");
        Level->site[m->x][m->y].creature = m;
    }
    else {
        mprint("You hear a puff of displaced air...");
        findspace(&m->x, &m->y, -1);
        Level->site[m->x][m->y].creature = m;
        m_death(m);
    }
}
