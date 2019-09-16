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
#ifndef __LEVEL_DATA_HPP__
#define __LEVEL_DATA_HPP__

#include <list>
#include <string>

#include <SDL2/SDL.h>

#include "base.hpp"
#include "gloader-fwd.hpp"
#include "pixdefs.hpp"
#include "obmap.hpp"
#include "pixie-fwd.hpp"
#include "pixie_data.hpp"
#include "pixien.hpp"
#include "screen-fwd.hpp"
#include "smooth.hpp"
#include "stats.hpp"
#include "walker-fwd.hpp"

class CampaignData
{
public:
    CampaignData(std::string const &id);
    ~CampaignData();

    bool load();
    bool save();
    bool save_as(std::string const &new_id);

    std::string getDescriptionLine(Sint32 i);

    std::string id;
    std::string title;
    float rating;
    std::string version;
    std::string authors;
    std::string contributors;
    std::list<std::string> description;
    Sint32 suggested_power;
    Sint32 first_level;

    Sint32 num_levels;

    PixieData icondata;
    Pixie *icon;
};

class LevelData
{
public:
    LevelData(Sint32 id);
    ~LevelData();

    bool load();
    bool save();

    Walker *add_ob(Uint8 order, Uint8 family, bool atstart=false);
    Walker *add_fx_ob(Uint8 order, Uint8 family);
    Walker *add_weap_ob(Uint8 order, Uint8 family);
    Sint16 remove_ob(Walker *ob);

    void create_new_grid();
    void resize_grid(Sint32 width, Sint32 height);
    void delete_grid();
    void delete_objects();
    void clear();

    void set_draw_pos(Sint32 topx, Sint32 topy);
    void add_draw_pos(Sint32 topx, Sint32 topy);
    void draw(Screen *myscreen);

    std::string get_description_line(Sint32 i);

    Sint32 id;
    std::string title;

    static Uint8 const TYPE_CAN_EXIT_WHENEVER = 0x1; // Can exit without defeating all enemies
    static Uint8 const TYPE_MUST_DESTROY_GENERATORS = 0x2; // Must destroy genreators to exit
    static Uint8 const TYPE_MUST_PROTECT_NAMED_NPCS = 0x4; // Must protect named NPCs or else you lose

    Uint8 type;

    std::string grid_file;
    Sint16 par_value;
    Sint16 time_bonus_limit; // Frames until you get no time bonus
    PixieData grid;
    Sint32 pixmaxx;
    Sint32 pixmaxy;

    Smoother mysmoother;
    Loader *myloader;
    Sint32 numobs;

    std::list<Walker *> oblist;
    std::list<Walker *> fxlist; // fx -- explosions, etc.
    std::list<Walker *> weaplist; // Weapons

    // Keep a list of dead guys so weapons can still have valid owners
    std::list<Walker *> dead_list;

    ObjectMap *myobmap;
    std::list<std::string> description;

    // Drawing details
    PixieData pixdata[PIX_MAX];
    PixieN *back[PIX_MAX];
    Sint32 topx;
    Sint32 topy;
};

#endif
