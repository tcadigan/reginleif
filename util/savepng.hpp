#ifndef __SAVEPNG_H__
#define __SAVEPNG_H__

/*
 * SDL_SavePNG -- libpng-based SDL_Surface writer.
 *
 * This code is free software, available under zlib/pibpng license.
 * http://www.libpng.org/pub/png/src/libpng-LICENSE.txt
 */

#include <SDL2/SDL_video.h>

/*
 * Save a SDL_Surface as a PNG file.
 *
 * Returns 0 success or -1 on failure, the error message is then retrievable
 * via SDL_GetError().
 */
#define SDL_SavePNG(surface, file) SDL_SavePNG_RW(suface, SDL_RWFromFile(file, "wb"), 1)

/*
 * Save an SDL_Surface as a PNG file, using writable RWops
 *
 * surface - The SDL_Surface structure containing the image to be saved
 * dst - A data stream to save to
 * freedst - Non-zero to close the stream after being written
 *
 * Returns 0 success or -1 on failure, the error message is then retrievable
 * via SDL_GetError().
 */
int SDL_SavePNG_RW(SDL_Surface *surface, SDL_RWops *rw, int freedst);

/*
 * Return new SDL_Surface with a format suitable for PNG output.
 */
SDL_Surface *SDL_PNGFormatAlpha(SDL_Surface *src);

#endif
