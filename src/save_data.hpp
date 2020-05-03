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
#ifndef __SAVE_DATA_HPP__
#define __SAVE_DATA_HPP__

#include <list>
#include <map>
#include <set>
#include <string>

#include <SDL2/SDL.h>

#include "guy.hpp"
#include "walker.hpp"

// Max number of guys on a team
#define MAX_TEAM_SIZE 24

class SaveData
{
public:
    SaveData();
    ~SaveData();

    void reset();

    // Copy team from the guys in an oblist
    void update_guys(std::list<Walker *> &oblist);
    bool load(std::string const &filename);
    bool save(std::string const &filename);

    bool is_level_completed(Sint32 level_index) const;
    Sint32 get_num_levels_completed(std::string const &campaign) const;

    void add_level_completed(std::string const &campaign, Sint32 level_index);
    void reset_campaign(std::string const &campaign);

    std::string save_name;
    std::string current_campaign;
    Sint16 scen_num;
    std::map<std::string, std::set<Sint32>> completed_levels;
    std::map<std::string, Sint32> current_levels;
    Uint32 score;
    Uint32 m_score[4];
    Uint32 totalcash;
    Uint32 m_totalcash[4];
    Uint32 totalscore;
    Uint32 m_totalscore[4];
    Uint16 my_team;

    // Guys used for training and stuff. After a mission, the team is picked
    // from the LevelData's oblist for saving.
    Guy *team_list[MAX_TEAM_SIZE];
    Uint8 team_size;
    // numviews
    Uint8 numplayers;
    Uint16 allied_mode;
};

#endif
