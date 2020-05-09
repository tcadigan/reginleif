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
#ifndef __GPARSER_HPP__
#define __GPARSER_HPP__

#include <map>
#include <string>
#include <vector>

#include <SDL2/SDL.h>

class ConfigStore
{
public:
    bool load_settings();
    void commandline(Sint32 argc, std::vector<std::string> const &args);
    bool save_settings();

    void apply_setting(std::string const &category, std::string const &setting, std::string const &value);
    std::string get_setting(std::string const &category, std::string const &setting);
    bool is_on(std::string const &category, std::string const &setting);
    std::map<std::string, std::map<std::string, std::string>> data;
};

extern ConfigStore cfg;

void toggle_effect(std::string const &category, std::string const &setting);
void toggle_rendering_engine();

#endif
