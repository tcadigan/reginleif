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
#include "results_screen.hpp"

#include "base.hpp"
#include "button.hpp"
#include "guy.hpp"
#include "level_editor.hpp"
#include "picker.hpp"
#include "screen.hpp"
#include "stats.hpp"
#include "text.hpp"
#include "troop_result.hpp"
#include "util.hpp"
#include "walker.hpp"

#ifdef OUYA
#include "ouya_controller.hpp"
#endif

#include <sstream>

#define OG_OK 4

void show_ending_popup(Sint32 ending, Sint32 nextlevel)
{
    std::stringstream buf;
    // 1 = lose, for some reason
    if (ending == 1) {
        // Generic defeat
        if (nextlevel == -1) {
            popup_dialog("Defeat!", "YOUR MEN ARE CRUSHED!");
        } else {
            // We're withdrawing to another level
            buf << "Retreating to Level "
                << nextlevel << std::endl
                << "(You may take this field later)";

            std::string msg(buf.str());
            buf.clear();
            msg.resize(255);

            popup_dialog("Retreat!", msg);
        }
    } else if (ending == SCEN_TYPE_SAVE_ALL) {
        // Failed to save a guy
        popup_dialog("Defeat!", "YOU ARE DEFEATED!\nYOU FAILED TO KEEP YOUR ALLY ALIVE!");
    } else if (ending == 0) {
        // We won
        // This scenario is completed...
        if (myscreen->save_data.is_level_completed(myscreen->save_data.scen_num)) {
            buf << "Moving on to Level " << nextlevel;
            std::string msg(buf.str());
            buf.clear();
            msg.resize(255);

            popup_dialog("Traveling on...", msg);
        } else {
            popup_dialog("Victory!", "You have won the battle!");
        }
    }
}

bool results_screen(Sint32 ending, Sint32 nextlevel)
{
    // Popup the ending dialog
    show_ending_popup(ending, nextlevel);

    return false;
}

Sint32 get_num_foes(LevelData &level)
{
    Sint32 result = 0;

    for (auto const & ob : level.oblist) {
        // Not dead, not hired, not on team
        if (ob && !ob->dead && (ob->query_order() == ORDER_LIVING)
            && (ob->myguy == nullptr) && (ob->team_num != 0)) {
            ++result;
        }
    }

    return result;
}

Uint32 get_time_bonus(Sint32 playernum)
{
    if (playernum > 0) {
        return 0;
    }

    Uint32 frames = myscreen->framecount;
    Uint32 time_limit = 0;

    if (myscreen->level_data.time_bonus_limit > 0) {
        time_limit = myscreen->level_data.time_bonus_limit;
    }

    Log("Frames used: %d\n", frames);

    if (frames >= time_limit) {
        return 0;
    }

    Sint16 par_value = myscreen->level_data.par_value;
    Uint32 score = myscreen->save_data.m_score[playernum];
    float multiplier = ((1 + (par_value / 10.0f)) * static_cast<float>(time_limit - frames)) / time_limit;

    Log("Time bonus: %.0f\n", score * multiplier);

    return (score * multiplier);
}

bool results_screen(Sint32 ending, Sint32 nextlevel, std::map<Sint32, Guy *> &before, std::map<Sint32, Walker *> &after)
{
    // Popup the ending dialog
    show_ending_popup(ending, nextlevel);

    LevelData &level_data = myscreen->level_data;
    SaveData &save_data = myscreen->save_data;
    Sint32 num_foes_left = get_num_foes(level_data);
    Sint32 num_foes_total = 0;
    LevelData original_level(level_data.id);
    original_level.load();
    num_foes_total = get_num_foes(original_level);

    Text &mytext = myscreen->text_normal;
    Text &bigtext = myscreen->text_big;
    Uint32 bonuscash[4] = { 0, 0, 0, 0 };
    Uint32 allscore = 0;
    Uint32 allbonuscash = 0;
    std::stringstream buf;

    for (Sint32 i = 0; i < 4; ++i) {
        allscore += save_data.m_score[i];
    }

    // We won
    if (ending == 0) {
        // Calculate bonuses
        for (Sint32 i = 0; i < 4; ++i) {
            bonuscash[i] = get_time_bonus(i);
            allbonuscash += bonuscash[i];
        }

        // Already won, no bonus
        if (save_data.is_level_completed(save_data.scen_num)) {
            for (Sint32 i = 0; i < 4; ++i) {
                bonuscash[i] = 0;
            }

            allbonuscash = 0;
        }
    }

    // Now show the result
    std::set<Sint32> used_troops;
    std::vector<TroopResult> troops;

    // Get the guys from "before"
    for (auto const & e : before) {
        used_troops.insert(e.first);
        troops.push_back(TroopResult(e.second, after[e.first]));
    }

    // Get the ones from "after" that weren't in "before"
    for (auto const & e : after) {
        if (used_troops.insert(e.first).second) {
            troops.push_back(TroopResult(before[e.first], e.second));
        }
    }

    Walker *mvp = nullptr;
    float mvp_points = 0;

    for (auto const & e : troops) {
        float points = 0;

        if (e.after == nullptr) {
            continue;
        }

        points = e.after->myguy->scen_damage + (3 * e.after->myguy->scen_damage_taken);

        if (mvp_points < points) {
            mvp = e.after;
            mvp_points = points;
        }
    }

    // Hold indices for troops
    std::vector<Sint32> recruits;
    std::vector<Sint32> losses;
    Sint32 i = 0;

    for (auto const & e : troops) {
        if (e.is_dead()) {
            losses.push_back(i);
        } else if (e.is_new()) {
            recruits.push_back(i);
        }

        ++i;
    }

    bool retry = false;
    Sint32 mode = 0;
    float scroll = 0.0f;
    Sint32 frame = 0;
    Sint16 screenW = 320;
    Sint32 screenH = 200;

    SDL_Rect area;
    area.x = 50;
    area.y = 20;
    area.w = screenW - (2 * area.x);
    area.h = screenH - (2 * area.y);

    SDL_Rect area_inner = {
        area.x + 3,
        area.y + 17,
        area.w - 6,
        area.h - 34
    };

    // Buttons
    SDL_Rect ok_rect = {
        static_cast<Sint16>((area.x + (area.w / 2)) - 45),
        static_cast<Sint16>((area.y + area.h) - 14),
        35,
        10
    };

    SDL_Rect retry_rect = {
        static_cast<Sint16>((area.x + (area.w / 2)) - 100),
        static_cast<Sint16>((area.y + area.h) - 14),
        35,
        10
    };

    SDL_Rect overview_rect = {
        static_cast<Sint16>((area.x + (area.w / 2)) - 100),
        static_cast<Sint16>(area.y + 4),
        50,
        10
    };

    SDL_Rect troops_rect = {
        static_cast<Sint16>((area.x + (area.w / 2)) + 50),
        static_cast<Sint16>(area.y + 4),
        50,
        10
    };

    // Controller input
    Sint32 retvalue = 0;
    Sint32 highlighted_button = 0;
    Sint32 ok_index = 0;
    Sint32 retry_index = 1;
    Sint32 overview_index = 2;
    Sint32 troops_index = 3;
    Sint32 num_buttons = 4;

    Button buttons[] = {
        Button("OK", KEYSTATE_UNKNOWN, ok_rect.x, ok_rect.y, ok_rect.w, ok_rect.h, 0, -1, MenuNav::UpRight(overview_index, retry_index)),
        Button("RETRY", KEYSTATE_UNKNOWN, retry_rect.x, retry_rect.y, retry_rect.w, retry_rect.h, 0, -1, MenuNav::UpLeft(troops_index, ok_index)),
        Button("OVERVIEW", KEYSTATE_UNKNOWN, overview_rect.x, overview_rect.y, overview_rect.w, overview_rect.h, 0, -1, MenuNav::DownRight(ok_index, troops_index)),
        Button("TROOPS", KEYSTATE_UNKNOWN, troops_rect.x, troops_rect.y, troops_rect.w, troops_rect.h, 0, -1, MenuNav::DownLeft(retry_index, overview_index))
    };

    bool done = false;

    while (!done) {
        // Reset the timer count to zero...
        reset_timer();

        if (myscreen->end) {
            break;
        }

        // Get keys and stuff
        get_input_events(POLL);

        handle_menu_nav(buttons, highlighted_button, retvalue, false);

        // Mouse stuff...
        MouseState &mymouse = query_mouse();
        Sint32 mx = mymouse.x;
        Sint32 my = mymouse.y;

#ifdef USE_CONTROLLER_INPUT
        OuyaController const &c = OuyaControllerManager::getController(0);
        float v = c.getAxisValue(OuyaController::AXIS_LS_Y) + c.getAxisValue(OuyaController::AXIS_RS_Y);

        if (fabs(v) > OuyaController::DEADZONE) {
            scroll -= (-5 * v);
        }
#else
        scroll -= get_and_reset_scroll_amount();
#endif

        if (scroll < 0.0f) {
            scroll = 0.0f;
        }

        bool do_click = mymouse.left;
        bool do_ok = ((do_click
                       && (ok_rect.x <= mx)
                       && (mx <= (ok_rect.x + ok_rect.w))
                       && (ok_rect.y <= my)
                       && (my <= (ok_rect.y + ok_rect.h)))
                      || ((retvalue == OG_OK)
                          && (highlighted_button == ok_index)));

        bool do_retry = ((do_click
                          && (retry_rect.x <= mx)
                          && (mx <= (retry_rect.x + retry_rect.w))
                          && (retry_rect.y <= my)
                          && (my <= (retry_rect.y + retry_rect.h)))
                         || ((retvalue == OG_OK)
                             && (highlighted_button == retry_index)));

        bool do_overview = ((do_click
                             && (overview_rect.x <= mx)
                             && (mx <= (overview_rect.x + overview_rect.w))
                             && (overview_rect.y <= my)
                             && (my <= (overview_rect.y + overview_rect.h)))
                            || ((retvalue == OG_OK)
                                && (highlighted_button == overview_index)));

        bool do_troops = ((do_click
                           && (troops_rect.x <= mx)
                           && (mx <= (troops_rect.x + troops_rect.w))
                           && (troops_rect.y <= my)
                           && (my <= (troops_rect.y + troops_rect.h)))
                          || ((retvalue == OG_OK)
                              && (highlighted_button == troops_index)));

        if (mymouse.left) {
            while (mymouse.left) {
                get_input_events(WAIT);
            }
        }

        // Ok
        if (do_ok) {
            myscreen->soundp->play_sound(SOUND_BOW);
            done = true;
        } else if (do_retry) {
            // Retry
            myscreen->soundp->play_sound(SOUND_BOW);


            if (ending == 0) {
                buf << "Try this level again?" << std::endl
                    << "You will lose your progress" << std::endl
                    << "on this level.";
            } else {
                buf << "Try this level again?";
            }

            std::string msg(buf.str());
            buf.clear();

            if (yes_or_no_prompt("Retry level", msg, false)) {
                // Try again
                done = true;
                retry = true;
            }
        } else if (do_overview) {
            // Overview
            myscreen->soundp->play_sound(SOUND_BOW);
            mode = 0;
        } else if (do_troops) {
            // Troops
            myscreen->soundp->play_sound(SOUND_BOW);
            mode = 1;
        }

        retvalue = 0;

        // Draw
        myscreen->draw_button(area.x, area.y, (area.x + area.w) - 1, (area.y + area.h) - 1, 1, 1);
        myscreen->draw_button_inverted(area_inner.x, area_inner.y, area_inner.w, area_inner.h);
        bigtext.write_xy_center(area.x + (area.w / 2), area.y + 4, RED, "RESULTS");

        int y = 0;

        if (mode == 0) {
            // Overview
            Sint32 x = area.x + 12;
            y = (area.y + 30) - scroll;

            if (ending == 0) {
                // TODO: Show total possible gold collected? Hos is this factored into allscore
                if ((area_inner.y < y) && ((y + 10) < (area_inner.y + area_inner.h))) {
                    buf << allscore * 2 << " Gold       ";
                    std::string msg(buf.str());
                    buf.clear();

                    mytext.write_xy_center_shadow(area.x + (area.w / 2), y, YELLOW, msg);
                    buf << allscore * 2 << "      Gained";
                    msg = buf.str();
                    buf.clear();
                    mytext.write_xy_center(area.x + (area.w / 2), y, DARK_BLUE, msg);
                }

                if ((area_inner.y < y) && ((y + 10) < (area_inner.y + area_inner.h))) {
                    if (allbonuscash > 0) {
                        mytext.write_xy_center_shadow(area.x + (area.w / 2), y + 9, YELLOW, "+ %d time Bonus", allbonuscash);
                    }
                }

                y += 22;
            }

            if ((area_inner.y < y) && ((y + 10) < (area_inner.y + area_inner.h))) {
                if (ending == 0) {
                    buf << num_foes_total - num_foes_left << " Foes         ";
                    std::string msg(buf.str());
                    buf.clear();

                    mytext.write_xy_center_shadow(area.x + (area.w / 2), y, PURE_WHITE, msg);

                    buf << num_foes_total - num_foes_left << "      Defeated";
                    msg = buf.str();
                    buf.clear();

                    mytext.write_xy_center(area.x + (area.w / 2), y, DARK_BLUE, msg);
                } else {
                    buf << num_foes_total - num_foes_left << " of "
                        << num_foes_total << " Foes         ";
                    std::string msg(buf.str());
                    buf.clear();

                    mytext.write_xy_center_shadow(area.x + (area.w / 2), y, PURE_WHITE, msg);

                    buf << num_foes_total - num_foes_left << "    "
                        << num_foes_total << "      Defeated";
                    msg = buf.str();
                    buf.clear();

                    mytext.write_xy_center(area.x + (area.w / 2), y, DARK_BLUE, msg);
                }
            }

            y += 22;

            if (mvp != nullptr) {
                if ((area_inner.y < y) && ((y + 10) < (area_inner.y + area_inner.h))) {
                    buf << "MVP: " << mvp->myguy->name << " the "
                        << get_family_string(mvp->myguy->family);
                    mytext.write_xy_center(area.x + (area.w / 2), y, DARK_BLUE, buf.str());
                    buf.clear();
                    mytext.write_xy_center(area.x + (area.w / 2), y + 8, DARK_BLUE, "(%.0f pts)", mvp_points);
                    y += 22;
                }
            }

            if ((ending == 0) && (recruits.size() > 0)) {
                if ((area_inner.y < y) && ((y + 10) < (area_inner.y + area_inner.h))) {
                    mytext.write_xy(x, y, DARK_BLUE, "%d Recruits:", recruits.size());
                }

                y += 22;

                for (auto const & e : recruits) {
                    if ((area_inner.y < y) && ((y + 10) < (area_inner.y + area_inner.h))) {
                        buf << " + " << troops[e].get_name() << " the "
                            << troops[e].get_class_name() << " LVL "
                            << troops[e].get_level();

                        mytext.write_xy(x, y, DARK_BLUE, buf.str());
                        buf.clear();
                    }

                    y += 11;
                }

                y += 11;
            }

            // Won or lost due to NPC
            if ((ending != 1) && (losses.size() > 0)) {
                if ((area_inner.y < y) && ((y + 10) < (area_inner.y + area_inner.h))) {
                    mytext.write_xy(x, y, DARK_BLUE, "%d Losses:", losses.size());
                }

                y += 22;

                for (auto const & e : losses) {
                    if ((area_inner.y < y) && ((y + 10) < (area_inner.y + area_inner.h))) {
                        buf << " - " << troops[e].get_name() << " the "
                            << troops[e].get_class_name() << " LVL "
                            << troops[e].get_level();

                        mytext.write_xy(x, y, DARK_BLUE, buf.str());
                        buf.clear();
                    }

                    y += 11;
                }

                y += 11;
            }
        } else if (mode == 1) {
            Sint32 barH = 5;

            // Troops
            y = (area.y + 30) - scroll;

            for (Uint32 i = 0; i < troops.size(); ++i) {
                Sint32 x = area.x + 12;
                Sint32 tallies = troops[i].get_tallies();

                if ((area_inner.y < y) && ((y + 10) < (area_inner.y + area_inner.h))) {
                    Sint32 name_w = mytext.write_xy(x, y, PURE_BLACK, troops[i].get_name());
                    buf << " the " << troops[i].get_class_name();
                    name_w += mytext.write_xy(x + name_w, y, PURE_BLACK + 2, buf.str());
                    buf.clear();

                    if (troops[i].gained_level()) {
                        buf << " LVL UP " << troops[i].get_level();
                        mytext.write_xy(x + name_w, y, YELLOW, buf.str());
                        buf.clear();
                    } else if (troops[i].lost_level()) {
                        buf << " LVL DOWN " << troops[i].get_level();
                        mytext.write_xy(x + name_w, y, RED, buf.str());
                        buf.clear();
                    } else {
                        buf << " LVL " << troops[i].get_level();
                        mytext.write_xy(x + name_w, y, DARK_GREEN, buf.str());
                        buf.clear();
                    }
                }

                y += 10;

                if ((area_inner.y < y) && ((y + 10) < (area_inner.y + area_inner.h))) {
                    troops[i].draw_guy(x + 5, y + 2, frame * troops[i].get_hp());

                    // HP
                    if (troops[i].is_dead()) {
                        mytext.write_xy(x + 15, y, RED, "LOST");
                    } else {
                        x += 15;
                        mytext.write_xy(x, y, RED, "HP");
                        x += 14;
                        myscreen->fastbox(x, y, 60 * troops[i].get_hp(), barH, RED);
                        myscreen->fastbox_outline(x, y, 60, barH, PURE_BLACK);

                        // XP
                        x += 70;
                        mytext.write_xy(x, y, DARK_GREEN, "EXP");
                        x += 20;
                        float base = 60 * troops[i].get_xp_base();
                        float gain = 60 * troops[i].get_xp_gain();

                        if (gain >= 0) {
                            myscreen->fastbox(x, y, base, barH, DARK_GREEN);
                            myscreen->fastbox(x + base, y, gain, barH, LIGHT_GREEN);
                        } else {
                            myscreen->fastbox((x + 60) + gain, y, -gain, barH, RED);
                        }

                        myscreen->fastbox_outline(x, y, 60, barH, PURE_BLACK);

                        if (gain > 0.0f) {
                            mytext.write_xy(x + 63, y, DARK_GREEN, "+");
                        } else if (gain < 0.0f) {
                            mytext.write_xy(x + 63, y, RED, "-");
                        }
                    }
                }

                if (tallies > 0) {
                    y += 10;

                    if ((area_inner.y < y) && ((y + 10) < (area_inner.y + area_inner.h))) {
                        buf << tallies << " Tall";

                        if (tallies == 1) {
                            buf << "y";
                        } else {
                            buf << "ies";
                        }

                        mytext.write_xy(area.x + 20, y, DARK_GREEN, buf.str());
                        buf.clear();
                    }
                }

                if (troops[i].gained_level()) {
                    std::vector<std::string> specials = troops[i].get_gained_specials();

                    if (!specials.empty()) {
                        y += 10;

                        if ((area_inner.y < y) && ((y + 10) < (area_inner.y + area_inner.h))) {
                            buf << "Gained special";

                            if (specials.size() != 1) {
                                buf << "s";
                            }

                            mytext.write_xy(area.x + 20, y, DARK_BLUE, buf.str());
                            buf.clear();
                        }

                        for (auto const & e : specials) {
                            y += 10;

                            if ((area_inner.y < y) && ((y + 10) < (area_inner.y + area_inner.h))) {
                                mytext.write_xy(area.x + 30, y, DARK_BLUE, e);
                            }
                        }
                    }
                }

                y += 13;

                if ((area_inner.y < y) && ((y + 10) < (area_inner.y + area_inner.h))) {
                    myscreen->hor_line(area_inner.x + 6, y - 3, area_inner.w - 30, GREY - 4);
                }
            }
        }

        // Draw scroll indicator
        if ((y + scroll) > ((area_inner.y + area_inner.h) - 30)) {
            myscreen->ver_line(area_inner.x, area_inner.y + (area_inner.h * (scroll / ((y + scroll) - area.y))), 6, PURE_BLACK);
        }

        // Limit the scrolling depending on how long 'y' is
        if (y < (area_inner.y + 30)) {
            scroll = (y + scroll) - (area_inner.y + 30);
        }

        for (Sint32 i = 0; i < num_buttons; ++i) {
            if (((mode == 0) && (i == overview_index)) || ((mode == 1) && (i == troops_index))) {
                myscreen->draw_button_inverted(buttons[i].x, buttons[i].y, buttons[i].sizex, buttons[i].sizey);
            } else {
                myscreen->draw_button(buttons[i].x, buttons[i].y, (buttons[i].x + buttons[i].sizex) - 1, (buttons[i].y + buttons[i].sizey) - 1, 1, 1);
            }

            mytext.write_xy((buttons[i].x + (buttons[i].sizex / 2)) - (3 * buttons[i].label.size()), buttons[i].y + 2, buttons[i].label, DARK_BLUE, 1);
        }

        draw_highlight(buttons[highlighted_button]);
        myscreen->buffer_to_screen(0, 0, 320, 200);
        SDL_Delay(10);

        ++frame;

        if (frame > 1000000) {
            frame = 0;
        }
    }

    return retry;
}
