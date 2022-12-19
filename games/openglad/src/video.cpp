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

// Video object code

#include "video.hpp"

#include "base.hpp"
#include "gparser.hpp"
#include "input.hpp"
#include "io.hpp"
#include "pal32.hpp"
#include "screen.hpp"
#include "util.hpp"

#include <sstream>

#define VIDEO_BUFFER_WIDTH 320
#define VIDEO_WIDTH 320
#define VIDEO_SIZE 64000
#define CX_SCREEN 320
#define CY_SCREEN 200

#define ASSERT(x)                               \
    if (!(x)) {                                 \
        return 0;                               \
    }                                           \

Screen *E_Screen;

void blend_pixel(SDL_Surface *surface, Sint32 x, Sint32 y, Uint32 color, Uint8 alpha)
{
    Uint32 Rmask = surface->format->Rmask;
    Uint32 Gmask = surface->format->Gmask;
    Uint32 Bmask = surface->format->Bmask;
    Uint32 Amask = surface->format->Amask;
    Uint32 R;
    Uint32 G;
    Uint32 B;
    // SDL_ALPHA_OPAQUE
    Uint32 A = 0;
    Uint32 *pixel;

    switch (surface->format->BytesPerPixel) {
    case 1:
        // Assuming 8-bpp
    {
        Uint8 *pixel = (static_cast<Uint8 *>(surface->pixels) + (y * surface->pitch)) + x;
        Uint8 dR = surface->format->palette->colors[*pixel].r;
        Uint8 dG = surface->format->palette->colors[*pixel].g;
        Uint8 dB = surface->format->palette->colors[*pixel].b;
        Uint8 sR = surface->format->palette->colors[color].r;
        Uint8 sG = surface->format->palette->colors[color].g;
        Uint8 sB = surface->format->palette->colors[color].b;

        dR = dR + (((sR - dR) * alpha) >> 8);
        dG = dG + (((sG - dG) * alpha) >> 8);
        dB = dB + (((sB - dB) * alpha) >> 8);

        *pixel = SDL_MapRGB(surface->format, dR, dG, dB);
    }

    break;
    case 2:
        // Probably 15-bpp or 16-bpp
    {
        Uint16 *pixel = (static_cast<Uint16 *>(surface->pixels) + ((y * surface->pitch) / 2)) + x;
        Uint32 dc = *pixel;

        R = ((dc & Rmask) + ((((color & Rmask) - (dc & Rmask)) * alpha) >> 8)) & Rmask;
        G = ((dc & Gmask) + ((((color & Gmask) - (dc & Gmask)) * alpha) >> 8)) & Gmask;
        B = ((dc & Bmask) + ((((color & Bmask) - (dc & Bmask)) * alpha) >> 8)) & Bmask;

        if (Amask) {
            A = ((dc & Amask) + ((((color & Amask) - (dc & Amask)) * alpha) >> 8)) & Amask;
        }

        *pixel = ((R | G) | B) | A;
    }

    break;
    case 3:
        // Slow 24-bpp mode, usually not used
    {
        Uint8 *pix = (static_cast<Uint8 *>(surface->pixels) + (y * surface->pitch)) + (x * 3);
        Uint8 rshift8 = surface->format->Rshift / 8;
        Uint8 gshift8 = surface->format->Gshift / 8;
        Uint8 bshift8 = surface->format->Bshift / 8;
        Uint8 ashift8 = surface->format->Ashift / 8;

        Uint8 dR;
        Uint8 dG;
        Uint8 dB;
        Uint8 dA = 0;

        Uint8 sR;
        Uint8 sG;
        Uint8 sB;
        Uint8 sA = 0;

        pix = (static_cast<Uint8 *>(surface->pixels) + (y * surface->pitch)) + (x * 3);

        dR = *(pix + rshift8);
        dG = *(pix + gshift8);
        dB = *(pix + bshift8);
        dA = *(pix + ashift8);

        sR = (color >> surface->format->Rshift) & 0xFF;
        sG = (color >> surface->format->Gshift) & 0xFF;
        sB = (color >> surface->format->Bshift) & 0xFF;
        sA = (color >> surface->format->Ashift) & 0xFF;

        dR = dR + (((sR - dR) * alpha) >> 8);
        dG = dG + (((sG - dG) * alpha) >> 8);
        dB = dB + (((sB - dB) * alpha) >> 8);
        dA = dA + (((sA - dA) * alpha) >> 8);

        *(pix + rshift8) = dR;
        *(pix + gshift8) = dG;
        *(pix + bshift8) = dB;
        *(pix + ashift8) = dA;
    }

    break;
    case 4:
        // Probably 32-bpp
    {
        pixel = (static_cast<Uint32 *>(surface->pixels) + ((y * surface->pitch) / 4)) + x;
        Uint32 dc = *pixel;
        R = color & Rmask;
        G = color & Gmask;
        B = color & Bmask;
        // Keep this as 0 to avoid corruption of non-alpha surfaces
        A = 0;

        // Blend and keep dest alpha
        if (alpha != SDL_ALPHA_OPAQUE) {
            R = ((dc & Rmask) + (((R - (dc & Rmask)) * alpha) >> 8)) & Rmask;
            G = ((dc & Gmask) + (((G - (dc & Gmask)) * alpha) >> 8)) & Gmask;
            B = ((dc & Bmask) + (((B - (dc & Bmask)) * alpha) >> 8)) & Bmask;
        }

        if (Amask) {
            A = dc & Amask;
        }

        *pixel = ((R | G) | B) | A;
    }

    break;
    }
}

void putpixel(SDL_Surface *surface, Sint32 x, Sint32 y, Uint32 pixel)
{
    if ((x < 0) || (y < 0) || (x >= surface->w) || (y >= surface->h)) {
        return;
    }

    Sint32 bpp = surface->format->BytesPerPixel;
    // Here p is the address to the pix we want to set
    Uint8 *p = (static_cast<Uint8 *>(surface->pixels) + (y * surface->pitch)) + (x * bpp);

    switch (bpp) {
    case 1:
        *p = pixel;

        break;
    case 2:
        *reinterpret_cast<Uint16 *>(p) = pixel;

        break;
    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xFF;
            p[1] = (pixel >> 8) & 0xFF;
            p[2] = pixel & 0xFF;
        } else {
            p[0] = pixel & 0xFF;
            p[1] = (pixel >> 8) & 0xFF;
            p[2] = (pixel >> 16) & 0xFF;
        }

        break;
    case 4:
        *reinterpret_cast<Uint32 *>(p) = pixel;

        break;
    }
}

Video::Video()
    : text_normal(TEXT_1)
    , text_big(TEXT_BIG)
{
    Sint32 i;
    RenderEngine render;
    fullscreen = 0;
    render = NoZoom;

    if (cfg.is_on("graphics", "fullscreen")) {
        fullscreen = 1;
    } else {
        fullscreen = 0;
    }

    std::string qresult = cfg.get_setting("graphics", "render");

    if (qresult == "normal") {
        render = NoZoom;
    } else if (qresult == "sai") {
        render = SAI;
    } else if (qresult == "eagle") {
        render = EAGLE;
    } else if (qresult == "double") {
        render = DOUBLE;
    }

    fadeDuration = 500;

    // Load our palettes...
    load_and_set_palette(ourpalette);
    load_palette(redpalette);

    // Create the red-shifted palette
    for (i = 32; i < 86; ++i) {
        redpalette[i].g = redpalette[i].r / 2;
        redpalette[i].b = redpalette[i].g / 2;
    }

    load_palette(bluepalette);

    E_Screen = new Screen(render, 640, 400, fullscreen);
}

Video::~Video()
{
    delete E_Screen;
    SDL_Quit();
}

void Video::set_fullscreen(bool fullscreen)
{
    if (fullscreen) {
        SDL_SetWindowFullscreen(E_Screen->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowSize(E_Screen->window, 320, 200);
    }

    SDL_GetWindowSize(E_Screen->window, &window_w, &window_h);
}

Uint8 *Video::getbuffer()
{
    return &videobuffer[0];
}

void Video::clearbuffer()
{
    E_Screen->clear();
}

void Video::clearbuffer(Sint32 x, Sint32 y, Sint32 w, Sint32 h)
{
    E_Screen->clear(x, y, w, h);
}

void Video::clear_window()
{
    E_Screen->clear_window();
}

void Video::draw_box(Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Uint8 color, bool filled)
{
    // Assume topleft-bottomright specs
    Sint32 xlength = (x2 - x1) + 1;
    Sint32 ylength = (y2 - y1) + 1;
    Sint32 i;

    // Hollow box
    if (!filled) {
        hor_line(x1, y1, xlength, color);
        hor_line(x1, y2, xlength, color);
        ver_line(x1, y1, ylength, color);
        ver_line(x2, y1, ylength, color);
    } else {
        for (i = 0; i < ylength; ++i) {
            hor_line(x1, y1 + i, xlength, color);
        }
    }
}

void Video::draw_box(Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Uint8 color, bool filled, bool tobuffer)
{
    // Assume topleft-bottomright specs
    Sint32 xlength = (x2 - x1) + 1;
    Sint32 ylength = (y2 - y1) + 1;
    Sint32 i;

    // Hollow box
    if (!filled) {
        hor_line(x1, y1, xlength, color, tobuffer);
        hor_line(x1, y2, xlength, color, tobuffer);
        ver_line(x1, y1, ylength, color, tobuffer);
        ver_line(x2, y1, ylength, color, tobuffer);
    } else {
        for (i = 0; i < ylength; ++i) {
            hor_line(x1, y1 + i, xlength, color, tobuffer);
        }
    }
}

void Video::draw_rect_filled(Sint32 x, Sint32 y, Uint32 w, Uint32 h, Uint8 color, Uint8 alpha)
{
    for (Uint32 i = 0; i < h; ++i) {
        hor_line_alpha(x, y + i, w, color, alpha);
    }
}

void Video::draw_button(SDL_Rect const &rect, Sint32 border)
{
    draw_button(rect.x, rect.y, rect.x + rect.w - 1, rect.y + rect.h - 1, border);
}

void Video::draw_button_inverted(SDL_Rect const &rect)
{
    draw_text_bar(rect.x, rect.y, (rect.x + rect.w) - 1, (rect.y + rect.h) - 1);
}

void Video::draw_button_inverted(Sint32 x, Sint32 y, Uint32 w, Uint32 h)
{
    draw_text_bar(x, y, (x + w) - 1, (y + h) - 1);
}

void Video::draw_button(Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, bool border)
{
    // Assume topleft-bottomright specs
    Sint32 xlength = (x2 - x1) + 1;
    Sint32 ylength = (y2 - y1) + 1;
    Sint32 i;

    // Hollow box
    if (border) {
        // Top, old 14
        hor_line(x1, y1, xlength, 15);
        // Bottom, old 10
        hor_line(x1, y2, xlength, 11);
        // Left, old 13
        ver_line(x1, y1, ylength, 14);
        // Right, old 11
        ver_line(x2, y1, ylength, 12);
        draw_button(x1 + 1, y1 + 1, x2 - 1, y2 - 1, border - 1);
    } else {
        for (i = 0; i < ylength; ++i) {
            // Facing, old 12
            hor_line(x1, y1 + 1, xlength, 13);
        }
    }
}

void Video::draw_button(Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, bool border, bool tobuffer)
{
    // Assume topleft-bottomright specs
    Sint32 xlength = (x2 - x1) + 1;
    Sint32 ylength = (y2 - y1) + 1;
    Sint32 i;

    // Hollow box
    if (border) {
        // Top, old 14
        hor_line(x1, y1, xlength, 15, tobuffer);
        // Bottom, old 10
        hor_line(x1, y2, xlength, 11, tobuffer);
        // Left, old 13
        ver_line(x1, y1, ylength, 14, tobuffer);
        // Right, old 11
        ver_line(x2, y1, ylength, 12, tobuffer);
        draw_button(x1 + 1, y1 + 1, x2 - 1, y2 - 1, border - 1, tobuffer);
    } else {
        for (i = 0; i < ylength; ++i) {
            // Facing, old 12
            hor_line(x1, y1 + i, xlength, 13, tobuffer);
        }
    }
}

void Video::draw_button_colored(Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, bool use_border, Uint8 base_color, Uint8 high_color, Uint8 shadow_color)
{
    // Assume topleft-bottomright specs
    Sint32 xlength = (x2 - x1) + 1;
    Sint32 ylength = (y2 - y1) + 1;
    Sint32 i;
    bool tobuffer = true;

    if (use_border) {
        // Fill
        for (i = 0; i < (ylength - 2); ++i) {
            // Facing
            hor_line(x1 + 1, (y1 + 1) + i, xlength - 2, base_color, tobuffer);
        }

        // Borders
        // Top
        hor_line(x1, y1, xlength, high_color, tobuffer);
        // Bottom
        hor_line(x1, y2, xlength, shadow_color, tobuffer);
        // Left
        ver_line(x1, y1, ylength, high_color, tobuffer);
        // Right
        ver_line(x2, y1, ylength, shadow_color, tobuffer);
    } else {
        // Fill
        for (i = 0; i < ylength; ++i) {
            // Facing
            hor_line(x1, y1 + i, xlength, base_color, tobuffer);
        }
    }
}

// Draws an empty but headed dialog box, returns the edge at
// which to draw text...does NOT display to screen.
Sint32 Video::draw_dialog(Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, std::string const &header)
{
    // Large text
    Text &dialogtext = text_big;
    Sint32 centerx = x1 + ((x2 - x1) / 2);
    Sint32 left;
    Sint16 textwidth;

    // Single-border width, to buffer
    draw_button(x1, y1, x2, y2, 1, 1);
    // Header field
    draw_text_bar(x1 + 4, y1 + 4, x2 - 4, y1 + 18);
    textwidth = dialogtext.query_width(header);
    left = centerx - (textwidth / 2);

    // Display a title?
    if (!header.empty()) {
        // Draw header to buffer
        dialogtext.write_xy(left, y1 + 6, header, RED, 1);
    }

    // Draw box for text
    draw_text_bar(x1 + 4, y1 + 20, x2 - 4, y2 - 4);

    // Where text should begin to display, left-aligned
    return (x1 + 6);
}

void Video::draw_text_bar(Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2)
{
    // Assume topleft-bottomright specs
    Sint32 xlength = (x2 - x1) + 1;
    Sint32 ylength = (y2 - y1) + 1;

    // First draw the filled, generic grey bar facing
    // filled, to buffer
    draw_box(x1, y1, x2, y2, 12, 1, 1);

    // Draw the indented border
    // Top
    hor_line(x1, y1, xlength, 10, 1);
    // Bottom
    hor_line(x1, y2, xlength, 15, 1);
    // Left
    ver_line(x1, y1, ylength, 11, 1);
    // Right
    ver_line(x2, y1, ylength, 14, 1);
}

void Video::darken_screen()
{
    for (Sint32 i = 0; i < 320; ++i) {
        for (Sint32 j = 0; j < 200; ++j) {
            pointb(i, j, PURE_BLACK, 100);
        }
    }
}

/*
 * This version of fastbox writes directly to screen memory;
 * The following version, with an extra parameter, writes to
 * the buffer instead. Note that it does NOT update (to screen)
 * the area which it changes...
 */
void Video::fastbox(Sint32 startx, Sint32 starty, Sint32 xsize, Sint32 ysize, Uint8 color)
{
    // buffers: We should always draw into the back buffer
    fastbox(startx, starty, xsize, ysize, color, 1);
}

Uint32 get_Uint32_color(Uint8 color)
{
    SDL_Color palette_color = query_palette_reg(color);

    return SDL_MapRGB(E_Screen->render->format,
                      palette_color.r,
                      palette_color.g,
                      palette_color.b);
}

// This is the version which writes to the buffer...
void Video::fastbox(Sint32 startx, Sint32 starty, Sint32 xsize, Sint32 ysize, Uint8 color, bool flag)
{
    SDL_Rect rect;

    // Zardux: FIX: Small check to make sure we're not trying to put in anitmatter or something
    if ((xsize < 0) || (ysize < 0) || (startx < 0) || (starty < 0)) {
        return;
    }

    // Then write to screen directly
    if (!flag) {
        fastbox(startx, starty, xsize, ysize, color);

        return;
    }

    // buffers: Create the rect to fill with SDL_FillRect
    rect.x = startx;
    rect.y = starty;
    rect.w = xsize;
    rect.h = ysize;

    SDL_Color palette_color = query_palette_reg(color);
    SDL_FillRect(E_Screen->render, &rect, SDL_MapRGB(E_Screen->render->format,
                                                     palette_color.r,
                                                     palette_color.g,
                                                     palette_color.b));
}

void Video::fastbox_outline(Sint32 startx, Sint32 starty, Sint32 xsize, Sint32 ysize, Uint8 color)
{
    draw_box(startx, starty, startx + xsize, starty + ysize, color, 0);
}

// Place a point on the screen
// buffers: PORT: this point func is equivalent to drawing directly to screen
void Video::point(Sint32 x, Sint32 y, Uint8 color)
{
    pointb(x, y, color);
}

// buffers: PORT: This draws a point in the offscreen buffer
// buffers: PORT: Used for all the funcs that draw stuff in the offscreen buf
void Video::pointb(Sint32 x, Sint32 y, Uint8 color)
{
    Uint32 c;

    // buffers: This does bound checking (just to be safe)
    if ((x < 0) || (x > 319) || (y < 0) || (y > 199)) {
        return;
    }

    SDL_Color palette_color = query_palette_reg(color);

    c = SDL_MapRGB(E_Screen->render->format,
                   palette_color.r,
                   palette_color.g,
                   palette_color.b);
    putpixel(E_Screen->render, x, y, c);
}

void Video::pointb(Sint32 x, Sint32 y, Uint8 color, Uint8 alpha)
{
    Sint32 c;

    // buffers: This does bound checking (just to be safe)
    if ((x < 0) || (x > 319) || (y < 0) || (y > 199)) {
        return;
    }

    SDL_Color palette_color = query_palette_reg(color);
    c = SDL_MapRGB(E_Screen->render->format,
                   palette_color.r,
                   palette_color.g,
                   palette_color.b);

    blend_pixel(E_Screen->render, x, y, c, alpha);
}

// buffers: This sets the color using the raw RGB values. No * 4...
void Video::pointb(Sint32 x, Sint32 y, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_Rect rect;
    Sint32 c;
    c = SDL_MapRGB(E_Screen->render->format, r, g, b);

    rect.x = x;
    rect.y = y;
    rect.w = 1;
    rect.h = 1;
    SDL_FillRect(E_Screen->render, &rect, c);
}

// buffers: Draw color using an offset
void Video::pointb(Sint32 offset, Uint8 color)
{
    Sint32 x;
    Sint32 y;

    y = offset / 320;
    x = offset - (y * 320);

    pointb(x, y, color);
}

// Place a horizontal line on the screen.
// buffers: This func originally drew directly to the screen
void Video::hor_line(Sint32 x, Sint32 y, Sint32 length, Uint8 color)
{
    hor_line(x, y, length, color, true);
}

void Video::hor_line(Sint32 x, Sint32 y, Sint32 length, Uint8 color, bool tobuffer)
{
    Sint32 i;

    if (!tobuffer) {
        hor_line(x, y, length, color);
    } else {
        for (i = 0; i < length; ++i) {
            pointb(x + i, y, color);
        }
    }
}

void Video::hor_line_alpha(Sint32 x, Sint32 y, Sint32 length, Uint8 color, Uint8 alpha)
{
    Sint32 i;

    for (i = 0; i < length; ++i) {
        pointb(x + i, y, color, alpha);
    }
}

// Place a vertical line on the screen.
// buffers: This func originally drew directly to the screen
void Video::ver_line(Sint32 x, Sint32 y, Sint32 length, Uint8 color)
{
    // buffers: We always want to draw to the back buffer now
    ver_line(x, y, length, color, true);
}

void Video::ver_line(Sint32 x, Sint32 y, Sint32 length, Uint8 color, bool tobuffer)
{
    Sint32 i;

    if (!tobuffer) {
        ver_line(x, y, length, color);
    } else {
        for (i = 0; i < length; ++i) {
            pointb(x, y + i, color);
        }
    }
}

// From SPriG
void Video::draw_line(Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Uint8 color)
{
    SDL_Surface *Surface = E_Screen->render;

    if (Surface == nullptr) {
        return;
    }

    // Did the line miss the screen completely?
    if (((x1 < 0) && (x2 < 0)) || ((y1 < 0) && (y2 < 0))) {
        return;
    }

    if (((x1 >= Surface->w) && (x2 >= Surface->w)) || ((y1 >= Surface->h) && (y2 >= Surface->h))) {
        return;
    }

    Uint32 Color = get_Uint32_color(color);
    Sint16 dx;
    Sint16 dy;
    Sint16 sdx;
    Sint16 sdy;
    Sint16 x;
    Sint16 y;
    Sint16 px;
    Sint16 py;

    dx = x2 - x1;
    dy = y2 - y1;

    if (dx < 0) {
        sdx = -1;
    } else {
        sdx = 1;
    }

    if (dy < 0) {
        sdy = -1;
    } else {
        sdy = 1;
    }

    dx = (sdx * dx) + 1;
    dy = (sdy * dy) + 1;

    y = 0;
    x = 0;

    px = x1;
    py = y1;

    if (dx >= dy) {
        for (x = 0; x < dx; ++x) {
            putpixel(Surface, px, py, color);

            y += dy;

            if (y >= dx) {
                y -= dx;
                py += sdy;
            }

            px += sdx;
        }
    } else {
        for (y = 0; y < dy; ++y) {
            putpixel(Surface, px, py, Color);

            x += dx;

            if (x >= dy) {
                x -= dy;
                px += sdx;
            }

            py += sdy;
        }
    }
}

//
// Video::do_cycle
// Cycle the palette for flame and water motion
// query and set functions are located in pal32.cpp
// buffers: PORT: Added & to the last 3 args of the query_palette_reg funcs
void Video::do_cycle(Sint32 curmode, Sint32 maxmode)
{
    // buffers: PORT: Changed these two arrays to ints

    // Avoid over-runs
    curmode %= maxmode;

    // Then cycle on 0
    if (!curmode) {
        // For orange:
        // Get first color
        SDL_Color cur_color = query_palette_reg(ORANGE_END);
        SDL_Color temp_color;

        for (Uint8 i = ORANGE_END; i > ORANGE_START; --i) {
            temp_color = query_palette_reg(i - 1);
            set_palette_reg(i, cur_color.r, cur_color.g, cur_color.b);
        }

        // Reassign last to first
        set_palette_reg(ORANGE_START, temp_color.r, temp_color.g, temp_color.b);

        // For blue:
        // Get first color
        cur_color = query_palette_reg(WATER_END);

        for (Uint8 i = WATER_END; i > WATER_START; --i) {
            temp_color = query_palette_reg(i - 1);
            set_palette_reg(i, cur_color.r, cur_color.g, cur_color.b);
        }

        // Reassign last to first
        set_palette_reg(WATER_START, temp_color.r, temp_color.g, temp_color.b);
    }
}

//
// Video::putdata
// Draws objects to screen, respecting transparency
// Used by text
void Video::putdata(Sint32 startx, Sint32 starty, Sint32 xsize, Sint32 ysize, Uint8 *sourcedata)
{
    Sint32 curx;
    Sint32 cury;
    Uint8 curcolor;
    Uint32 num = 0;

    for (cury = starty; cury < (starty + ysize); ++cury) {
        for (curx = startx; curx < (startx + xsize); ++curx) {
            curcolor = sourcedata[num];
            ++num;

            if (!curcolor) {
                continue;
            }

            // buffers: PORT: Draw the point
            point(curx, cury, curcolor);
        }
    }
}

// putdata with alpha blending
void Video::putdata_alpha(Sint32 startx, Sint32 starty, Sint32 xsize, Sint32 ysize, Uint8 *sourcedata, Uint8 alpha)
{
    Sint32 curx;
    Sint32 cury;
    Uint8 curcolor;
    Uint32 num = 0;

    for (cury = starty; cury < (starty + ysize); ++cury) {
        for (curx = startx; curx < (startx + xsize); ++curx) {
            curcolor = sourcedata[num];
            ++num;

            if (!curcolor) {
                continue;
            }

            pointb(curx, cury, curcolor, alpha);
        }
    }
}

void Video::putdatatext(Sint32 startx, Sint32 starty, Sint32 xsize, Sint32 ysize, Uint8 *sourcedata)
{
    Sint32 curx;
    Sint32 cury;
    Uint8 curcolor;
    Uint32 num = 0;
    Sint32 color;
    SDL_Rect rect;

    for (cury = starty; cury < (starty + ysize); ++cury) {
        for (curx = startx; curx < (startx + xsize); ++curx) {
            curcolor = sourcedata[num];
            ++num;

            if (!curcolor) {
                continue;
            }

            SDL_Color palette_color = query_palette_reg(curcolor);
            color = SDL_MapRGB(E_Screen->render->format,
                               palette_color.r,
                               palette_color.g,
                               palette_color.b);

            rect.x = curx;
            rect.y = cury;
            rect.w = 1;
            rect.h = 1;
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "test\n");
            SDL_FillRect(E_Screen->render, &rect, color);
        }
    }
}

// Video::putdata
// Draws object to screen, respecting transparency
// used by text
void Video::putdata(Sint32 startx, Sint32 starty, Sint32 xsize, Sint32 ysize, Uint8 *sourcedata, Uint8 color)
{
    Sint32 curx;
    Sint32 cury;
    Uint8 curcolor;
    Uint32 num = 0;

    for(cury = starty; cury < (starty + ysize); ++cury) {
        for(curx = startx; curx < (startx + xsize); ++curx) {
            curcolor = sourcedata[num];++num;

            if (!curcolor) {
                continue;
            }

            if (curcolor > 247) {
                curcolor = color;
            }

            point(curx, cury, curcolor);
        }
    }
}

void Video::putdatatext(Sint32 startx, Sint32 starty, Sint32 xsize, Sint32 ysize, Uint8 *sourcedata, Uint8 color)
{
    Sint32 curx;
    Sint32 cury;
    Uint8 curcolor;
    Uint32 num = 0;
    Sint32 scolor;
    SDL_Rect rect;

    for (cury = starty; cury < (starty + ysize); ++cury) {
        for (curx = startx; curx < (startx + xsize); ++curx) {
            curcolor = sourcedata[num];
            ++num;

            if (!curcolor) {
                continue;
            }

            if (curcolor > 247) {
                curcolor = color;
            }

            SDL_Color palette_color = query_palette_reg(curcolor);
            scolor = SDL_MapRGB(E_Screen->render->format,
                                palette_color.r,
                                palette_color.g,
                                palette_color.b);

            rect.x = curx;
            rect.y = cury;
            rect.w = 1;
            rect.h = 1;
            SDL_FillRect(E_Screen->render, &rect, scolor);
        }
    }
}

/*
 * Video::putbufer
 * Used to put tiles into the buffer as we compose the screen
 * tilestartx, tilestarty are the upper leve corner of the tile's position on
 * screen, which may be negative since we have tiles offscreen.
 * tilewidth, tileheight are the tile size, which will usually be GRID_SIZE
 * but this leaves things open.
 * portstartx, portstarty, portendx, portendy allow us to clip to a rectangular
 * window on screen, i.e. a viewscreen.
 * sourceptr is a pointer to the video data to be copied into the buffer.
 */
void Video::putbuffer(Sint32 tilestartx, Sint32 tilestarty,
                      Sint32 tilewidth, Sint32 tileheight,
                      Sint32 portstartx, Sint32 portstarty,
                      Sint32 portendx, Sint32 portendy,
                      Uint8 *sourceptr)
{
    Sint32 i;
    Sint32 j;
    Sint32 num;
    Sint32 xmin = 0;
    Sint32 xmax = tilewidth;
    Sint32 ymin = 0;
    Sint32 ymax = tileheight;
    // Number of rows and width of each row in the source
    Sint32 totrows;
    Sint32 rowsize;
    Uint8 *sourcebufptr = &sourceptr[0];

    if ((tilestartx >= portendx) || (tilestarty >= portendy)) {
        // Abort, the tile is drawing outsize of clipping region
        return;
    }

    // This clips on the right edge
    if ((tilestartx + tilewidth) > portendx) {
        // Stop drawing after xmax bytes
        xmax = portendx - tilestartx;
    } else if (tilestartx < portstartx) {
        // This clips on the left edge
        xmin = portstartx - tilestartx;
        tilestartx = portstartx;
    }

    // This clips on the bottom edge
    if ((tilestarty + tileheight) > portendy) {
        ymax = portendy - tilestarty;
    } else if (tilestarty < portstarty) {
        // This clips the top edge
        ymin = portstarty - tilestarty;
        tilestarty = portstarty;
    }

    // How many rows to copy
    totrows = ymax - ymin;
    // How many bytes to copy
    rowsize = xmax - xmin;

    if ((totrows <= 0) || (rowsize <= 0)) {
        // This happens on bad args
        return;
    }

    // buffers: Draws graphic. Actually uses the above bound checking now (7/18/02)
    num = 0;

    for (i = ymin; i < ymax; ++i) {
        for (j = xmin; j < xmax; ++j){
            num = (i * tilewidth) + j;
            pointb((j + tilestartx) - xmin, (i + tilestarty) - ymin, sourcebufptr[num]);
        }
    }
}

void Video::putbuffer_alpha(Sint32 tilestartx, Sint32 tilestarty,
                            Sint32 tilewidth, Sint32 tileheight,
                            Sint32 portstartx, Sint32 portstarty,
                            Sint32 portendx, Sint32 portendy,
                            Uint8 *sourceptr, Uint8 alpha)
{
    Sint32 i;
    Sint32 j;
    Sint32 num;
    Sint32 xmin = 0;
    Sint32 xmax = tilewidth;
    Sint32 ymin = 0;
    Sint32 ymax = tileheight;
    // Number of rows and width of each row in the source
    Sint32 totrows;
    Sint32 rowsize;
    Uint8 *sourcebufptr = &sourceptr[0];

    if ((tilestartx >= portendx) || (tilestarty >= portendy)) {
        // Abort, the tile is drawing outside the clipping region
        return;
    }

    // This clips on the right edge
    if ((tilestartx + tilewidth) > portendx) {
        // Stop drawing after xmax bytes
        xmax = portendx - tilestartx;
    } else if (tilestartx < portstartx) {
        // this clips on the left edge
        xmin = portstartx - tilestartx;
        tilestartx = portstartx;
    }

    // This clips on the bottom edge
    if ((tilestarty + tileheight) > portendy) {
        ymax = portendy - tilestarty;
    } else if (tilestarty - portstarty) {
        // This clips the top edge
        ymin = portstarty - tilestarty;
        tilestarty = portstarty;
    }

    // How many rows to copy
    totrows = ymax - ymin;
    // How many bytes to copy
    rowsize = xmax - xmin;

    if ((totrows <= 0) || (rowsize <= 0)) {
        // This happens on bad args
        return;
    }

    // buffers: Draws graphic. Actually uses the above bound checking now (7/18/02)
    num = 0;

    for (i = ymin; i < ymax; ++i){
        for (j = xmin; j < xmax; ++j) {
            num = (i * tilewidth) + j;
            pointb((j + tilestartx) - xmin, (i + tilestarty) - ymin, sourcebufptr[num], alpha);
        }
    }
}

// buffers: This is the SDL_Surface accelerated version of putbuffer
void Video::putbuffer(Sint32 tilestartx, Sint32 tilestarty,
                      Sint32 tilewidth, Sint32 tileheight,
                      Sint32 portstartx, Sint32 portstarty,
                      Sint32 portendx, Sint32 portendy,
                      SDL_Surface *sourceptr)
{
    SDL_Rect rect;
    SDL_Rect temp;
    Sint32 xmin = 0;
    Sint32 xmax = tilewidth;
    Sint32 ymin = 0;
    Sint32 ymax = tileheight;
    // Number of rows and width of each row in the source
    Sint32 totrows;
    Sint32 rowsize;

    if ((tilestartx >= portendx) || (tilestarty >= portendy)) {
        // Abort, the tile is drawing outside the clipping region
        return;
    }

    // This clips on the right edge
    if ((tilestartx + tilewidth) > portendx) {
        // Stop drawing after xmax bytes;
        xmax = portendx - tilestartx;
    } else if (tilestartx < portstartx) {
        // This clips on the left edge
        xmin = portstartx - tilestartx;
        tilestartx = portstartx;
    }

    // This clips on the bottom edge
    if ((tilestarty + tileheight) > portendy) {
        ymax = portendy - tilestarty;
    } else if (tilestarty < portstarty) {
        // This clips the top edge
        ymin = portstarty - tilestarty;
        tilestarty = portstarty;
    }

    // How many rows to copy
    totrows = ymax - ymin;
    // How many bytes to copy
    rowsize = xmax - xmin;

    if ((totrows <= 0) || (rowsize <= 0)) {
        // This happens on bad args
        return;
    }

    rect.x = tilestartx;
    rect.y = tilestarty;
    temp.x = xmin;
    temp.y = ymin;
    temp.w = xmax - xmin;
    temp.h = ymax - ymin;
    // TODO: Convert this to RenderCopy
    SDL_BlitSurface(sourceptr, &temp, E_Screen->render, &rect);
}

/*
 * walkputbuffer draws active guys to the screen (basically all non-tiles
 * c-only since it isn't used that often (despite what you may think)
 * walkerstartx, walkerstarty are the screen position we will try to draw to
 * walkerwidth, walkerheight define the object's size
 * portstartx, portstarty, portendx, portendy define a clipping rectangle
 * sourceptr holds the walker data
 * teamcolor is used for recoloring the guys to the appropriate team
 */
void Video::walkputbuffer(Sint32 walkerstartx, Sint32 walkerstarty,
                          Sint32 walkerwidth, Sint32 walkerheight,
                          Sint32 portstartx, Sint32 portstarty,
                          Sint32 portendx, Sint32 portendy,
                          Uint8 *sourceptr, Uint8 teamcolor)
{
    Sint32 curx;
    Sint32 cury;
    Uint8 curcolor;
    Sint32 xmin = 0;
    Sint32 xmax = walkerwidth;
    Sint32 ymin = 0;
    Sint32 ymax = walkerheight;
    Sint32 walkoff = 0;
    Sint32 walkshift = 0;
    Sint32 totrows;
    Sint32 rowsize;

    if ((walkerstartx >= portendx) || (walkerstarty >= portendy)) {
        // Walker is below or to the right of the viewport
        return;
    }

    // Clip the left edge of the view
    if (walkerstartx < portstartx) {
        // Start drawing walker at xmin
        xmin = portstartx - walkerstartx;
        walkerstartx = portstartx;
    } else if ((walkerstartx + walkerwidth) > portendx) {
        // Clip the right edge
        // Stop drawing walker at xmax
        xmax = portendx - walkerstartx;
    }

    // Clip the top edge
    if (walkerstarty < portstarty) {
        // Start drawing walker at ymin
        ymin = portstarty - walkerstarty;
        walkerstarty = portstarty;
    } else if ((walkerstarty + walkerheight) > portendy) {
        // Clip the bottom edge
        // Stop drawing walker at ymax
        ymax = portendy - walkerstarty;
    }

    // How many rows to copy
    totrows = ymax - ymin;
    // How many bytes to copy
    rowsize = xmax - xmin;

    if ((totrows <= 0) || (rowsize <= 0)) {
        // This happens on bad args
        return;
    }

    // Note!! The clipper makes the assumption that no object is larger than
    // the view it will be clipped to in either dimension!!!
    walkshift = walkerwidth - rowsize;

    walkoff = (ymin * walkerwidth) + xmin;

    for (cury = 0; cury < totrows; ++cury) {
        for (curx = 0; curx < rowsize; ++curx) {
            curcolor = sourceptr[walkoff];
            ++walkoff;

            if (!curcolor) {
                continue;
            }

            if (curcolor > 247) {
                curcolor = teamcolor + (255 - curcolor);
            }

            // buffers: PORT: videobuffer[buffoff++] = curcolor;
            pointb(walkerstartx + curx, walkerstarty + cury, curcolor);
        }

        walkoff += walkshift;
    }
}

void Video::walkputbuffer_flash(Sint32 walkerstartx, Sint32 walkerstarty,
                                Sint32 walkerwidth, Sint32 walkerheight,
                                Sint32 portstartx, Sint32 portstarty,
                                Sint32 portendx, Sint32 portendy,
                                Uint8 *sourceptr, Uint8 teamcolor)
{
    Sint32 curx;
    Sint32 cury;
    Uint8 curcolor;
    Sint32 xmin = 0;
    Sint32 xmax = walkerwidth;
    Sint32 ymin = 0;
    Sint32 ymax = walkerheight;
    Sint32 walkoff = 0;
    Sint32 walkshift = 0;
    Sint32 totrows;
    Sint32 rowsize;

    if ((walkerstartx >= portendx) || (walkerstarty >= portendy)) {
        // Walker is below or to the right of the viewport
        return;
    }

    // Clip the left edge of the view
    if (walkerstartx < portstartx) {
        // Start drawing walker at xmin
        xmin = portstartx - walkerstartx;
        walkerstartx = portstartx;
    } else if ((walkerstartx + walkerwidth) > portendx) {
        // Clip the right edge
        // Stop drawing walker at xmax
        xmax = portendx - walkerstartx;
    }

    // Clip the top edge
    if (walkerstarty < portstarty) {
        // Start drawing walker at ymin
        ymin = portstarty - walkerstarty;
        walkerstarty = portstarty;
    } else if ((walkerstarty + walkerheight) > portendy) {
        // Clip the bottom edge
        // Stop drawing walker at ymax
        ymax = portendy - walkerstarty;
    }

    // How many rows to copy
    totrows = ymax - ymin;
    // How many bytes to copy
    rowsize = xmax - xmin;

    if ((totrows <= 0) || (rowsize <= 0)) {
        // This happens on bad args
        return;
    }

    // Note!! The clipper makes the assumption that no object is larger than
    // the view it will be clipped to in either dimension!!!
    walkshift = walkerwidth - rowsize;

    walkoff = (ymin * walkerwidth) + xmin;

    for (cury = 0; cury < totrows; ++cury) {
        for (curx = 0; curx < rowsize; ++curx) {
            curcolor = sourceptr[walkoff];
            ++walkoff;

            if (!curcolor) {
                continue;
            }

            if (curcolor > 247) {
                curcolor = teamcolor + (255 - curcolor);
            }

            SDL_Color palette_color = query_palette_reg(curcolor);

            if (palette_color.r > 155) {
                palette_color.r = 255;
            } else {
                palette_color.r += 100;
            }

            if (palette_color.g > 155) {
                palette_color.g = 255;
            } else {
                palette_color.g += 100;
            }

            if (palette_color.b > 155) {
                palette_color.b = 255;
            } else {
                palette_color.b += 100;
            }

            pointb(walkerstartx + curx,
                   walkerstarty + cury,
                   palette_color.r,
                   palette_color.g,
                   palette_color.b);
        }

        walkoff += walkshift;
    }
}

void Video::walkputbuffertext(Sint32 walkerstartx, Sint32 walkerstarty,
                              Sint32 walkerwidth, Sint32 walkerheight,
                              Sint32 portstartx, Sint32 portstarty,
                              Sint32 portendx, Sint32 portendy,
                              Uint8 *sourceptr, Uint8 teamcolor)
{
    Sint32 curx;
    Sint32 cury;
    Uint8 curcolor;
    Sint32 xmin = 0;
    Sint32 xmax = walkerwidth;
    Sint32 ymin = 0;
    Sint32 ymax = walkerheight;
    Sint32 walkoff = 0;
    Sint32 walkshift = 0;
    Sint32 totrows;
    Sint32 rowsize;
    Sint32 color;
    SDL_Rect rect;

    if ((walkerstartx >= portendx) || (walkerstarty >= portendy)) {
        // Walker is below or to the right of the viewport
        return;
    }

    // Clip the left edge of the view
    if (walkerstartx < portstartx) {
        // Start drawing walker at xmin
        xmin = portstartx - walkerstartx;
        walkerstartx = portstartx;
    } else if ((walkerstartx + walkerwidth) > portendx) {
        // Clip the right edge
        // Stop drawing walker at xmax
        xmax = portendx - walkerstartx;
    }

    // Clip the top edge
    if (walkerstarty < portstarty) {
        // Start drawing walker at ymin
        ymin = portstarty - walkerstarty;
        walkerstarty = portstarty;
    } else if ((walkerstarty + walkerheight) > portendy) {
        // Clip the bottom edge
        // Stop drawing walker at ymax
        ymax = portendy - walkerstarty;
    }

    // How many rows to copy
    totrows = ymax - ymin;
    // How many bytes to copy
    rowsize = xmax - xmin;

    if ((totrows <= 0) || (rowsize <= 0)) {
        // This happens on bad args
        return;
    }

    // Note!! The clipper makes the assumption that no object is larger than
    // the view it will be clipped to in either dimension!!!
    walkshift = walkerwidth - rowsize;

    walkoff = (ymin * walkerwidth) + xmin;

    for (cury = 0; cury < totrows; ++cury) {
        for (curx = 0; curx < rowsize; ++curx) {
            curcolor = sourceptr[walkoff];
            ++walkoff;

            if (!curcolor) {
                continue;
            }

            if (curcolor > 247) {
                curcolor = teamcolor + (255 - curcolor);
            }

            SDL_Color palette_color = query_palette_reg(curcolor);
            color = SDL_MapRGB(E_Screen->render->format,
                               palette_color.r,
                               palette_color.g,
                               palette_color.b);

            rect.x = curx + walkerstartx;
            rect.y = cury + walkerstarty;
            rect.w = 1;
            rect.h = 1;
            SDL_FillRect(E_Screen->render, &rect, color);
        }

        walkoff += walkshift;
    }
}

void Video::walkputbuffertext_alpha(Sint32 walkerstartx, Sint32 walkerstarty,
                                    Sint32 walkerwidth, Sint32 walkerheight,
                                    Sint32 portstartx, Sint32 portstarty,
                                    Sint32 portendx, Sint32 portendy,
                                    Uint8 *sourceptr, Uint8 teamcolor, Uint8 alpha)
{
    Sint32 curx;
    Sint32 cury;
    Uint8 curcolor;
    Sint32 xmin = 0;
    Sint32 xmax = walkerwidth;
    Sint32 ymin = 0;
    Sint32 ymax = walkerheight;
    Sint32 walkoff = 0;
    Sint32 walkshift = 0;
    Sint32 totrows;
    Sint32 rowsize;

    if ((walkerstartx >= portendx) || (walkerstarty >= portendy)) {
        // Walker is below or to the right of the viewport
        return;
    }

    // Clip the left edge of the view
    if (walkerstartx < portstartx) {
        // Start drawing walker at xmin
        xmin = portstartx - walkerstartx;
        walkerstartx = portstartx;
    } else if ((walkerstartx + walkerwidth) > portendx) {
        // Clip the right edge
        // Stop drawing walker at xmax
        xmax = portendx - walkerstartx;
    }

    // Clip the top edge
    if (walkerstarty < portstarty) {
        // Start drawing walker at ymin
        ymin = portstarty - walkerstarty;
        walkerstarty = portstarty;
    } else if ((walkerstarty + walkerheight) > portendy) {
        // Clip the bottom edge
        // Stop drawing walker at ymax
        ymax = portendy - walkerstarty;
    }

    // How many rows to copy
    totrows = ymax - ymin;
    // How many bytes to copy
    rowsize = xmax - xmin;

    if ((totrows <= 0) || (rowsize <= 0)) {
        // this happens on bad args
        return;
    }

    // Note!! The clipper makse the assumption that no object is larger than
    // the view it will be clipped to in either dimension!!!
    walkshift = walkerwidth - rowsize;

    walkoff = (ymin * walkerwidth) + xmin;

    for (cury = 0; cury < totrows; ++cury) {
        for (curx = 0; curx < rowsize; ++curx) {
            curcolor = sourceptr[walkoff];
            ++walkoff;

            if (!curcolor) {
                continue;
            }

            if (curcolor > 247) {
                curcolor = teamcolor + (255 - curcolor);
            }

            pointb(curx + walkerstartx, cury + walkerstarty, teamcolor, alpha);
        }

        walkoff += walkshift;
    }
}

void Video::walkputbuffer(Sint32 walkerstartx, Sint32 walkerstarty,
                          Sint32 walkerwidth, Sint32 walkerheight,
                          Sint32 portstartx, Sint32 portstarty,
                          Sint32 portendx, Sint32 portendy,
                          Uint8 *sourceptr, Uint8 teamcolor,
                          Uint8 mode, Sint32 invisibility,
                          Uint8 outline, Uint8 shifttype)
{
    Sint32 curx;
    Sint32 cury;
    Uint8 curcolor;
    Uint8 bufcolor;
    Sint32 xmin = 0;
    Sint32 xmax = walkerwidth;
    Sint32 ymin = 0;
    Sint32 ymax = walkerheight;
    Sint32 walkoff = 0;
    Sint32 buffoff = 0;
    Sint32 walkshift = 0;
    Sint32 totrows;
    Sint32 rowsize;
    Sint8 shift;
    Sint32 yval;
    Sint32 xval;
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Sint32 tx;
    Sint32 ty;
    Sint32 tempbuf;

    if ((walkerstartx >= portendx) || (walkerstarty >= portendy)) {
        // Walker is below or to the right of the viewport
        return;
    }

    // Clip the left edge of the view
    if (walkerstartx < portstartx) {
        // Start drawing walker at xmin
        xmin = portstartx - walkerstartx;
        walkerstartx = portstartx;
    } else if ((walkerstartx + walkerwidth) > portendx) {
        // Clip the right edge
        // Stop drawing walker at xmax
        xmax = portendx - walkerstartx;
    }

    // Clip the top edge
    if (walkerstarty < portstarty) {
        // Start drawing walker at ymin
        ymin = portstarty - walkerstarty;
        walkerstarty = portstarty;
    } else if ((walkerstarty + walkerheight) > portendy) {
        // Clip the bottom edge
        // Stop drawing at ymax
        ymax = portendy - walkerstarty;
    }

    // How many rows to copy
    totrows = ymax - ymin;
    // How many bytes to copy
    rowsize = xmax - xmin;

    if ((totrows <= 0) || (rowsize <= 0)) {
        // This happens on bad args
        return;
    }

    // Note!! The clipper makes the assumption that no object is larger than
    // the view it will be clipped to in either dimension!!!
    walkshift = walkerwidth - rowsize;

    walkoff = (ymin * walkerwidth) + xmin;
    buffoff = (walkerstarty * VIDEO_BUFFER_WIDTH) + walkerstartx;
    xval = walkerstartx;
    yval = walkerstarty;

    // Zardus: FIX: And now we simply replace all the videobuffer stuff with pointb
    switch (mode) {
    case INVISIBLE_MODE:
        for (cury = 0; cury < totrows; ++cury) {
            for (curx = 0; curx < rowsize; ++curx) {
                curcolor = sourceptr[walkoff];
                ++walkoff;

                if (!curcolor) {
                    if (outline) {
                        if ((curx > 0) && sourceptr[walkoff - 2]) {
                            pointb(xval, yval, outline);
                            ++xval;
                            continue;
                        }

                        if ((curx < (rowsize - 1)) && sourceptr[walkoff]) {
                            pointb(xval, yval, outline);
                            ++xval;
                            continue;
                        }

                        if ((cury > 0) && sourceptr[(walkoff - 1) - walkerwidth]) {
                            pointb(xval, yval, outline);
                            ++xval;
                            continue;
                        }

                        if ((cury < (totrows - 1)) && sourceptr[(walkoff - 1) + walkerwidth]) {
                            pointb(xval, yval, outline);
                            ++xval;
                            continue;
                        }
                        // End of outline check
                    }

                    ++xval;
                    continue;
                    // End of transparency check
                }

                if (curcolor > 247) {
                    curcolor = teamcolor + (255 - curcolor);
                }

                if (outline) {
                    if ((curx == 0) || (cury == 0)
                        || (curx == (walkerwidth - 1)) || (cury == (totrows - 1))) {
                        pointb(xval, yval, outline);
                        ++xval;
                        continue;
                    }
                    // End outline
                }

                if (getRandomSint32(invisibility) > 8) {
                    ++xval;
                    // videobuffer[bufoff++] teamcolor + random(7);
                    continue;
                }

                pointb(xval, yval, curcolor);
                ++xval;
                // End of each row
            }

            walkoff += walkshift;
            ++yval;
            xval = walkerstartx;
            // End of all rows
        }

        break;
        // End INVSIBILE
    case OUTLINE_MODE:
        for (cury = 0; cury < totrows; ++cury) {
            for (curx = 0; curx < rowsize; ++curx) {
                curcolor = sourceptr[walkoff];
                ++walkoff;

                if (!curcolor) {
                    if ((curx> 0) && sourceptr[walkoff - 2]) {
                        pointb(xval, yval, outline);
                        ++xval;
                        continue;
                    }

                    if ((curx < (rowsize - 1)) && sourceptr[walkoff]) {
                        pointb(xval, yval, outline);
                        ++xval;
                        continue;
                    }

                    if ((cury > 0) && sourceptr[(walkoff - 1) - walkerwidth]) {
                        pointb(xval, yval, outline);
                        ++xval;
                        continue;
                    }

                    if ((cury < (totrows - 1)) && sourceptr[(walkoff - 1) + walkerwidth]) {
                        pointb(xval, yval, outline);
                        ++xval;
                        continue;
                    }

                    ++xval;
                    continue;
                    // End of transparencey check
                }

                if (curcolor > 247) {
                    curcolor = teamcolor + (255 - curcolor);
                }

                if ((curx == 0) || (cury == 0)
                    || (curx == (walkerwidth - 1)) || (cury == (totrows - 1))) {
                    pointb(xval, yval, outline);
                    ++xval;
                    continue;
                }

                pointb(xval, yval, curcolor);
                // End of each row
            }

            walkoff += walkshift;
            xval = walkerstartx;
            ++yval;
            // End of all rows
        }

        break;
        // end OUTLINE
    case PHANTOM_MODE:
        // buffers: PORT: Ported the below block of code
        switch (shifttype) {
        case SHIFT_LEFT:
            shift = -1;

            break;
        case SHIFT_RIGHT:
            shift = 1;

            break;
        case SHIFT_RIGHT_RANDOM:
            shift = getRandomSint32(2);

            break;
        default:
            shift = 0;

            break;
            // end switch (shifttype)
        }

        for (cury = 0; cury < totrows; ++cury) {
            for (curx = 0; curx < rowsize; ++curx) {
                curcolor = sourceptr[walkoff];
                ++walkoff;

                if (!curcolor) {
                    ++buffoff;
                    continue;
                }

                // buffers: This is a messy optimization. Sorry.
                if (shifttype == SHIFT_RANDOM) {
                    tempbuf = buffoff + getRandomSint32(2);
                    ty = tempbuf / 320;
                    tx = tempbuf - (ty * 320);
                    get_pixel(tx, ty, &r, &g, &b);

                    ty = buffoff / 320;
                    tx = buffoff - (ty * 320);

                    pointb(tx,ty, r, g, b);

                    ++buffoff;
                } else if (shifttype == SHIFT_LIGHTER) {
                    bufcolor = get_pixel(buffoff);

                    if (((bufcolor %8) != 0) && (bufcolor != 0)) {
                        --bufcolor;
                    }

                    pointb(buffoff, bufcolor);
                    ++buffoff;
                } else if (shifttype == SHIFT_DARKER) {
                    bufcolor = get_pixel(buffoff);

                    if (((bufcolor % 7) != 0) && (bufcolor < 255)) {
                        ++bufcolor;
                    }

                    pointb(buffoff++, bufcolor);
                } else if (shifttype == SHIFT_BLOCKY) {
                    if (cury % 2) {
                        pointb(buffoff, get_pixel(buffoff - 320));
                    } else if (curx % 2) {
                        pointb(buffoff, get_pixel(buffoff - 2));
                    }
                } else {
                    pointb(buffoff, get_pixel(buffoff + shift));
                    ++buffoff;
                }

                // End of each row
            }

            walkoff += walkshift;
            // End all rows
        }

        break;
        // End case PHANTOM
    default:
        // NORMAL walkputbuffer
        for (cury = 0; cury < totrows; ++cury) {
            for (curx = 0; curx < rowsize; ++curx) {
                curcolor = sourceptr[walkoff];
                ++walkoff;

                if (!curcolor) {
                    ++buffoff;
                    continue;
                }

                if (curcolor > 247) {
                    curcolor = teamcolor + (255 - curcolor);
                }

                videobuffer[buffoff] = curcolor;
                ++buffoff;
                // End each row
            }

            walkoff += walkshift;
            // End all rows
        }

        break;
        // End default
        // End switch of mode
    }
}

// Video::buffer_to_string
// Copies all or a portion of the video buffer to the screen
// viewstartx, viewstarty, viewwidth, viewheight define a rectangle which
// can be used to draw only a portion of the buffer to screen,
// and is used to draw viewscreens when we don't need a full update
// NOTE!! This function requires that you pass it a rectangle which is
// a multiple of four WIDE, or it will NOT draw correctly
// This is designed this way with the assumption that screen draws area
// the slowest thing we can possibly do.
void Video::buffer_to_screen(Sint32 viewstartx, Sint32 viewstarty,
                             Sint32 viewwidth, Sint32 viewheight)
{
    E_Screen->swap(viewstartx, viewstarty, viewwidth, viewheight);
}

// buffers: Like buffer_to_screen but automatically swaps the entire screen
void Video::swap(void)
{
    buffer_to_screen(0, 0, 320, 200);
}

// buffers: Get pixel's RGB values if you have X,Y
void Video::get_pixel(Sint32 x, Sint32 y, Uint8 *r, Uint8 *g, Uint8 *b)
{
    Uint32 col = 0;
    Uint8 q = 0;
    Uint8 w = 0;
    Uint8 e = 0;

    Uint8 *p = static_cast<Uint8 *>(E_Screen->render->pixels);
    p += E_Screen->render->pitch * y;
    p += E_Screen->render->format->BytesPerPixel * x;

    memcpy(&col, p, E_Screen->render->format->BytesPerPixel);

    SDL_GetRGB(col, E_Screen->render->format, &q, &w, &e);
    *r = q;
    *g = w;
    *b = e;
}

// buffers: Get pixel index if you have X,Y
Sint32 Video::get_pixel(Sint32 x, Sint32 y, Sint32 *index)
{
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Sint32 i;

    get_pixel(x, y, &r, &g, &b);
    r /= 4;
    g /= 4;
    b /= 4;

    for (i = 0; i < 256; ++i) {
        SDL_Color palette_color = query_palette_reg(i);

        if ((r == palette_color.r)
            && (g == palette_color.g)
            && (b == palette_color.b)) {
            *index = i;
            return i;
        }
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "DEBUG: Could not find color: %d %d %d\n", r, g, b);

    return 0;
}

// buffers: Get pixel index if you have a buffer offset
Sint32 Video::get_pixel(Sint32 offset)
{
    Sint32 x;
    Sint32 y;
    Sint32 t;

    y = offset / 320;
    x = offset - (y * 320);

    return get_pixel(x, y, &t);
}

bool Video::save_screenshot()
{
    SDL_Surface *surf;

    switch (E_Screen->Engine) {
    case SAI:
    case EAGLE:
        surf = E_Screen->render2;

        break;
    default:
        surf = E_Screen->render;

        break;
    }

    static int i = 1;
    std::stringstream buf;
    buf << "screenshot" << i;
    buf << ".bmp";

    ++i;

    std::string filename(buf.str());
    buf.clear();
    filename.resize(200);
    SDL_RWops *rwops = open_write_file(std::filesystem::path(filename));

    if (rwops == nullptr) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Failed to open file for screenshot.\n");

        return false;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Saving screensshot: %s\n", filename.c_str());

    bool result = false;

    if (SDL_SaveBMP_RW(surf, rwops, 1) >= 0) {
        result = true;
    }

    return result;
}

/*
 * ******************************
 * Fading routines! Thanks, Erik!
 * ******************************
 */
// Show transition between two screen at "amount" between them.
//
// "pSurface" is the surface you want to apply the fade to,
// "fadeFrom" is a copy of what the old screen looks like, and
// "fadeTo" is a copy what the normal screen looks like,
// neither faded in or out, but just normal.
// NOTE: fadeFrom, fadeTo, and pSurface must be the same size and dimensions.
void Video::FadeBetween24(SDL_Surface *pSurface, Uint8 const *fadeFromRGB,
                          Uint8 const *fadeToRGB, Sint32 const amount)
{
    Uint8 *pw = static_cast<Uint8 *>(pSurface->pixels);
    Uint32 size = pSurface ->pitch * pSurface->h;

    Sint32 const nOldAmt = fadeDuration - amount;
    Uint8 const *pFrom = fadeFromRGB;
    Uint8 const *pTo = fadeToRGB;

    // Mix pixels in "from" and "to" images by "amount"
    Uint8 *pStop = pw + size;

    while (pw != pStop) {
        *pw = ((nOldAmt * *pFrom) + (amount * *pTo)) / fadeDuration;
        ++pw;
        ++pFrom;
        ++pTo;
        *pw = ((nOldAmt * *pFrom) + (amount * *pTo)) / fadeDuration;
        ++pw;
        ++pFrom;
        ++pTo;
        *pw = ((nOldAmt * *pFrom) + (amount * *pTo)) / fadeDuration;
        ++pw;
        ++pFrom;
        ++pTo;
        ++pw;
        ++pFrom;
        ++pTo;
    }
}

// Fade between two screens.
// Time effect to be independent of machine speed.
// "pOldSurface" (in) Surface that contains starting image.
// "pNewSurface" (in) Image that destination surface will change to.
// "DestSurface" Surface which is the destination
Sint32 Video::FadeBetween(SDL_Surface *pOldSurface, SDL_Surface *pNewSurface, SDL_Surface *DestSurface)
{
    bool bOldNull = false;
    bool bNewNull = false;
    Sint32 i = 1;

    // Set NULL points to temporary black screens
    // (for simple fade-in/out effects).
    if (!pOldSurface) {
        bOldNull = true;
        pOldSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, CX_SCREEN, CY_SCREEN, 24, 0, 0, 0, 0);
        SDL_FillRect(pOldSurface, nullptr, 0);
    }

    if (!pNewSurface) {
        bNewNull = true;
        pNewSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, CX_SCREEN, CY_SCREEN, 24, 0, 0, 0, 0);
        SDL_FillRect(pNewSurface, nullptr, 0);
    }

    if (bOldNull && bNewNull) {
        // Nothing to do
        return 0;
    }

    // Lock the screen for directo access to the pixels
    if (SDL_MUSTLOCK(pOldSurface)) {
        if (SDL_LockSurface(pOldSurface) < 0) {
            return 0;
        }
    }

    // The new surface shouldn't need a lock unless it is somehow a screen surface
    ASSERT(!SDL_MUSTLOCK(pNewSurface));

    // The dimensions and format of the old and new surface must match exactly.
    ASSERT(pOldSurface->pitch == pNewSurface->pitch);
    ASSERT(pOldSurface->w == pNewSurface->w);
    ASSERT(pOldSurface->h == pNewSurface->h);
    ASSERT(pOldSurface->format->Rmask == pNewSurface->format->Rmask);
    ASSERT(pOldSurface->format->Rshift == pNewSurface->format->Rshift);
    ASSERT(pOldSurface->format->Rloss == pNewSurface->format->Rloss);
    ASSERT(pOldSurface->format->Gmask == pNewSurface->format->Gmask);
    ASSERT(pOldSurface->format->Gshift == pNewSurface->format->Gshift);
    ASSERT(pOldSurface->format->Gloss == pNewSurface->format->Gloss);
    ASSERT(pOldSurface->format->Bmask == pNewSurface->format->Bmask);
    ASSERT(pOldSurface->format->Bshift == pNewSurface->format->Bshift);
    ASSERT(pOldSurface->format->Bloss == pNewSurface->format->Bloss);
    ASSERT(pOldSurface->format->BytesPerPixel == pNewSurface->format->BytesPerPixel);

    // Extract RGB pixel values from each image.
    Sint32 const bpp = pNewSurface->format->BytesPerPixel;
    // 24-bit color only supported
    ASSERT(bpp == 4);

    Uint32 size = pOldSurface->pitch * pOldSurface->h;
    Uint8 *colorsf;
    Uint8 *colorst;
    colorsf = new Uint8[size];
    colorst = new Uint8[size];

    Uint8 *prf = static_cast<Uint8 *>(pOldSurface->pixels);
    Uint8 *prt = static_cast<Uint8 *>(pNewSurface->pixels);
    memcpy(colorsf, prf, size);
    memcpy(colorst, prt, size);

    // fade from old to new surface. Effect takes constant time.
    Uint32 dwFirstPaint = SDL_GetTicks();
    Uint32 dwNow = dwFirstPaint;

    // Allow first frame to show some change
    FadeBetween24(DestSurface, colorsf, colorst, (dwNow - dwFirstPaint) + 50);

    E_Screen->swap(0, 0, 320, 200);
    dwNow = SDL_GetTicks();

    get_input_events(POLL);

    if (query_key_press_event()) {
        i = -1;
    }

    // Constant-time effect
    while ((i != -1) && (((dwNow - dwFirstPaint) + 50) < fadeDuration)) {
        // Allow first frame to show some change
        FadeBetween24(DestSurface, colorsf, colorst, (dwNow - dwFirstPaint) + 50);

        E_Screen->swap(0, 0, 320, 200);
        dwNow = SDL_GetTicks();

        get_input_events(POLL);

        if (query_key_press_event()) {
            i = -1;
        }
    }

    if (SDL_MUSTLOCK(pNewSurface)) {
        SDL_UnlockSurface(pNewSurface);
    }

    // Show new screen entirely.
    SDL_BlitSurface(pNewSurface, nullptr, pOldSurface, nullptr);
    // Screen::Swap() does the work
    E_Screen->swap(0, 0, 320, 200);

    // Clean up.
    delete[] colorsf;
    delete[] colorst;

    if (bOldNull) {
        SDL_FreeSurface(pOldSurface);
    }

    if (bNewNull) {
        SDL_FreeSurface(pNewSurface);
    }

    return 1;
}

Sint32 Video::fadeblack(bool fade_in)
{
    SDL_Surface *black = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 200, 32, 0, 0, 0, 0);
    Sint32 i;

    if (fade_in) {
        // Fade from black
        i = FadeBetween(black, E_Screen->render, E_Screen->render);
    } else {
        // Fade to black
        i = FadeBetween(E_Screen->render, black, E_Screen->render);
    }

    SDL_FreeSurface(black);

    return i;
}
