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

#include "gloader-fwd.hpp"
#include "guy.hpp"
#include "input.hpp"
#include "obmap.hpp"
#include "pixien.hpp"
#include "screen-fwd.hpp"
#include "text.hpp"

// Definition of a button
enum ButtonDefinitionEnum : Uint8 {
    BUT_STR = 0,
    BUT_DEX = 1,
    BUT_CON = 2,
    BUT_INT = 3,
    BUT_ARMOR = 4,
    BUT_LEVEL = 5
};

// Button edge-colors
enum ButtonEdgeEnum : Uint8 {
    BUTTON_BOTTOM = 11, // 10
    BUTTON_RIGHT = 12, // 11
    BUTTON_FACING = 13, // 12
    BUTTON_LEFT = 14, // 13
    BUTTON_TOP = 15 // 14
};

#define MAX_BUTTONS 50 // Max buttons per screen

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
    static MenuNav UpRight(Sint32 up, Sint32 right);
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

class Button
{
public:
    std::string label;
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

    Button(std::string const &label, Sint32 hotkey, Sint32 x, Sint32 y,
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

    Button(Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 callback_ID,
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

class VirtualButton
{
public:
    VirtualButton(); // this should only be used for pointers!!
    VirtualButton(Sint32 xpos, Sint32 ypos, Sint32 wide, Sint32 high,
        Sint32 func(Sint32), Sint32 pass, std::string const &msg, Sint32 hot);
    VirtualButton(Sint32 xpos, Sint32 ypos, Sint32 wide, Sint32 high,
        Sint32 func_code, Sint32 pass, std::string const &msg, Sint32 hot);
    VirtualButton(Sint32 xpos, Sint32 ypos, Sint32 wide, Sint32 high,
        Sint32 func_code, Sint32 pass, std::string const &msg, Uint8 family,
        Sint32 hot);
    ~VirtualButton();

    void set_graphic(Uint8 family);
    Sint32 leftclick(Button *buttons=nullptr); // Checks all buttons for the click
    Sint32 leftclick(Sint32 whichone); // Clicks this vbutton
    Sint32 rightclick(Button *buttons=nullptr); // Is called when the button is right clicked
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
    Sint32 (*fun)(Sint32 arg1); // The function this button calls when clicked, with one arg
    Sint32 myfunc;
    Sint32 arg; // The arg to be passed to the function when called
    VirtualButton *next; // A pointer to the next button
    VirtualButton *prev; // A pointer to the previous button
    Uint8 had_focus; // Did we recently have focus?
    Uint8 do_outline; // force an outline
    Uint8 depressed;
    PixieN *mypixie;
    Sint32 hotkey;
    Uint8 color;
    bool hidden;
    bool no_draw; // Does not draw but still accept clicks
};

extern VideoScreen *myscreen;
extern VirtualButton *allbuttons[MAX_BUTTONS];

VirtualButton *init_buttons(Button *buttons, Sint32 numbuttons);
void draw_backdrop();
void draw_buttons(Button *buttons, Sint32 numbuttons);

// Function definitions...
enum FunctionDefinitionEnum : Uint8 {
    BEGIN_MENU = 1,
    CREATE_TEAM_MENU = 2,
    SET_PLAYER_MODE = 3,
    QUIT_MENU = 4,
    CREATE_VIEW_MENU = 5,
    CREATE_TRAIN_MENU = 6,
    CREATE_HIRE_MENU = 7,
    CREATE_LOAD_MENU = 8,
    CREATE_SAVE_MENU = 9,
    GO_MENU = 10,
    RETURN_MENU = 11,
    CYCLE_TEAM_GUY = 12,
    DECREASE_STAT = 13,
    INCREASE_STAT = 14,
    EDIT_GUY = 15,
    CYCLE_GUY = 16,
    ADD_GUY = 17,
    DO_SAVE = 18,
    DO_LOAD = 19,
    NAME_GUY = 20,
    CREATE_DETAIL_MENU = 21,
    NULL_MENU = 22,
    DO_SET_SCEN_LEVEL = 23,
    SET_DIFFICULTY = 24,
    CHANGE_TEAM = 25,
    ALLIED_MODE = 26,
    CHANGE_HIRE_TEAM = 27,
    YES_OR_NO = 28,
    DO_PICK_CAMPAIGN = 29,
    DO_LEVEL_EDIT = 30,
    MAIN_OPTIONS = 31,
    TOGGLE_SOUND = 32,
    TOGGLE_RENDERING_ENGINE = 33,
    TOGGLE_FULLSCREEN = 34,
    OVERSCAN_ADJUST = 35,
    TOGGLE_MINI_HP_BAR = 36,
    TOGGLE_HIT_FLASH = 37,
    TOGGLE_HIT_RECOIL = 38,
    TOGGLE_ATTACK_LUNGE = 39,
    TOGGLE_HIT_ANIM = 40,
    TOGGLE_DAMAGE_NUMBERS = 41,
    TOGGLE_HEAL_NUMBERS = 42,
    TOGGLE_GORE = 43,
    RESTORE_DEFAULT_SETTINGS = 44
};

#endif
