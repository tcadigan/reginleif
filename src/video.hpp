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
#ifndef __VIDEO_HPP__
#define __VIDEO_HPP__

// The definition of the VIDEO class

#include "base.hpp"
#include "text.hpp"

class video
{
public:
    video();
    virtual ~video();

    void set_fullscreen(bool fullscreen);
    void clearbuffer();
    void clearbuffer(Sint32 x, Sint32 y, Sint32 w, Sint32 h);
    void clear_window();
    Uint8 *getbuffer();
    void putblack(Sint32 startx, Sint32 starty, Sint32 xsize, Sint32 ysize);
    void fastbox(Sint32 startx, Sint32 starty, Sint32 xsize, Sint32 ysize, Uint8 color);
    void fastbox(Sint32 startx, Sint32 starty, Sint32 xsize, Sint32 ysize, Uint8 color, Uint8 flag);
    void fastbox_outline(Sint32 startx, Sint32 starty, Sint32 xsize, Sint32 ysize, Uint8 color);
    void point(Sint32 x, Sint32 y, Uint8 color);
    // buffers: PORT: Added below prototype
    void pointb(Sint32 x, Sint32 y, Uint8 color);
    void pointb(Sint32 x, Sint32 y, Uint8 color, Uint8 alpha);
    void pointb(Sint32 offset, Uint8 color);
    void pointb(Sint32 x, Sint32 y, Sint32 r, Sint32 g, Sint32 b);
    void hor_line(Sint32 x, Sint32 y, Sint32 length, Uint8 color);
    void hor_line(Sint32 x, Sint32 y, Sint32 length, Uint8 color, Sint32 tobuffer);
    void hor_line_alpha(Sint32 x, Sint32 y, Sint32 length, Uint8 color, Uint8 alpha);
    void ver_line(Sint32 x, Sint32 y, Sint32 length, Uint8 color);
    void ver_line(Sint32 x, Sint32 y, Sint32 length, Uint8 color, Sint32 tobuffer);
    void draw_line(Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Uint8 color);
    void do_cycle(Sint32 curmode, Sint32 maxmode);
    void putdata(Sint32 startx, Sint32 starty, Sint32 xsize, Sint32 ysize, Uint8 *sourcedata);
    void putdata(Sint32 startx, Sint32 starty, Sint32 xsize, Sint32 ysize, Uint8 *sourcedata, Uint8 color);
    void putdata_alpha(Sint32 startx, Sint32 starty, Sint32 xsize, Sint32 ysize, Uint8 *sourcedata, Uint8 alpha);
    void putdatatext(Sint32 startx, Sint32 starty, Sint32 xsize, Sint32 ysize, Uint8 *sourcedata);
    void putdatatext(Sint32 startx, Sint32 starty, Sint32 xsize, Sint32 ysize, Uint8 *sourcedata, Uint8 color);
    void putbuffer(Sint32 tilestartx, Sint32 tilestarty, Sint32 tilewidth, Sint32 tileheight, Sint32 portstartx, Sint32 portstarty, Sint32 portendx, Sint32 portendy, Uint8 *sourceptr);
    void putbuffer(Sint32 tilestartx, Sint32 tilestarty, Sint32 tilewidth, Sint32 tileheight, Sint32 portstartx, Sint32 portstarty, Sint32 portendx, Sint32 portendy, SDL_Surface *sourceptr);
    void putbuffer_alpha(Sint32 tilestartx, Sint32 tilestarty, Sint32 tilewidth, Sint32 tileheight, Sint32 portstartx, Sint32 portstarty, Sint32 portendx, Sint32 portendy, Uint8 *sourcepotr, Uint8 alpha);
    void walkputbuffer(Sint32 walkerstartx, Sint32 walkerstarty, Sint32 walkerwidth, Sint32 walkerheight, Sint32 portstartx, Sint32 portstarty, Sint32 portendx, Sint32 portendy, Uint8 *sourceptr, Uint8 teamcolor);
    void walkputbuffer(Sint32 walkerstartx, Sint32 walkerstarty, Sint32 walkerwidth, Sint32 walkerheight, Sint32 portstartx, Sint32 portstarty, Sint32 portendx, Sint32 portendy, Uint8 *sourceptr, Uint8 teamcolor, Uint8 mode, Sint32 invisibility, Uint8 outline, Uint8 shifttype);
    void walkputbuffer_flash(Sint32 walkerstartx, Sint32 walkerstarty, Sint32 walkerwidth, Sint32 walkerheight, Sint32 portstartx, Sint32 portstarty, Sint32 portendx, Sint32 portendy, Uint8 *sourceptr, Uint8 teamcolor);
    void walkputbuffertext(Sint32 walkerstartx, Sint32 walkerstarty, Sint32 walkerwidth, Sint32 walkerheight, Sint32 portstartx, Sint32 portstarty, Sint32 portendx, Sint32 portendy, Uint8 *sourceptr, Uint8 teamcolor);
    void walkputbuffertext_alpha(Sint32 walkerstartx, Sint32 walkerstarty, Sint32 walkerwidth, Sint32 walkerheight, Sint32 portstartx, Sint32 portstarty, Sint32 portendx, Sint32 portendy, Uint8 *sourceptr, Uint8 teamcolor, Uint8 alpha);
    void draw_box(Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Uint8 color, Sint32 filled);
    void draw_box(Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Uint8 color, Sint32 filled, Sint32 tobuffer);
    void draw_rect_filled(Sint32 x, Sint32 y, Uint32 w, Uint32 h, Uint8 color, Uint8 alpha);
    void draw_button(SDL_Rect const &rect, Sint32 border);
    void draw_button_inverted(SDL_Rect const &rec);
    void draw_button_inverted(Sint32 x, Sint32 y, Uint32 w, Uint32 h);
    void draw_button(Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 border);
    void draw_button(Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 border, Sint32 tobuffer);
    void draw_button_colored(Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, bool use_border, Sint32 base_color, Sint32 high_color=15, Sint32 shadow_color=11);
    Sint32 draw_dialog(Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Uint8 const *header);
    void draw_text_bar(Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2);
    void darken_screen();
    void swap(void);
    void get_pixel(Sint32 x, Sint32 y, Uint8 *r, Uint8 *g, Uint8 *b);
    Sint32 get_pixel(Sint32 x, Sint32 y, Sint32 *index);
    Sint32 get_pixel(Sint32 offset);

    bool save_screenshot();

    // Fading code: (thanks Erik!)
    void FadeBetween24(SDL_Surface *, Uint8 const *, Uint8 const *, Sint32 const);
    Sint32 FadeBetween(SDL_Surface *, SDL_Surface *, SDL_Surface *);
    Sint32 fadeblack(bool fade_in);

    Sint32 fadeDuration;

    // Our standard glad palette
    Uint8 ourpalette[768];
    // For "faded" backgrounds during menus
    Uint8 redpalette[768];
    // For special effects like time freeze
    Uint8 blupalette[768];
    // Store the dos palette so we can restore it later
    Uint8 dospalette[768];
    // Our new unified video buffer
    Uint8 videobuffer[64000];
    // Color cycling on or off
    Sint16 cyclemode;

    // buffers: screen vars
    SDL_Surface *window;
    Sint32 screen_width;
    Sint32 screen_height;
    Sint32 fullscreen;
    Sint32 pdouble;

    text text_normal;
    text text_big;
};

#endif
