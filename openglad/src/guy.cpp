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

#include "picker.hpp"

#define RAISE 1.85 // Please also change in picker.cpp

static Sint32 guy_id_counter = 0;

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

Guy::Guy(Guy const &copy)
    : family(copy.family)
    , strength(copy.strength)
    , dexterity(copy.dexterity)
    , constitution(copy.constitution)
    , intelligence(copy.intelligence)
    , armor(copy.armor)
    , exp(copy.exp)
    , kills(copy.kills)
    , level_kills(copy.level_kills)
    , total_damage(copy.total_damage)
    , total_hits(copy.total_hits)
    , total_shots(copy.total_shots)
    , teamnum(copy.teamnum)
    , scen_damage(copy.scen_damage)
    , scen_kills(copy.scen_kills)
    , scen_damage_taken(copy.scen_damage_taken)
    , scen_min_hp(copy.scen_min_hp)
    , scen_shots(copy.scen_shots)
    , scen_hits(copy.scen_hits)
    , id(copy.id)
    , level(copy.level)
{
    name = copy.name;
}

Guy::~Guy()
{
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
    cost += (std::pow(temp, RAISE) * statcosts[family][0]);

    // Get dexterity cost...
    temp = std::max(dexterity - normal.dexterity, 0); // Difference...
    cost += (std::pow(temp, RAISE) * statcosts[family][1]);

    // Get constitution cost...
    temp = std::max(constitution - normal.constitution, 0); // Difference...
    cost += (std::pow(temp, RAISE) * statcosts[family][2]);

    // Get intelligence cost...
    temp = std::max(intelligence - normal.intelligence, 0); // Difference...
    cost += (std::pow(temp, RAISE) * statcosts[family][3]);

    // Get armor cost...
    temp = std::max(armor - normal.armor, 0); // Difference...
    cost += (std::pow(temp, RAISE) * statcosts[family][4]);

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
