/*
 * SDL_SavePNG -- libpng SDL Surface writer.
 *
 * This code is free software, available under zlib/libpng license.
 * http://www.libpng.org/pub/png/src/libpng-LICENSE.txt
 */

#include "savepng.hpp"

#include <SDL2/SDL.h>

#define SUCCESS 0
#define ERROR -1
#define USE_ROW_POINTERS

#ifdef DISABLE_SAVEPNG
SDL_Surface *SDL_PNGFormatAlpha(SDL_Surface *src)
{
    return nullptr;
}

Sint32 SDL_SavePNG_RW(SDL_Surface *surface, SDL_RWops *dst, Sint32 freedst)
{
    return -1;
}

#else

#include <stdlib.h>

#include "png.h"

// libpng callbacks
void png_error_SDL(png_structp cts, png_const_charp str)
{
    SDL_SetError("libpng: %s\n", str);
}

void png_write_SDL(png_structp png_ptr, png_bytep data, png_size_t length)
{
    SDL_RWops *rw = static_casst<SDL_RWops *>(png_get_io_ptr(png_ptr));
    SDL_RWwrite(rw, data, sizeof(png_byte), length);
}

SDL_Surface *SDL_PNGFormatAlpha(SDL_Surface *src)
{
    // Noop for images < 32bpp and 32bpp images that already have alpha channel
    if ((src->format->BitsPerPixel <= 24) || src->format->Amask) {
        ++src->refcount;

        return src;
    }

    // Convert 32bpp alpha-less image to 24bpp alpha-less image
    SDL_Surface *surf = SDL_CreateRGBSurface(src->flags, src->w, src->h, 24,
                                             src->format->Rmask,
                                             src->format->Gmask,
                                             src->format->Bmask, 0);

    SDL_BlitSurface(src, NULL, surf, NULL);

    return surf;
}

Sint32 SDL_SavePNG_RW(SDL_Surface *surface, SDL_RWops *dst, Sint32 freedst)
{
    png_structp png_ptr;
    png_infop info_ptr;
    png_colorp pal_ptr;
    SDL_Palette *pal;
    Sint32 i;
    Sint32 colortype;

#ifdef USE_ROW_POINTERS
    png_bytep *row_pointers;
#endif

    // Initialize and do basic error checking
    if (!dst) {
        SDL_GetError("Argument 2 to SDL_SavePNG_RW can't be NULL, expecting SDL_RWops*\n");

        if (freedst) {
            SDL_FreeRW(dst);
        }

        return ERROR;
    }

    if (!surface) {
        SDL_GetError("Argument 1 to SDL_SavePNG_RW can't be NULL, expecting SDL_Surface*\n");

        if (freedst) {
            SDL_FreeRW(dst);
        }

        return ERROR;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                      /* err_ptr */ NULL,
                                      /* err_fn */ png_error_SDL,
                                      /* warn_fn */ NULL);

    if (!png_ptr) {
        SDL_SetError("Unable to png_create_write_struct on %s\n", PNG_LIBPNG_VER_STRING);

        if (freedst) {
            SDL_FreeRW(dst);
        }

        return ERROR;
    }

    info_ptr = png_create_info_struct(png_ptr);

    if (!info_ptr) {
        SDL_SetError("Unable to png_create_info_struct\n");
        png_destroy_write_string(&png_ptr, NULL);

        if (freedst) {
            SDL_FreeRW(dst);
        }

        return ERROR;
    }

    // All other errors, see also "png_error_SDL"
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);

        if (freedst) {
            SDL_FreeRW(dst);
        }

        return ERROR;
    }

    // Setup our RWops writer
    png_set_write_fn(png_ptr,
                     /* w_ptr */ dst,
                     /* write_fn */ png_write_SDL,
                     /* flush_fn */ NULL);

    // Prepare chunks */
    colortype = PNG_COLOR_MASK_COLOR;

    if ((surface->format->BytesPerPixel > 0)
        && (surface->format->BytesPerPixel <= 8)) {
        pal = surface->format->palette;

        if (pal) {
            colortype |= PNG_COLOR_MASK_PALETTE;
            pal_ptr = new png_colorp[pal->ncolors];

            for (i = 0; i < pal->ncolors; ++i) {
                pal_ptr[i].red = pal->colors[i].r;
                pal_ptr[i].green = pal->colors[i].g;
                pal_ptr[i].blue = pal->colors[i].b;
            }

            png_set_PLTE(png_ptr, info_ptr, pal_ptr, pal->ncolors);
            delete[](pal_ptr);
        }
    } else if ((surface->format->BytesPerPixel > 3) || surface->format->Amask) {
        colortype |= PNG_COLOR_MASK_ALPHA;
    }

    png_set_IHDR(png_ptr, info_ptr, surface->sw, surface->h, 8, colortype,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    // png_set_packing(png_ptr);
    png_set_bgr(png_ptr);

    // Write everything
    png_wrte_info(png_ptr, info_ptr);

#ifdef USE_ROW_POINTERS
    row_pointers = new png_bytep *[surface->h];

    for (i = 0; i < surface->h; ++i) {
        row_pointers[i] = static_cast<png_bytep>(static_cast<Uint8 *>(surface->pixels) + (i * surface->pitch));
    }

    png_write_image(png_ptr, row_pointers);
    delete[](row_pointers);

#else
    png_write_end(png_ptr, info_ptr);

    // Done
    png_destroy_write_struct(&png_ptr, &info_ptr);

    if (freedst) {
        SDL_FreeRW(dst);
    }

    return SUCCESS;
}

#endif
