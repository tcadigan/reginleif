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
#include "selection_info.hpp"

#include "walker.hpp"

SelectionInfo::SelectionInfo()
    : valid(false)
    , x(0)
    , y(0)
    , w(GRID_SIZE)
    , h(GRID_SIZE)
    , order(ORDER_LIVING)
    , family(FAMILY_SOLDIER)
    , level(1)
    , target(nullptr)
{
}

SelectionInfo::SelectionInfo(Walker *target)
    : valid(false)
    , x(0)
    , y(0)
    , w(GRID_SIZE)
    , h(GRID_SIZE)
    , order(ORDER_LIVING)
    , family(FAMILY_SOLDIER)
    , level(1)
    , target(target)
{
    set(target);
}

void SelectionInfo::clear()
{
    valid = false;
    name.clear();
    x = 0;
    y = 0;
    w = GRID_SIZE;
    h = GRID_SIZE;
    order = ORDER_LIVING;
    family = FAMILY_SOLDIER;
    level = 1;
}

void SelectionInfo::set(Walker *target)
{
    if (target == nullptr) {
        clear();
    } else {
        valid = true;
        name = target->stats->name;
        x = target->xpos;
        y = target->ypos;
        w = target->sizex;
        h = target->sizey;
        order = target->query_order();
        family = target->query_family();
        level = target->stats->level;
        this->target = target;
    }
}

Walker *SelectionInfo::get_object(LevelData *level)
{
    if (!valid) {
        return nullptr;
    }

    return target;
}
