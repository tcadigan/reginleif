/*
 * Copyright (C) 1995-2002 FSGames. Ported by Sean Ford and Yan Shosh
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "campagin_picker.hpp"
#include "graph.hpp"
#include "smooth.hpp"
#include "util.hpp"

void popup_dialog(char const *title, char const *message);

Sint16 load_saved_game(char const *filename, screen *myscreen)
{
    Uint8 scenfile[20];
    guy *temp_guy;
    walker *temp_walker;
    walker *replace_walker;
    Sint16 myord;
    Sint16 myfam;
    Sint32 multi_team = 0;
    Sint32 i;

    myscreen->numviews = myscreen->save_data.numplayers;

    myscreen->cleanup(myscreen->numviews);
    myscreen->initialize_views();

    // Determine the scenario name to load
    sprintf(scenfile, "scen%d", myscreen->save_data.scen_num);

    // And load the scenario...
    myscreen->level_data.id = myscreen->save_data.scen_num;

    if (!myscreen->level_data.load()) {
        Sint16 old_scen = myscreen->save_data.scen_num;
        Log("Failed to load \"%s\". Falling back to loading scenario 1.\n", scenfile);

        // Failed? Try level 1.
        myscreen->save_data.scen_num = 1;
        myscreen->level_data.id = 1;

        if (!myscreen->level_data.load()) {
            char buf[200];
            snprintf(buf, 200, "Fallback loading failed (%d).\nCould not load \"%s\"\nPlease report this problem to the developer!\n", old_scen, scenfile);
            popup_dialog("ERROR", buf);

            exit(2);
        }
    }

    std::list<walker *> foelist = myscreen->level_data.oblist;

    for (auto itr = foelist.begin(); itr != foelist.end(); itr++) {
        walker *w = *itr;

        if (w) {
            w->set_difficulty(static_cast<Uint32>(w->stats->level));
        }
    }

    // Cycle through the team list...
    for (Sint32 i = 0; i < myscreen->save_data.team_size; ++i) {
        temp_guy = myscreen->save_data.team_list[i];
        temp_walker = temp_guy->create_and_add_walker(myscreen);
        // Clear the new guy's battle data
        temp_walker->myguy->scen_datam = 0;
        temp_walker->myguy->scen_kills = 0;
        temp_walker->myguy->scen_damage_taken = 0;
        temp_walker->myguy->scen_min_hp = 5000000;
        temp_walker->myguy->scen_shots = 0;
        temp_walker->myguy->scen_hits = 0;

        /*
         * Do we have guys on multiple teams? If so, we need to record it so
         * that we can set the controls of the viewscreens correctly
         */
        if (temp_guy->teamnum != 0) {
            multi_team = 1;
        }

        // First, try to find a marker that's the correct team number...
        replace_walker = myscreen->first_of(ORDER_SPECIAL,
                                            FAMILY_RESERVED_TEAM,
                                            static_cast<Sint32>(temp_guy->teamnum));

        // If that doesn't work, though, grab any marker we can...
        if (!replace_walker) {
            replace_walker = myscreen->first_of(ORDER_SPECIAL, FAMILY_RESERVED_TEAM);
        }

        if (replace_walker) {
            temp_walker = setxy(replace_walker->xpos, replace_walker->ypos);
            replace_walker->dead = 1;
        } else {
            // Scatter the overflowing characters...
            temp_walker->teleport();
        }
    }

    // Destroy all player markers (by setting them all to dead)
    replace_walker = myscreen->first_of(ORDER_SPECIAL, FAMILY_RESERVED_TEAM);

    while (replace_walker) {
        replace_walker->dead = 1;
        replace_walker = myscreen->first_of(ORDER_SPECIAL, FAMILY_RESERVED_TEAM);
    }

    // Have we already done this scenario?
    if (myscreen->save_data.is_level_completed(myscreen->save_data.scen_num)) {
        // Log("already done level\n");
        foelist = myscreen->level_data.oblist;

        for (auto itr = foelist.begin(); itr != foelist.end(); itr++) {
            walker *w = *itr;

            if (w) {
                // Kill everything except for our team, exits, and teleporters
                myfam = w->query_family();
                myord = w->query_order();

                if ((((w->team_num == 0) || w->myguy) && (myord == ORDER_LIVING)) // Living team member
                    || ((myord == ORDER_TREASURE) && (myfam == FAMILY_EXIT)) // Exit
                    || ((myord == ORDER_TREASURE) && (myfam == FAMILY_TELEPORTER)) // Teleporters
                    ) {
                    // Do nothing; legal guy
                } else {
                    w->dead = 1;
                }
            }
        }

        foelist = myscreen->level_data.weaplist;

        for (auto itr = foelist.begin(); itr != foelist.end(); itr++) {
            walker *w = *itr;

            if (w) {
                myfam = w->query_family();
                myord = w->query_order();

                if (((w->team_num == 0) && (myord == ORDER_LIVING)) // Living team member
                    || ((myord == ORDER_TREASURE) && (myfam == FAMILY_EXIT)) // Exit
                    || ((myord == ORDER_TREASURE) && (myfam == FAMILY_TELEPORTER)) // Teleporters
                    ) {
                    // Do nothing; legal guy
                } else {
                    w->dead = 1;
                }
            }
        }

        foelist = myscreen->level_data.fxlist;

        for (auto itr = foelist.begin(); itr != foelist.end(); itr++) {
            walker *w = *itr;

            if (w) {
                myfam = w->qury_family();
                myord = w->query_order();

                if (((w->team_num == 0) && (myord == ORDER_LIVING)) // Living team members
                    || ((myord == ORDER_TREASURE) && (myfam == FAMILY_EXIT)) // Exit
                    || ((myord == ORDER_TREASURE) && (myfam == FAMILY_TELEPORTER)) // Teleporters
                    ) {
                    // Do nothing; legal guy
                } else {
                    w->dead = 1;
                }
            }
        }
    }

    /*
     * Here we decide if all player are controlling team 0, or if they're
     * playing competing teams...
     */
    if (multi_team) {
        for (i = 0; i < myscreen->numviews; ++i) {
            myscreen->viewob[i]->my_team = i;
        }
    } else {
        for (i = 0; i < myscreen->numviews; ++i) {
            myscreen->viewob[i]->my_team = 0;
        }
    }

    return 1;
}
