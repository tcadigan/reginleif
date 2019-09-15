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
#ifndef __WALKER_HPP__
#define __WALKER_HPP__

// Definition of WALKER class

#include "base.hpp"
#include "obmap.hpp"
#include "pixien.hpp"

// TODO: Move this to screen class so it doesn't get overlapped
//       by other walkers drawing
class DamageNumber
{
public:
    DamageNumber(float x, float y, float value, Uint8 color);

    void draw(viewscreen *view_buf);

    float x;
    float y;
    float t;
    float value;
    Uint8 color;
};

class walker : public pixieN
{
public:
    walker(PixieData const &data);
    virtual ~walker();

    Sint16 reset(void);
    Sint16 move(Sint16 x, Sint16 y);
    void worldmove(float x, float y);
    void setworldxy(float x, float y);
    bool walk();
    bool walkstep(float x, float y);
    Sint16 draw(viewscreen *view_buf);
    Sint16 draw_tile(viewscreen *view_buf);
    void draw_path(viewscreen *view_buf);
    void find_path_to_foe();
    void follow_path_to_foe();
    Sint16 init_file();
    Sint16 init_file(Sint16 xdir, Sint16 ydir);
    void set_weapon_heading(walker *weapon);
    walker *fire();
    Sint16 set_act_type(Sint16 num);
    Sint16 restore_act_type();
    Sint16 query_act_type();
    Sint16 set_old_act_type(Sint16 num);
    Sint16 query_old_act_type();
    Sint16 attack(walker *target);
    Sint16 set_order_family(Uint8 order, Uint8 family);
    walker *create_weapon();
    Sint16 fire_check(Sint16 xdelta, Sint16 ydelta);
    Sint16 query_next_to();
    Sint16 special();
    Sint16 teleport();
    Sint16 teleport_ranged(Sint32 range);
    Sint32 turn_undead(Sint32 range, Sint32 power);
    bool turn(Sint16 targetdir);
    // How many (of 8) spaces around us are clear
    Sint16 spaces_clear();
    // Transfer values to new walker
    void transfer_stats(walker *newob);
    // Change picture, etc.
    void transform_to(Uint8 whatorder, Uint8 whatfamily);
    // Make a permanent stain...
    void generate_bloodspot();
    // Center us on target
    void center_on(walker *target);
    Sint32 distance_to_ob(walker *target);
    Sint32 distance_to_ob_center(walker *target);
    Uint8 query_team_color();
    Sint32 is_friendly(walker *target);
    Sint32 is_friendly_to_team(Uint8 team);
    float get_current_angle();
    void do_heal_effects(walker *healer, walker *target, Sint16 amount);
    void do_hit_effects(walker *attacker, walker *target, Sint16 tempdamage);
    void do_combat_damage(walker *attacker, walker *target, Sint16 tempdamage);

    Uint8 query_family()
    {
        return family;
    }

    Sint16 query_type(Uint8 oval, Uint8 fval)
    {
        return ((oval == order) && (fval == family));
    }

    // Called when death/destruction occurs...
    virtual Sint16 death();
    virtual Sint16 facing(Sint16 x, Sint16 y);
    virtual void set_difficulty(Uint32 whatlevel);
    virtual walker *do_summon(Uint8 whatfamily, Uint16 lifetime);
    virtual Sint16 check_special();
    virtual Sint16 shove(walker *target, Sint16 x, Sint16 y);
    virtual Sint16 eat_me(walker *eater);
    virtual void set_direct_frame(Sint16 whichframe);
    virtual Sint16 animate();
    virtual Sint16 collide(walker *ob);
    virtual short act();
    virtual bool walk(float x, float y);
    virtual Sint16 setxy(Sint16 x, Sint16 y);
    virtual Uint8 query_order()
    {
        return order;
    }

    friend class statistics;
    friend class command;

    // Used to open doors
    Uint32 keys;
    // Used for seeing treasures, etc. on radar
    Sint16 view_all;
    // Is our shifter/alternate key pressed?
    Sint16 shifter_down;
    // Used if an object has extra rounds this cycle
    Sint16 bonus_rounds;
    // If death has already been called
    Sint16 death_called;
    // For fighter's blades
    Sint16 weapons_left;
    Sint16 yo_delay;
    // Current direction facing
    Uint8 curdir;
    Uint8 cycle;
    Uint8 **ani;
    Uint8 action;

    // Zardus: FIX: Let's make these unsigned so that real_team_num doesn't
    //              wrap around from 255 to -1 :)
    Uint8 team_num;
    // For "Charm", etc.
    Uint8 ream_team_num;
    Uint8 ani_type;
    // Floating point buffer for movement
    float worldx;
    float worldy;
    float stepsize;
    // Used for elven forestwalk
    float normal_stepsize;
    Sint32 lineofsight;
    float damage;
    float fire_frequency;
    float busy;
    statistics *stats;
    walker *collide_ob;
    walker *foe;
    walker *leader;
    // For weapons
    walker *owner;
    walker *myself;
    // Our special stats...
    guy *myguy;
    // Safety check
    Sint16 dead;
    // For non-colliding objects
    Uint8 ignore;
    Uint8 default_weapon;
    Uint8 current_weapon;
    // Are we being used by anyone?
    Uint8 user;
    // For bonus flight...
    Sint16 flight_left;
    Sint16 invulnerable_left;
    Sint16 invisibility_left;
    // If we're still being charmed
    Sint16 charm_left;
    Uint8 drawcycle;
    Uint8 current_special;
    // How much like summoned guys have...
    Sint32 lifetime;
    // Cycles after failed exit choice
    Sint16 skip_exit;
    Uint8 outline;
    // These two are used for speed potions, etc.
    Sint16 speed_bonus;
    Sint16 speed_bonus_left;
    // Delay after being hit
    Sint16 regen_delay;

    // Zardus: ADD: in_act should be set wile in action
    bool in_act;
    obmap *myobmap;
    Sint32 path_check_counter;
    // Result from pathfinding
    std::vector<void *> path_to_foe;

    std::list<DamageNumber> damage_numbers;

    bool hurt_flash;
    float attack_lunge;
    float attack_lunge_angle;
    float hit_recoil;
    float hit_recoil_angle;

    float last_hitpoints;

protected:
    Sint16 act_generate();
    Sint16 act_fire();
    Sint16 act_guard();

    virtual Sint16 act_random();

    Uint8 act_type;
    Uint8 old_act_type;
    // Proposed direction facing
    Uint8 enddir;
    Uint8 order;
    Uint8 family;
};

#endif
