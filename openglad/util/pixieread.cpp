// buffers 08/08/02
// Reads and displays pixie data
// Useful for debugging stuff

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <SDL2/SDL.h>

int main (int argc, char *argv[])
{
    SDL_RWops *file;
    Uint8 numframes;
    Uint8 x;
    Uint8 y;
    Uint8 *data;
    Sint32 i;
    Sint32 j;
    Sint32 d;

    if (argc != 2) {
        std::cout << "USAGE: pixieread file.pix" << std::endl;

        exit(0);
    }

    std::cout << "reading pixie: " << argv[1] << std::endl;

    file = SDL_RWFromFile(argv[1], "rb");
    if (file == nullptr) {
        std::cout << "error while trying to open " << argv[1] << std::endl;

        exit(0);
    }

    SDL_RWread(file, &numframes, 1, 1);
    SDL_RWread(file, &x, 1, 1);
    SDL_RWread(file, &y, 1, 1);

    data = new Uint8[(numframes * x) * y];

    SDL_RWread(file, &data, 1, (numframes * x) * y);

    std::cout << "=================== " << argv[1] << " ===================" << std::endl
              << "num of frames: " << static_cast<unsigned>(numframes)
              << " x: " << static_cast<unsigned>(x)
              << " y: " << static_cast<unsigned>(y) << std::endl
              << "data: ";

    d = 0;
    for (i = 0; i < y; ++i) {
        for (j = 0; j < x; ++ j) {
            std::cout << std::setw(3) << static_cast<unsigned>(data[d]) << " ";
            ++d;
        }

        std::cout << std::endl << "      ";
    }

    std::cout << std::endl << "==========================================" << std::endl;

    delete[](data);
    SDL_RWclose(file);

    return 1;
}
