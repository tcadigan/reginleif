/*
 * console.cpp
 * 2006 Shamus Young
 *
 * This module is not at all needed. It's just an output window used for
 * debugging.
 *
 * Sadly, I had some proprietary code in here, so I couldn't release it.
 * Still, this thing is pretty simple: Make a window, and when other modules
 * call Console(), just print the text to the window. the program works
 * just fine without this, but development is easier when you can print stuff
 * to the console and get feedback while it's building.
 */

#include <cstdio>
#include <cstdarg>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

void console(char const *message, ...)
{
    char msg_text[256];
    va_list marker;

    va_start(marker, message);
    vsprintf(msg_text, message, marker);
    va_end(marker);

    SDL_Log("%s", msg_text);
    // Now the message is store in msg_text, you should probably display
    // it or something...
}

void console_term(void)
{
}

void console_init(void)
{
}
