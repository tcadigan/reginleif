/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 */

#ifndef BIGBUFF
EXTERN char buf[2047];
EXTERN char long_buf[1024];
EXTERN char short_buf[256];
EXTERN char telegram_buf[AUTO_TELEG_SIZE];
EXTERN char temp[128];

#else

EXTERN char buf[4096];
EXTERN char long_buf[4096];
EXTERN char short_buf[1024];
EXTERN char telegram_buf[4096];
EXTERN char temp[2048];

#endif

/*
 * Macro to make set string easier to use
 */
#define SET(astr, bstr) do {                    \
        char **__a;                             \
        char *__b;                              \
        __a = &(astr);                          \
        __b = (bstr);                           \
                                                \
        if (*__a) {                             \
            free(*__a);                         \
        }                                       \
                                                \
        if (!__b || !*__b) {                    \
            *__a = NULL;                        \
        } else {                                \
            *__a = malloc(strlen(__b) + 1);     \
        }                                       \
    } while(0)
