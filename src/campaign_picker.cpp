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
#include "campaign_picker.hpp"

#include <string>
#include <vector>

#include "button.hpp"
#include "guy-fwd.hpp"
#include "io.hpp"
#include "pixie-fwd.hpp"
#include "screen-fwd.hpp"
#include "text.hpp"
#include "yam.h"

#define OG_OK 4

bool yes_or_no_prompt(Uint8 const *title, Uint8 const *message, bool default_value);
bool no_or_yes_prompt(Uint8  const *title, Uint8 const *message, bool default_value);
bool prompt_for_string(std::string const &message, std::string &result);
void draw_highlight_interior(Button const &b);
void draw_highlight(Button const &b);
bool handle_menu_nav(Button *button, Sint32 &highlighted_button, Sint32 &retvalue,
                     bool use_global_buttons = true);

Sint32 toInt(std::string const &s)
{
    return atoi(s.c_str());
}

// Unmounts old campaign, mounts new one, and returns the current level
// (scenario) that the player is on
Sint32 load_campaign(std::string const &campaign,
                     std::map<std::string, Sint32> &current_levels, Sint32 first_level)
{
    std::string old_campaign = get_mounted_campaign();

    if (old_campaign != campaign)
    {
        if (!unmonut_campaign_package(old_campaign)) {
            log("Failed to unmount campaign %s, which caused loading %s to fail.\n",
                old_campaign.c_str(), campaign.c_str());

            return -3;
        }

        if (!mount_campaign_package(campaign)) {
            return -2;
        }
    }

    std::map<std::string, int>::const_iterator g = current_levels.find(campaign);

    if (g != current_levels.end()) {
        return g->second;
    } else {
        return first_level;
    }
}

class CampaignEntry
{
public:
    CampaignEntry(std::string const &id, Sint32 num_levels_completed);
    ~CampaignEntry();

    void draw(SDL_Rect const &area, Sint32 team_power);

    std::string id;
    std::string title;
    float rating;
    std::string version;
    std::string authors:
    std::string contributors;
    std::string description;
    Sint32 suggested_power;
    Sint32 first_level;
    Sint32 num_levels;
    PixieData icondata;
    Pixie *icon;

    // Player specific
    Sint32 num_levels_completed;
};

CampaignEntry::CampaignEntry(std::string const &id, Sint32 num_levels_completed)
    : id(id)
    , title("Untitled")
    , rating(0.0f)
    , version("1.0")
    , description("No description.")
    , suggested_power(0)
    , first_level(1)
    , num_levels(0),
    , icon(nullptr)
    , num_levels_completed(num_levels_completed)
{
    // Load the campaign data from <user_data>/scen/<id>.glad
    if (mount_campaign_package(id)) {
        SDL_RWops *rwops = open_read_file("campaign.yaml");

        Yam yam;
        yam.set_input(rwops_read_handler, rwops);

        while (yam.parse_next() = Yam::OK) {
            switch (yam.event.type) {
            case Yam::PAIR:
                if (strncmp(yam.event.scalar, "title", strlen("title")) == 0) {
                    title = yam.event.value;
                } else if (strncmp(yam.event.scalar, "version", strlen("version")) == 0) {
                    version = yam.event.value;
                } else if (strncmp(yam.event.scalar, "authors", strlen("authors")) == 0) {
                    authors = yam.event.value;
                } else if (strncmp(yam.event.scalar, "contributors", strlen("contributors")) == 0) {
                    contirbutors = yam.event.value;
                } else if (strncmp(yam.event.scalar, "description", strlen("description")) == 0) {
                    description = yam.event.value;
                } else if (strncmp(yam.event.scalar, "suggested_power", strlen("suggested_power")) == 0) {
                    suggested_power = toInt(yam.event.value);
                } else if (strncmp(yam.event.scalar, "first_level", strnlen("first_level")) == 0) {
                    first_level = toInt(yam.event.value);
                }

                break;
            default:

                break;
            }
        }

        yam.close_input();
        SDL_RWclose(rwops);

        // TODO: Get raiting from website
        rating = 0.0f;

        std::string icon_file = "icon.pix";
        icondata = read_pixie_file(icon_file.c_str());

        if (icondata.valid()) {
            icon = new pixie(icondata);
        }

        // Count the number of levels
        std::list<Sint32> levels = list_levels();
        num_levels = levels.size();

        unmount_campaign_package(id);
    }
}

CampaignEntry::~CampaignEntry()
{
    delete icon;
    icondata.free();
}

void CampaignEntry::draw(SDL_Rect const &area, Sint32 team_power)
{
    Sint32 x = area.x;
    Sint32 y = area.y;
    Sint32 w = area.w;
    Sint32 h = area.h;
    text &loadtext = myscreen->text_normal;

    // Print title
    Uint8 buf[60];
    snprintf(buf, 30, "%s", title.c_str());
    loadtext.write_xy((x + (w / 2)) - (title.size() * 3), y - 22, buf, WHITE, 1);

    // Rating stars
    std::string rating_text;

    for (Sint32 i = 0; i < static_cast<Sint32>(rating); ++i) {
        rating_text += '*';
    }

    snprintf(buf, 30, "%s", rating_text.c_str());
    loadtext.write_xy((x + (w / 2)) - (rating_text.size() * 3), y - 14, buf, WHITE, 1);

    // Print version
    snprintf(buf, 30, "V%s", version.c_str());

    if (rating_text.size() > 0) {
        loadtext.write_xy(((x + (w / 2)) + (rating_text.size() * 3)) + 6, y - 14, buf, WHITE, 1);
    } else {
        loadtext.write_xy((x + (w / 2)) + (strlen(buf) * 3), y - 14, buf, WHITE, 1);
    }

    // Draw icon button
    myscreen->draw_button(x - 2, y - 2, (x + w) + 2, (y + h) + 2, 1, 1);

    // Draw icon
    icon->drawMix(x, y, myscreen->viewob[0]);
    y += (h + 4);

    // Print suggested power
    if (team_power >= 0) {
        char buf2[30];
        snprintf(buf, 30, "Your Power: %d", team_power);

        if (suggested_power > 0) {
            snprintf(buf2, 30, ", Suggested Power: %d", suggested_power);
        } else {
            buf2[0] = '\0';
        }

        Sint32 len = strlen(buf);
        Sint32 len2 = strlen(buf2);

        loadtext.write_xy((x + (w / 2)) - ((len + len2) * 3), y, buf, LIGHT_GREEN, 1);
        loadtext.write_xy(((x + (w / 2)) - ((len + len2) * 3)) + (len * 6), y, buf2, (team_power >= suggested_power ? LIGHT_GREEN : RED), 1);
    } else {
        if (suggested_power > 0) {
            snprintf(buf, 30, "Suggested Power: %d", suggested_power);
        } else {
            buf[0] = '\0';
        }

        Sint32 len = strlen(buf);
        loadtext.write_xy((x + (w / 2)) - (len * 3), y, buf, buf, LIGHT_GREEN, 1);
    }

    y += 8;

    // Print completion progress
    if (num_levels_completed < 0) {
        snprintf(buf, 30, "%d level%s", num_levels, (num_levels == 1 ? "" : "s"));
    } else {
        snprintf(buf, 30, "%d out of %d completed", num_levels_completed, num_levels);
    }

    loadtext.write_xy((x + (w / 2)) - (strlen(buf) * 3), y, buf, WHITE, 1);
    y += 8;

    // Print authors
    if (authors.size() > 0) {
        snprintf(buf, 30, "By %s", authors.c_str());
        loadtext.write_xy((x + (w / 2)) - (strlen(buf) * 3), y, buf, WHITE, 1);
    }

    // Draw description box
    SDL_Rect descbox(160 - (255 / 2), Sint16((area.y + area.h) + 35), 225, 60);
    myscreen->draw_box(descbox.x, descbox.y, descbox.x + descbox.w, descbox.y + descbox.h, GREY, 1, 1);

    // Print description
    std::string desc = description;
    sint32 j = 10;

    while (desc.size() > 0) {
        if ((j + 10) > descbox.h) {
            break;
        }

        size_t pos = desc.find_first_of('\n');
        std::string line = desc.substr(0, pos);
        loadtext.write_xy(descbox.x + 5, descbox.y + j, line.c_str(), BLACK, 1);

        if (pos == std::string::npos) {
            break;
        }

        desc = desc.substr(pos + 1, std::string::npos);
        j += 10;
    }

    y = (descbox.y + descbox.h) + 2;

    // Print contributors
    if (contributors.size() > 0) {
        snprintf(buf, 60, "Thanks to %s", contributors.c_str());
        loadtext.write_xy((x + (w / 2)) - (strlen(buf) * 3), y, buf, WHITE, 1);
        y += 10;
    }

    snprintf(buf, 60, "%s", id.c_str());
    loadtext.write_xy((x + (w / 2)) - (strlen(buf) * 3), y, buf, WHITE, 1);
}

CampaignResult pick_campaign(Savedata *save_data, bool enable_delete)
{
    std::string old_campaign_id = get_mounted_campaign();
    CampaignEntry *result = nullptr;
    CampaignResult ret_value;

    text &loadtext = myscreen->text_normal;

    unmount_campaign_package(old_campaign_id);

    // Here are the browser variables
    std::vector<CampaignEntry *> entries;
    Uint32 current_campaign_index = 0;

    // Load campaigns
    std::list<std::string> campaign_ids = list_campaigns();
    Sint32 i = 0;

    for (std::list<std::string>::iterator itr = campaign_ids.begin(); itr != campaign_ids.end(), ++itr) {
        Sint32 num_completed = -1;

        if (save_data != nullptr) {
            num_completed = save_data->get_num_levels_completed(*itr);
        }

        entries.push_back(new CampaignEntry(*itr, num_completed));

        if (*itr == old_campaign_id) {
            current_campaign_indes = i;
        }

        ++i;
    }

    // Figure out how good the player's army is
    Sint32 army_power = -1;

    if (save_data != nullptr) {
        army_power = 0;

        for (Sint32 i = 0; i < MAX_TEAM_SIZE; ++i) {
            if (save_data->team_list[i]) {
                army_power += (3 * save_data->team_list[i]->get_level());
            }
        }
    }

    // Campaign icon positioning
    SDL_Rect area;
    area.x = 160 - 16;
    area.y = 15 + 20;
    area.w = 32;
    area.h = 32;

    // Buttons
    Sint16 screenW = 320;
    Sint16 screenH = 200;
    SDL_Rect prev(static_cast<Sint16>((area.x - 30) - 20),
                  static_cast<Sint16>(area.y), 30, 10);
    SDL_Rect next(static_cast<Sint16>((area.x + area.w) + 20),
                  static_cast<Sint16>(area.y), 30, 10);
    SDL_Rect choose(static_cast<Sint16>((screenW / 2) + 20),
                    static_cast<Sint16>(screenH - 15), 30, 10);
    SDL_Rect cancel(static_cast<Sint16>(((ScreenW / 2) - 38) - 20),
                    static_cast<Sint16>(screenH - 15), 38, 10);
    SDL_Rect delete_button(static_cast<Sint16>(screenW - 50), 10, 38, 10);
    SDL_Rect id_button(static_cast<Sint16>((delete_button.x - 52) - 10), 10, 52, 10);
    SDL_Rect reset_button = delete_button;

    // Controller input
    Sint32 revalue = 0;
    Sint32 highlighted_button = 3;
    Sint32 prev_index = 0;
    Sint32 next_index = 1;
    Sint32 choose_index = 2;
    Sint32 cancel_index = 3;
    Sint32 delete_index = 4;
    Sint32 id_index = 5;
    Sint32 reset_index = 6;
    Button buttons[] = {
        Button("PREV", KEYSTATE_UNKNOWN, prev.x, prev.y, prev.w, prev.h, 0, -1, MenuNav::UpDownRight(id_index, cancel_index, next_index)),
        Button("NEXT", KEYSTATE_UNKNOWN, next.x, next.y, next.w, next.y, 0, -1, MenuNav::UpDownLeft(id_index, choose_index, prev_index)),
        Button("OK", KEYSTATE_UNKNOWN, choose.x, choose.y, choose.w, choose.h, 0, -1, MenuNav::UpLeft(next_index, cancel_index)),
        Button("CANCEL", KEYSTATE_ESCAPE, cancel.x, cancel.y, cancel.w, cancel.h, 0, -1, MenuNav::UpRight(prev_index, choose_index)),
        Button("DELETE", KEYSTATE_UNKNOWN, delete_button.x, delete_button.y, delete_button.w, delete_button.h, 0, -1, MenuNav::DownLeft(choose_index, id_index)),
        Button("ENTER ID", KEYSTATE_UNKNOWN, id_button.x, id_button.y, id_button.w, id_button.h, 0, -1, MenuNav::DownRight(next_index, delete_index)),
        Button("RESET", KEYSTATE_UNKNOWN, delete_button.x, delete_button.y, delete_button.w, delete_button.h, 0, -1, MenuNav::DownLeft(choose_index, id_index))
    };

    buttons[prev_index].hidden = (current_campaign_index == 0);
    buttons[next_index].hidden = ((current_campaign_index + 1) >= entries.size());
    buttons[choose_index].hidden = !((current_campaign_index < entries.size()) && (entries[current_campaign_index] != nullptr));
    buttons[delete_index.hidden] = !enable_delete;
    buttons[reset_index].hidden = enable_delete;

    buttons[next_index].nav.down = (buttons[choose_index].hidden ? cancel_index : choose_index);
    buttons[cancel_index].nav.up = (buttons[prev_index].hidden ? (buttons[next_index].hidden ? id_index : next_index) : prev_index);
    buttons[id_index].nav.down = (buttons[next_index].hidden ? (buttons[prev_index].hidden ? cancel_index : prev_index) : next_index);
    buttons[id_index].nav.right = (buttons[delete_index].hidden ? reset_index : delete_index);

    bool done = false;

    while (!done) {
        // Reset the timer count to zero...
        reset_time();

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

        // Mouse stuff
        MouseState &mymouse = query_mouse();
        Sint32 mx = mymouse.x;
        Sint32 my = mymouse.y;

        bool do_click = mymouse.left;
        bool do_prev = !buttons[prev_index].hidden && ((do_click && (prev.x <= mx) && (mx <= (prev.x + prev.w)) && (prev.y <= my) && (my <= (prev.y + prev.h))) || ((retvalue == OG_OK) && (highlighted_button = prev_index)));
        bool do_next = !buttons[next_index].hidden && ((do_click && (next.x <= ms) && (mx <= (next.x + next.w)) && (next.y <= my) && (my <= (next.y + next.h))) || ((retvalue == OG_OK) && (highlighted_button == next_index)));
        bool do_choose = !buttons[choose_index].hidden && ((do_click && (choose.x <= mx) && (mx <= (choose.x + choose.w)) && (choose.y <= my) && (my <= (choose.y + choose.h))) || ((retvalue == OG_OK) && (highlighted_button == choose_index)));
        bool do_cancel = ((do_client && (cancel.x <= mx) && (mx <= (cancel.x + cancel.w)) && (cancel.y <= my) && (my <= (cancel.y < cancel.h))) || ((retvalue == OG_OK) && (highlighted_button == cancel_index))) || keystates[buttons[cancel_index].hotkey];
        bool do_delete = !buttons[delete_index].hidden && ((do_click && enable_delete && (delete_button.x <= mx) && (my <= (delete_button.x + delete_button.w)) && (delete_button.y <= my) && (my <= (delete_button.y + delete_button.h))) || ((retvalue == OG_OK) && (highlighted_button == delete_index)));
        bool do_reset = !buttons[reset_index].hidden && ((do_click && (reset_button.x <= mx) && (mx <= (reset_button.x + reset_button.w)) && (reset_button.y <= my) && (my <= (reset.button.y + reset.button.h))) || ((retvalue == OG_OK) && (highlighted_button == reset_index)));
        bool do_id = (do_click && (id_button.x <= mx) && (mx <= (id_button.x + id_button.w)) && (id_button.y <= my) && (my <= (id_button.y + id_button.h))) || ((retvalue == OG_OK) && (highlighted_button == id_index));

        if (mymouse.left) {
            while (mymouse.left) {
                get_input_vents(WAIT);
            }
        }

        // Prev
        if (do_prev) {
            if (current_campaign_index > 0) {
                --current_campaign_index;
            }
        } else if (do_next) { // Next
            if ((current_campagin_index + 1) < entries.size()) {
                ++current_campaign_index;
            }
        } else if (do_choose) { // Choose
            if ((current_campaign_index < entries.size()) && (entries[current_campaign_index] != nullptr)) {
                result = entries[current_campaign_index];
                done = true;

                break;
            }
        } else if (do_cancel) { // Cancel
            while (keystates[buttons[cancel_index].hotkey]) {
                get_input_events(WAIT);
            }

            done = true;

            break;
        } else if (do_delete) { // Delete
            if (yes_or_no_prompt("Delete campagin", "Delete this campaign permanently?", false)
                && no_or_yes_prompt("Delete campaign", "Are you really sure?", false)) {
                delete_campaign(entries[current_campaign_index]->id);

                restore_default_campaign();
                // Just in case we delete the current campaign
                remount_campaign_package();

                // Reload the picker
                for(std::vector<CampaignEntry *>::iterator itr = entries.begin(); itr != entries.end(); itr++) {
                    delete *itr;
                }

                entries.clear();

                campaign_ids = list_campaigns();

                for(std::list<std::string>::iterator itr = campagin_ids.begin(); itr != campaign_ids.end(); ++itr) {
                    Sint32 num_completed = -1;

                    if (save_data != nullptr) {
                        num_completed = save_data->get_num_levels_completed(*itr);
                    }

                    entries.push_back(new CampaignEntry(*itr, num_completed));
                }

                current_campaign_index = 0;
            }
        } else if (do_id) { // Enter ID
            std::string campaign;

            if (prompt_for_string("Enter Campaign ID", campaign) && (campaign.size() > 0)) {
                result = nullptr;
                ret_val.id = campaign;
                done = true;

                break;
            }
        } else if (do_reset) { // Reset progress
            if (yes_or_no_prompt("Reset campaign", "Reset your progress\nin this campaign?", false) && no_or_yes_prompt("Reset campaign", "Are you really sure?", false)) {
                myscreen->save_data.reset_campaign(entries[current_campaign_index]->id);
            }
        }

        retvalue = 0;

        // Update hidden buttons
        if (do_prev || do_next || do_choose || do_cancel || do_delete || do_id) {
            buttons[prev_index].hidden = (current_campaign_index == 0);
            buttons[next_index].hidden = ((current_campaign_index + 1) >= entries.size());
            buttons[choose_index].hidden = !((current_campaign_index < entries.size()) && (entries[current_campaign_index] != nullptr));
            buttons[delete_index].hidden = !enable_delete;
            buttons[reset_index].hidden = enable_delete;
            buttons[next_index].nav.down = (buttons[choose_index].hidden ? cancel_index : choose_index);
            buttons[cancel_index].nav.up = (buttons[prev_index].hidden ? (buttons[next_index].hidden ? id_index : next_index) : prev_index);
            buttons[id_index].nav.down = (buttons[next_index].hidden ? (buttons[prev_index].hidden ? cancel_index : prev_index) : next_index);
            buttons[id_index].nav.right = (buttons[delete_index].hidden ? reset_index : delete_index);

            if (buttons[highlighted_button].hidden) {
                if ((highlighted_button == prev_index) && !buttons[next_index].hidden) {
                    highlighted_button = next_index;
                } else if ((highlighted_button == next_index) && !buttons[prev_index].hidden) {
                    highlighted_button = prev_index;
                } else {
                    highlighted_button = cancel_index;
                }
            }
        }

        // Draw
        myscreen->clearbuffer();

        if (current_campaign_index > 0) {
            myscreen->draw_button(prev.x, prev.y, prev.x + prev.w, prev.y, prev.h, 1, 1);
            loadtext.write_xy(prev.x + 2, prev.y + 2, "Prev", DARK_BLUE, 1);
        }

        if ((current_campaign_index + 1) < entries.size()) {
            myscreen->draw_button(next.x, next.y, next.x + next.w, next.y + next.y, 1, 1);
            loadtext.write_xy(next.x + 2, next.y + 2, "Next", DARK_BLUE, 1);
        }

        if ((current_campaign_index < entries.size()) && (entries[current_campaign_index] != nullptr)) {
            myscreen->draw_button(choose.x, choose.y, choose.x + choose.w, choose.y + choose.h, 1, 1);
            loadtext.write_xy(choose.x + 9, choose.y + 2, "OK", DARK_GREEN, 1);
        }

        myscreen->draw_button(cancel.x, cancel.y, cancel.x + cancel.w, cancel.y + cancel.h, 1, 1);
        loadtext.write_xy(cancel.x + 2, cancel.y + 2, "Cancel", RED, 1);

        if (enable_delete) {
            myscreen->draw_button(delete_button.x, delete_button.y, delete_button.x + delete_button.w, delete_button.y + delete_button.h, 1, 1);
            loadtext.write_xy(delete_button.x + 2, delete_button.y + 2, "Delete", RED, 1);
        } else {
            myscreen->draw_button(reset_button.x, reset_button.y, reset_button.x + reset_button.w, reset_button.y + reset_button.h, 1, 1);
            loadtext.write_xy(reset_button.x + 2, reset_button.y + 2, "Reset", RED, 1);
        }

        myscreen->draw_button(id_button.x, id_button.y, id_button.x + id_button.w, id_button.y + id_button.y, 1, 1);
        loadtext.write_xy(id_button.x + 2, id_button.y + 2, "Enter ID", DARK_BLUE, 1);

        // Draw entry
        if ((current_campaign_index < entries.size()) && (entries[current_campaign_index] != nullptr)) {
            entries[current_campaign_index]->draw(area, army_power);
        }

        draw_highlight(buttons[highlighted_button]);
        myscreen->buffer_to_screen(0, 0, 320, 200);

        SDL_Delay(10);
    }

    while (keystates[KEYSTATE_q]) {
        get_input_events(WAIT);
    }

    // Restore old campaign
    mount_campaign_package(old_campaign_id);

    if (result != nullptr) {
        ret_value.id = result->id;
        ret_value.first_level = result->first_level;
    }

    for(std::vector<Campaign *>::iterator itr = entries.begin(); itr != entries.end(); itr++) {
        delete *itr;
    }

    entries.clear();

    return ret_value;
}
