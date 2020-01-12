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
#include "gloader.hpp"

#include <cstring>
#include <sstream>

#include "effect.hpp"
#include "graphlib.hpp"
#include "gparser.hpp"
#include "living.hpp"
#include "picker.hpp"
#include "stats.hpp"
#include "treasure.hpp"
#include "util.hpp"
#include "walker.hpp"
#include "weap.hpp"

#define SIZE_ORDERS 7 // See graph.hpp
#define SIZE_FAMILIES 21 // See also NUM_FAMILIES in graph.hpp
// PIX(a, b) ((SIZE_FAMILIES * a) + b) // Moved to graph.hpp

extern float derived_bonuses[NUM_FAMILIES][8];

// These are for monsters and us
Sint8 bit1[] = { 1, 5, 1, 9, -1 }; // Up
Sint8 bit2[] = { 13, 17, 13, 21, -1 }; // Up-right
Sint8 bit3[] = { 2, 6, 2, 10, -1 }; // Right
Sint8 bit4[] = { 14, 18, 14, 22, -1 }; // Down-right
Sint8 bit5[] = { 0, 4, 0, 8, -1 }; // Down
Sint8 bit6[] = { 12, 16, 12, 20, -1 }; // Down-left
Sint8 bit7[] = { 3, 7, 3, 11, -1 }; // Left
Sint8 bit8[] = { 15, 19, 15, 23, -1 }; // Up-left

Sint8 att1[] = { 1, 5, 1, -1 }; // Up
Sint8 att2[] = { 13, 17, 13, -1 }; // Up-right
Sint8 att3[] = { 2, 6, 2, -1 }; // Right
Sint8 att4[] = { 14, 18, 14, -1 }; // Down-right
Sint8 att5[] = { 0, 4, 0, -1 }; // Down
Sint8 att6[] = { 12, 16, 12, -1 }; // Down-left
Sint8 att7[] = { 3, 7, 3, -1 }; // Left
Sint8 att8[] = { 15, 19, 15, -1 }; // Up-left

Sint8 bitm2[] = { 21, 25, 21, 29, -1 }; // Up-right
Sint8 bitm4[] = { 22, 26, 22, 30, -1 }; // Down-right
Sint8 bitm6[] = { 20, 24, 20, 28, -1 }; // Down-left
Sint8 bitm8[] = { 23, 27, 23, 31, -1 }; // Up-left

Sint8 mageatt1[] = { 5, 17, 1, -1 }; // Up
Sint8 mageatt2[] = { 25, 33, 21, -1 }; // Up-right
Sint8 mageatt3[] = { 6, 18, 2, -1 }; // Right
Sint8 mageatt4[] = { 26, 34, 22, -1 }; // Down-right
Sint8 mageatt5[] = { 4, 16, 0, -1 }; // Down
Sint8 mageatt6[] = { 24, 32, 20, -1 }; // Down-left
Sint8 mageatt7[] = { 7, 19, 3, -1 }; // Left
Sint8 mageatt8[] = { 27, 35, 23, -1 }; // Up-left

Sint8 tele_out1[] = { 12, 13, 14, 15, -1 };

Sint8 tele_in1[] = { 15, 14, 13, 12, 1, -1 }; // Up
Sint8 tele_in2[] = { 15, 14, 13, 12, 2, -1 }; // Right
Sint8 tele_in3[] = { 15, 14, 13, 12, 3, -1 }; // Down
Sint8 tele_in4[] = { 15, 14, 13, 12, 4, -1 }; // Left

// Big skeleton, who is currently different...
Sint8 gs_down[] = { 0, 1, 2, 3, -1 }; // True "down"
Sint8 gs_up[] = { 3, 2, 1, 0, -1 }; // Faked up :)

// Skeleton growing
Sint8 skel_grow[] = { 27, 26, 25, 24, 0, -1 };
Sint8 skel_shrink[] = { 0, 24, 25, 26, 27, -1 };

// For slime unidirectional movement
Sint8 slime_pulse[] = { 0, 0, 1, 1, 2, 2, 1, 1, -1 };

Sint8 slime_split[] = { 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, -1 };

Sint8 small_slime[] = {
    0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2,
    2, 1, 1, -1
};

// These are for "effect" objects
Sint8 series_8[] = { 0, 1, 2, 3, 4, 5, 6, 7, -1 };

Sint8 *aniexpand8[] = {
    series_8, series_8, series_8, series_8, series_8, series_8, series_8,
    series_8, series_8, series_8, series_8, series_8, series_8, series_8,
    series_8, series_8
};

// Sint8 series_16[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, -1 };
Sint8 series_16[] = { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, -1 };

Sint8 *ani16[] = {
    series_16, series_16, series_16, series_16, series_16, series_16, series_16,
    series_16, series_16, series_16, series_16, series_16, series_16, series_16,
    series_16, series_16
};

Sint8 bomb1[] = {
    0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 4, 5, 4, 5, 4, 5, 4, 5,
    6, 7, 6, 7, 6, 7, 6, 7, 8, 9, 8, 9, 8, 9, 8, 9, 10, 11, 10, 11, 10, 11,
    10, 11, 12, 12, -1
};

Sint8 *anibomb1[] = {
    bomb1, bomb1, bomb1, bomb1, bomb1, bomb1, bomb1, bomb1, bomb1, bomb1,
    bomb1, bomb1, bomb1, bomb1, bomb1, bomb1,
};

Sint8 explosion1[] = { 0, 1, 2, -1 };
Sint8 *aniexplosion1[] = {
    explosion1, explosion1, explosion1, explosion1, explosion1, explosion1,
    explosion1, explosion1, explosion1, explosion1, explosion1, explosion1,
    explosion1, explosion1
};

/*
 * How do animations work?
 * animate() sets the walker graphic to: ani[curdir + (ani_type * NUM_FACINGS)][cycle]
 * ani_type of 0 causes an effect object to lst only one frame.
 * So any lsating animation usually has ani_type of 1, which means "ani" needs
 * to store at least 16 elements (NUM_FACINGS == 8).
 * The animation can be directional due to the use of curdir.
 * The Sint8[] are the actual frame indices for the animation. -1 means to end
 * the animation.
 */

Sint8 hit1[] = { 0, 1, -1 };
Sint8 hit2[] = { 0, 2, -1 };
Sint8 hit3[] = { 0, 3, -1 };
Sint8 *anihit[] = {
    hit1, hit1, hit1, hit1, hit1, hit1,
    hit1, hit1, hit1, hit1, hit1, hit1,
    hit2, hit2, hit2, hit2, hit2, hit2,
    hit2, hit2, hit3, hit3, hit3, hit3,
    hit3, hit3, hit3, hit3
};

Sint8 cloud_cycle[] = { 0, 1, 2, 3, 2, 1, -1 };
Sint8 *anicloud[] = {
    cloud_cycle, cloud_cycle, cloud_cycle, cloud_cycle, cloud_cycle,
    cloud_cycle, cloud_cycle, cloud_cycle, cloud_cycle, cloud_cycle,
    cloud_cycle, cloud_cycle, cloud_cycle, cloud_cycle, cloud_cycle,
    cloud_cycle
};

// Make TP marker
Sint8 marker_cycle[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, -1
};

Sint8 *animarker[] = {
    marker_cycle, marker_cycle, marker_cycle, marker_cycle, marker_cycle,
    marker_cycle, marker_cycle, marker_cycle, marker_cycle, marker_cycle,
    marker_cycle, marker_cycle, marker_cycle, marker_cycle, marker_cycle,
    marker_cycle
};

// These are for livings no
Sint8 *animan[] = {
    bit1, bit2, bit3, bit4, bit5, bit6, bit7, bit8,
    att1, att2, att3, att4, att5, att6, att7, att8
};

Sint8 *aniskel[] = {
    bit1, bit2, bit3, bit4, bit5, bit6, bit7, bit8,
    att1, att2, att3, att4, att5, att6, att7, att8,
    // == tele_out
    skel_shrink, skel_shrink, skel_shrink, skel_shrink,
    skel_shrink, skel_shrink, skel_shrink, skel_shrink,
    // Grow from the ground (tele_in)
    skel_grow, skel_grow, skel_grow, skel_grow,
    skel_grow, skel_grow, skel_grow, skel_grow
};

Sint8 *animage[] = {
    bit1, bitm2, bit3, bitm4, bit5, bitm6, bit7, bitm8,
    // 8 == attack
    mageatt1, mageatt2, mageatt3, mageatt4,
    mageatt5, mageatt6, mageatt7, mageatt8,
    // 16 == tele_out
    tele_out1, tele_out1, tele_out1, tele_out1,
    tele_out1, tele_out1, tele_out1, tele_out1,
    // 24 == tele_in
    tele_in1, tele_in1, tele_in2, tele_in2,
    tele_in3, tele_in3, tele_in4, tele_in4
};

// Giant skeleton...
Sint8 *anigs[] = {
    gs_down, gs_up, gs_down, gs_up, gs_down, gs_up, gs_down, gs_up,
    gs_down, gs_up, gs_down, gs_up, gs_down, gs_up, gs_down, gs_up
};

Sint8 *anislime[] = {
    // 0 == walk
    slime_pulse, slime_pulse, slime_pulse, slime_pulse,
    slime_pulse, slime_pulse, slime_pulse, slime_pulse,
    // 8 == attack
    slime_pulse, slime_pulse, slime_pulse, slime_pulse,
    slime_pulse, slime_pulse, slime_pulse, slime_pulse,
    // 16 == tele_out (ignored)
    slime_pulse, slime_pulse, slime_pulse, slime_pulse,
    nullptr, nullptr, nullptr, nullptr,
    // 24 == tele_in (ignored)
    nullptr, nullptr, nullptr, nullptr,
    slime_split, slime_split, slime_split, slime_split,
    // 32 == slime splits
    slime_split, slime_split, slime_split, slime_split
};

Sint8 * ani_small_slime[] = {
    small_slime, small_slime, small_slime, small_slime, small_slime,
    small_slime, small_slime, small_slime, small_slime, small_slime,
    small_slime, small_slime, small_slime, small_slime, small_slime,
    small_slime
};

// These are for knives
Sint8 kni1[] = { 7, 6, 5, 4, 3, 2, 1, 0, -1 }; // Clockwise?
Sint8 kni2[] = { 0, 1, 2, 3, 4, 5, 6, 7, -1 }; // Counter-clockwise?
Sint8 *anikni[] = {
    kni2, kni2, kni1, kni1, kni1, kni1, kni2, kni2, kni2, kni2,
    kni1, kni1, kni1, kni1, kni2, kni2
};

// These are for the rocks
Sint8 rock1[] = { 0, -1 };
Sint8 *anirock[] = {
    rock1, rock1, rock1, rock1, rock1, rock1, rock1, rock1,
    rock1, rock1, rock1, rock1, rock1, rock1, rock1, rock1
};

Sint8 grow1[] = { 4, 3, 2, 1, 0, -1 };
Sint8 *anitree[] = {
    rock1, rock1, rock1, rock1, rock1, rock1, rock1, rock1,
    grow1, grow1, grow1, grow1, grow1, grow1, grow1, grow1
};

Sint8 door1[] = { 0, -1 };
Sint8 door2[] = { 1, -1 };
Sint8 *anidoor[] = {
    door1, door1, door2, door2, door1, door1, door2, door2,
    door1, door1, door2, door2, door1, door1, door2, door2
};

Sint8 dooropen1[] = { 0, 2, 3, 4, 1, -1 };
Sint8 dooropen2[] = { 1, 4, 3, 2, 0, -1 };
Sint8 *anidooropen[] = {
    door2, door2, door1, door1, door2, door2, door1, door1,
    dooropen1, dooropen1, dooropen2, dooropen2,
    dooropen1, dooropen1, dooropen2, dooropen2
};

Sint8 arrow1[] = { 1, -1 }; // Up
Sint8 arrow2[] = { 5, -1 }; // Up-right
Sint8 arrow3[] = { 2, -1 }; // Right
Sint8 arrow4[] = { 6, -1 }; // Down-right
Sint8 arrow5[] = { 0, -1 }; // Down
Sint8 arrow6[] = { 4, -1 }; // Down-left
Sint8 arrow7[] = { 3, -1 }; // Left
Sint8 arrow8[] = { 7, -1 }; // up-left
Sint8 *aniarrow[] = {
    arrow1, arrow2, arrow3, arrow4, arrow5, arrow6, arrow7, arrow8,
    arrow1, arrow2, arrow3, arrow4, arrow5, arrow6, arrow7, arrow8
};

// These are for the slimes' blobs
Sint8 blob1[] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0, -1 };
Sint8 *aniblob1[] = {
    blob1, blob1, blob1, blob1, blob1, blob1, blob1, blob1,
    blob1, blob1, blob1, blob1, blob1, blob1, blob1, blob1
};

Sint8 none1[] = { 0 , -1 };
Sint8 *aninone[] = {
    none1, none1, none1, none1, none1, none1, none1, none1,
    none1, none1, none1, none1, none1, none1, none1, none1
};

// For the tower generator
Sint8 towerglow1[] = { 1, 1, 1, 2, 2, 0, -1 };
Sint8 *anitower[] = {
    none1, none1, none1, none1, none1, none1, none1, none1,
    towerglow1, towerglow1, towerglow1, towerglow1,
    towerglow1, towerglow1, towerglow1, towerglow1
};

// For tent generator
Sint8 tent1[] = { 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 0, -1 };
Sint8 *anitent[] = {
    none1, none1, none1, none1, none1, none1, none1, none1,
    tent1, tent1, tent1, tent1, tent1, tent1, tent1, tent1
};

Sint8 blood1[] = { 3, 2, 1, 0, -1 };
Sint8 *aniblood[] = {
    rock1, rock1, rock1, rock1, rock1, rock1, rock1, rock1,
    blood1, blood1, blood1, blood1, blood1, blood1, blood1, blood1
};

// These are for the cleric's glow thing
Sint8 glowgrow[] = { 0, 1, 2, 3, -1 };
Sint8 glowpulse[] = { 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, -1 };
Sint8 *aniglowgrow[] = {
    rock1, rock1, rock1, rock1, rock1, rock1, rock1, rock1,
    glowgrow, glowgrow, glowgrow, glowgrow,
    glowgrow, glowgrow, glowgrow, glowgrow,
    glowpulse, glowpulse, glowpulse, glowpulse,
    glowpulse, glowpulse, glowpulse, glowpulse
};

// Treasure animations
Sint8 food1[] = { 0, -1 };
Sint8 *anifood[] = {
    food1, food1, food1, food1, food1, food1, food1, food1,
    food1, food1, food1, food1, food1, food1, food1, food1
};

PixieData data_copy(PixieData const &d)
{
    PixieData result;

    if (!d.valid()) {
        return result;
    }

    result.frames = d.frames;
    result.w = d.w;
    result.h = d.h;

    Sint32 len = (d.w * d.h) * d.frames;
    result.data = new Uint8[len];
    memcpy(result.data, d.data, len);

    return result;
}

Loader::Loader()
    : graphics(nullptr)
    , animations(nullptr)
    , stepsizes(nullptr)
    , lineofsight(nullptr)
    , act_types(nullptr)
    , damage(nullptr)
    , fire_frequency(nullptr)
{
    memset(hitpoints, 0, 200 * sizeof(float));

    // hitpoints = new Sint8[SIZE_ORDERS * SIZE_FAMILIES];
    graphics = new PixieData[SIZE_ORDERS * SIZE_FAMILIES];
    stepsizes = new float[SIZE_ORDERS * SIZE_FAMILIES];
    lineofsight = new Sint32[SIZE_ORDERS * SIZE_FAMILIES];
    damage = new float[SIZE_ORDERS * SIZE_FAMILIES];
    fire_frequency = new float[SIZE_ORDERS * SIZE_FAMILIES];

    act_types = new Uint8[SIZE_ORDERS * SIZE_FAMILIES];
    memset(act_types, ACT_RANDOM, SIZE_ORDERS * SIZE_FAMILIES);

    animations = new Sint8 **[SIZE_ORDERS * SIZE_FAMILIES];
    memset(animations, 0, SIZE_ORDERS * SIZE_FAMILIES);

    // Livings
    graphics[PIX(ORDER_LIVING, FAMILY_SOLDIER)] = read_pixie_file(std::string("footman.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_ELF)] = read_pixie_file(std::string("elf.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_ARCHER)] = read_pixie_file(std::string("archer.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_THIEF)] = read_pixie_file(std::string("thief.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_MAGE)] = read_pixie_file(std::string("mage.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_SKELETON)] = read_pixie_file(std::string("skeleton.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_CLERIC)] = read_pixie_file(std::string("cleric.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_FIRE_ELEMENTAL)] = read_pixie_file(std::string("firelem.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_FAERIE)] = read_pixie_file(std::string("faerie.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_SLIME)] = read_pixie_file(std::string("amoeba3.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_SMALL_SLIME)] = read_pixie_file(std::string("s_slime.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_MEDIUM_SLIME)] = read_pixie_file(std::string("m_slime.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_GHOST)] = read_pixie_file(std::string("ghost.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_DRUID)] = read_pixie_file(std::string("druid.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_ORC)] = read_pixie_file(std::string("orc.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_BIG_ORC)] = read_pixie_file(std::string("orc2.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_BARBARIAN)] = read_pixie_file(std::string("barby.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_ARCHMAGE)] = read_pixie_file(std::string("archmage.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_GOLEM)] = read_pixie_file(std::string("golem1.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_GIANT_SKELETON)] = read_pixie_file(std::string("gs1.pix"));
    graphics[PIX(ORDER_LIVING, FAMILY_TOWER1)] = read_pixie_file(std::string("towersm1.pix"));

    for (Sint32 i = 0; i < NUM_FAMILIES; ++i) {
        hitpoints[PIX(ORDER_LIVING, i)] = derived_bonuses[i][0];
        damage[PIX(ORDER_LIVING, i)] = derived_bonuses[i][2];
        stepsizes[PIX(ORDER_LIVING, i)] = derived_bonuses[i][6];
        fire_frequency[PIX(ORDER_LIVING, i)] = derived_bonuses[i][7];
    }

    act_types[PIX(ORDER_LIVING, FAMILY_SOLDIER)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_ELF)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_ARCHER)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_THIEF)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_MAGE)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_SKELETON)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_CLERIC)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_FIRE_ELEMENTAL)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_FAERIE)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_SLIME)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_SMALL_SLIME)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_MEDIUM_SLIME)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_GHOST)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_DRUID)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_ORC)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_BIG_ORC)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_BARBARIAN)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_ARCHMAGE)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_GOLEM)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_GIANT_SKELETON)] = ACT_RANDOM;
    act_types[PIX(ORDER_LIVING, FAMILY_TOWER1)] = ACT_RANDOM;

    animations[PIX(ORDER_LIVING, FAMILY_SOLDIER)] = animan;
    animations[PIX(ORDER_LIVING, FAMILY_ELF)] = animan;
    animations[PIX(ORDER_LIVING, FAMILY_ARCHER)] = animan;
    animations[PIX(ORDER_LIVING, FAMILY_THIEF)] = animan;
    animations[PIX(ORDER_LIVING, FAMILY_MAGE)] = animage;
    animations[PIX(ORDER_LIVING, FAMILY_SKELETON)] = aniskel;
    animations[PIX(ORDER_LIVING, FAMILY_CLERIC)] = animan;
    animations[PIX(ORDER_LIVING, FAMILY_FIRE_ELEMENTAL)] = animan;
    animations[PIX(ORDER_LIVING, FAMILY_FAERIE)] = animan;
    animations[PIX(ORDER_LIVING, FAMILY_SLIME)] = anislime;
    animations[PIX(ORDER_LIVING, FAMILY_SMALL_SLIME)] = ani_small_slime;
    animations[PIX(ORDER_LIVING, FAMILY_MEDIUM_SLIME)] = ani_small_slime;
    animations[PIX(ORDER_LIVING, FAMILY_GHOST)] = animan;
    animations[PIX(ORDER_LIVING, FAMILY_DRUID)] = animan;
    animations[PIX(ORDER_LIVING, FAMILY_ORC)] = animan;
    animations[PIX(ORDER_LIVING, FAMILY_BIG_ORC)] = animan;
    animations[PIX(ORDER_LIVING, FAMILY_BARBARIAN)] = animan;
    animations[PIX(ORDER_LIVING, FAMILY_ARCHMAGE)] = animage;
    animations[PIX(ORDER_LIVING, FAMILY_GOLEM)] = animan;
    animations[PIX(ORDER_LIVING, FAMILY_GIANT_SKELETON)] = anigs;
    animations[PIX(ORDER_LIVING, FAMILY_TOWER1)] = anifood;

    // AI's understanding of how much range its ranged attack has so it will
    // try to shoot.
    lineofsight[PIX(ORDER_LIVING, FAMILY_SOLDIER)] = 7;
    lineofsight[PIX(ORDER_LIVING, FAMILY_ELF)] = 8;
    lineofsight[PIX(ORDER_LIVING, FAMILY_ARCHER)] = 12;
    lineofsight[PIX(ORDER_LIVING, FAMILY_THIEF)] = 10;
    lineofsight[PIX(ORDER_LIVING, FAMILY_MAGE)] = 7;
    lineofsight[PIX(ORDER_LIVING, FAMILY_SKELETON)] = 7;
    lineofsight[PIX(ORDER_LIVING, FAMILY_CLERIC)] = 4;
    lineofsight[PIX(ORDER_LIVING, FAMILY_FIRE_ELEMENTAL)] = 10;
    lineofsight[PIX(ORDER_LIVING, FAMILY_FAERIE)] = 8;
    lineofsight[PIX(ORDER_LIVING, FAMILY_SLIME)] = 4;
    lineofsight[PIX(ORDER_LIVING, FAMILY_SMALL_SLIME)] = 2;
    lineofsight[PIX(ORDER_LIVING, FAMILY_MEDIUM_SLIME)] = 3;
    lineofsight[PIX(ORDER_LIVING, FAMILY_GHOST)] = 12;
    lineofsight[PIX(ORDER_LIVING, FAMILY_DRUID)] = 10;
    lineofsight[PIX(ORDER_LIVING, FAMILY_ORC)] = 20;
    lineofsight[PIX(ORDER_LIVING, FAMILY_BIG_ORC)] = 25;
    lineofsight[PIX(ORDER_LIVING, FAMILY_BARBARIAN)] = 12;
    lineofsight[PIX(ORDER_LIVING, FAMILY_ARCHMAGE)] = 10;
    lineofsight[PIX(ORDER_LIVING, FAMILY_GOLEM)] = 20;
    lineofsight[PIX(ORDER_LIVING, FAMILY_GIANT_SKELETON)] = 20;
    lineofsight[PIX(ORDER_LIVING, FAMILY_TOWER1)] = 10;

    // Weapons
    graphics[PIX(ORDER_WEAPON, FAMILY_KNIFE)] = read_pixie_file(std::string("knife.pix"));
    graphics[PIX(ORDER_WEAPON, FAMILY_ROCK)] = read_pixie_file(std::string("rock.pix"));
    graphics[PIX(ORDER_WEAPON, FAMILY_ARROW)] = read_pixie_file(std::string("arrow.pix"));
    graphics[PIX(ORDER_WEAPON, FAMILY_FIRE_ARROW)] = read_pixie_file(std::string("farrow.pix"));
    graphics[PIX(ORDER_WEAPON, FAMILY_FIREBALL)] = read_pixie_file(std::string("fire.pix"));
    graphics[PIX(ORDER_WEAPON, FAMILY_TREE)] = read_pixie_file(std::string("tree.pix"));
    graphics[PIX(ORDER_WEAPON, FAMILY_METEOR)] = read_pixie_file(std::string("meteor.pix"));
    graphics[PIX(ORDER_WEAPON, FAMILY_SPRINKLE)] = read_pixie_file(std::string("sparkle.pix"));

    if (cfg.is_on("effects", "gore")) {
        graphics[PIX(ORDER_WEAPON, FAMILY_BLOOD)] = read_pixie_file(std::string("blood.pix"));
        graphics[PIX(ORDER_TREASURE, FAMILY_STAIN)] = read_pixie_file(std::string("stain.pix"));
    } else {
        graphics[PIX(ORDER_WEAPON, FAMILY_BLOOD)] = read_pixie_file(std::string("blood_friendly.pix"));
        graphics[PIX(ORDER_TREASURE, FAMILY_STAIN)] = read_pixie_file(std::string("stain_friendly.pix"));
    }

    graphics[PIX(ORDER_WEAPON, FAMILY_BONE)] = read_pixie_file(std::string("bone1.pix"));
    graphics[PIX(ORDER_WEAPON, FAMILY_BLOB)] = read_pixie_file(std::string("sl_ball.pix"));
    graphics[PIX(ORDER_WEAPON, FAMILY_LIGHTNING)] = read_pixie_file(std::string("lightnin.pix"));
    graphics[PIX(ORDER_WEAPON, FAMILY_GLOW)] = read_pixie_file(std::string("clerglow.pix"));
    graphics[PIX(ORDER_WEAPON, FAMILY_WAVE)] = read_pixie_file(std::string("wave.pix"));
    graphics[PIX(ORDER_WEAPON, FAMILY_WAVE2)] = read_pixie_file(std::string("wave2.pix"));
    graphics[PIX(ORDER_WEAPON, FAMILY_WAVE3)] = read_pixie_file(std::string("wave3.pix"));
    graphics[PIX(ORDER_WEAPON, FAMILY_CIRCLE_PROTECTION)] = read_pixie_file(std::string("wave2.pix"));
    graphics[PIX(ORDER_WEAPON, FAMILY_HAMMER)] = read_pixie_file(std::string("hammer.pix"));

    graphics[PIX(ORDER_WEAPON, FAMILY_DOOR)] = read_pixie_file(std::string("door.pix"));
    graphics[PIX(ORDER_WEAPON, FAMILY_BOULDER)] = read_pixie_file(std::string("boulder1.pix"));

    hitpoints[PIX(ORDER_WEAPON, FAMILY_KNIFE)] = 6;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_BONE)] = 5;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_ROCK)] = 4;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_ARROW)] = 5;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_FIRE_ARROW)] = 7;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_FIREBALL)] = 8;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_TREE)] = 50;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_METEOR)] = 12;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_SPRINKLE)] = 1;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_BLOB)] = 1;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_LIGHTNING)] = 60;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_GLOW)] = 50;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_WAVE)] = 50;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_WAVE2)] = 50;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_WAVE3)] = 50;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_CIRCLE_PROTECTION)] = 50;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_HAMMER)] = 10;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_DOOR)] = 5000;
    hitpoints[PIX(ORDER_WEAPON, FAMILY_BOULDER)] = 50;

    act_types[PIX(ORDER_WEAPON, FAMILY_KNIFE)] = ACT_FIRE;
    act_types[PIX(ORDER_WEAPON, FAMILY_BONE)] = ACT_FIRE;
    act_types[PIX(ORDER_WEAPON, FAMILY_ROCK)] = ACT_FIRE;
    act_types[PIX(ORDER_WEAPON, FAMILY_ARROW)] = ACT_FIRE;
    act_types[PIX(ORDER_WEAPON, FAMILY_FIRE_ARROW)] = ACT_FIRE;
    act_types[PIX(ORDER_WEAPON, FAMILY_FIREBALL)] = ACT_FIRE;
    act_types[PIX(ORDER_WEAPON, FAMILY_TREE)] = ACT_SIT;
    act_types[PIX(ORDER_WEAPON, FAMILY_METEOR)] = ACT_FIRE;
    act_types[PIX(ORDER_WEAPON, FAMILY_SPRINKLE)] = ACT_FIRE;
    act_types[PIX(ORDER_WEAPON, FAMILY_BLOOD)] = ACT_DIE;
    act_types[PIX(ORDER_WEAPON, FAMILY_BLOB)] = ACT_FIRE;
    act_types[PIX(ORDER_TREASURE, FAMILY_STAIN)] = ACT_CONTROL;
    act_types[PIX(ORDER_WEAPON, FAMILY_LIGHTNING)] = ACT_FIRE;
    act_types[PIX(ORDER_WEAPON, FAMILY_GLOW)] = ACT_SIT;
    act_types[PIX(ORDER_WEAPON, FAMILY_WAVE)] = ACT_FIRE;
    act_types[PIX(ORDER_WEAPON, FAMILY_WAVE2)] = ACT_FIRE;
    act_types[PIX(ORDER_WEAPON, FAMILY_WAVE3)] = ACT_FIRE;
    act_types[PIX(ORDER_WEAPON, FAMILY_CIRCLE_PROTECTION)] = ACT_SIT;
    act_types[PIX(ORDER_WEAPON, FAMILY_HAMMER)] = ACT_FIRE;
    act_types[PIX(ORDER_WEAPON, FAMILY_DOOR)] = ACT_SIT;
    act_types[PIX(ORDER_WEAPON, FAMILY_BOULDER)] = ACT_FIRE;

    animations[PIX(ORDER_WEAPON, FAMILY_KNIFE)] = anikni;
    animations[PIX(ORDER_WEAPON, FAMILY_BONE)] = anikni;
    animations[PIX(ORDER_WEAPON, FAMILY_ROCK)] = anirock;
    animations[PIX(ORDER_WEAPON, FAMILY_ARROW)] = aniarrow;
    animations[PIX(ORDER_WEAPON, FAMILY_FIRE_ARROW)] = aniarrow;
    animations[PIX(ORDER_WEAPON, FAMILY_FIREBALL)] = aniarrow;
    animations[PIX(ORDER_WEAPON, FAMILY_TREE)] = anitree;
    animations[PIX(ORDER_WEAPON, FAMILY_METEOR)] = aniarrow;
    animations[PIX(ORDER_WEAPON, FAMILY_SPRINKLE)] = anikni;
    animations[PIX(ORDER_WEAPON, FAMILY_BLOOD)] = aniblood;
    animations[PIX(ORDER_WEAPON, FAMILY_BLOB)] = aniblob1;
    animations[PIX(ORDER_TREASURE, FAMILY_STAIN)] = aniblood;
    animations[PIX(ORDER_WEAPON, FAMILY_LIGHTNING)] = aniarrow;
    animations[PIX(ORDER_WEAPON, FAMILY_GLOW)] = aniglowgrow;
    animations[PIX(ORDER_WEAPON, FAMILY_WAVE)] = aniarrow;
    animations[PIX(ORDER_WEAPON, FAMILY_WAVE2)] = aniarrow;
    animations[PIX(ORDER_WEAPON, FAMILY_WAVE3)] = aniarrow;
    animations[PIX(ORDER_WEAPON, FAMILY_CIRCLE_PROTECTION)] = anifood;
    animations[PIX(ORDER_WEAPON, FAMILY_HAMMER)] = aniarrow;
    animations[PIX(ORDER_WEAPON, FAMILY_DOOR)] = anidoor;
    animations[PIX(ORDER_WEAPON, FAMILY_BOULDER)] = aninone;

    stepsizes[PIX(ORDER_WEAPON, FAMILY_KNIFE)] = 5;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_BONE)] = 6;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_ROCK)] = 5;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_ARROW)] = 8;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_FIRE_ARROW)] = 8;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_FIREBALL)] = 6;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_TREE)] = 0;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_METEOR)] = 7;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_SPRINKLE)] = 6;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_BLOOD)] = 0;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_BLOB)] = 2;
    stepsizes[PIX(ORDER_TREASURE, FAMILY_STAIN)] = 0;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_LIGHTNING)] = 9;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_GLOW)] = 0;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_WAVE)] = 6;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_WAVE2)] = 4;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_WAVE3)] = 3;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_CIRCLE_PROTECTION)] = 1;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_HAMMER)] = 6;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_DOOR)] = 0;
    stepsizes[PIX(ORDER_WEAPON, FAMILY_BOULDER)] = 10;

    // Acts as weapon's range (pixel range == lineofsight * stepsize)
    lineofsight[PIX(ORDER_WEAPON, FAMILY_KNIFE)] = 7;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_BONE)] = 6;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_ROCK)] = 8;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_ARROW)] = 12;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_FIRE_ARROW)] = 12;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_FIREBALL)] = 7;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_TREE)] = 1;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_METEOR)] = 9;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_SPRINKLE)] = 10;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_BLOB)] = 11;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_BLOOD)] = 1;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_LIGHTNING)] = 13;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_GLOW)] = 1;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_WAVE)] = 3;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_WAVE2)] = 4;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_WAVE3)] = 6;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_CIRCLE_PROTECTION)] = 110;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_HAMMER)] = 4;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_DOOR)] = 1;
    lineofsight[PIX(ORDER_WEAPON, FAMILY_BOULDER)] = 9;

    // Strength of weapon
    damage[PIX(ORDER_WEAPON, FAMILY_KNIFE)] = 6;
    damage[PIX(ORDER_WEAPON, FAMILY_BONE)] = 5;
    damage[PIX(ORDER_WEAPON, FAMILY_ROCK)] = 4;
    damage[PIX(ORDER_WEAPON, FAMILY_ARROW)] = 5;
    damage[PIX(ORDER_WEAPON, FAMILY_FIRE_ARROW)] = 7;
    damage[PIX(ORDER_WEAPON, FAMILY_FIREBALL)] = 10;
    damage[PIX(ORDER_WEAPON, FAMILY_TREE)] = 0;
    damage[PIX(ORDER_WEAPON, FAMILY_METEOR)] = 12;
    damage[PIX(ORDER_WEAPON, FAMILY_SPRINKLE)] = 1;
    damage[PIX(ORDER_WEAPON, FAMILY_BLOB)] = 1;
    damage[PIX(ORDER_WEAPON, FAMILY_BLOOD)] = 0;
    damage[PIX(ORDER_WEAPON, FAMILY_LIGHTNING)] = 6;
    damage[PIX(ORDER_WEAPON, FAMILY_GLOW)] = 0;
    damage[PIX(ORDER_WEAPON, FAMILY_WAVE)]= 16;
    damage[PIX(ORDER_WEAPON, FAMILY_WAVE2)] = 12;
    damage[PIX(ORDER_WEAPON, FAMILY_WAVE3)] = 10;
    damage[PIX(ORDER_WEAPON, FAMILY_CIRCLE_PROTECTION)] = 0;
    damage[PIX(ORDER_WEAPON, FAMILY_HAMMER)] = 9;
    damage[PIX(ORDER_WEAPON, FAMILY_DOOR)] = 0;
    damage[PIX(ORDER_WEAPON, FAMILY_BOULDER)] = 25;

    fire_frequency[PIX(ORDER_WEAPON, FAMILY_KNIFE)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_BONE)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_ROCK)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_ARROW)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_FIRE_ARROW)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_FIREBALL)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_TREE)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_METEOR)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_SPRINKLE)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_BLOB)] = 2;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_BLOOD)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_LIGHTNING)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_GLOW)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_WAVE)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_WAVE2)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_WAVE3)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_CIRCLE_PROTECTION)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_HAMMER)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_DOOR)] = 0;
    fire_frequency[PIX(ORDER_WEAPON, FAMILY_BOULDER)] = 0;

    // Treasure items (food, etc.)
    graphics[PIX(ORDER_TREASURE, FAMILY_DRUMSTICK)] = read_pixie_file(std::string("food1.pix"));
    graphics[PIX(ORDER_TREASURE, FAMILY_GOLD_BAR)] = read_pixie_file(std::string("bar1.pix"));
    graphics[PIX(ORDER_TREASURE, FAMILY_SILVER_BAR)] = data_copy(graphics[PIX(ORDER_TREASURE, FAMILY_GOLD_BAR)]);
    graphics[PIX(ORDER_TREASURE, FAMILY_MAGIC_POTION)] = read_pixie_file(std::string("bottle.pix"));
    graphics[PIX(ORDER_TREASURE, FAMILY_INVIS_POTION)] = data_copy(graphics[PIX(ORDER_TREASURE, FAMILY_MAGIC_POTION)]);
    graphics[PIX(ORDER_TREASURE, FAMILY_INVULNERABLE_POTION)] = data_copy(graphics[PIX(ORDER_TREASURE, FAMILY_MAGIC_POTION)]);
    graphics[PIX(ORDER_TREASURE, FAMILY_FLIGHT_POTION)] = data_copy(graphics[PIX(ORDER_TREASURE, FAMILY_MAGIC_POTION)]);
    graphics[PIX(ORDER_TREASURE, FAMILY_EXIT)] = read_pixie_file(std::string("16exit1.pix"));
    graphics[PIX(ORDER_TREASURE, FAMILY_TELEPORTER)] = read_pixie_file(std::string("teleport.pix"));
    graphics[PIX(ORDER_TREASURE, FAMILY_LIFE_GEM)] = read_pixie_file(std::string("lifegem.pix"));
    graphics[PIX(ORDER_TREASURE, FAMILY_KEY)] = read_pixie_file(std::string("key.pix"));
    graphics[PIX(ORDER_TREASURE, FAMILY_SPEED_POTION)] = data_copy(graphics[PIX(ORDER_TREASURE, FAMILY_MAGIC_POTION)]);

    hitpoints[PIX(ORDER_TREASURE, FAMILY_DRUMSTICK)] = 10;
    hitpoints[PIX(ORDER_TREASURE, FAMILY_GOLD_BAR)] = 1000;
    hitpoints[PIX(ORDER_TREASURE, FAMILY_SILVER_BAR)] = 100;

    act_types[PIX(ORDER_TREASURE, FAMILY_DRUMSTICK)] = ACT_CONTROL;
    act_types[PIX(ORDER_TREASURE, FAMILY_GOLD_BAR)] = ACT_CONTROL;
    act_types[PIX(ORDER_TREASURE, FAMILY_SILVER_BAR)] = ACT_CONTROL;
    act_types[PIX(ORDER_TREASURE, FAMILY_MAGIC_POTION)] = ACT_CONTROL;
    act_types[PIX(ORDER_TREASURE, FAMILY_INVIS_POTION)] = ACT_CONTROL;
    act_types[PIX(ORDER_TREASURE, FAMILY_INVULNERABLE_POTION)] = ACT_CONTROL;
    act_types[PIX(ORDER_TREASURE, FAMILY_FLIGHT_POTION)] = ACT_CONTROL;
    act_types[PIX(ORDER_TREASURE, FAMILY_EXIT)] = ACT_CONTROL;
    act_types[PIX(ORDER_TREASURE, FAMILY_TELEPORTER)] = ACT_CONTROL;
    act_types[PIX(ORDER_TREASURE, FAMILY_LIFE_GEM)] = ACT_CONTROL;
    act_types[PIX(ORDER_TREASURE, FAMILY_KEY)] = ACT_CONTROL;
    act_types[PIX(ORDER_TREASURE, FAMILY_SPEED_POTION)] = ACT_CONTROL;

    animations[PIX(ORDER_TREASURE, FAMILY_DRUMSTICK)] = anifood;
    animations[PIX(ORDER_TREASURE, FAMILY_GOLD_BAR)] = anifood;
    animations[PIX(ORDER_TREASURE, FAMILY_SILVER_BAR)] = anifood;
    animations[PIX(ORDER_TREASURE, FAMILY_MAGIC_POTION)] = anifood;
    animations[PIX(ORDER_TREASURE, FAMILY_INVIS_POTION)] = anifood;
    animations[PIX(ORDER_TREASURE, FAMILY_INVULNERABLE_POTION)] = anifood;
    animations[PIX(ORDER_TREASURE, FAMILY_FLIGHT_POTION)] = anifood;
    animations[PIX(ORDER_TREASURE, FAMILY_EXIT)] = anifood;
    animations[PIX(ORDER_TREASURE, FAMILY_TELEPORTER)] = anifood;
    animations[PIX(ORDER_TREASURE, FAMILY_LIFE_GEM)] = anifood;
    animations[PIX(ORDER_TREASURE, FAMILY_KEY)] = anifood;
    animations[PIX(ORDER_TREASURE, FAMILY_SPEED_POTION)] = anifood;

    stepsizes[PIX(ORDER_TREASURE, FAMILY_DRUMSTICK)] = 5;

    // Generator
    graphics[PIX(ORDER_GENERATOR, FAMILY_TENT)] = read_pixie_file(std::string("tent.pix"));
    graphics[PIX(ORDER_GENERATOR, FAMILY_TOWER)] = read_pixie_file(std::string("tower4.pix"));
    graphics[PIX(ORDER_GENERATOR, FAMILY_BONES)] = read_pixie_file(std::string("bonepile.pix"));
    graphics[PIX(ORDER_GENERATOR, FAMILY_TREEHOUSE)] = read_pixie_file(std::string("bigtree.pix"));

    hitpoints[PIX(ORDER_GENERATOR, FAMILY_TENT)] = 100;

    act_types[PIX(ORDER_GENERATOR, FAMILY_TENT)] = ACT_GENERATE;
    act_types[PIX(ORDER_GENERATOR, FAMILY_TOWER)] = ACT_GENERATE;
    act_types[PIX(ORDER_GENERATOR, FAMILY_BONES)] = ACT_GENERATE;
    act_types[PIX(ORDER_GENERATOR, FAMILY_TREEHOUSE)] = ACT_GENERATE;

    animations[PIX(ORDER_GENERATOR, FAMILY_TENT)] = anitent;
    animations[PIX(ORDER_GENERATOR, FAMILY_TOWER)] = anitower;
    animations[PIX(ORDER_GENERATOR, FAMILY_BONES)] = aninone;
    animations[PIX(ORDER_GENERATOR, FAMILY_TREEHOUSE)] = aninone;

    stepsizes[PIX(ORDER_GENERATOR, FAMILY_TENT)] = 0;
    stepsizes[PIX(ORDER_GENERATOR, FAMILY_TOWER)] = 0;
    stepsizes[PIX(ORDER_GENERATOR, FAMILY_BONES)] = 0;
    stepsizes[PIX(ORDER_GENERATOR, FAMILY_TREEHOUSE)] = 0;

    lineofsight[PIX(ORDER_GENERATOR, FAMILY_TENT)] = 0;
    lineofsight[PIX(ORDER_GENERATOR, FAMILY_TOWER)] = 0;
    lineofsight[PIX(ORDER_GENERATOR, FAMILY_BONES)] = 0;
    lineofsight[PIX(ORDER_GENERATOR, FAMILY_TREEHOUSE)] = 0;

    damage[PIX(ORDER_GENERATOR, FAMILY_TENT)] = 0;
    damage[PIX(ORDER_GENERATOR, FAMILY_TOWER)] = 0;
    damage[PIX(ORDER_GENERATOR, FAMILY_BONES)] = 2;
    damage[PIX(ORDER_GENERATOR, FAMILY_TREEHOUSE)] = 0;

    fire_frequency[PIX(ORDER_GENERATOR, FAMILY_TENT)] = 0;
    fire_frequency[PIX(ORDER_GENERATOR, FAMILY_TOWER)] = 0;
    fire_frequency[PIX(ORDER_GENERATOR, FAMILY_BONES)] = 0;
    fire_frequency[PIX(ORDER_GENERATOR, FAMILY_TREEHOUSE)] = 0;

    // Specials...
    graphics[PIX(ORDER_SPECIAL, FAMILY_RESERVED_TEAM)] = read_pixie_file(std::string("team.pix"));

    // Effects...
    graphics[PIX(ORDER_FX, FAMILY_EXPAND)] = read_pixie_file(std::string("expand8.pix"));
    graphics[PIX(ORDER_FX, FAMILY_GHOST_SCARE)] = read_pixie_file(std::string("expand8.pix"));
    graphics[PIX(ORDER_FX, FAMILY_BOMB)] = read_pixie_file(std::string("bomb1.pix"));
    graphics[PIX(ORDER_FX, FAMILY_EXPLOSION)] = read_pixie_file(std::string("boom1.pix"));
    graphics[PIX(ORDER_FX, FAMILY_FLASH)] = read_pixie_file(std::string("telflash.pix"));
    graphics[PIX(ORDER_FX, FAMILY_MAGIC_SHIELD)] = read_pixie_file(std::string("mshield.pix"));
    graphics[PIX(ORDER_FX, FAMILY_KNIFE_BACK)] = read_pixie_file(std::string("knife.pix"));
    graphics[PIX(ORDER_FX, FAMILY_CLOUD)] = read_pixie_file(std::string("cloud.pix"));
    graphics[PIX(ORDER_FX, FAMILY_MARKER)] = read_pixie_file(std::string("marker.pix"));
    graphics[PIX(ORDER_FX, FAMILY_BOOMERANG)] = read_pixie_file(std::string("boomer.pix"));
    graphics[PIX(ORDER_FX, FAMILY_CHAIN)] = read_pixie_file(std::string("lightnin.pix"));
    graphics[PIX(ORDER_FX, FAMILY_DOOR_OPEN)] = read_pixie_file(std::string("door.pix"));
    graphics[PIX(ORDER_FX, FAMILY_HIT)] = read_pixie_file(std::string("hit.pix"));

    animations[PIX(ORDER_FX, FAMILY_EXPAND)] = aniexpand8;
    animations[PIX(ORDER_FX, FAMILY_GHOST_SCARE)] = aniexpand8;
    animations[PIX(ORDER_FX, FAMILY_BOMB)] = anibomb1;
    animations[PIX(ORDER_FX, FAMILY_EXPLOSION)] = aniexplosion1;
    animations[PIX(ORDER_FX, FAMILY_FLASH)] = aniexpand8;
    animations[PIX(ORDER_FX, FAMILY_MAGIC_SHIELD)] = anikni;
    animations[PIX(ORDER_FX, FAMILY_KNIFE_BACK)] = anikni;
    animations[PIX(ORDER_FX, FAMILY_BOOMERANG)] = ani16;
    animations[PIX(ORDER_FX, FAMILY_CLOUD)] = anicloud;
    animations[PIX(ORDER_FX, FAMILY_MARKER)] = animarker;
    animations[PIX(ORDER_FX, FAMILY_CHAIN)] = aniarrow;
    animations[PIX(ORDER_FX, FAMILY_DOOR_OPEN)] = anidooropen;
    animations[PIX(ORDER_FX, FAMILY_HIT)] = anihit;

    stepsizes[PIX(ORDER_FX, FAMILY_CLOUD)] = 4;
    stepsizes[PIX(ORDER_FX, FAMILY_CHAIN)] = 12; // REALLY fast!

    lineofsight[PIX(ORDER_FX, FAMILY_CHAIN)] = 15;

    hitpoints[PIX(ORDER_FX, FAMILY_MAGIC_SHIELD)] = 100;
    hitpoints[PIX(ORDER_FX, FAMILY_BOOMERANG)] = 50;

    damage[PIX(ORDER_FX, FAMILY_MAGIC_SHIELD)] = 10;
    damage[PIX(ORDER_FX, FAMILY_BOOMERANG)] = 8;
    damage[PIX(ORDER_FX, FAMILY_CLOUD)] = 20;

    // These are button graphics...
    graphics[PIX(ORDER_BUTTON1, FAMILY_NORMAL1)] = read_pixie_file(std::string("normal1.pix"));
    graphics[PIX(ORDER_BUTTON1, FAMILY_PLUS)] = read_pixie_file(std::string("butplus.pix"));
    graphics[PIX(ORDER_BUTTON1, FAMILY_MINUS)] = read_pixie_file(std::string("butminus.pix"));
    graphics[PIX(ORDER_BUTTON1, FAMILY_WRENCH)] = read_pixie_file(std::string("wrench.pix"));
};

Loader::~Loader(void)
{
    for (Sint32 i = 0; i < (SIZE_ORDERS * SIZE_FAMILIES); ++i) {
        graphics[i].free();
    }

    delete[] graphics;
    delete[] animations;
    delete[] act_types;
    delete[] stepsizes;
    delete[] lineofsight;
    delete[] damage;
    delete[] fire_frequency;
};

void Loader::set_derived_stats(Walker *w, Uint8 order, Uint8 family)
{
    w->stepsize = stepsizes[PIX(order, family)];
    w->normal_stepsize = w->stepsize;
    w->lineofsight = lineofsight[PIX(order, family)];
    w->damage = damage[PIX(order, family)];
    w->fire_frequency = fire_frequency[PIX(order, family)];
}

Walker *Loader::create_walker(Uint8 order, Uint8 family, VideoScreen *myscreen, bool cache_weapons)
{
    Walker *ob;

    if ((order == ORDER_LIVING) && (family >= NUM_FAMILIES)) {
        family = FAMILY_SOLDIER;
    }

    if (!graphics[PIX(order, family)].valid()) {
        std::stringstream buf;
        buf << "No valid graphics for walker!" << std::endl
            << "Order: " << order << ", Family: " << family << std::endl
            << "Please report this to the developer!";

        popup_dialog("ERROR", buf.str());

        return nullptr;
    }

    if (order == ORDER_LIVING) {
        ob = new Living(graphics[PIX(order, family)]);
    } else if (order == ORDER_WEAPON) {
        ob = new Weap(graphics[PIX(order, family)]);
    } else if (order == ORDER_TREASURE) {
        ob = new Treasure(graphics[PIX(order, family)]);
    } else if (order == ORDER_FX) {
        ob = new Effect(graphics[PIX(order, family)]);
    } else {
        ob = new Walker(graphics[PIX(order, family)]);
    }

    if (ob == nullptr) {
        return nullptr;
    }

    ob->stats->hitpoints = hitpoints[PIX(order, family)];
    ob->stats->max_hitpoints = hitpoints[PIX(order, family)];
    ob->stats->special_cost[0] = 0; // Shouldn't be used
    ob->stats->weapon_cost = 1; // Default value

    set_walker(ob, order, family);

    if (order == ORDER_LIVING) {
        ob->set_frame(ob->ani[ob->curdir][0]);
    }

    return ob;
}

Walker *Loader::set_walker(Walker *ob, Uint8 order, Uint8 family)
{
    ob->set_order_family(order, family);
    ob->set_act_type(act_types[PIX(order, family)]);
    ob->ani = animations[PIX(order, family)];

    set_derived_stats(ob, order, family);

    for (Sint16 i = 0; i < NUM_SPECIALS; ++i) {
        ob->stats->special_cost[i] = 5000;
    }

    // For special settings
    switch (order) {
    case ORDER_LIVING:
        switch (family) {
        case FAMILY_SOLDIER:
            ob->stats->special_cost[1] = 25; // Charge
            ob->stats->special_cost[2] = 100; // Boomerang
            ob->stats->special_cost[3] = 120; // Whirlwind
            ob->stats->special_cost[4] = 150; // Disarm
            ob->stats->weapon_cost = 2;
            ob->default_weapon = FAMILY_KNIFE;

            break;
        case FAMILY_ELF:
            ob->stats->special_cost[1] = 10;
            ob->stats->special_cost[2] = 20;
            ob->stats->special_cost[3] = 30;
            ob->stats->special_cost[4] = 40;
            ob->stats->set_bit_flags(BIT_FORESTWALK, 1);
            ob->default_weapon = FAMILY_ROCK;

            break;
        case FAMILY_ARCHER:
            ob->stats->special_cost[1] = 20; // Fire arrows
            ob->stats->special_cost[2] = 60; // 3-arrows
            ob->stats->special_cost[3] = 70; // Exploding bolt
            ob->default_weapon = FAMILY_ARROW;

            break;
        case FAMILY_THIEF:
            ob->stats->special_cost[1] = 35; // Bomb
            ob->stats->special_cost[2] = 125; // Cloak
            ob->stats->special_cost[3] = 100; // Taunt
            ob->stats->special_cost[4] = 150; // Poison cloud
            ob->default_weapon = FAMILY_KNIFE;

            break;
        case FAMILY_CLERIC:
            ob->stats->special_cost[1] = 2; // Heal / mystic mace
            ob->stats->special_cost[2] = 20; // Skeleton
            ob->stats->special_cost[3] = 50; // Ghost
            ob->stats->special_cost[4] = 150; // Raise dead
            ob->stats->weapon_cost = 8;
            ob->default_weapon = FAMILY_GLOW; // FAMILY_TREE

            break;
        case FAMILY_SKELETON:
            ob->stats->special_cost[1] = 10; // Tunnel
            ob->stats->weapon_cost = 0; // Free bones
            ob->ani_type = ANI_SKEL_GROW;
            ob->default_weapon = FAMILY_BONE;
        case FAMILY_FAERIE:
            ob->stats->weapon_cost = 2;
            ob->stats->set_bit_flags(BIT_FLYING, 1);
            ob->stats->set_bit_flags(BIT_ANIMATE, 1);
            ob->default_weapon = FAMILY_SPRINKLE;

            break;
        case FAMILY_MAGE:
            ob->stats->special_cost[1] = 15; // Teleport
            ob->stats->special_cost[2] = 60; // Warp space
            ob->stats->special_cost[3] = 500; // Freeze time
            ob->stats->special_cost[4] = 70; // Energy wave
            ob->stats->special_cost[5] = 100; // Heartburst
            ob->stats->weapon_cost = 5;
            ob->default_weapon = FAMILY_FIREBALL;

            break;
        case FAMILY_ARCHMAGE:
            ob->stats->special_cost[1] = 10; // Teleport
            ob->stats->special_cost[2] = 80; // Heartburst
            ob->stats->special_cost[3] = 500; // Summon elemental
            ob->stats->special_cost[4] = 150; // Mind-control enemies
            ob->stats->weapon_cost = 12;
            ob->default_weapon = FAMILY_FIREBALL;

            break;
        case FAMILY_FIRE_ELEMENTAL:
            ob->stats->special_cost[1] = 50; // Fireballs
            ob->stats->set_bit_flags(BIT_ANIMATE, 1);
            ob->stats->max_magicpoints = 150;
            ob->default_weapon = FAMILY_METEOR;

            break;
        case FAMILY_SLIME:
        case FAMILY_SMALL_SLIME:
        case FAMILY_MEDIUM_SLIME:
            ob->stats->special_cost[1] = 30;

            ob->stats->set_bit_flags(BIT_ANIMATE, 1); // Always wiggle

            if (order == FAMILY_SMALL_SLIME) {
                ob->stats->set_bit_flags(BIT_NO_RANGED, 1); // No ranged attack
            }

            ob->stats->max_magicpoints = 50;
            // ob->stats->magicpoints = 0;
            ob->stats->weapon_cost = 0; // Free slimeball
            ob->default_weapon = FAMILY_BLOB;

            break;
        case FAMILY_GHOST:
            ob->stats->special_cost[1] = 30; // Scare
            ob->stats->set_bit_flags(BIT_ANIMATE, 1); // Always move
            ob->stats->set_bit_flags(BIT_FLYING, 1);
            ob->stats->set_bit_flags(BIT_ETHEREAL, 1);
            ob->stats->set_bit_flags(BIT_NO_RANGED, 1);
            ob->stats->weapon_cost = 0; // Free melee
            ob->default_weapon = FAMILY_KNIFE;

            break;
        case FAMILY_DRUID:
            ob->stats->special_cost[1] = 15; // Grow tree
            ob->stats->special_cost[2] = 80; // Summon faerie
            ob->stats->special_cost[3] = 150; // Reveal items
            ob->stats->special_cost[4] = 200; // Protection shield
            ob->stats->weapon_cost = 4;
            ob->default_weapon = FAMILY_LIGHTNING;

            break;
        case FAMILY_ORC:
            ob->stats->special_cost[1] = 25; // Howl
            ob->stats->special_cost[2] = 20; // Eat corpse
            ob->stats->set_bit_flags(BIT_NO_RANGED, 1);
            ob->stats->weapon_cost = 2;
            ob->default_weapon = FAMILY_ROCK;

            break;
        case FAMILY_BIG_ORC:
            ob->stats->weapon_cost = 2;
            ob->default_weapon = FAMILY_KNIFE;

            break;
        case FAMILY_BARBARIAN:
            ob->stats->special_cost[1] = 20; // Hurl boulder
            ob->stats->special_cost[2] = 30; // Exploding boulder
            ob->stats->weapon_cost = 2;
            ob->default_weapon = FAMILY_HAMMER;

            break;
        case FAMILY_GOLEM:
            ob->stats->weapon_cost = 2;
            // ob->stats->set_bit_flags(BIT_NO_RANGED, 1);
            ob->default_weapon = FAMILY_BOULDER; // Default for now

            break;
        case FAMILY_GIANT_SKELETON:
            ob->stats->weapon_cost = 2;
            ob->default_weapon = FAMILY_BOULDER; // Default for now

            break;
        case FAMILY_TOWER1: // Not *really* a living...
            // ob->stepsize = 0;
            // ob->normal_stepsize = 0;
            ob->stats->weapon_cost = 2;
            ob->default_weapon = FAMILY_ARROW;

            break;
        default:
            ob->transform_to(ORDER_LIVING, FAMILY_SOLDIER);

            return ob;
        }

        ob->current_weapon = ob->default_weapon;

        break; // End living things
    case ORDER_WEAPON:
        switch (family) {
        case FAMILY_ROCK:
            ob->stats->set_bit_flags(BIT_FORESTWALK, 1);

            break;
        case FAMILY_FIREBALL:
            ob->stats->set_bit_flags(BIT_MAGICAL, 1);

            break;
        case FAMILY_METEOR:
            ob->stats->set_bit_flags(BIT_MAGICAL, 1);

            break;
        case FAMILY_SPRINKLE:
            ob->stats->set_bit_flags(BIT_FLYING, 1);

            break;
        case FAMILY_GLOW: // Cleric's shield glad
            ob->lifetime = 350;

            break;
        case FAMILY_WAVE:
            ob->stats->set_bit_flags(BIT_IMMORTAL, 1);
            ob->stats->set_bit_flags(BIT_NO_COLLIDE, 1);
            ob->stats->set_bit_flags(BIT_PHANTOM, 1);
            ob->stats->set_bit_flags(BIT_FLYING, 1);
            ob->stats->set_bit_flags(BIT_MAGICAL, 1);

            break;
        case FAMILY_WAVE2:
            ob->stats->set_bit_flags(BIT_IMMORTAL, 1);
            ob->stats->set_bit_flags(BIT_NO_COLLIDE, 1);
            ob->stats->set_bit_flags(BIT_PHANTOM, 1);
            ob->stats->set_bit_flags(BIT_FLYING, 1);
            ob->stats->set_bit_flags(BIT_MAGICAL, 1);

            break;
        case FAMILY_WAVE3:
            ob->stats->set_bit_flags(BIT_IMMORTAL, 1);
            ob->stats->set_bit_flags(BIT_NO_COLLIDE, 1);
            ob->stats->set_bit_flags(BIT_PHANTOM, 1);
            ob->stats->set_bit_flags(BIT_FLYING, 1);
            ob->stats->set_bit_flags(BIT_MAGICAL, 1);

            break;
        case FAMILY_CIRCLE_PROTECTION:
            ob->stats->set_bit_flags(BIT_IMMORTAL, 1);
            ob->stats->set_bit_flags(BIT_NO_COLLIDE, 1);
            ob->stats->set_bit_flags(BIT_PHANTOM, 1);
            ob->stats->set_bit_flags(BIT_FLYING, 1);
            ob->ani_type = 5; // Anything non-zero

            break;
        default:

            break;
        }

        break; // End of weapons
    case ORDER_TREASURE:
        switch (family) {
        case FAMILY_STAIN: // Permanent bloodstains
            ob->ignore = 1;

            break;
        case FAMILY_GOLD_BAR:
            ob->set_direct_frame(0);

            break;
        case FAMILY_SILVER_BAR:
            ob->set_direct_frame(1);

            break;
        case FAMILY_MAGIC_POTION:
            ob->set_direct_frame(0);

            break;
        case FAMILY_INVIS_POTION:
            ob->set_direct_frame(1);

            break;
        case FAMILY_INVULNERABLE_POTION:
            ob->set_direct_frame(2);

            break;
        case FAMILY_FLIGHT_POTION:
            ob->set_direct_frame(11);

            break;
        case FAMILY_SPEED_POTION:
            ob->set_direct_frame(3);

            break;
        default:

            break;
        }

        break; // End of treasures
    case ORDER_GENERATOR:
        switch (family) {
        case FAMILY_TOWER:
            ob->stats->weapon_cost = 0;
            ob->default_weapon = FAMILY_MAGE;

            break;
        case FAMILY_BONES: // Ghost bone pile
            ob->stats->weapon_cost = 0;
            ob->default_weapon = FAMILY_GHOST;

            break;
        case FAMILY_TREEHOUSE: // Elf treehouse
            ob->stats->weapon_cost = 0;
            ob->default_weapon = FAMILY_ELF;

            break;
        default:
            ob->stats->weapon_cost = 0;
            ob->default_weapon = FAMILY_SKELETON;

            break;
        }

        break; // End of generators
    case ORDER_FX:
        ob->ani_type = 0;

        switch (family) {
        case FAMILY_MAGIC_SHIELD:
            ob->stats->set_bit_flags(BIT_PHANTOM, 1);

            break;
        case FAMILY_CLOUD: // Poison cloud
            ob->stats->set_bit_flags(BIT_NO_COLLIDE, 1);
            ob->stats->set_bit_flags(BIT_FLYING, 1);

            break;
        default:

            break;
        }

        break; // End of FX
    default:

        break; // End of all orders
    }

    return ob;
}

// This is used for grabbing a PixieN directly, not through a Walker
PixieN *Loader::create_pixieN(Uint8 order, Uint8 family)
{
    PixieN *newpixie;

    if (!graphics[PIX(order, family)].valid()) {
        std::stringstream buf;
        buf << "Alert! No valid graphics for PixieN!" << std::endl;
        Log("%s", buf.str().c_str());

        return nullptr;
    }

    newpixie = new PixieN(graphics[PIX(order, family)]);

    return newpixie;
}
