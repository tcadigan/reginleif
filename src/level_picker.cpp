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
#include "level_picker.hpp"

#include "browser_entry.hpp"
#include "button.hpp"
#include "guy.hpp"
#include "io.hpp"
#include "level_data.hpp"
#include "picker.hpp"
#include "radar.hpp"
#include "screen.hpp"
#include "stats.hpp"
#include "text.hpp"
#include "util.hpp"
#include "walker.hpp"

#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <list>
#include <sstream>
#include <string>
#include <sys/stat.h>

#define NUM_BROWSE_RADARS 3
#define MAX_TEAM_SIZE 24 // Max number of guys on a team
#define OK 4

bool prompt_for_string(std::string const &message, std::string &result);
void draw_highlight_interior(Button const &b);
void draw_highlight(Button const &b);
bool handle_menu_nav(Button *buttons, Sint32 &highlighted_button,
                     Sint32 &retvalue, bool use_global_vbuttons=true);

void getLevelStats(LevelData &level_data, Sint32 *max_enemy_level,
                   float *average_enemy_level, Sint32 *num_enemies,
                   float *difficulty, std::list<Sint32> &exits)
{
    Sint32 num = 0;
    Sint32 level_sum = 0;
    Sint32 difficulty_sum = 0;
    Sint32 difficulty_sum_friends = 0;
    Sint32 diff_per_level = 3;
    Sint32 max_level = 0;

    exits.clear();

    // Go through objects
    for (auto const &ob : level_data.oblist) {
        switch (ob->query_order()) {
        case ORDER_LIVING:
            if (ob->team_num != 0) {
                ++num;
                level_sum = ob->stats->level;
                difficulty_sum += diff_per_level * ob->stats->level;

                if (ob->stats->level > max_level) {
                    max_level = ob->stats->level;
                }
            } else {
                difficulty_sum_friends += (diff_per_level * ob->stats->level);
            }

            break;
        }
    }

    // Go through effects
    for (auto const ob : level_data.fxlist) {
        switch (ob->query_order()) {
        case ORDER_TREASURE:
            if (ob->query_family() == FAMILY_EXIT) {
                exits.push_back(ob->stats->level);
            }

            break;
        }
    }

    *num_enemies = num;
    *max_enemy_level = max_level;

    if (num == 0) {
        *average_enemy_level = 0;
    } else {
        *average_enemy_level = level_sum / static_cast<float>(num);
    }

    exits.sort();
    exits.unique();
}

bool isDir(std::string const &filename)
{
    struct stat status;
    stat(filename.c_str(), &status);

    return (status.st_mode & S_IFDIR);
}

bool sort_scen(std::string const &first, std::string const &second)
{
    std::string s1;
    std::string s1num;
    std::string s2;
    std::string s2num;

    bool gotNum = false;

    for (auto const &e : first) {
        if (!gotNum && isalpha(e)) {
            s1 += e;
        } else {
            s1num += e;
        }
    }

    gotNum = false;
    for (auto const &e : second) {
        if (!gotNum && isalpha(e)) {
            s2 += e;
        } else {
            s2num += e;
        }
    }

    if (s1 == s2) {
        return (atoi(s1num.c_str()) < atoi(s2num.c_str()));
    }

    return (first < second);
}

// Load a scenario...
Sint32 pick_level(VideoScreen *screenp, Sint32 default_level, bool enable_delete)
{
    Sint32 result = default_level;
    Text &loadtext = myscreen->text_normal;

    // Here are the browser variables
    BrowserEntry *entries[NUM_BROWSE_RADARS];

    std::vector<Sint32> level_list(list_levels_v());
    Sint32 level_list_length = level_list.size();

    // This indexes into the level_list
    Sint32 current_level_index = 0;

    // Figure out the list index for the current scen_level, so we can start there
    for (Sint32 i = 0; i < level_list_length; ++i) {
        if (level_list[i] == default_level) {
            current_level_index = i;
        }
    }

    // Load the radars (minimaps)
    for (Sint32 i = 0; i < NUM_BROWSE_RADARS; ++i) {
        if (i < level_list_length) {
            entries[i] = new BrowserEntry(myscreen, i, level_list[current_level_index + i]);
        } else {
            entries[i] = nullptr;
        }
    }

    Sint32 selected_entry = -1;

    // Figure out how good the player's army is
    Sint32 army_power = 0;

    for (Sint32 i = 0; i < MAX_TEAM_SIZE; ++i) {
        if (myscreen->save_data.team_list[i]) {
            army_power += (3 * myscreen->save_data.team_list[i]->get_level());
        }
    }

    // Buttons
    Sint16 screenW = 320;
    Sint32 screenH = 200;
    SDL_Rect prev = { static_cast<Sint16>(screenW - 150), 20, 30, 10 };
    SDL_Rect next = {
        static_cast<Sint16>(screenW - 150),
        static_cast<Sint16>(screenH - 50),
        30,
        10
    };

    SDL_Rect descbox = {
        static_cast<Sint16>(prev.x - 40),
        static_cast<Sint16>(prev.y + 15),
        185,
        static_cast<Uint16>((next.y - 10) - (prev.y + prev.h))
    };

    SDL_Rect choose = {
        static_cast<Sint16>(screenW - 50),
        static_cast<Sint16>(screenH - 30),
        30,
        30
    };

    SDL_Rect cancel = {
        static_cast<Sint16>(screenW - 100),
        static_cast<Sint16>(screenH - 30),
        38,
        10
    };

    SDL_Rect delete_button = { static_cast<Sint16>(screenW - 50), 10, 38, 10 };
    SDL_Rect id_button = {
        static_cast<Sint16>((delete_button.x - 52) - 10),
        10,
        52,
        10
    };

    // Controller input
    Sint32 retvalue = 0;
    Sint32 highlighted_button = 3;
    Uint8 prev_index = 0;
    Uint8 next_index = 1;
    Uint8 choose_index = 2;
    Uint8 cancel_index = 3;
    Uint8 delete_index = 4;
    Uint8 id_index = 5;
    Uint8 entry1_index = 6;
    Uint8 entry2_index = 7;
    Uint8 entry3_index = 8;

    Button buttons[] = {
        Button("PREV", KEYSTATE_UNKNOWN, prev.x, prev.y, prev.w, prev.h, 0, -1, MenuNav::DownLeftRight(next_index, entry1_index, id_index)),
        Button("NEXT", KEYSTATE_UNKNOWN, next.x, next.y, next.w, next.h, 0, -1, MenuNav::UpLeftRight(prev_index, entry3_index, cancel_index)),
        Button("OK", KEYSTATE_UNKNOWN, choose.x, choose.y, choose.w, choose.h, 0, -1, MenuNav::UpLeft(id_index, cancel_index), true),
        Button("CANCEL", KEYSTATE_ESCAPE, cancel.x, cancel.y, cancel.w, cancel.h, 0, -1, MenuNav::UpLeftRight(id_index, next_index, choose_index)),
        Button("DELETE", KEYSTATE_UNKNOWN, delete_button.x, delete_button.y, delete_button.w, delete_button.h, 0, -1, MenuNav::DownLeft(choose_index, id_index), true),
        Button("ENTER ID", KEYSTATE_UNKNOWN, id_button.x, id_button.y, id_button.w, id_button.h, 0, -1, MenuNav::DownLeftRight(cancel_index, prev_index, delete_index)),
        Button("1", KEYSTATE_UNKNOWN, 10, 15, 40, 53 - 12, 0, -1, MenuNav::DownRight(entry2_index, prev_index)),
        Button("2", KEYSTATE_UNKNOWN, 10, 15 + (53 + 12), 40, 53 - 12, 0, -1, MenuNav::UpDownRight(entry1_index, entry3_index, next_index)),
        Button("3", KEYSTATE_UNKNOWN, 10, 15 + ((53 + 12) * 2), 40, 53 - 12, 0, -1, MenuNav::UpRight(entry2_index, next_index))
    };

    bool done = false;

    while (!done) {
        // Reset the timer count to zero...
        reset_timer();

        if (myscreen->end) {
            break;
        }

        // Get keys and stuff
        get_input_events(POLL);
        handle_menu_nav(buttons, highlighted_button, retvalue, false);

        // Quit if 'q' is pressed
        if (keystates[KEYSTATE_q]) {
            done = true;
        }

        // Mouse stuff...
        MouseState &mymouse = query_mouse();
        Sint32 mx = mymouse.x;
        Sint32 my = mymouse.y;
        bool do_click = mymouse.left;
        bool do_prev = false;
        bool do_next = false;
        bool do_choose = false;
        bool do_cancel = false;
        bool do_delete = false;
        bool do_id = false;
        bool do_select = false;

        if ((do_click
                && (prev.x <= mx)
                && (mx <= (prev.x + prev.w))
                && (prev.y <= my)
                && (my <= (prev.y + prev.h)))
            || ((retvalue == OK) && (highlighted_button == prev_index))) {
            do_prev = true;
        }

        if ((do_click
                && (next.x <= mx)
                && (mx <= (next.x + next.w))
                && (next.y <= my)
                && (my <= (next.y + next.h)))
            || ((retvalue == OK) && (highlighted_button == next_index))) {
            do_next = true;
        }

        if ((selected_entry >= 0)
            && ((do_click
                    && (choose.x <= mx)
                    && (mx <= (choose.x + choose.w))
                    && (choose.y <= my)
                    && (my <= (choose.y + choose.h)))
                || ((retvalue == OK) && (highlighted_button == choose_index)))) {
            do_choose = true;
        }

        if ((do_click
                && (cancel.x <= mx)
                && (mx <= (cancel.x + cancel.w))
                && (cancel.y <= my)
                && (my <= (cancel.y + cancel.h)))
            || ((retvalue == OK) && (highlighted_button == cancel_index))
            || keystates[buttons[cancel_index].hotkey]) {
            do_cancel = true;
        }

        if ((selected_entry >= 0)
            && ((do_click
                    && enable_delete
                    && (delete_button.x <= mx)
                    && (mx <= (delete_button.x + delete_button.w))
                    && (delete_button.y <= my)
                    && (my <= (delete_button.y + delete_button.h)))
                || ((retvalue == OK) && (highlighted_button == delete_index)))) {
            do_delete = true;
        }

        if ((do_click
                && (id_button.x <= mx)
                && (mx <= (id_button.x + id_button.w))
                && (id_button.y <= my)
                && (my <= (id_button.y + id_button.h)))
            || ((retvalue == OK) && (highlighted_button == id_index))) {
            do_id = true;
        }

        if (do_click
            || ((retvalue == OK) && ((highlighted_button == entry1_index)
                    || (highlighted_button == entry2_index)
                    || (highlighted_button == entry3_index)))) {
            do_select = true;
        }

        if (mymouse.left) {
            while (mymouse.left) {
                get_input_events(WAIT);
            }
        }

        // Prev
        if (do_prev) {
            if (current_level_index > 0) {
                selected_entry = -1;

                if ((highlighted_button == delete_index)
                    || (highlighted_button == choose_index)) {
                    highlighted_button = prev_index;
                }

                --current_level_index;

                // Delete the bottom one and shift the rest down
                delete entries[NUM_BROWSE_RADARS - 1];

                for (Sint32 i = NUM_BROWSE_RADARS - 1; i > 0; --i) {
                    entries[i] = entries[i - 1];

                    if (entries[i] != nullptr) {
                        entries[i]->updateIndex(i);
                    }
                }

                // Load the new top one
                if (current_level_index < level_list_length) {
                    entries[0] = new BrowserEntry(myscreen, 0, level_list[current_level_index]);
                }
            }
        } else if (do_next) {
            // Next
            if (current_level_index < (level_list_length - NUM_BROWSE_RADARS)) {
                selected_entry = -1;

                if ((highlighted_button == delete_index)
                    || (highlighted_button == choose_index)) {
                    highlighted_button = prev_index;
                }

                ++current_level_index;

                // Delete the top one and shift the rest up
                delete entries[0];

                for (Sint32 i = 0; i < (NUM_BROWSE_RADARS - 1); ++i) {
                    entries[i] = entries[i + 1];

                    if (entries[i] != nullptr) {
                        entries[i]->updateIndex(i);
                    }
                }

                // Load the new bottom one
                if (((current_level_index + NUM_BROWSE_RADARS) - 1) < level_list_length) {
                    entries[NUM_BROWSE_RADARS - 1] = new BrowserEntry(myscreen, NUM_BROWSE_RADARS - 1, level_list[(current_level_index + NUM_BROWSE_RADARS) - 1]);
                }
            }
        } else if (do_choose) {
            // Choose
            if (selected_entry != -1) {
                result = level_list[current_level_index + selected_entry];
                done = true;

                break;
            }
        } else if (do_cancel) {
            // Cancel
            while (keystates[buttons[cancel_index].hotkey]) {
                get_input_events(WAIT);
            }

            done = true;

            break;
        } else if (do_delete) {
            // Delete
            if (yes_or_no_prompt("Delete level", "Delete this level permanently?", false)) {
                delete_level(level_list[current_level_index + selected_entry]);

                // Reload the picker
                level_list = list_levels_v();
                level_list_length = level_list.size();

                // Make sure our currently showing radars are not blank
                if ((current_level_index + NUM_BROWSE_RADARS) >= level_list_length) {
                    if (level_list_length > NUM_BROWSE_RADARS) {
                        current_level_index = level_list_length - NUM_BROWSE_RADARS;
                    } else {
                        current_level_index = 0;
                    }
                }

                // Load the radars (minimaps)
                for (Sint32 i = 0; i < NUM_BROWSE_RADARS; ++i) {
                    delete entries[i];

                    if (i < level_list_length) {
                        entries[i] = new BrowserEntry(myscreen, i, level_list[current_level_index + i]);
                    } else {
                        entries[i] = nullptr;
                    }
                }

                selected_entry = -1;

                if ((highlighted_button == delete_index) || (highlighted_button = choose_index)) {
                    highlighted_button = prev_index;
                }
            }
        } else if (do_id) {
            // Enter ID
            std::string level;

            if (prompt_for_string("Enter Level ID (num)", level) && !level.empty()) {
                result = atoi(level.c_str());
                done = true;

                break;
            }
        } else if (do_select) {
            // Select
            selected_entry = -1;

            if ((highlighted_button == delete_index) || (highlighted_button == choose_index)) {
                highlighted_button = prev_index;
            }

            // Select
            for (Sint32 i = 0; i < NUM_BROWSE_RADARS; ++i) {
                if ((i < level_list_length) && (entries[i] != nullptr)) {
                    Sint32 x = entries[i]->myradar.xloc;
                    Sint32 y = entries[i]->myradar.yloc;
                    Sint32 w = entries[i]->myradar.xview;
                    Sint32 h = entries[i]->myradar.yview;
                    SDL_Rect b = {
                        static_cast<Sint16>(x - 2),
                        static_cast<Sint16>(y - 2),
                        static_cast<Uint16>(w + 2),
                        static_cast<Uint16>(h + 2)
                    };

                    if ((do_click
                            && (b.x <= mx)
                            && (mx <= (b.x + b.w))
                            && (b.y <= my)
                            && (my <= (b.y + b.h)))
                        || ((retvalue == OK)
                            && ((highlighted_button - entry1_index) == i))) {
                        selected_entry = i;

                        break;
                    }
                }
            }
        }

        retvalue = 0;

        // Update hidden buttons
        if ((selected_entry >= 0) && enable_delete) {
            buttons[delete_index].hidden = false;
        } else {
            buttons[delete_index].hidden = true;
        }

        if (selected_entry >= 0) {
            buttons[choose_index].hidden = false;
        } else {
            buttons[choose_index].hidden = true;
        }

        // Draw
        myscreen->clearbuffer();

        std::stringstream buf;
        std::string temp;
        buf << "Army Power: " << army_power;
        temp = buf.str();
        buf.clear();
        temp.resize(20);
        loadtext.write_xy(prev.x + 50, prev.y + 2, temp, RED, 1);

        myscreen->draw_button(prev.x, prev.y, prev.x + prev.w, prev.y + prev.h, 1, 1);
        loadtext.write_xy(prev.x + 2, prev.y + 2, "Prev", DARK_BLUE, 1);
        myscreen->draw_button(next.x, next.y, next.x + next.w, next.y + next.h, 1, 1);
        loadtext.write_xy(next.x + 2, next.y + 2, "Next", DARK_BLUE, 1);

        if ((selected_entry != -1)
            && (selected_entry < level_list_length)
            && (entries[selected_entry] != nullptr)) {
            myscreen->draw_button(choose.x, choose.y, choose.x + choose.w, choose.y + choose.h, 1, 1);
            loadtext.write_xy(choose.x + 9, choose.y + 2, "OK", DARK_GREEN, 1);
            loadtext.write_xy(next.x, choose.y + 20, entries[selected_entry]->level_name, DARK_GREEN, 1);
        }

        myscreen->draw_button(cancel.x, cancel.y, cancel.x + cancel.w, cancel.y + cancel.h, 1, 1);
        loadtext.write_xy(cancel.x + 2, cancel.y + 2, "Cancel", RED, 1);

        if ((selected_entry >= 0) && enable_delete) {
            myscreen->draw_button(delete_button.x, delete_button.y, delete_button.x + delete_button.w, delete_button.y + delete_button.h, 1, 1);
            loadtext.write_xy(delete_button.x + 2, delete_button.y + 2, "Delete", RED, 1);
        }

        myscreen->draw_button(id_button.x, id_button.y, id_button.x + id_button.w, id_button.y + id_button.h, 1, 1);
        loadtext.write_xy(id_button.x + 2, id_button.y + 2, "Enter ID", DARK_BLUE, 1);

        if (selected_entry != -1) {
            Sint32 i = selected_entry;

            if ((i < level_list_length) && (entries[i] != nullptr)) {
                Sint32 x = entries[i]->myradar.xloc - 4;
                Sint32 y = entries[i]->myradar.yloc - 4;
                Sint32 w = entries[i]->myradar.xview + 8;
                Sint32 h = entries[i]->myradar.yview + 8;
                myscreen->draw_box(x, y, w + x, y + h, DARK_BLUE, 1, 1);
            }
        }

        for (Sint32 i = 0; i < NUM_BROWSE_RADARS; ++i) {
            if ((i < level_list_length) && (entries[i] != nullptr)) {
                entries[i]->draw(myscreen);
            }
        }

        // Description
        if ((selected_entry != -1)
            && (selected_entry < level_list_length)
            && (entries[selected_entry] != nullptr)) {
            myscreen->draw_box(descbox.x, descbox.y, descbox.x + descbox.w, descbox.y + descbox.h, GREY, 1, 1);

            for (Sint32 i = 0; i < entries[selected_entry]->scentextlines; ++i) {
                if ((((prev.y + 20) + (10 * i)) + 1) > (descbox.y + descbox.h)) {
                    break;
                }

                loadtext.write_xy(descbox.x, (descbox.y + (10 * i)) + 1, entries[selected_entry]->scentext[i], BLACK, 1);
            }
        }

        draw_highlight(buttons[highlighted_button]);
        myscreen->buffer_to_screen(0, 0,  320, 200);
        SDL_Delay(10);
    }

    while (keystates[KEYSTATE_q]) {
        get_input_events(WAIT);
    }

    for (Sint32 i = 0; i < NUM_BROWSE_RADARS; ++i) {
        delete entries[i];
    }

    return result;
}
