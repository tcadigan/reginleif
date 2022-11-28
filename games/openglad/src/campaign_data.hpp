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
#ifndef __CAMPAIGN_DATA_HPP__
#define __CAMPAIGN_DATA_HPP__

#include "pixie.hpp"

#include <filesystem>
#include <list>
#include <string>

#include <SDL2/SDL.h>

class CampaignData
{
public:
    CampaignData(std::filesystem::path const &id);
    ~CampaignData();

    bool load();
    bool save();
    bool save_as(std::filesystem::path const &new_id);

    std::string getDescriptionLine(Uint32 i);

    std::filesystem::path id;
    std::string title;
    float rating;
    std::string version;
    std::string authors;
    std::string contributors;
    std::list<std::string> description;
    Sint32 suggested_power;
    Sint32 first_level;

    Sint32 num_levels;

    Pixie *icon;
};

#endif
