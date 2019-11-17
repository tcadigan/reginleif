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
#ifndef __SCREEN_HPP__
#define __SCREEN_HPP__

// Definition of SCREEN class

#include "screen-fwd.hpp"

#include <map>
#include <set>
#include <string>

#include "gloader-fwd.hpp"
#include "level_data.hpp"
#include "obmap.hpp"
#include "save_data.hpp"
#include "smooth.hpp"
#include "soundob.hpp"
#include "text.hpp"
#include "video.hpp"

class VideoScreen : public Video
{
public:
    // Called with '1' for numviews
    VideoScreen();
    VideoScreen(Sint16 howmany);
    virtual ~VideoScreen();

    void reset(Sint16 howmany);
    void ready_for_battle(Sint16 howmany);
    void initialize_views();
    void cleanup(Sint16 howmany);
    void clear();
    Video *get_video_ob();
    bool query_passable(float x, float y, Walker *ob);
    bool query_object_passable(float x, float y, Walker *ob);
    bool query_grid_passable(float x, float y, Walker *ob);
    Sint16 redraw();
    void refresh();
    Walker *first_of(Uint8 whatorder, Uint8 whatfamily, Sint32 team_num=1);
    Sint16 input(SDL_Event const &event);
    Sint16 continuous_input();
    Sint16 act();

    Sint16 endgame(Sint16 ending);
    Sint16 endgame(Sint16 ending, Sint16 nextlevel); // What level next?
    void draw_panels(Sint16 howmany);
    Walker *find_near_foe(Walker *ob);
    Walker *find_far_foe(Walker *ob);
    Walker *find_nearest_blood(Walker *who);
    Walker *find_nearest_player(Walker *ob);
    std::list<Walker *> find_in_range(std::list<Walker *> &somelist, Sint32 range, Sint16 *howmany, Walker *ob);
    std::list<Walker *> find_foes_in_range(std::list<Walker *> &somelist, Sint32 range, Sint16 *howmany, Walker *ob);
    std::list<Walker *> find_friends_in_range(std::list<Walker *> &somelis, Sint32 range, Sint16 *howmany, Walker *ob);
    std::list<Walker *> find_foe_weapons_in_range(std::list<Walker *> &somelist, Sint32 range, Sint16 *howmany, Walker *ob);
    Uint8 damage_tile(Sint16 xloc, Sint16 yloc); // Damage the specified tile
    void do_notify(std::string const &message, Walker *who); // Printing text
    void report_mem();
    Walker *set_walker(Walker *ob, Uint8 order, Uint8 family);
    Uint8 const *get_scen_title(Uint8 const *filename, VideoScreen *master);
    void add_level_completed(std::string const &campaign, Sint32 level_index);

    Sint32 get_num_levels_completed(std::string const &campaign) const;
    bool is_level_completed(Sint32 level_index) const;

    // General drawing data
    Uint8 newpalette[768];
    Sint16 palmode;

    // Level data
    LevelData level_data;

    // Save data
    SaveData save_data;

    // Game state
    float control_hp; // Last turn's hitpoints
    Uint8 end;
    Uint8 timer_wait;

    // Set true when all our foes are dead
    Uint16 level_done;

    // We should reset the level and go again
    bool retry;

    std::string special_name[NUM_FAMILIES][NUM_SPECIALS];
    std::string alternate_name[NUM_FAMILIES][NUM_SPECIALS];

    // Stops enemies from acting
    Uint8 enemy_freeze;

    SoundObject *soundp;
    Uint16 redrawme;
    ViewScreen *viewob[5];
    Uint16 numviews;
    Uint32 timerstart;
    Uint32 framecount;
};

#endif
