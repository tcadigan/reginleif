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

#include "input.hpp"
#include "pixien.hpp"
#include "video_screen.hpp"
#include "virtual_button.hpp"

void draw_backdrop()
{
    Sint32 i;

    for (i = 0; i < 5; ++i) {
        if (backdrops[i]) {
            backdrops[i]->draw(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
                               myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                               myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
        }
    }
}

void draw_buttons(Button *buttons, Sint32 numbuttons)
{
    Sint32 i;

    for (i = 0; i < numbuttons; ++i) {
        if (buttons[i].hidden || buttons[i].no_draw) {
            continue;
        }

        allbuttons[i]->vdisplay();
        myscreen->draw_box(allbuttons[i]->xloc - 1,
            allbuttons[i]->yloc - 1, allbuttons[i]->xend, allbuttons[i]->yend,
            0, 0, 1);
    }
}

Button::Button(std::string const &label, Sint32 hotkey, Sint32 x, Sint32 y,
               Sint32 w, Sint32 h, Sint32 callback_ID, Sint32 callback_arg,
               MenuNav const &nav, bool hidden)
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
{
}

Button::Button(Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 callback_ID,
               Sint32 callback_arg, MenuNav const &nav, bool hidden,
               bool no_draw)
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
{
}
