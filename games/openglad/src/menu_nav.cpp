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
#include "menu_nav.hpp"

MenuNav MenuNav::Up(Sint32 up)
{
    return MenuNav(up, -1, -1, -1);
}

MenuNav MenuNav::Down(Sint32 down)
{
    return MenuNav(-1, down, -1, -1);
}

MenuNav MenuNav::Left(Sint32 left)
{
    return MenuNav(-1, -1, left, -1);
}

MenuNav MenuNav::Right(Sint32 right)
{
    return MenuNav(-1, -1, -1, right);
}

MenuNav MenuNav::UpDown(Sint32 up, Sint32 down)
{
    return MenuNav(up, down, -1, -1);
}

MenuNav MenuNav::UpLeft(Sint32 up, Sint32 left)
{
    return MenuNav(up, -1, left, -1);
}

MenuNav MenuNav::UpRight(Sint32 up, Sint32 right)
{
    return MenuNav(up, -1, -1, right);
}

MenuNav MenuNav::UpDownLeft(Sint32 up, Sint32 down, Sint32 left)
{
    return MenuNav(up, down, left, -1);
}

MenuNav MenuNav::UpDownRight(Sint32 up, Sint32 down, Sint32 right)
{
    return MenuNav(up, down, -1, right);
}

MenuNav MenuNav::UpLeftRight(Sint32 up, Sint32 left, Sint32 right)
{
    return MenuNav(up, -1, left, right);
}

MenuNav MenuNav::DownLeft(Sint32 down, Sint32 left)
{
    return MenuNav(-1, down, left, -1);
}

MenuNav MenuNav::DownRight(Sint32 down, Sint32 right)
{
    return MenuNav(-1, down, -1, right);
}

MenuNav MenuNav::DownLeftRight(Sint32 down, Sint32 left, Sint32 right)
{
    return MenuNav(-1, down, left, right);
}

MenuNav MenuNav::LeftRight(Sint32 left, Sint32 right)
{
    return MenuNav(-1, -1, left, right);
}

MenuNav MenuNav::UpDownLeftRight(Sint32 up, Sint32 down, Sint32 left, Sint32 right)
{
    return MenuNav(up, down, left, right);
}

MenuNav MenuNav::All(Sint32 up, Sint32 down, Sint32 left, Sint32 right)
{
    return MenuNav(up, down, left, right);
}

MenuNav MenuNav::None()
{
    return MenuNav();
}

MenuNav::MenuNav()
    : up(-1)
    , down(-1)
    , left(-1)
    , right(-1)
{
}

MenuNav::MenuNav(Sint32 up, Sint32 down, Sint32 left, Sint32 right)
    : up(up)
    , down(down)
    , left(left)
    , right(right)
{
}
