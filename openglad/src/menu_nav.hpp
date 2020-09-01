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
#ifndef __MENU_NAV_HPP__
#define __MENU_NAV_HPP__

#include <SDL2/SDL.h>

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

#endif
