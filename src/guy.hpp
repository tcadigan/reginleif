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
#ifndef __GUY_HPP__
#define __GUY_HPP__

#include "base.hpp"

#include <string>

#include <SDL2/SDL.h>

#define BASE_GUY_HP 30

// Holds attributes for characters.
// Used to store character data in SaveData's team_list.
// Used as walker::myguy in game for various attribute dependent effects for
// walkers who are on the player team.

class Guy
{
public:
    Guy();
    Guy(Sint32 whatfamily);
    Guy(Guy const &copy);
    ~Guy();

    Sint32 query_heart_value(); // How much are we worth?
    void upgrade_to_level(Sint16 level, bool set_xp=true);
    void set_level_number(Sint16 level);

    Sint16 get_level() const;
    float get_hp_bonus() const;
    float get_mp_bonus() const;
    float get_damage_bonus() const;
    float get_armor_bonus() const;
    float get_speed_bonus() const;
    float get_fire_frequency_bonus() const;

    std::string name; // Must be 12 characters or less
    Uint8 family; // Our family
    Sint16 strength;
    Sint16 dexterity;
    Sint16 constitution;
    Sint16 intelligence;
    Sint16 armor;
    Uint32 exp;
    Sint16 kills; // Version 3+
    Sint32 level_kills; // Version 3+

    // This will not be exact after changing damage to floating point,
    // but binary serialization of floats is messy.
    Sint32 total_damage; // Version 4+
    Sint32 total_hits; // Version 4+
    Sint32 total_shots; // Version 4+
    Sint16 teamnum; // Version 5+

    // Stats for the last battle
    float scen_damage;
    Sint16 scen_kills;
    float scen_damage_taken;
    float scen_min_hp;
    Sint16 scen_shots;
    Sint16 scen_hits;

    // An ID for comparing old guys with their duplicated counterparts
    // after battle
    Sint32 id;

protected:
    Sint16 level;
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

#endif
