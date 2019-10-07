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
#include "button.hpp"

#include "gloader.hpp"
#include "gparser.hpp"
#include "input.hpp"
#include "io.hpp"
#include "screen.hpp"

extern Sint16 scen_level;
extern PixieN *backdrops[5];

#define REDRAW 2 // We just exited a menu, so redraw your buttons
#define OK 4 // This function was successful, continue normal operation

VirtualButton *allbuttons[MAX_BUTTONS];
Sint16 dumbcount;

VirtualButton *init_buttons(Button *buttons, Sint32 numbuttons)
{
    Sint32 i;

    // Skip # 0!
    for (i = 0; i < MAX_BUTTONS; ++i) {
        if (allbuttons[i]) {
            delete allbuttons[i];
        }

        allbuttons[i] = nullptr;
    }

    for (i = 0; i < numbuttons; ++i) {
        allbuttons[i] = new VirtualButton(buttons[i].x, buttons[i].y,
                                          buttons[i].sizex, buttons[i].sizey, buttons[i].myfun,
                                          buttons[i].arg1, buttons[i].label, buttons[i].hotkey);
        allbuttons[i]->hidden = buttons[i].hidden;
        allbuttons[i]->no_draw = buttons[i].no_draw;
    }

    return allbuttons[0];
}

void draw_backdrop()
{
    Sint32 i;

    for (i = 0; i < 5; ++i) {
        if (backdrops[i]) {
            backdrops[i]->draw(myscreen->viewob[0]);
        }
    }
}

void draw_buttons(Button *buttons, Sint32 numbuttons)
{
    Sint32 i;

    for (i = 0; i < numbuttons; ++i) {
        if (buttons[i].hidden || buttons[i].no_draw) {
            continue;
        }

        allbuttons[i]->vdisplay();
        myscreen->draw_box(allbuttons[i]->xloc - 1,
            allbuttons[i]->yloc - 1, allbuttons[i]->xend, allbuttons[i]->yend,
            0, 0, 1);
    }
}

Sint32 yes_or_no(Sint32 arg)
{
    return arg;
}

void toggle_effect(std::string const &category, std::string const &setting)
{
    if (cfg.is_on(category, setting)) {
        cfg.apply_setting(category, setting, "off");
    } else {
        cfg.apply_setting(category, setting, "on");
    }
}

void toggle_rendering_engine()
{
    std::string engine = cfg.get_setting("graphics", "render");

    if (engine == "sai") {
        engine = "eagle";
    } else if (engine == "eagle") {
        engine = "normal";
    } else {
        engine = "sai";
    }

    cfg.apply_setting("graphics", "render", "engine");
}

MenuNav MenuNav::Up(Sint32 up)
{
    return MenuNav(up, -1, -1, -1);
}

MenuNav MenuNav::Down(Sint32 down)
{
    return MenuNav(-1, down, -1, -1);
}

MenuNav MenuNav::Left(Sint32 left)
{
    return MenuNav(-1, -1, left, -1);
}

MenuNav MenuNav::Right(Sint32 right)
{
    return MenuNav(-1, -1, -1, right);
}

MenuNav MenuNav::UpDown(Sint32 up, Sint32 down)
{
    return MenuNav(up, down, -1, -1);
}

MenuNav MenuNav::UpLeft(Sint32 up, Sint32 left)
{
    return MenuNav(up, -1, left, -1);
}

MenuNav MenuNav::UpRight(Sint32 up, Sint32 right)
{
    return MenuNav(up, -1, -1, right);
}

MenuNav MenuNav::UpDownLeft(Sint32 up, Sint32 down, Sint32 left)
{
    return MenuNav(up, down, left, -1);
}

MenuNav MenuNav::UpDownRight(Sint32 up, Sint32 down, Sint32 right)
{
    return MenuNav(up, down, -1, right);
}

MenuNav MenuNav::UpLeftRight(Sint32 up, Sint32 left, Sint32 right)
{
    return MenuNav(up, -1, left, right);
}

MenuNav MenuNav::DownLeft(Sint32 down, Sint32 left)
{
    return MenuNav(-1, down, left, -1);
}

MenuNav MenuNav::DownRight(Sint32 down, Sint32 right)
{
    return MenuNav(-1, down, -1, right);
}

MenuNav MenuNav::DownLeftRight(Sint32 down, Sint32 left, Sint32 right)
{
    return MenuNav(-1, down, left, right);
}

MenuNav MenuNav::LeftRight(Sint32 left, Sint32 right)
{
    return MenuNav(-1, -1, left, right);
}

MenuNav MenuNav::UpDownLeftRight(Sint32 up, Sint32 down, Sint32 left, Sint32 right)
{
    return MenuNav(up, down, left, right);
}

MenuNav MenuNav::All(Sint32 up, Sint32 down, Sint32 left, Sint32 right)
{
    return MenuNav(up, down, left, right);
}

MenuNav MenuNav::None()
{
    return MenuNav();
}

MenuNav::MenuNav()
    : up(-1)
    , down(-1)
    , left(-1)
    , right(-1)
{
}

MenuNav::MenuNav(Sint32 up, Sint32 down, Sint32 left, Sint32 right)
    : up(up)
    , down(down)
    , left(left)
    , right(right)
{
}

// VirtualButton functions, VirtualButton is a button class that will be self-controlled
VirtualButton::VirtualButton(Sint32 xpos, Sint32 ypos, Sint32 wide, Sint32 high,
                             Sint32 func(Sint32), Sint32 pass, std::string const &msg,
                             Sint32 hot)
{
    arg = pass;
    fun = func;
    myfunc = 0;
    xloc = xpos;
    yloc = ypos;
    width = wide;
    height = high;
    xend = xloc + width;
    yend = yloc + height;
    label = msg;
    next = nullptr;
    // prev = nullptr;
    had_focus = 0;
    do_outline = 0;
    depressed = 0;
    mypixie = nullptr; // By default, no graphic picture
    hotkey = hot;

    // vdisplay();
    color = BUTTON_FACING;
    hidden = false;
    no_draw = false;
}

VirtualButton::VirtualButton(Sint32 xpos, Sint32 ypos, Sint32 wide, Sint32 high,
                             Sint32 func_code, Sint32 pass, std::string const &msg, Sint32 hot)
{
    arg = pass;
    fun = nullptr; // Don't use this!
    myfunc = func_code;
    xloc = xpos;
    yloc = ypos;
    width = wide;
    height = high;
    xend = xloc + width;
    yend = yloc + height;
    label = msg;
    next = nullptr;
    // prev = nullptr;
    had_focus = 0;
    do_outline = 0;
    mypixie = nullptr; // No graphic by default
    hotkey = hot;

    // vdisplay();
    color = BUTTON_FACING;
    hidden = false;
    no_draw = false;
}

VirtualButton::VirtualButton(Sint32 xpos, Sint32 ypos, Sint32 wide, Sint32 high,
                             Sint32 func_code, Sint32 pass, std::string const &msg,
                             Uint8 family, Sint32 hot)
{
    arg = pass;
    fun = nullptr; // Don't use this!
    myfunc = func_code;
    xloc = xpos;
    yloc = ypos;
    width = wide;
    height = high;
    xend = xloc + width;
    yend = yloc + height;
    label = msg;
    next = nullptr;
    // prev = nullptr;
    had_focus = 0;
    do_outline = 0;
    depressed = 0;
    mypixie = myscreen->level_data.myloader->create_pixieN(ORDER_BUTTON1, family);
    hotkey = hot;
    width = mypixie->sizex;
    height = mypixie->sizey;
    xend = xloc + width;
    yend = yloc + height;

    // vdisplay();
    color = BUTTON_FACING;
    hidden = false;
    no_draw = false;
}

// For pointers
VirtualButton::VirtualButton()
{
    next = nullptr;
    // prev = nullptr;
    had_focus = 0;
    do_outline = 0;
    depressed = 0;
    mypixie = nullptr;
}

VirtualButton::~VirtualButton()
{
    // myscreen->draw_box(xloc - 4, yloc - 4, xend + 4, yend + 4, 0, 1, 1);
    delete mypixie;

    /*
     * release_mouse();
     * myscreen->buffer_to_screen(xloc - 4, yloc - 4, xend + 4, yend + 4);
     * grab_mouse();
     *
     * if (next != nullptr) {
     *     delete next;
     *     next = nullptr;
     * }
     */
}

void VirtualButton::set_graphic(Uint8 family)
{
    if (mypixie) {
        delete mypixie;
    }

    mypixie = myscreen->level_data.myloader->create_pixieN(ORDER_BUTTON1, family);
    width = mypixie->sizex;
    height = mypixie->sizey;
    xend = xloc + width;
    yend = yloc + height;
    // vdisplay();
}

void VirtualButton::vdisplay()
{
    if (hidden || no_draw) {
        return;
    }

    if (do_outline) {
        vdisplay(2);

        return;
    }

    Text &mytext = myscreen->text_normal;

    // Then use the graphic
    if (mypixie) {
        mypixie->draw(xloc, yloc, myscreen->viewob[0]);

        if (label.size()) {
            mytext.write_xy(static_cast<Sint16>(((xloc + xend) / 2) - (((label.size() - 1) * (mytext.letters.w + 1)) / 2)),
                            static_cast<Sint16>(yloc + ((height - mytext.letters.h) / 2)),
                            label,
                            static_cast<Uint8>(DARK_BLUE),
                            1);
        }
    } else {
        // Front
        myscreen->draw_box(xloc, yloc, xend - 1, yend - 1, color, 1, 1);

        // Top edge
        myscreen->draw_box(xloc, yloc, xend - 2, yloc, BUTTON_TOP, 1, 1);

        // Left
        myscreen->draw_box(xloc, yloc + 1, xloc, yend - 2, BUTTON_LEFT, 1, 1);

        // Right
        myscreen->draw_box(xend - 1, yloc + 1, xend - 1, yend - 2, BUTTON_RIGHT, 1, 1);

        // Bottom
        myscreen->draw_box(xloc + 1, yend - 1, xend - 1, yend - 1, BUTTON_BOTTOM, 1, 1);

        if (label.size()) {
            mytext.write_xy(static_cast<Sint16>(((xloc + xend) / 2) - (((label.size() - 1) * (mytext.letters.w + 1)) / 2)),
                            static_cast<Sint16>(yloc + ((height - mytext.letters.h) / 2)),
                            label.c_str(),
                            static_cast<Uint8>(DARK_BLUE),
                            1);
        }
    }
}

void VirtualButton::vdisplay(Sint32 status)
{
    if (hidden || no_draw) {
        return;
    }

    // Do normal
    if (!status) {
        vdisplay();

        return;
    }

    Text &mytext = myscreen->text_normal;

    // Then use the graphic
    if (mypixie) {
        mypixie->draw(xloc, yloc, myscreen->viewob[0]);

        if (label.size()) {
            mytext.write_xy(static_cast<Sint16>(((xloc + xend) / 2) - (((label.size() - 1) * (mytext.letters.w + 1)) / 2)),
                            static_cast<Sint16>(yloc + ((height - mytext.letters.h) / 2)),
                            label.c_str(),
                            static_cast<Uint8>(DARK_BLUE),
                            1);
        }
    } else if (status == 1) {
        // Front
        myscreen->draw_box(xloc, yloc, xend - 1, yend - 1, BUTTON_FACING - 3, 1, 1);

        // Top edge
        myscreen->draw_box(xloc, yloc, xend - 2, yloc, BUTTON_BOTTOM, 1, 1);

        // Left
        myscreen->draw_box(xloc, yloc + 1, xloc, yend - 2, BUTTON_LEFT, 1, 1);

        // Right
        myscreen->draw_box(xend - 1, yloc + 1, xend - 1, yend - 2, BUTTON_RIGHT, 1, 1);

        // Bottom
        myscreen->draw_box(xloc + 1, yend - 1, xend - 1, yend - 1, BUTTON_TOP, 1, 1);

        if (label.size()) {
            mytext.write_xy(static_cast<Sint16>(((xloc + xend) / 2) - (((label.size() - 1) * (mytext.letters.w + 1)) / 2)),
                            static_cast<Sint16>(yloc + ((height - mytext.letters.h) / 2)),
                            label,
                            static_cast<Uint8>(DARK_BLUE),
                            1);
        }
    } else if (status == 2) { // Special (red) button...
        // Front
        myscreen->draw_box(xloc, yloc, xend - 1, yend - 1, BUTTON_FACING + 32, 1, 1);

        // Top edge
        myscreen->draw_box(xloc, yloc, xend - 2, yloc, BUTTON_TOP + 32, 1, 1);

        // Left
        myscreen->draw_box(xloc, yloc + 1, xloc, yend - 2, BUTTON_LEFT + 32, 1, 1);

        // Right
        myscreen->draw_box(xend - 1, yloc + 1, xend - 1, yend - 2, BUTTON_RIGHT + 32, 1, 1);

        // Bottom
        myscreen->draw_box(xloc + 1, yend - 1, xend - 1, yend - 1, BUTTON_BOTTOM + 32, 1, 1);

        if (label.size()) {
            mytext.write_xy(static_cast<Sint16>(((xloc + xend) / 2) - (((label.size() - 1) * (mytext.letters.w + 1)) / 2)),
                static_cast<Sint16>(yloc + ((height - mytext.letters.h) / 2)),
                            label.c_str(),
                static_cast<Uint8>(DARK_BLUE),
                1);
        }
    }

    release_mouse();
    // buffers: myscreen->buffer_to_screen(0, 0, 320, 200);
    // Zardus: The following isn't really needed and it messes up the fading
    // myscreen->buffer_to_screen(xloc, yloc, xend - xloc, yend - yloc);
    grab_mouse();
}

Sint32 VirtualButton::leftclick(Button *buttons)
{
    Sint32 whichone = 0;
    Sint32 retvalue = 0;

    // First check hotkeys...
    while (allbuttons[whichone]) {
        if ((buttons == nullptr) || !buttons[whichone].hidden) {
            retvalue = allbuttons[whichone]->leftclick(1);

            if (retvalue != -1) {
                return retvalue;
            }
        }

        ++whichone;
    }

    // Now a normal click...
    whichone = 0;

    while (allbuttons[whichone]) {
        if ((buttons == nullptr) || !buttons[whichone].hidden) {
            retvalue = allbuttons[whichone]->leftclick(2);

            if (retvalue != -1) {
                return retvalue;
            }
        }

        ++whichone;
    }

    // None worked
    return 0;
}

Sint32 VirtualButton::rightclick(Button *buttons)
{
    Sint32 whichone = 0;
    Sint32 retvalue = 0;

    while (allbuttons[whichone]) {
        if ((buttons == nullptr) || !buttons[whichone].hidden) {
            retvalue = allbuttons[whichone]->rightclick(whichone);

            if (retvalue != -1) {
                return retvalue;
            }
        }

        ++whichone;
    }

    // None worked
    return 0;
}

Sint32 VirtualButton::leftclick(Sint32 whichbutton)
{
    if (hidden) {
        return -1;
    }

    Sint32 retvalue = 0;

    // Hotkeys
    if (whichbutton == 1) {
        if (keystates[hotkey]) {
            myscreen->soundp->play_sound(SOUND_BOW);
            vdisplay(1);
            vdisplay();

            if (myfunc) {
                retvalue = do_call(myfunc, arg);
            }

            while (keystates[hotkey]) {
                get_input_events(WAIT);
            }

            return retvalue;
        }
    } else if (whichbutton == 2) { // Normal click
        if (mouse_on()) {
            myscreen->soundp->play_sound(SOUND_BOW);
            vdisplay(1);
            vdisplay();

            if (myfunc) {
                retvalue = do_call(myfunc, arg);
            }

            return retvalue;
        }
    }

    // Wasn't focused on us
    return -1;
}

Sint32 VirtualButton::rightclick(Sint32 whichbutton)
{
    if (hidden) {
        return -1;
    }

    Sint32 retvalue = 0;

    if (whichbutton) {
        whichbutton = 1;
    }

    if (mouse_on()) {
        myscreen->soundp->play_sound(SOUND_BOW);
        vdisplay(1);
        vdisplay();

        if (myfunc) {
            retvalue = do_call_right(myfunc, arg);
        }

        return retvalue;
    }

    // Wasn't focused on us
    return -1;
}

Sint32 VirtualButton::mouse_on()
{
    if (hidden) {
        return 0;
    }

    Sint32 mousex;
    Sint32 mousey;
    MouseState &mymouse = query_mouse();
    mousex = mymouse.x;
    mousey = mymouse.y;

    if ((mousex > xloc) && (mousex < xend) && (mousey > yloc) && (mousey < yend)) {
        // Just gained focus
        if (!had_focus) {
            // vdisplay();
            if (mypixie) {
                myscreen->draw_box(xloc - 1, yloc - 1, xend, yend, 27, 0, 1);
            } else {
                myscreen->draw_box(xloc - 1, yloc - 1, xend, yend, 27, 0, 1);
            }

            myscreen->buffer_to_screen(0, 0, 320, 200);

            had_focus = 1;
        }

        return 1;
    } else {
        if (had_focus) {
            // vdisplay();
            if (mypixie) {
                myscreen->draw_box(xloc - 1, yloc - 1, xend, yend, 0, 0, 1);
            } else {
                myscreen->draw_box(xloc - 1, yloc - 1, xend, yend, 0, 0, 1);
            }

            myscreen->buffer_to_screen(0, 0, 320, 200);
        }

        return 0;
    }
}

Sint32 VirtualButton::do_call(Sint32 whatfunc, Sint32 arg)
{
    switch (whatfunc) {
    case BEGIN_MENU:
        return beginmenu(arg);
    case CREATE_TEAM_MENU:
        return create_team_menu(arg);
    case SET_PLAYER_MODE:
        return set_player_mode(arg);
    case QUIT_MENU:
        quit(arg);

        return 1;
    case CREATE_VIEW_MENU:
        return create_view_menu(arg);
    case CREATE_TRAIN_MENU:
        return create_train_menu(arg);
    case CREATE_HIRE_MENU:
        return create_hire_menu(arg);
    case CREATE_LOAD_MENU:
        return create_load_menu(arg);
    case CREATE_SAVE_MENU:
        return create_save_menu(arg);
    case GO_MENU:
        return go_menu(arg);
    case RETURN_MENU:
        return arg;
    case CYCLE_TEAM_GUY:
        return cycle_team_guy(arg);
    case DECREASE_STAT:
        return decrease_stat(arg);
    case INCREASE_STAT:
        return increase_stat(arg);
    case EDIT_GUY:
        return edit_guy(arg);
    case CYCLE_GUY:
        return cycle_guy(arg);
    case ADD_GUY:
        return add_guy(arg);
    case DO_SAVE:
        return do_save(arg);
    case DO_LOAD:
        return do_load(arg);
    case NAME_GUY: // Name some guy
        return name_guy(arg);
    case CREATE_DETAIL_MENU:
        return create_detail_menu(nullptr);
    case DO_SET_SCEN_LEVEL:
        return do_set_scen_level(arg);
    case DO_PICK_CAMPAIGN:
        return do_pick_campaign(arg);
    case SET_DIFFICULTY:
        return set_difficulty();
    case CHANGE_TEAM:
        return change_teamnum(arg);
    case CHANGE_HIRE_TEAM:
        return change_hire_teamnum(arg);
    case ALLIED_MODE:
        return change_allied();
    case DO_LEVEL_EDIT:
        return level_editor();
    case YES_OR_NO:
        return yes_or_no(arg);
    case MAIN_OPTIONS:
        return main_options();
    case TOGGLE_SOUND:
        toggle_effect("sound", "sound");

        return REDRAW;
    case TOGGLE_RENDERING_ENGINE:
        toggle_rendering_engine();

        return REDRAW;
    case TOGGLE_FULLSCREEN:
        toggle_effect("graphics", "fullscreen");
        myscreen->set_fullscreen(cfg.is_on("graphics", "fullscreen"));

        return REDRAW;
    case OVERSCAN_ADJUST:
        return overscan_adjust(arg);
    case TOGGLE_MINI_HP_BAR:
        toggle_effect("effects", "mini_hp_bar");

        return REDRAW;
    case TOGGLE_HIT_FLASH:
        toggle_effect("effects", "hit_flash");

        return REDRAW;
    case TOGGLE_HIT_RECOIL:
        toggle_effect("effects", "hit_recoil");

        return REDRAW;
    case TOGGLE_ATTACK_LUNGE:
        toggle_effect("effects", "attack_lunge");

        return REDRAW;
    case TOGGLE_HIT_ANIM:
        toggle_effect("effects", "hit_anim");

        return REDRAW;
    case TOGGLE_DAMAGE_NUMBERS:
        toggle_effect("effects", "damage_numbers");

        return REDRAW;
    case TOGGLE_HEAL_NUMBERS:
        toggle_effect("effects", "heal_numbers");

        return REDRAW;
    case TOGGLE_GORE:
        toggle_effect("effects", "gore");

        return REDRAW;
    case RESTORE_DEFAULT_SETTINGS:
        restore_default_settings();
        cfg.load_settings();

        return REDRAW;
    default:

        return OK;
    }
}

// For right-button
Sint32 VirtualButton::do_call_right(Sint32 whatfunc, Sint32 arg)
{
    switch (whatfunc) {
    case DECREASE_STAT:
        return decrease_stat(arg, 5);
    case INCREASE_STAT:
        return increase_stat(arg, 5);
    default:
        return OK;
    }
}
