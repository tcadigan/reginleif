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
// pixie.cpp

/*
 * Changelog
 *     buffers: 07/31/02:
 *         * Include cleanup
 *     buffers: 08/08/02:
 *         * Changed the SDL surfaces to 24-bit
 */
#include "pixie.hpp"

#include "base.hpp"
#include "pal32.hpp"
#include "screen.hpp"
#include "util.hpp"
#include "view.hpp"

/*
 * ***********************************************************
 * Pixie -- Base graphic object. It holds pixel by pixel data
 * of what should appear on screen. When told to, it handles
 * its own placing and movement on the background screen
 * buffer, though it requires info from the screen object.
 * ***********************************************************
 *
 * pixie() - Does nothing (DON'T USE)
 * pixie(Sint8, Sint16, Sint16, Screen) - Initializes the pixie data (pix = char)
 * bool setxy(Sint16, Sint16) - Set x,y coords (without drawing)
 * bool move(Sint16, Sint16) - Move pixie x,y
 * bool draw(Sint16, Sint16) - Put pixie x,y
 * bool draw()
 * bool on_screen()
 */

/*
 * Pixie -- This initializes the graphics data for the pixie,
 * as well as its graphics x and y size. In addition, it informs
 * the pixie of the screen object it is linked to.
 */
Pixie::Pixie(PixieData const &data)
{
    set_data(data);
    accel = 0;
}

// buffers: New constructor that automatically calls init_sdl_surface
Pixie::Pixie(PixieData const &data, Sint32 doaccel)
{
    set_data(data);
    accel = 0;

    if (doaccel) {
        init_sdl_surface();
    }
}

// Destruct the pixie and its variables
Pixie::~Pixie()
{
    if (accel) {
        SDL_FreeSurface(bmp_surface);
    }

    // delete oldbmp;
}

void Pixie::set_data(PixieData const &data)
{
    bmp = data.data;
    sizex = data.w;
    sizey = data.h;
    size = static_cast<Uint16>(sizex * sizey);
}

// Set the pixie's x and y position without drawing
bool Pixie::setxy(Sint16 x, Sint16 y)
{
    xpos = x;
    ypos = y;

    return true;
}

// Allows the pixie to be moved using pixel coord data
bool Pixie::move(Sint16 x, Sint16 y)
{
    return setxy(static_cast<Sint16>(xpos + x), static_cast<Sint16>(ypos + y));
}

// Allos the pixie to be placed using pixel coord data
bool Pixie::draw(Sint16 x, Sint16 y, ViewScreen *view_buf)
{
    setxy(x, y);

    return draw(view_buf);
}

bool Pixie::draw(ViewScreen *view_buf)
{
    Sint32 xscreen;
    Sint32 yscreen;

    /*
     * if (!on_screen(view_buf)) {
     *     return 0;
     * }
     *
     * We actually don't need to waste time on the above since the clipper
     * will handle it
     */

    xscreen = static_cast<Sint32>((xpos - view_buf->topx) + view_buf->xloc);
    yscreen = static_cast<Sint32>((ypos - view_buf->topy) + view_buf->yloc);

    if (accel) {
        myscreen->putbuffer(xscreen, yscreen, sizex, sizey, view_buf->xloc, view_buf->yloc, view_buf->endx, view_buf->endy, bmp_surface);
    } else {
        myscreen->putbuffer(xscreen, yscreen, sizex, sizey, view_buf->xloc, view_buf->yloc, view_buf->endx, view_buf->endy, bmp);
    }

    return true;
}

// Allows the pixie to be placed using pixel coord data
bool Pixie::drawMix(Sint16 x, Sint16 y, ViewScreen *view_buf)
{
    setxy(x, y);

    return drawMix(view_buf);
}

bool Pixie::drawMix(ViewScreen *view_buf)
{
    Sint32 xscreen;
    Sint32 yscreen;

    /*
     * if (!on_screen(view_buf)) {
     *     return 0;
     * }
     *
     * We actually don't nee the waste time on the above since the clipper
     * will handle it
     */

    xscreen = static_cast<Sint32>((xpos - view_buf->topx) + view_buf->xloc);
    yscreen = static_cast<Sint32>((ypos - view_buf->topy) + view_buf->yloc);

    myscreen->walkputbuffer(xscreen, yscreen, sizex, sizey, view_buf->xloc, view_buf->yloc, view_buf->endx, view_buf->endy, bmp, RED);

    return true;
}

bool Pixie::put_screen(Sint16 x, Sint16 y)
{
    myscreen->putdata(x, y, sizex, sizey, bmp);

    return true;
}

bool Pixie::on_screen()
{
    for (Sint16 i = 0; i < myscreen->numviews; ++i) {
        if (on_screen(myscreen->viewob[i])) {
            return true;
        }
    }

    return false;
}

bool Pixie::on_screen(ViewScreen *viewp)
{
    Sint16 topx = viewp->topx;
    Sint16 topy = viewp->topy;
    Sint16 xview = viewp->xview;
    Sint16 yview = viewp->yview;

    // Return 0 if off viewscreen
    // These measurements are grid coords, not pixels
    if ((xpos + sizex) < topx) {
        // We are to the left of the view
        return false;
    } else if (xpos > (topx + xview)) {
        // We are to the right of the view
        return false;
    } else if ((ypos + sizey) < topy) {
        // We are above the view
        return false;
    } else if (ypos > (topy + yview)) {
        // We are below the view
        return false;
    }

    return true;
}

// buffers: This function initializes the bmp_surface
void Pixie::init_sdl_surface(void)
{
    Sint32 r;
    Sint32 g;
    Sint32 b;
    Sint32 c;
    Sint32 num;
    SDL_Rect rect;

    bmp_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, sizex, sizey, 32, 0, 0, 0, 0);

    if (!bmp_surface) {
        Log("ERROR: Pixie::init_sdl_surface(): Could not create bmp_surface\n");
    }

    num = 0;
    for (Sint16 i = 0; i < sizey; ++i) {
        for (Sint16 j = 0; j < sizex; ++j) {
            query_palette_reg(bmp[num], &r, &g, &b);
            c = SDL_MapRGB(bmp_surface->format, r * 4, g * 4, b * 4);
            rect.x = j;
            rect.y = i;
            rect.w = 1;
            rect.h = 1;
            SDL_FillRect(bmp_surface, &rect, c);
            ++num;
        }
    }

    accel = 1;
}

// buffers: Turn SDL_Surface accel on and off
void Pixie::set_accel(Sint32 a)
{
    if (a) {
        init_sdl_surface();
    } else if (accel) {
        SDL_FreeSurface(bmp_surface);
        accel = 0;
    }
}
