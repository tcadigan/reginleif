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
#include "guy.hpp"

#include <algorithm>
#include <cmath>
#include <string>

#include "gloader.hpp"
#include "screen.hpp"
#include "walker.hpp"

#define RAISE 1.85 // Please also change in picker.cpp
#define BASE_GUY_HP 30

static Sint32 guy_id_counter = 0;

// Zardus: PORT, exception doesn't compile (DOS thing?):
// int matherr(struct exception *ex);

std::string get_family_string(Sint16 family);
Uint32 calculate_exp(Sint32 level);

Sint32 costlist[NUM_FAMILIES] = {
    250, // Soldier
    150, // Elf
    350, // Archer
    450, // Mage
    300, // Skeleton
    400, // Cleric
    600, // Fire elemental
    450, // Faerie
    700, // Slime (can't buy)
    700, // Small slime
    700, // Medium slime (can't buy)
    400, // Thief
    600, // Ghost
    350, // Druid
    300, // Orc
    1000, // Big orc
    350, // Barbarian
    450 // Archmage (not used)
};

Sint32 statlist[NUM_FAMILIES][6] = {
    // STR, DEX, CON, INT, ARMOR, LEVEL
    {   12,   6,  12,   8,     9,     1 }, // Soldier
    {    5,  14,   5,  12,     8,     1 }, // Elf
    {    6,  12,   6,  10,     5,     1 }, // Archer
    {    4,   6,   4,  16,     5,     1 }, // Mage
    {    9,  14,   9,   6,     6,     1 }, // Skeleton
    {    6,   7,   6,  14,     7,     1 }, // Cleric
    {   14,  10,  14,  14,     9,     1 }, // Fire elemental
    {    3,   8,   3,  14,     2,     1 }, // Faerie
    {   18,   2,  18,   7,     6,     1 }, // Slime
    {   18,   2,  18,   7,     6,     1 }, // Small slime
    {   18,   2,  18,   7,     6,     1 }, // Medium slime
    {    9,  12,  12,  10,     5,     1 }, // Thief
    {    6,  12,  18,  10,    15,     1 }, // Ghost
    {    7,   8,  14,  12,     7,     1 }, // Druid
    {   18,   8,  16,   5,    11,     1 }, // Orc
    {   18,   8,  16,   5,    11,     1 }, // Big orc
    {   14,   5,  14,   8,     8,     1 }, // Barbarian
    {    4,   6,   4,  16,     5,     1 } // Archmage
};

float derived_bonuses[NUM_FAMILIES][8] =
{
    //               HP, MP, ATK, RANGED ATK, RANGE, DEF, SPD, ATK SPD (delay)
    {  BASE_GUY_HP + 90,  0,  20,          0,     0,   0,   4,               6 }, // Soldier
    {  BASE_GUY_HP + 45,  0,  12,          0,     0,   0,   4,               5 }, // Elf
    {  BASE_GUY_HP + 60,  0,   8,          0,     0,   0,   4,               5 }, // Archer
    {  BASE_GUY_HP + 60,  0,   4,          0,     0,   0,   2,               4 }, // Mage
    {  BASE_GUY_HP + 30,  0,   4,          0,     0,   0,   6,            4.5f }, // Skeleton
    {  BASE_GUY_HP + 90,  0,  12,          0,     0,   0,   2,            7.5f }, // Cleric
    {  BASE_GUY_HP + 70,  0,  28,          0,     0,   0,   4,               5 }, // Fire Elemental
    {  BASE_GUY_HP + 45,  0,   5,          0,     0,   0,   4,               9 }, // Faerie
    { BASE_GUY_HP + 120,  0,  28,          0,     0,   0,   3,              11 }, // Slime
    {  BASE_GUY_HP + 50,  0,  12,          0,     0,   0,   2,              12 }, // Small slime
    {  BASE_GUY_HP + 80,  0,  20,          0,     0,   0,   2,              10 }, // Medium slime
    {  BASE_GUY_HP + 45,  0,  12,          0,     0,   0,   5,               5 }, // Thief
    {  BASE_GUY_HP + 20,  0,  12,          0,     0,   0,   4,               7 }, // Ghost
    {  BASE_GUY_HP + 80,  0,  10,          0,     0,   0,   3,               9 }, // Druid
    { BASE_GUY_HP + 110,  0,  23,          0,     0,   0,   3,               7 }, // Orc
    { BASE_GUY_HP + 150,  0,  28,          0,     0,   0,   3,               6 }, // Big orc
    { BASE_GUY_HP + 120,  0,  25,          0,     0,   0,   3,            5.5f }, // Barbarian
    { BASE_GUY_HP + 120,  0,   8,          0,     0,   0,   3,               1 }, // Archmage
    { BASE_GUY_HP + 270,  0,  60,          0,     0,   0,   8,               9 }, // Golem
    { BASE_GUY_HP + 270,  0,  60,          0,     0,   0,   8,               7 }, // Giant skeleton
    { BASE_GUY_HP + 100,  0,   0,          0,     0,   0,   0,               5 } // Tower
};

Sint32 statcosts[NUM_FAMILIES][6] = {
    // STR, DEX, CON, INT, ARMOR, LEVEL
    {    6,  10,   6,  25,    50,   200 }, // Soldier
    {   25,   6,  12,   8,    50,   200 }, // Elf
    {   15,   6,   9,  10,    50,   200 }, // Archer
    {   20,  15,  16,  25,    50,   200 }, // Mage
    {   15,   6,  16,  25,    50,   200 }, // Skeleton
    {   15,  15,   9,   6,    50,   200 }, // Cleric
    {    7,  10,  14,  12,    50,   200 }, // Fire elemental
    {   25,   6,  12,   8,    50,   200 }, // Faerie
    {   20,  20,   8,  14,    50,   200 }, // Slime
    {   20,  20,   8,  14,    50,   200 }, // Small slime
    {   20,  20,   8,  14,    50,   200 }, // Medium slime
    {   15,   6,   9,  10,    50,   200 }, // Thief
    {   16,  16,  16,  16,    45,   200 }, // Ghost
    {   15,  15,   7,   6,    50,   200 }, // Druid
    {    6,  15,   5,  40,    50,   200 }, // Orc
    {    6,  15,   5,  40,    50,   200 }, // Big orc
    {    5,  35,   5,  35,    50,   200 }, // Barbarian
    // {   25,  15,  20,   5,    50,   200 }, // Archmage
    {   30,  20,  25,   7,    55,   200 } // Archmage
};

Guy::Guy()
{
    name = "SOLDIER";
    family = FAMILY_SOLDIER;
    strength = 0;
    dexterity = 0;
    constitution = 0;
    intelligence = 0;
    level = 1;
    armor = 0;
    exp = 0;
    kills = 0;
    level_kills = 0;
    total_damage = 0;
    total_hits = 0;
    total_shots = 0;
    teamnum = 0;
    scen_damage = 0;
    scen_kills = 0;
    scen_damage_taken = 0;
    scen_min_hp = 5000000;
    scen_shots = 0;
    scen_hits = 0;
    id = guy_id_counter;

    ++guy_id_counter;
}

// Set defaults for various types
Guy::Guy(Sint32 whatfamily)
{
    family = whatfamily;
    kills = 0;
    level_kills = 0;
    total_damage = 0;
    total_hits = 0;
    total_shots = 0;
    exp = 0;
    teamnum = 0;
    scen_damage = 0;
    scen_kills = 0;
    scen_damage_taken = 0;
    scen_shots = 0;
    scen_hits = 0;

    // Set stats
    if (whatfamily <= FAMILY_ARCHMAGE) {
        strength = statlist[whatfamily][0];
        dexterity = statlist[whatfamily][1];
        constitution = statlist[whatfamily][2];
        intelligence = statlist[whatfamily][3];
        armor = statlist[whatfamily][4];
        level = statlist[whatfamily][5]; // Should always be 1...
    } else {
        strength = 12;
        dexterity = 6;
        constitution = 12;
        intelligence = 8;
        armor = 6;
        level = 1;
    }

    // Set name
    std::string family(get_family_string(whatfamily));
    name = family.substr(0, 12);
    id = guy_id_counter;

    ++guy_id_counter;
}

// How much are we worth?
Sint32 Guy::query_heart_value()
{
    // For base comparisons
    Guy normal(family);
    Sint32 cost = 0;
    Sint32 temp;

    // Get strength cost
    temp = std::max(strength - normal.strength, 0); // Difference...
    cost += static_cast<Sint32>(std::pow(temp, RAISE) * statcosts[family][0]);

    // Get dexterity cost...
    temp = std::max(dexterity - normal.dexterity, 0); // Difference...
    cost += static_cast<Sint32>(std::pow(temp, RAISE) * statcosts[family][1]);

    // Get constitution cost...
    temp = std::max(constitution - normal.constitution, 0); // Difference...
    cost += static_cast<Sint32>(std::pow(temp, RAISE) * statcosts[family][2]);

    // Get intelligence cost...
    temp = std::max(intelligence - normal.intelligence, 0); // Difference...
    cost += static_cast<Sint32>(std::pow(temp, RAISE) * statcosts[family][3]);

    // Get armor cost...
    temp = std::max(armor - normal.armor, 0); // Difference...
    cost += static_cast<Sint32>(std::pow(temp, RAISE) * statcosts[family][4]);

    // Add in the base cost value for the guy...
    cost += costlist[family];

    return cost;
}

void Guy::set_level_number(Sint16 level)
{
    this->level = level;
}

Sint16 Guy::get_level() const
{
    return level;
}

Sint32 calculate_level(Uint32 experience)
{
    Sint32 result = 0;

    while (calculate_exp(result) <= experience) {
        ++result;
    }

    return result;
}

Uint32 calculate_exp(Sint32 level)
{
    /*
     * fn = ((8000 * (level + 10)) / 10) + calculate_exp(level - 1);
     * excel: = ((8000 * (F4 + 10)) / 10) + G3
     *
     * Level      XP
     *     1       0
     *     2    9600
     *     3   20000
     *     4   31200
     *     5   43200
     *     6   56000
     *     7   69600
     *     8   84000
     *     9   99200
     *    19  115200
     *
     * This is practically linear, so each level costs 10000 more than the previous.
     */

    if (level <= 1) {
        return 0;
    }

    Sint32 level_1 = level - 1;
    Sint32 level_2 = std::max(level - 2, 0);

    return (((8000 + (2000 * level_1)) + (4000 * level_2)) + calculate_exp(level - 1));
}

void Guy::upgrade_to_level(Sint16 level, bool set_xp)
{
    Sint16 level_diff = level - this->level;
    Sint16 s = 8 * level_diff;
    Sint16 d = 6 * level_diff;
    Sint16 c = 8 * level_diff;
    Sint16 i = 8 * level_diff;
    Sint16 a = level_diff;

    switch (family) {
    case FAMILY_ELF:
        s *= 0.75f;
        d *= 1.5f;
        c *= 0.75f;

        break;
    case FAMILY_ARCHER:
        s *= 0.5f;
        d *= 1.5f;

        break;
    case FAMILY_MAGE:
    case FAMILY_ARCHMAGE:
        s *= 0.5f;
        c *= 0.5f;
        i *= 2.0f;

        break;
    case FAMILY_SKELETON:
        d *= 2.0f;
        c *= 0.5f;
        i *= 0.5f;

        break;
    case FAMILY_FIRE_ELEMENTAL:
        s *= 1.5f;
        c *= 0.5f;

        break;
    case FAMILY_FAERIE:
    case FAMILY_THIEF:
        s *= 0.5f;
        d *= 2.0f;
        c *= 0.5f;

        break;
    case FAMILY_DRUID:
        d *= 0.5f;
        i *= 1.5f;

        break;
    case FAMILY_ORC:
    case FAMILY_BIG_ORC:
    case FAMILY_BARBARIAN:
        s *= 1.5f;
        d *= 0.5f;
        c *= 1.5f;
        i *= 0.5f;

        break;
    case FAMILY_SOLDIER:
    case FAMILY_CLERIC:
    case FAMILY_SLIME:
    case FAMILY_SMALL_SLIME:
    case FAMILY_MEDIUM_SLIME:
    case FAMILY_GHOST:
    default:

        break;
    }

    strength += s;
    dexterity += d;
    constitution += c;
    intelligence += i;
    armor += a;

    this->level = level;

    if (set_xp) {
        exp = calculate_exp(level);
    }
}

// Derived stat calculations
float Guy::get_hp_bonus() const
{
    return (10 + (constitution * 3));
}

float Guy::get_mp_bonus() const
{
    return (10 + (intelligence * 3));
}

float Guy::get_damage_bonus() const
{
    return (strength / 4.0f);
}

float Guy::get_armor_bonus() const
{
    return armor;
}

float Guy::get_speed_bonus() const
{
    return (dexterity / 54.f);
}

float Guy::get_fire_frequency_bonus() const
{
    return (dexterity / 47.0f);
}

void Guy::update_derived_stats(Walker *w)
{
    Guy *temp_guy = w->myguy;
    myscreen->level_data.myloader->set_derived_stats(w, ORDER_LIVING, temp_guy->family);

    w->stats->max_hitpoints += temp_guy->get_hp_bonus();
    w->stats->hitpoints = w->stats->max_hitpoints;

    // No class base value for MP...
    w->stats->max_magicpoints = temp_guy->get_mp_bonus();
    w->stats->magicpoints = w->stats->max_magicpoints;

    w->damage += temp_guy->get_damage_bonus();

    // No class base value for armor...
    w->stats->armor = temp_guy->get_armor_bonus();

    // stepsize makes us run faster, max for a non-weapon is 12
    w->stepsize = std::min(w->stepsize + temp_guy->get_speed_bonus(), 12.0f);

    w->normal_stepsize = w->stepsize;

    // fire_frequency makse us fire faster, min is 1
    w->fire_frequency = std::max(w->fire_frequency - temp_guy->get_fire_frequency_bonus(), 1.0f);

    // Fighters: limited weapons
    if (w->query_family() == FAMILY_SOLDIER) {
        w->weapons_left = static_cast<Sint16>((w->stats->level + 1) / 2);
    }

    // Set the heal delay...
    w->stats->max_heal_delay = REGEN;
    // For purposes of calculation only
    w->stats->current_heal_delay = ((temp_guy->constitution + (temp_guy->strength / 6.0f)) + 20) + 1000;

    // This takes care of the integer part, not calculate the fraction
    while (w->stats->current_heal_delay > REGEN) {
        w->stats->current_heal_delay -= REGEN;
        ++w->stats->heal_per_round;
    }

    if (w->stats->current_heal_delay > 1) {
        w->stats->max_heal_delay /= static_cast<Sint32>(w->stats->current_heal_delay + 1);
    }

    // Start off without healing
    w->stats->current_heal_delay = 0;

    // Make sure we have at least a 2 wait, otherwise we should have calculated
    // our heal_per_round as one higher, and the math must have been screwed
    // up somehow
    w->stats->max_heal_delay = std::max(w->stats->max_heal_delay, 2);

    // Set the magic delay...
    w->stats->max_magic_delay = REGEN;
    w->stats->current_magic_delay = ((temp_guy->intelligence * 45) + (temp_guy->dexterity * 15)) + 200;

    // This takes care of the integer part, now calculate the fraction
    while (w->stats->current_magic_delay > REGEN) {
        w->stats->current_magic_delay -= REGEN;
        ++w->stats->magic_per_round;
    }

    if (w->stats->current_magic_delay > 1) {
        w->stats->max_magic_delay /= static_cast<Sint32>(w->stats->current_magic_delay + 1);
    }

    // Start off without magic regen
    w->stats->current_magic_delay = 0;

    // Make sure we have at least a 2 wait, otherwise we should have calculated
    // our magic_per_round as one higher, and the math must have been screwed
    // up somehow
    w->stats->max_magic_delay = std::max(w->stats->max_magic_delay, 2);
}

Walker *Guy::create_walker(VideoScreen *myscreen)
{
    Walker *temp_walker = myscreen->level_data.myloader->create_walker(ORDER_LIVING, this->family, nullptr);
    *temp_walker->myguy = *this;
    temp_walker->stats->level = this->level;

    update_derived_stats(temp_walker);

    // Set our team number...
    temp_walker->team_num = this->teamnum;
    temp_walker->real_team_num = 255;

    return temp_walker;
}

Walker *Guy::create_and_add_walker(VideoScreen *myscreen)
{
    Walker *temp_walker = myscreen->level_data.add_ob(ORDER_LIVING, this->family);
    *temp_walker->myguy = *this;
    temp_walker->stats->level = this->level;

    update_derived_stats(temp_walker);

    // Set our team number...
    temp_walker->team_num = this->teamnum;
    temp_walker->real_team_num = 255;

    return temp_walker;
}
