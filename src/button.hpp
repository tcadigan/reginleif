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
#ifndef __BUTTON_HPP__
#define __BUTTON_HPP__

#include <cmath>

#include "base.hpp"
#include "gloader.hpp"
#include "input.hpp"
#include "obmap.hpp"
#include "pixien.hpp"
#include "screen.hpp"
#include "text.hpp"

class vbutton;
class button;

// Definition of a button
#define BUT_STR 0
#define BUT_DEX 1
#define BUT_CON 2
#define BUT_INT 3
#define BUT_ARMOR 4
#define BUT_LEVEL 5

// Button edge-colors
#define BUTTON_FACING (char)13 // 12
#define BUTTON_TOP (char)15 // 14
#define BUTTON_BOTTOM (char)11 // 10
#define BUTTON_LEFT (char)14 // 13
#define BUTTON_RIGHT (char)12 // 11

#define MAX_BUTTONS 50 // Max buttons per screen

extern screen *myscreen;
extern vbutton *allbuttons[MAX_BUTTONS]

vbutton *init_buttons(button *buttons, Sint32 numbuttons);
void draw_backdrop();
void draw_buttons(button *buttons, Sint32 numbuttons);

// These are for picker...
Sint32 score_panel(screen *myscreen);
Sint32 mainmenu(Sint32 arg1);
Sint32 beginmenu(Sint32 arg1);
void quit(Sint32 arg1);
Sint32 load1(Sint32 arg1); // Begin a preset scenario...
Sint32 load2(Sint32 arg1);
Sint32 load3(Sint32 arg1);
Sint32 create_team_menu(Sint32 arg1); // Create/modify team members
Sint32 create_detail_menu(guy *arg1); // Detailed character information
Sint32 create_view_menu(Sint32 arg1); // View team members
Sint32 create_hire_menu(Sint32 arg1); // Purchase new team members
Sint32 create_train_menu(Sint32 arg1); // Edit or sell team members
Sint32 create_load_menu(Sint32 arg1); // Load a team
Sint32 create_save_menu(Sint32 arg1); // Save a team
Sint32 go_menu(Sint32 arg1); // Run glad...
Sint32 increase_stat(Sint32 arg1, Sint32 howmuch=1); // Increase a guy's stats
Sint32 decrease_stat(Sint32 arg1, Sint32 howmuch=1); // Decrease a guy's stats
Uint32 calculate_hire_cost();
Uint32 calculate_train_cost(guy *oldguy);
Sint32 cycle_guy(Sint32 whichway);
Sint32 cycle_team_guy(Sint32 whichway);
Sint32 add_guy(Sint32 ignoreme);
Sint32 edit_guy(Sint32 arg1); // Transfer stats...hardcoded
Sint32 do_save(Sint32 arg1); // Dummy function for saving team list
Sint32 do_load(Sint32 arg1); // Dummy function for loading team list
Sint32 delete_all(); // Delete entire team
Sint32 delete_first(); // Delete first guy on team list
Sint32 how_many(Sint32 whatfamily); // How many guys of family X on the team?
void statscopy(guy *dest, guy *source); // Copy stats from source => dest
Sint32 set_player_mode(Sint32 howmany);
Sint32 calculate_level(Uint32 temp_exp);
Uint32 calculate_exp(Sint32 level);
Sint32 return_menu(Sint32 arg);
Sint32 name_guy(Sint32 arg); // Name the current guy
Sint32 do_set_scen_level(Sint32 arg1);
Sint32 do_pick_campaign(Sint32 arg1);
Sint32 set_difficulty();
Sint32 change_teamnum(Sint32 arg);
Sint32 change_hire_teamnum(Sint32 arg);
Sint32 change_allied();
Sint32 level_editor();
Sint32 main_options();
Sint32 overscan_adjust(Sint32 arg);

// Function definitions...
#define BEGIN_MENU 1
#define CREATE_TEAM_MENU 2
#define SET_PLAYER_MODE 3
#define QUIT_MENU 4
#define CREATE_VIEW_MENU 5
#define CREATE_TRAIN_MENU 6
#define CREATE_HIRE_MENU 7
#define CREATE_LOAD_MENU 8
#define CREATE_SAVE_MENU 9
#define GO_MENU 10
#define RETURN_MENU 11
#define CYCLE_TEAM_GUY 12
#define DECREASE_STAT 13
#define INCREASE_STAT 14
#define EDIT_GUY 15
#define CYCLE_GUY 16
#define ADD_GUY 17
#define DO_SAVE 18
#define DO_LOAD 19
#define NAME_GUY 20
#define CREATE_DETAIL_MENU 21
#define NULL_MENU 22
#define DO_SET_SCEN_LEVEL 23
#define SET_DIFFICULTY 24
#define CHANGE_TEAM 25
#define ALLIED_MODE 26
#define CHANGE_HIRE_TEAM 27
#define YES_OR_NO 28
#define DO_PICK_CAMPAIGN 29
#define DO_LEVEL_EDIT 30
#define MAIN_OPTIONS 31
#define TOGGLE_SOUND 32
#define TOGGLE_RENDERING_ENGINE 33
#define TOGGLE_FULLSCREEN 34
#define OVERSCAN_ADJUST 35
#define TOGGLE_MINI_HP_BAR 36
#define TOGGLE_HIT_FLASH 37
#define TOGGLE_HIT_RECOIL 38
#define TOGGLE_ATTACK_LUNGE 39
#define TOGGLE_HIT_ANIM 40
#define TOGGLE_DAMAGE_NUMBERS 41
#define TOGGLE_HEAL_NUMBERS 42
#define TOGGLE_GORE 43
#define RESTORE_DEFAULT_SETTINGS 44

// Holds array indices for navigating menu buttons
class MenuNav
{
public:
    // Array indices for the button to move to. -1 is invalid/unused direction
    Sint32 up;
    Sint32 down;
    Sint32 left;
    Sint32 right;

    static MenuNav Up(Sint32 up);
    static MenuNav Down(Sint32 down);
    static MenuNav Left(Sint32 left);
    static MenuNav Right(Sint32 right);
    static MenuNav UpDown(Sint32 up, Sint32 down);
    static MenuNav UpLeft(Sint32 up, Sint32 left);
    static Menunav upRight(Sint32 up, Sint32 right);
    static MenuNav UpDownLeft(Sint32 up, Sint32 down, Sint32 left);
    static MenuNav UpDownRight(Sint32 up, Sint32 down, Sint32 right);
    static MenuNav UpLeftRight(Sint32 up, Sint32 left, Sint32 right);
    static MenuNav DownLeft(Sint32 down, Sint32 left);
    static MenuNav DownRight(Sint32 down, Sint32 right);
    static MenuNav DownLeftRight(Sint32 down, Sint32 left, Sint32 right);
    static MenuNav LeftRight(Sint32 left, Sint32 right);
    static MenuNav UpDownLeftRight(Sint32 up, Sint32 down, Sint32 left, Sint32 right);
    static MenuNav All(Sint32 up, Sint32 down, Sint32 left, Sint32 right);
    static MenuNav None();

private:
    MenuNav();
    MenuNav(Sint32 up, Sint32 down, Sint32 left, Sint32 right);
};

class button
{
public:
    std::string lable;
    Sint32 hotkey;
    Sint32 x;
    Sint32 y;
    Sint32 sizex;
    Sint32 sizey;
    Sint32 myfun; // Callback ID
    Sint32 arg1; // Argument to function fun
    MenuNav nav;
    bool hidden; // Does not draw or accept clicks
    bool no_draw; // Does not draw but still accepts clicks

    button(std::string const &label, Sint32 hotkey, Sint32 x, Sint32 y,
        Sint32 w, Sint32 h, Sint32 callback_ID, Sint32 callback_arg,
        MenuNav const &nav, bool hidden=false)
    : label(label)
        , hotkey(hotkey)
        , x(x)
        , y(y)
        , sizex(w)
        , sizey(h)
        , myfun(callback_ID)
        , arg1(callback_arg)
        , nav(nav)
        , hidden(hidden)
        , no_draw(false)
        {}

    button(Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 callback_ID,
        Sint32 callback_arg, MenuNav const &nav, bool hidden=false,
        bool no_draw=false)
    : hotkey(KEYSTATE_UNKNOWN)
        , x(x)
        , y(y)
        , sizex(w)
        , sizey(h)
        , myfun(callback_ID)
        , arg1(callback_arg)
        , nav(nav)
        , hidden(hidden)
        , no_draw(no_draw)
        {}
};

class vbutton
{
public:
    vbutton(); // this should only be used for pointers!!
    vbutton(Sint32 xpos, Sint32 ypos, Sint32 wide, Sint32 high,
        Sint32 func(Sint32), Sint32 pass, std::string const &msg, Sint32 hot);
    vbutton(Sint32 xpos, Sint32 ypos, Sint32 wide, Sint32 high,
        Sint32 func_code, Sint32 pass, std::string const &msg, int hot);
    vbutton(Sint32 xpos, Sint32 ypos, Sint32 wide, Sint32 high,
        Sint32 func_code, Sint32 pass, std::string const &msg, Uint8 family,
        int hot);
    ~vbutton();

    void set_graphic(Uint8 family);
    Sint32 leftclick(button *buttons=nullptr); // Checks all buttons for the click
    Sint32 leftclick(Sint32 whichone); // Clicks this vbutton
    Sint32 rightclick(button *buttons=nullptr); // Is called when the button is right clicked
    Sint32 rightclick(Sint32  whichone); // Clicks this vbutton
    Sint32 mouse_on(); // Determines if mouse is on this button, returns 1 it true
    void vdisplay();
    void vdisplay(Sint32 status); // Display depressed
    Sint32 do_call(Sint32 whatfunc, Sint32 arg);
    Sint32 do_call_right(Sint32 whatfunc, Sint32 arg); // For right-button

    Sint32 xloc; // The x position in screen-coords
    Sint32 yloc; // The y position in screen-coords
    std::string label; // The label on the button
    Sint32 width; // The button's width in pixels
    Sint32 height; // The button's height in pixels
    Sint32 xend; // xloc + width
    Sint32 yend; // yloc + height
    Sint32 (*func)(Sint32 arg1); // The function this button calls when clicked, with one arg
    Sint32 myfunc;
    Sint32 arg; // The arg to be passed to the function when called
    vbutton *next; // A pointer to the next button
    vbutton *prev; // A pointer to the previous button
    Uint8 had_focus; // Did we recently have focus?
    Uint8 do_outline; // force an outline
    Uint8 depressed;
    pixieN *mypixie;
    Sint32 hotkey;
    Uint8 color;
    bool hidden;
    bool no_draw; // Does not draw but still accept clicks
};

#endif
