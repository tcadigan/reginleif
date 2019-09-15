// Zardus 09/03/2002
// Edits pixie files
// Based on pixieread by Sean

#include <iostream>
#include <sstream>
#include <SDL2/SDL.h>

Sint8 ourcolors[] = {
     0,  0,  0,  8,  8,  8, 16, 16, 16, 24, 24, 24, 32, 32, 32, 40, 40, 40, 48,
    48, 48, 56, 56, 56,  1,  1,  1,  9,  9,  9, 17, 17, 17, 25, 25, 25, 33, 33,
    33, 41, 41, 41, 49, 49, 49, 57, 57, 57,  0,  0,  0, 15, 15, 15, 18, 18, 18,
    21, 21, 21, 24, 24, 24, 27, 27, 27, 30, 30, 30, 33, 33, 33, 36, 36, 36, 39,
    39, 39, 42, 42, 42, 45, 45, 45, 48, 48, 48, 51, 51, 51, 54, 54, 54, 57, 57,
    57, 57, 16, 16, 54, 18, 18, 51, 20, 20, 48, 22, 22, 45, 24, 24, 42, 26, 26,
    39, 28, 28, 36, 30, 30, 57,  0,  0, 52,  0,  0, 47,  0,  0, 42,  0,  0, 37,
     0,  0, 32,  0,  0, 27,  0,  0, 22,  0,  0, 16, 57, 16, 18, 54, 18, 20, 51,
    20, 22, 48, 22, 24, 45, 24, 26, 42, 26, 28, 39, 28, 30, 36, 30,  0, 57,  0,
     0, 52,  0,  0, 47,  0,  0, 42,  0,  0, 37,  0,  0, 32,  0,  0, 27,  0,  0,
    22,  0, 16, 16, 57, 18, 18, 54, 20, 20, 51, 22, 22, 48, 24, 24, 45, 26, 26,
    42, 28, 28, 39, 30, 30, 36,  0,  0, 57,  0,  0, 52,  0,  0, 47,  0,  0, 42,
     0,  0, 37,  0,  0, 32,  0,  0, 27,  0,  0, 22, 57, 57, 16, 54, 54, 18, 51,
    51, 20, 48, 48, 22, 45, 45, 24, 42, 42, 26, 39, 39, 28, 36, 36, 30, 57, 57,
     0, 52, 52,  0, 47, 47,  0, 42, 42,  0, 37, 37,  0, 32, 32,  0, 27, 27,  0,
    22, 22,  0, 57, 16, 57, 54, 18, 54, 51, 20, 51, 48, 22, 48, 45, 24, 45, 42,
    26, 42, 39, 28, 39, 36, 30, 36, 57,  0, 57, 52,  0, 52, 47,  0, 47, 42,  0,
    42, 37,  0, 37, 32,  0, 32, 27,  0, 27, 22,  0, 22, 16, 57, 57, 18, 54, 54,
    20, 51, 51, 22, 48, 48, 24, 45, 45, 26, 42, 42, 28, 39, 39, 30, 36, 36,  0,
    57, 57,  0, 52, 52,  0, 47, 47,  0, 42, 42,  0, 37, 37,  0, 32, 32,  0, 27,
    27,  0, 22, 22, 57, 41, 25, 52, 36, 20, 47, 31, 15, 42, 26, 10, 37, 21,  5,
    32, 16,  0, 27, 11,  0, 22,  6,  0, 50, 40, 30, 45, 35, 25, 40, 30, 20, 35,
    25, 15, 30, 20, 10, 25, 15,  5, 20, 10,  0, 15,  5,  0, 57, 25, 41, 52, 20,
    36, 47, 15, 31, 42, 10, 26, 37,  5, 21, 32,  0, 16, 27,  0, 11, 22,  0,  6,
    50, 30, 40, 45, 25, 35, 40, 20, 30, 35, 15, 25, 30, 10, 20, 25,  5, 15, 20,
     0, 10, 15,  0,  5,  0, 18,  6,  0, 16,  6,  0, 13,  5,  0, 11,  5,  0,  8,
     3,  0,  6,  2,  0,  3,  1,  0,  2,  0, 17, 17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 41, 25, 57, 36, 20, 52, 31, 15, 47, 26, 10, 42, 41,
     5, 37, 16,  0, 32, 11,  0, 27,  6,  0, 22, 40, 30, 50, 35, 25, 45, 30, 20,
    40, 25, 15, 35, 20, 10, 30, 15,  5, 25, 10,  0, 20,  5,  0, 15, 25, 41, 57,
    23, 39, 55, 21, 37, 53, 19, 35, 51, 17, 33, 49, 15, 31, 47, 13, 29, 45, 11,
    27, 43,  9, 25, 41,  7, 23, 39,  5, 21, 37,  3, 19, 35,  1, 17, 33,  0, 15,
    31,  0, 13, 29,  0, 11, 27, 57, 15,  0, 57, 21,  0, 57, 27,  0, 57, 33,  0,
    57, 39,  0, 57, 45,  0, 57, 51,  0, 57, 57,  0, 57, 15,  0, 57, 21,  0, 57,
    27,  0, 57, 33,  0, 57, 39,  0, 57, 45,  0, 57, 51,  0, 57, 57,  0, 57, 37,
    31, 51, 33, 27, 47, 28, 24, 43, 24, 20, 56, 35, 23, 52, 32, 24, 48, 30, 22,
    44, 27, 19, 28, 18, 18, 30, 20, 20, 32, 22, 22, 34, 24, 24, 36, 26, 26, 38,
    28, 28, 40, 30, 30, 42, 32, 32
};

int main(int argc, char *argv[])
{
    SDL_RWops *file;
    Uint8 numframes;
    Uint8 x;
    Uint8 y;
    Uint8 curcolor;
    Uint8 *data;
    Sint32 i;
    Sint32 j;
    Sint32 sizex;
    Sint32 sizey;
    Sint32 frame = 1;
    Sint32 mult = 3;
    SDL_Surface *pixie;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Event event;
    bool done = false;
    bool redowindow = true;
    bool redopicture = true;
    bool refreshpicture = true;
    bool leftclick = false;
    SDL_Color bg_color = { 0, 0, 0, 255 };

    if (argc != 2) {
        std::cout << "USAGE: pixedit file.pix" << std::endl;

        exit(0);
    }

    std::cout << "reading pixie: " << argv[1] << std::endl;

    file = SDL_RWFromFile(argv[1], "rb");
    if (file == NULL) {
        std::cout << "error while trying to open " << argv[1] << std::endl;

        exit(1);
    }

    SDL_RWread(file, &numframes, 1, 1);
    SDL_RWread(file, &x, 1, 1);
    SDL_RWread(file, &y, 1, 1);

    data = new Uint8[(numframes * x) * y];
    SDL_RWread(file, data, 1, (numframes * x) * y);
    SDL_RWclose(file);

    std::cout << "=================== " << argv[1] << " ==================="
              << std::endl << "num of frames: " << static_cast<unsigned>(numframes)
              << std::endl << "x: " << static_cast<unsigned>(x) << std::endl
              << "y: " << static_cast<unsigned>(y) << std::endl;

    SDL_Init(SDL_INIT_VIDEO);

    if (SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_SHOWN, &window, &renderer) < 0) {
        return 2;
    }

    while (!done) {
        if (redowindow) {
            sizex = x + 16;
            sizey = y;

            if (sizey < 64) {
                sizey = 64;
            }

            pixie = SDL_CreateRGBSurface(SDL_SWSURFACE, sizex * mult, sizey * mult, 16, 0, 0, 0, 0);
            if (pixie == NULL) {
                return 3;
            }

            SDL_FillRect(pixie,
                         NULL,
                         SDL_MapRGB(pixie->format, bg_color.r, bg_color.g, bg_color.b));

            redopicture = true;
            redowindow = false;
        }

        if (redopicture) {
            std::ostringstream buffer;
            buffer << "Frame " << frame << " at " << mult << "x";
            SDL_SetWindowTitle(window, buffer.str().c_str());

            SDL_FillRect(pixie,
                         NULL,
                         SDL_MapRGB(pixie->format, bg_color.r, bg_color.g, bg_color.b));

            // Draw sprite frame
            // for (i = 0; i < y; ++i) {
            //     for (j = 0; j < x; ++j) {
            for (i = y - 1; i >= 0; --i) {
                for (j = x - 1; j >= 0; --j) {
                    SDL_Rect rect;
                    Sint32 r;
                    Sint32 g;
                    Sint32 b;
                    Sint32 c;
                    Sint32 d;

                    d = data[((((frame - 1) * x) * y) + (i * x)) + j];
                    r = ourcolors[d * 3] * 4;
                    g = ourcolors[(d * 3) + 1] * 4;
                    b = ourcolors[(d * 3) + 2] * 4;

                    rect.x = j * mult;
                    rect.y = i * mult;
                    rect.w = mult;
                    rect.h = mult;

                    if ((r > 0) || (g > 0) || (b > 0)) {
                        c = SDL_MapRGB(pixie->format, r, g, b);
                        SDL_FillRect(pixie, &rect, c);
                    }
                }
            }

            // Draw palette
            for (i = 0; i < 32; ++i) {
                for (j = x; j < (x + 8); ++j) {
                    SDL_Rect rect;
                    Sint32 r;
                    Sint32 g;
                    Sint32 b;
                    Sint32 c;
                    Sint32 d;

                    d = ((i * 8) + j) - x;
                    r = ourcolors[d * 3] * 4;
                    g = ourcolors[(d * 3) + 1] * 4;
                    b = ourcolors[(d * 3) + 2] * 4;

                    rect.x = (x * mult) + (((j - x) * mult) * 2);
                    rect.y = (i * mult) * 2;
                    rect.w = mult * 2;
                    rect.h = mult * 2;

                    c = SDL_MapRGB(pixie->format, r, g, b);
                    SDL_FillRect(pixie, &rect, c);
                }
            }

            texture = SDL_CreateTextureFromSurface(renderer, pixie);
            if (texture == NULL) {
                return 4;
            }

            redopicture = false;
            refreshpicture = true;
        }

        if (refreshpicture) {
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            refreshpicture = false;
        }

        SDL_WaitEvent(&event);

        switch (event.type) {
        case SDL_QUIT:
            done = true;

            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
            case SDLK_ESCAPE:
                done = true;

                break;
            case SDLK_b:
                bg_color.r = rand() % 256;
                bg_color.g = rand() % 256;
                bg_color.b = rand() % 256;
                redopicture = true;

                break;
            case SDLK_LEFT:
                if (frame > 1) {
                    --frame;
                }

                redowindow = true;

                break;
            case SDLK_RIGHT:
                if (frame < numframes) {
                    ++frame;
                }

                redowindow = true;

                break;
            case SDLK_UP:
                ++mult;
                redowindow = true;

                break;
            case SDLK_DOWN:
                if (mult > 1) {
                    --mult;
                }

                redowindow = true;

                break;
            case SDLK_s:
                file = SDL_RWFromFile(argv[1], "w");
                if (file == NULL) {
                    std::cout << "error while trying to open " << argv[1] << std::endl;

                    exit(1);
                }

                SDL_RWwrite(file, &numframes, 1, 1);
                SDL_RWwrite(file, &x, 1, 1);
                SDL_RWwrite(file, &y, 1, 1);
                SDL_RWwrite(file, data, 1, (numframes * x) * y);
                SDL_RWclose(file);

                break;
            default:
                break;
            }

            break;
        case SDL_MOUSEBUTTONDOWN:
        {
            if (event.button.x >= (x * mult)) {
                Sint32 mousex = event.button.x;
                Sint32 mousey = event.button.y;
                mousex = (mousex - (x * mult)) / 2;
                mousey = mousey / (mult * 2);
                curcolor = (mousey * 8) + mousex;
            } else if (event.button.button > 1) {
                curcolor = data[(((frame - 1) * x) * y) + ((event.button.y / mult) * x) + (event.button.x / mult)];
            } else {
                leftclick = true;
            }

            break;
        }
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == 1) {
                leftclick = false;
            }

            break;
        default:
            break;
        }

        if (leftclick && ((event.button.y / mult) < y) && ((event.button.x / mult) < x)) {
            int spot;
            spot = ((event.button.y / mult) * x) + (event.button.x / mult);
            data[((frame * x) * y) + spot] = curcolor;
            redopicture = true;
        }
    }

    SDL_Quit();

    delete[](data);

    return 0;
}
