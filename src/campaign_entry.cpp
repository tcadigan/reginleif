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
#include "campaign_entry.hpp"

#include "text.hpp"
#include "yam.h"

#include <list>

CampaignEntry::CampaignEntry(std::string const &id, Sint32 num_levels_completed)
    : id(id)
    , title("Untitled")
    , rating(0.0f)
    , version("1.0")
    , description("No description.")
    , suggested_power(0)
    , first_level(1)
    , num_levels(0)
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
                    contributors = yam.event.value;
                } else if (strncmp(yam.event.scalar, "description", strlen("description")) == 0) {
                    description = yam.event.value;
                } else if (strncmp(yam.event.scalar, "suggested_power", strlen("suggested_power")) == 0) {
                    suggested_power = std::stoi(yam.event.value);
                } else if (strncmp(yam.event.scalar, "first_level", strlen("first_level")) == 0) {
                    first_level = std::stoi(yam.event.value);
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

        icondata = read_pixie_file(std:string("icon.pix"));

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
