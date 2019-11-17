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
#include "simple_button.hpp"

#include "screen.hpp"

SimpleButton::SimpleButton(std::string const &label, Sint32 x, Sint32 y, Uint32 w, Uint32 h, bool remove_border, bool draw_top_separator)
    : label(label)
    , remove_border(remove_border)
    , draw_top_separator(draw_top_separator)
    , centered(false)
{
    set_colors_normal();

    area.x = x;
    area.y = y;
    area.w = w;
    area.h = h;
}

void SimpleButton::draw(VideoScreen *myscreen)
{
    myscreen->draw_button_colored(area.x, area.y, (area.x + area.w) - 1, (area.y + area.h) - 1,
                                  !remove_border, base_color, high_color, shadow_color);

    if (remove_border && draw_top_separator) {
        myscreen->hor_line(area.x, area.y, area.w, shadow_color);
    }

    Text &mytext = myscreen->text_normal;

    if (centered) {
        mytext.write_xy((area.x + (area.w / 2)) - (3 * label.size()),
                        (area.y + (area.h / 2)) - 2,
                        label.c_str(), text_color, 1);
    } else {
        mytext.write_xy(area.x + 2, (area.y + (area.h / 2)) - 2, label.c_str(), text_color, 1);
    }
}

bool SimpleButton::contains(Sint32 x, Sint32 y) const
{
    return ((area.x <= x)
            && (x < (area.x + area.w))
            && (area.y <= y)
            && (y < (area.y + area.h)));
}

void SimpleButton::set_colors_normal()
{
    text_color = DARK_BLUE;
    base_color = 13;
    high_color = 14;
    shadow_color = 12;
}

void SimpleButton::set_colors_enabled()
{
    text_color = 80;
    base_color = 64;
    high_color = 72;
    shadow_color = 74;
}

void SimpleButton::set_colors_disabled()
{
    text_color = 3;
    base_color = 10;
    high_color = 12;
    shadow_color = 14;
}

void SimpleButton::set_colors_active()
{
    text_color = WHITE;
    base_color = ORANGE_START;
    high_color = ORANGE_START + 3;
    shadow_color = ORANGE_START + 5;
}
