#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <SDL2/SDL.h>

int main(int argc, char *argv[])
{
    Uint8 pal[768];
    SDL_RWops *file;
    Sint32 i;

    file = SDL_RWFromFile("../our.pal", "rb");
    SDL_RWread(file, pal, 1, 768);

    for (i = 0; i < 256; ++i) {
        std::cout << "color " << i << ": "
                  << static_cast<unsigned>(pal[i * 3])
                  << " " << static_cast<unsigned>(pal[(i * 3) + 1])
                  << " " << static_cast<unsigned>(pal[(i * 3) + 2])
                  << std::endl;
    }

    // for (i = 0; i < 768; ++i) {
    //     std::cout << i << ": " << pal[i] << std::endl;
    // }

    SDL_RWclose(file);
}
