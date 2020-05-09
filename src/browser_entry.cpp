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
#include "browser_entry.hpp"

#include <sstream>

BrowserEntry::BrowserEntry(VideoScreen *screenp, Sint32 index, Sint32 scen_num)
    : level_data(scen_num)
    , screenp(screenp)
{
    level_data.load();
    screenp->viewob[0]->myradar->start(level_data,
                                       screenp->viewob[0]->endx, screenp->viewob[0]->endy,
                                       get_yloc());

    Sint32 w = get_xview();
    Sint32 h = get_yview();

    mapAreas.w = w;
    mapAreas.h = h;
    mapAreas.x = 10;
    mapAreas.y = 5 + (65 * index);

    screenp->viewob[0]->myradar->xloc = (mapAreas.x + (mapAreas.w / 2)) - (w / 2);
    screenp->viewob[0]->myradar->yloc = mapAreas.y + 10;

    getLevelStats(level_data, &max_enemy_level, &average_enemy_level,
                  &num_enemies, &difficulty, exits);

    // Store this level's info
    level_name = level_data.title;

    if (level_name.size() > 20) {
        level_name.resize(20);
        level_name.append("...");
    }

    scentextlines = level_data.description.size();

    Sint32 i = 0;
    for (auto const &e : level_data.description) {
        scentext[i] = e;
        scentext[i].resize(80);
        ++i;

        if (i >= 80) {
            break;
        }
    }
}

BrowserEntry::~BrowserEntry()
{
}

void BrowserEntry::updateIndex(Sint32 index)
{
    Sint32 w = get_xview();
    mapAreas.y = 5 + (65 * index);
    screenp->viewob[0]->myradar->xloc = (mapAreas.x + (mapAreas.w / 2)) - (w / 3);
    screenp->viewob[0]->myradar->yloc = mapAreas.y + 10;
}

void BrowserEntry::draw(VideoScreen *screenp)
{
    Sint32 x = get_xloc();
    Sint32 y = get_yloc();
    Sint32 w = get_xview();
    Sint32 h = get_yview();

    myscreen->draw_button(x - 2, y - 2, (x + w) + 2, (y + h) + 2, 1, 1);
    // Draw radar
    screenp->viewob[0]->myradar->draw(level_data, nullptr);

    Text &loadtext = myscreen->text_normal;
    loadtext.write_xy(mapAreas.x, mapAreas.y, level_name, DARK_BLUE, 1);

    std::stringstream buf;
    std::string temp;
    buf << "ID: " << level_data.id;
    temp = buf.str();
    buf.clear();
    temp.resize(30);
    loadtext.write_xy((x + w) + 5, y, temp, WHITE, 1);

    buf << "Enemies: " << num_enemies;
    temp = buf.str();
    buf.clear();
    temp.resize(30);
    loadtext.write_xy((x + w) + 5, y + 8, temp, WHITE, 1);

    buf << "Max level: " << max_enemy_level;
    temp = buf.str();
    buf.clear();
    temp.resize(30);
    loadtext.write_xy((x + w) + 5, y + 16, temp, WHITE, 1);

    buf << "Avg level: " << average_enemy_level;
    temp = buf.str();
    buf.clear();
    temp.resize(30);
    loadtext.write_xy((x + w) + 5, y + 24, temp, WHITE, 1);

    buf << "Difficulty: " << difficulty;
    temp = buf.str();
    buf.clear();
    temp.resize(30);
    loadtext.write_xy((x + w) + 5, y + 32, temp, RED, 1);

    if (!exits.empty()) {
        buf << "Exits: ";
        bool first = true;
        for (auto const &e : exits) {
            if (!first) {
                buf << ", ";
            } else {
                first = false;
            }

            buf << e;
        }

        temp = buf.str();
        buf.clear();

        if (temp.size() > 19) {
            temp.resize(16);
            temp.append("...");
        }

        loadtext.write_xy((x + w) + 5, y + 40, temp, WHITE, 1);
    }
}

void BrowserEntry::getLevelStats(LevelData &level_data, Sint32 *max_enemy_level,
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
                level_sum = ob->stats.level;
                difficulty_sum += diff_per_level * ob->stats.level;

                if (ob->stats.level > max_level) {
                    max_level = ob->stats.level;
                }
            } else {
                difficulty_sum_friends += (diff_per_level * ob->stats.level);
            }

            break;
        }
    }

    // Go through effects
    for (auto const ob : level_data.fxlist) {
        switch (ob->query_order()) {
        case ORDER_TREASURE:
            if (ob->query_family() == FAMILY_EXIT) {
                exits.push_back(ob->stats.level);
            }

            break;
        }
    }

    *num_enemies = num;
    *max_enemy_level = max_level;

    if (num == 0) {
        *average_enemy_level = 0;
    } else {
        *average_enemy_level = level_sum / num;
    }

    exits.sort();
    exits.unique();
}

Sint16 BrowserEntry::get_xloc()
{
    return screenp->viewob[0]->myradar->xloc;
}

Sint16 BrowserEntry::get_yloc()
{
    return screenp->viewob[0]->myradar->yloc;
}

Sint16 BrowserEntry::get_xview()
{
    return screenp->viewob[0]->myradar->xview;
}

Sint16 BrowserEntry::get_yview()
{
    return screenp->viewob[0]->myradar->yview;
}
