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
#ifndef __LEVEL_EDITOR_HPP__
#define __LEVEL_EDITOR_HPP__

#include "level_data.hpp"
#include "level_editor_data.hpp"
#include "rectf.hpp"
#include "screen.hpp"
#include "simple_button.hpp"
#include "walker.hpp"

#include <SDL2/SDL.h>

#include <list>
#include <set>
#include <string>
#include <utility>
#include <vector>

enum EventTypeEnum : Uint8 {
    HANDLED_EVENT,
    TEXT_EVENT,
    SCROLL_EVENT,
    MOUSE_MOTION_EVENT,
    MOUSE_DOWN_EVENT,
    MOUSE_UP_EVENT,
    KEY_DOWN_EVENT
};

class LevelEditor
{
public:
    LevelEditor();

private:
    LevelEditorData data;
};

bool does_campaign_exist(std::string const &campaign_id);
bool create_new_campaign(std::string const &campaign_id);
void importCampaignPicker();
void shareCampaign(VideoScreen *myscreen);
bool prompt_for_string_block(std::string const &message, std::list<std::string> &result);
bool prompt_for_string(std::string const &message, std::string &result);
bool button_showing(std::list<std::pair<SimpleButton *, std::set<SimpleButton *>>> const &ls, SimpleButton *elem);
bool activate_sub_menu_button(Sint32 mx, Sint32 my, std::list<std::pair<SimpleButton *, std::set<SimpleButton *>>> &current_menu, SimpleButton &button, bool is_in_top_menu=false);
bool activate_menu_choice(Sint32 mx, Sint32 my, LevelEditorData &data, SimpleButton &button, bool is_in_top_menu=false);
bool activate_menu_toggle_choice(Sint32 mx, Sint32 my, LevelEditorData &data, SimpleButton &button, bool is_in_top_menu=false);
void get_connected_level_exits(Sint32 current_level, std::list<Sint32> const &levels, std::set<Sint32> &connected, std::list<std::string> &problems);
bool is_in_selection(Walker *w, std::vector<SelectionInfo> const &selection);
void add_contained_objects_to_selection(LevelData *level, Rectf const &area, std::vector<SelectionInfo> &selection);
bool are_objects_outside_area(LevelData *level, Sint32 x, Sint32 y, Sint32 w, Sint32 h);
EventTypeEnum handle_basic_editor_event(SDL_Event const &event);
Sint32 level_editor();
void set_screen_pos(VideoScreen *myscreen, Sint32 x, Sint32 y);
Uint8 get_random_matching_tile(Sint32 whatback);
Sint32 check_collide(Sint32 x, Sint32 y, Sint32 xsize, Sint32 ysize,
                     Sint32 x2, Sint32 y2, Sint32 xsize2, Sint32 ysize2);
bool some_hit(Sint32 x, Sint32 y, Walker *ob, LevelData *data);

#endif
