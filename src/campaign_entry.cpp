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

#include "base.hpp"
#include "io.hpp"
#include "graphlib.hpp"
#include "pixie.hpp"
#include "screen.hpp"
#include "text.hpp"
#include "util.hpp"

#include <list>
#include <sstream>

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
        SDL_RWops *rwops = open_read_file("campaign.ini");

        Sint64 size = SDL_RWsize(rwops);

        std::stringstream buf;
        Sint64 chunk_size = 1024;
        char str[chunk_size];
        Sint64 total = 0;
        Sint64 read = 1;

        while ((total < size) && (read != 0)) {
            read = SDL_RWread(rwops, str, sizeof(char), std::min(chunk_size, size - total));
            total += read;
            std::string inp(str, read);
            buf << inp;
        }

        SDL_RWclose(rwops);

        if (total != size) {
            Log("Unable to read complete file");

            return;
        }

        std::string line;
        std::string key;
        std::string value;
        std::string::size_type delim_pos;

        while (std::getline(buf, line)) {
            delim_pos = line.find_first_of("=");

            if (delim_pos == std::string::npos) {
                Log("Skipping unknown ini line\n");

                continue;
            }

            key = line.substr(delim_pos);
            value = line.substr(delim_pos + 1, line.size());

            if (key == "title") {
                title = value;
            } else if (key == "version") {
                version = value;
            } else if (key == "authors") {
                authors = value;
            } else if (key == "contributors") {
                contributors = value;
            } else if (key == "description") {
                description = value;
            } else if (key == "suggested_power") {
                suggested_power = std::stoi(value);
            } else if (key == "first_level") {
                first_level = std::stoi(value);
            } else {
                Log("Unknown ini key '%s'\n", key.c_str());
            }
        }

        // TODO: Get raiting from website
        rating = 0.0f;

        icondata = read_pixie_file(std::string("icon.pix"));

        if (icondata.valid()) {
            icon = new Pixie(icondata);
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
    Text &loadtext = myscreen->text_normal;
    std::stringstream buf;

    // Print title
    title.resize(30);
    loadtext.write_xy((x + (w / 2)) - (title.size() * 3), y - 22, title, WHITE, 1);

    // Rating stars
    std::string rating_text;

    for (Sint32 i = 0; i < static_cast<Sint32>(rating); ++i) {
        rating_text += '*';
    }

    rating_text.resize(30);
    loadtext.write_xy((x + (w / 2)) - (rating_text.size() * 3), y - 14, rating_text, WHITE, 1);

    // Print version
    buf << "V" << version;
    std::string ver(buf.str());
    buf.clear();
    ver.resize(30);

    if (rating_text.size() > 0) {
        loadtext.write_xy(((x + (w / 2)) + (rating_text.size() * 3)) + 6, y - 14, ver, WHITE, 1);
    } else {
        loadtext.write_xy((x + (w / 2)) + (ver.length() * 3), y - 14, ver, WHITE, 1);
    }

    // Draw icon button
    myscreen->draw_button(x - 2, y - 2, (x + w) + 2, (y + h) + 2, 1, 1);

    // Draw icon
    icon->setxy(x, y);
    icon->drawMix(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
                  myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                  myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
    y += (h + 4);

    // Print suggested power
    if (team_power >= 0) {
        buf << "Your Power: " << team_power;
        std::string my_power(buf.str());
        buf.clear();
        my_power.resize(30);

        std::string s_power;
        if (suggested_power > 0) {
            buf << "Suggested Power: " << suggested_power;
            s_power = buf.str();
            buf.clear();
            s_power.resize(30);
        }

        Sint32 len = my_power.length();
        Sint32 len2 = s_power.length();

        loadtext.write_xy((x + (w / 2)) - ((len + len2) * 3), y, my_power, LIGHT_GREEN, 1);
        loadtext.write_xy(((x + (w / 2)) - ((len + len2) * 3)) + (len * 6), y, s_power, (team_power >= suggested_power ? LIGHT_GREEN : RED), 1);
    } else {
        std::string s_power;
        if (suggested_power > 0) {
            buf << "Suggested Power: " << suggested_power;
            s_power = buf.str();
            buf.clear();
            s_power.resize(30);
        }

        Sint32 len = s_power.length();
        loadtext.write_xy((x + (w / 2)) - (len * 3), y, s_power, LIGHT_GREEN, 1);
    }

    y += 8;

    // Print completion progress
    if (num_levels_completed < 0) {
        buf << num_levels << "level" << (num_levels == 1 ? "" : "s");
    } else {
        buf << num_levels_completed << " out of " << num_levels << " completed";
    }

    std::string completion(buf.str());
    buf.clear();
    completion.resize(30);

    loadtext.write_xy((x + (w / 2)) - (completion.length() * 3), y, completion, WHITE, 1);
    y += 8;

    // Print authors
    if (authors.size() > 0) {
        buf << "By " << authors;
        std::string auths(buf.str());
        buf.clear();
        auths.resize(30);
        loadtext.write_xy((x + (w / 2)) - (auths.length() * 3), y, auths, WHITE, 1);
    }

    // Draw description box
    SDL_Rect descbox = {160 - (255 / 2), Sint16((area.y + area.h) + 35), 225, 60};
    myscreen->draw_box(descbox.x, descbox.y, descbox.x + descbox.w, descbox.y + descbox.h, GREY, 1, 1);

    // Print description
    std::string desc = description;
    Sint32 j = 10;

    std::list<std::string> lines = explode(description, '|');
    std::list<std::string>::iterator itr = lines.begin();

    while (desc.size() > 0) {
        if ((j + 10) > descbox.h) {
            break;
        }

        if (itr != lines.end()) {
            loadtext.write_xy(descbox.x + 5, descbox.y + j, *itr, BLACK, 1);
        }

        j += 10;
    }

    y = (descbox.y + descbox.h) + 2;

    // Print contributors
    if (contributors.size() > 0) {
        buf << "Thanks to " << contributors;
        std::string contrib(buf.str());
        buf.clear();
        contrib.resize(60);
        loadtext.write_xy((x + (w / 2)) - (contrib.length() * 3), y, contrib, WHITE, 1);
        y += 10;
    }

    id.resize(60);
    loadtext.write_xy((x + (w / 2)) - (id.length() * 3), y, id, WHITE, 1);
}
