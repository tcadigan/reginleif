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

#include "button.hpp"

#include "pixien.hpp"

#include <string>

#include <SDL2/SDL.h>

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

extern VirtualButton *allbuttons[MAX_BUTTONS];

VirtualButton *init_buttons(Button *buttons, Sint32 numbuttons);
