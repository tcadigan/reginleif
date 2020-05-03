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
#include "troop_result.hpp"

#include "gloader.hpp"
#include "guy.hpp"
#include "picker.hpp"
#include "results_screen.hpp"
#include "screen.hpp"
#include "view.hpp"
#include "walker.hpp"

TroopResult::TroopResult(Guy *before, Walker *after)
    : before(before)
    , after(after)
{
    if ((after != nullptr) && (after->myguy == nullptr)) {
        after = nullptr;
    }
}

std::string TroopResult::get_name()
{
    if (before != nullptr) {
        return before->name;
    }

    if (after != nullptr) {
        return after->myguy->name;
    }

    return std::string();
}

Uint8 TroopResult::get_family()
{
    Uint8 family = FAMILY_SOLDIER;

    if (before != nullptr) {
        family = before->family;
    }

    if (after != nullptr) {
        family = after->myguy->family;
    }

    return family;
}

std::string TroopResult::get_class_name()
{
    if (before != nullptr) {
        return get_family_string(before->family);
    }

    if (after != nullptr) {
        return get_family_string(after->myguy->family);
    }

    return std::string();
}

Sint32 TroopResult::get_level()
{
    if (after != nullptr) {
        return calculate_level(after->myguy->exp);
    }

    if (before != nullptr) {
        return calculate_level(before->exp);
    }

    return 0;
}

bool TroopResult::gained_level()
{
    if ((after == nullptr) || (before == nullptr)) {
        return false;
    }

    return (calculate_level(after->myguy->exp) > before->get_level());
}

bool TroopResult::lost_level()
{
    if ((after == nullptr) || (before == nullptr)) {
        return false;
    }

    return (calculate_level(after->myguy->exp) < before->get_level());
}

std::vector<std::string> TroopResult::get_gained_specials()
{
    std::vector<std::string> result;

    Sint32 family = get_family();

    Sint32 test1 = get_level() - 1;

    // We're on a special gaining level
    if ((test1 % 3) == 0) {
        // This is the special #
        test1 = (test1 / 3) + 1;

        // Raise this when we have more than 4 specials
        if ((test1 <= 4) && (myscreen->special_name[family][test1] ==  "NONE")) {
            result.push_back(myscreen->special_name[family][test1]);
        }
    }

    return result;
}

float TroopResult::get_xp_base()
{
    if (before == nullptr) {
        return 0.0f;
    }

    if (gained_level()) {
        return 0.0f;
    }

    return ((before->exp - calculate_exp(before->get_level())) / static_cast<float>(calculate_exp(before->get_level() + 1)));
}

float TroopResult::get_xp_gain()
{
    if ((after == nullptr) || (before == nullptr)) {
        return 0.0f;
    }

    if (gained_level()) {
        return ((after->myguy->exp - calculate_exp(before->get_level() + 1)) / static_cast<float>(calculate_exp(before->get_level() + 2)));
    }

    if (lost_level()) {
        return ((after->myguy->exp - before->exp) / static_cast<float>(calculate_exp(before->get_level())));
    }

    return ((after->myguy->exp - before->exp) / static_cast<float>(calculate_exp(before->get_level() + 1)));
}

Sint32 TroopResult::get_tallies()
{
    if (after == nullptr) {
        return 0;
    }

    return after->myguy->scen_kills;
}

float TroopResult::get_hp() const
{
    if (after == nullptr) {
        return 0.0f;
    }

    if (after->myguy->scen_min_hp > after->stats.hitpoints) {
        return 1.0f;
    }

    return (after->myguy->scen_min_hp / after->stats.max_hitpoints);
}

bool TroopResult::is_dead() const
{
    return (get_hp() <= 0.0f);
}

bool TroopResult::is_new() const
{
    return ((before == nullptr) && (after != nullptr));
}

void TroopResult::draw_guy(Sint32 cx, Sint32 cy, Sint32 frame)
{
    Guy *myguy = nullptr;

    if (after != nullptr) {
        myguy = after->myguy;
    } else if (before != nullptr) {
        myguy = before;
    } else {
        return;
    }

    if (!is_dead() && (myguy != nullptr)) {
        show_guy(frame, myguy, cx, cy);
    }
}

// Shows the current guy...
void TroopResult::show_guy(Sint32 frames, Guy *myguy, Sint16 centerx, Sint16 centery)
{
    Walker *mywalker;
    Sint32 i;
    Sint32 newfamily;

    if (!myguy) {
        return;
    }

    frames = abs(frames);

    newfamily = myguy->family;

    mywalker = create_walker(ORDER_LIVING, newfamily);
    mywalker->stats.bit_flags = 0;
    mywalker->curdir = FACE_DOWN;
    mywalker->ani_type = ANI_WALK;

    for (i = 0; i <= ((frames / 4) % 4); ++i) {
        mywalker->animate();
    }

    mywalker->team_num = myguy->teamnum;

    ViewScreen *view_buf = myscreen->viewob[0];
    mywalker->setxy(((centerx - (mywalker->sizex / 2)) + view_buf->topx) - view_buf->xloc,
                    ((centery - (mywalker->sizey / 2)) + view_buf->topy) - view_buf->yloc);

    mywalker->draw(view_buf->topx, view_buf->topy,
                   view_buf->xloc, view_buf->yloc,
                   view_buf->endx, view_buf->endy,
                   view_buf->control);

    delete mywalker;
}
