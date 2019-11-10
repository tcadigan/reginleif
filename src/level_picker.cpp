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

#include "button.hpp"
#include "guy.hpp"
#include "level_data.hpp"
#include "radar.hpp"
#include "stats.hpp"
#include "text.hpp"
#include "walker.hpp"

#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <list>
#include <string>
#include <sys/stat.h>

#define MAX_TEAM_SIZE 24 // Max number of guys on a team
#define OK 4

bool yes_or_no_prompt(Uint8 const *title, Uint8 const *message, bool default_value);
bool prompt_for_string(std::string const &message, std::string &result);
void draw_highlight_interior(Button const &b);
void draw_highlight(Button const &b);
bool handle_menu_nav(Button *buttons, Sint32 &highlighted_button,
                     Sint32 &retvalue, bool use_global_vbuttons=true);

void getLevelStats(LevelData &level_data, Sint32 *max_enemy_level,
                   float *average_enemy_level, Sint32 *num_enemies,
                   float *difficulty, std::list<Sint32> &exits)
{
    
