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
#ifndef __PICKER_HPP__
#define __PICKER_HPP__

#include "button-fwd.hpp"
#include "guy-fwd.hpp"

#include <SDL2/SDL.h>
#include <string>

void picker_main();
bool yes_or_no_prompt(std::string const &title, std::string const &message, bool default_value);
bool no_or_yes_prompt(std::string const &title, std::string const &message, bool default_value);
void popup_dialog(std::string const &title, std::string const &message);
void timed_dialog(std::string const &message, float delay_seconds=3.0f);
void draw_highlight(Button const &b);
void draw_highlight_interior(Button const &b);
bool handle_menu_nav(Button *buttons, Sint32 &highlighted_button, Sint32 &retvalue, bool use_global_vbuttons=true);
std::string get_family_string(Sint16 family);
Sint32 beginmenu(Sint32 arg1);
Sint32 set_player_mode(Sint32 howmany);
void quit(Sint32 arg1);
Sint32 create_team_menu(Sint32 arg1); // Create/modify team members
Sint32 create_view_menu(Sint32 arg1); // View team members
Sint32 create_train_menu(Sint32 arg1); // Edit or sell team members
Sint32 create_hire_menu(Sint32 arg1); // Purchase new team members
Sint32 create_load_menu(Sint32 arg1); // Load a team
Sint32 create_save_menu(Sint32 arg1); // Save a team
Sint32 create_detail_menu(Guy *arg1); // Detailed character information
Sint32 go_menu(Sint32 arg1); // Run glad...
Sint32 add_guy(Sint32 ignoreme);
Sint32 edit_guy(Sint32 arg1); // Transfer stats...hardcoded
Sint32 name_guy(Sint32 arg); // Name the current guy
Sint32 cycle_guy(Sint32 whichway);
Sint32 cycle_team_guy(Sint32 whichway);
Sint32 increase_stat(Sint32 arg1, Sint32 howmuch=1); // Increase a guy's stats
Sint32 decrease_stat(Sint32 arg1, Sint32 howmuch=1); // Decrease a guy's stats
Sint32 do_save(Sint32 arg1); // Dummy function for saving team list
Sint32 do_load(Sint32 arg1); // Dummy function for loading team list
Sint32 do_set_scen_level(Sint32 arg1);
Sint32 do_pick_campaign(Sint32 arg1);
Sint32 set_difficulty();
Sint32 change_teamnum(Sint32 arg);
Sint32 change_hire_teamnum(Sint32 arg);
Sint32 change_allied();
Sint32 level_editor();
Sint32 main_options();
Sint32 overscan_adjust(Sint32 arg);
Uint32 calculate_exp(Sint32 level);
Sint32 calculate_level(Uint32 temp_exp);

#endif
