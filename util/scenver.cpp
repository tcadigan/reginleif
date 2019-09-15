#include <cstdio>
#include <iostream>
#include <SDL2/SDL.h>

int main(int argc, char *argv[])
{
    Uint8 temptext[10] = "XXX";
    Uint8 versionnumber = 0;
    SDL_RWops *infile;

    infile = SDL_RWFromFile(argv[1], "r");
    SDL_RWread(infile, temptext, 3, 1);
    SDL_RWread(infile, &versionnumber, 1, 1);

    std::cout << argv[1] << " is a version " << versionnumber << " scenario" << std::endl;

    return versionnumber;
}
