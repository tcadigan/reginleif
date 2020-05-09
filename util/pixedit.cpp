// Zardus 09/03/2002
// Edits pixie files
// Based on pixieread by Sean

#include <iostream>
#include <sstream>
#include <SDL2/SDL.h>

static constexpr SDL_Color const ourcolors[256] = {
    SDL_Color{0, 0, 0, 255}, SDL_Color{32, 32, 32, 255},
    SDL_Color{64, 64, 64, 255}, SDL_Color{96, 96, 96, 255},
    SDL_Color{128, 128, 128, 255}, SDL_Color{160, 160, 160, 255},
    SDL_Color{192, 192, 192, 255}, SDL_Color{224, 224, 224, 255},
    SDL_Color{4, 4, 4, 255}, SDL_Color{36, 36, 36, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{100, 100, 100, 255},
    SDL_Color{132, 132, 132, 255}, SDL_Color{164, 164, 164, 255},
    SDL_Color{196, 196, 196, 255}, SDL_Color{228, 228, 228, 255},
    SDL_Color{0, 0, 0, 255}, SDL_Color{60, 60, 60, 255},
    SDL_Color{72, 72, 72, 255}, SDL_Color{84, 84, 84, 255},
    SDL_Color{96, 96, 96, 255}, SDL_Color{108, 108, 108, 255},
    SDL_Color{120, 120, 120, 255}, SDL_Color{132, 132, 132, 255},
    SDL_Color{144, 144, 144, 255}, SDL_Color{156, 156, 156, 255},
    SDL_Color{168, 168, 168, 255}, SDL_Color{180, 180, 180, 255},
    SDL_Color{192, 192, 192, 255}, SDL_Color{204, 204, 204, 255},
    SDL_Color{216, 216, 216, 255}, SDL_Color{228, 228, 228, 255},
    SDL_Color{228, 64, 64, 255}, SDL_Color{216, 72, 72, 255},
    SDL_Color{204, 80, 80, 255}, SDL_Color{192, 88, 88, 255},
    SDL_Color{180, 96, 96, 255}, SDL_Color{168, 104, 104, 255},
    SDL_Color{156, 112, 112, 255}, SDL_Color{144, 120, 120, 255},
    SDL_Color{228, 0, 0, 255}, SDL_Color{208, 0, 0, 255},
    SDL_Color{188, 0, 0, 255}, SDL_Color{168, 0, 0, 255},
    SDL_Color{148, 0, 0, 255}, SDL_Color{128, 0, 0, 255},
    SDL_Color{108, 0, 0, 255}, SDL_Color{88, 0, 0, 255},
    SDL_Color{64, 228, 64, 255}, SDL_Color{72, 216, 72, 255},
    SDL_Color{80, 204, 80, 255}, SDL_Color{88, 192, 88, 255},
    SDL_Color{96, 180, 96, 255}, SDL_Color{104, 168, 104, 255},
    SDL_Color{112, 156, 112, 255}, SDL_Color{120, 144, 120, 255},
    SDL_Color{0, 228, 0, 255}, SDL_Color{0, 208, 0, 255},
    SDL_Color{0, 188, 0, 255}, SDL_Color{0, 168, 0, 255},
    SDL_Color{0, 148, 0, 255}, SDL_Color{0, 128, 0, 255},
    SDL_Color{0, 108, 0, 255}, SDL_Color{0, 88, 0, 255},
    SDL_Color{64, 64, 228, 255}, SDL_Color{72, 72, 216, 255},
    SDL_Color{80, 80, 204, 255}, SDL_Color{88, 88, 192, 255},
    SDL_Color{96, 96, 180, 255}, SDL_Color{104, 104, 168, 255},
    SDL_Color{112, 112, 156, 255}, SDL_Color{120, 120, 144, 255},
    SDL_Color{0, 0, 228, 255}, SDL_Color{0, 0, 208, 255},
    SDL_Color{0, 0, 188, 255}, SDL_Color{0, 0, 168, 255},
    SDL_Color{0, 0, 148, 255}, SDL_Color{0, 0, 128, 255},
    SDL_Color{0, 0, 108, 255}, SDL_Color{0, 0, 88, 255},
    SDL_Color{228, 228, 64, 255}, SDL_Color{216, 216, 72, 255},
    SDL_Color{204, 204, 80, 255}, SDL_Color{192, 192, 88, 255},
    SDL_Color{180, 180, 96, 255}, SDL_Color{168, 168, 104, 255},
    SDL_Color{156, 156, 112, 255}, SDL_Color{144, 144, 120, 255},
    SDL_Color{228, 228, 0, 255}, SDL_Color{208, 208, 0, 255},
    SDL_Color{188, 188, 0, 255}, SDL_Color{168, 168, 0, 255},
    SDL_Color{148, 148, 0, 255}, SDL_Color{128, 128, 0, 255},
    SDL_Color{108, 108, 0, 255}, SDL_Color{88, 88, 0, 255},
    SDL_Color{228, 64, 228, 255}, SDL_Color{216, 72, 216, 255},
    SDL_Color{204, 80, 204, 255}, SDL_Color{192, 88, 192, 255},
    SDL_Color{180, 96, 180, 255}, SDL_Color{168, 104, 168, 255},
    SDL_Color{156, 112, 156, 255}, SDL_Color{144, 120, 144, 255},
    SDL_Color{228, 0, 228, 255}, SDL_Color{208, 0, 208, 255},
    SDL_Color{188, 0, 188, 255}, SDL_Color{168, 0, 168, 255},
    SDL_Color{148, 0, 148, 255}, SDL_Color{128, 0, 128, 255},
    SDL_Color{108, 0, 108, 255}, SDL_Color{88, 0, 88, 255},
    SDL_Color{64, 228, 228, 255}, SDL_Color{72, 216, 216, 255},
    SDL_Color{80, 204, 204, 255}, SDL_Color{88, 192, 192, 255},
    SDL_Color{96, 180, 180, 255}, SDL_Color{104, 168, 168, 255},
    SDL_Color{112, 156, 156, 255}, SDL_Color{120, 144, 144, 255},
    SDL_Color{0, 228, 228, 255}, SDL_Color{0, 208, 208, 255},
    SDL_Color{0, 188, 188, 255}, SDL_Color{0, 168, 168, 255},
    SDL_Color{0, 148, 148, 255}, SDL_Color{0, 128, 128, 255},
    SDL_Color{0, 108, 108, 255}, SDL_Color{0, 88, 88, 255},
    SDL_Color{228, 164, 100, 255}, SDL_Color{208, 144, 80, 255},
    SDL_Color{188, 124, 60, 255}, SDL_Color{168, 104, 40, 255},
    SDL_Color{148, 84, 20, 255}, SDL_Color{128, 64, 0, 255},
    SDL_Color{108, 44, 0, 255}, SDL_Color{88, 24, 0, 255},
    SDL_Color{200, 160, 120, 255}, SDL_Color{180, 140, 100, 255},
    SDL_Color{160, 120, 80, 255}, SDL_Color{140, 100, 60, 255},
    SDL_Color{120, 80, 40, 255}, SDL_Color{100, 60, 20, 255},
    SDL_Color{80, 40, 0, 255}, SDL_Color{60, 20, 0, 255},
    SDL_Color{228, 100, 164, 255}, SDL_Color{208, 80, 144, 255},
    SDL_Color{188, 60, 124, 255}, SDL_Color{168, 40, 104, 255},
    SDL_Color{148, 20, 84, 255}, SDL_Color{128, 0, 64, 255},
    SDL_Color{108, 0, 44, 255}, SDL_Color{88, 0, 24, 255},
    SDL_Color{200, 120, 160, 255}, SDL_Color{180, 100, 140, 255},
    SDL_Color{160, 80, 120, 255}, SDL_Color{140, 60, 100, 255},
    SDL_Color{120, 40, 80, 255}, SDL_Color{100, 20, 60, 255},
    SDL_Color{80, 0, 40, 255}, SDL_Color{60, 0, 20, 255},
    SDL_Color{0, 72, 24, 255}, SDL_Color{0, 64, 24, 255},
    SDL_Color{0, 52, 20, 255}, SDL_Color{0, 44, 20, 255},
    SDL_Color{0, 32, 12, 255}, SDL_Color{0, 24, 8, 255},
    SDL_Color{0, 12, 4, 255}, SDL_Color{0, 8, 0, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 68, 68, 255}, SDL_Color{68, 68, 68, 255},
    SDL_Color{68, 100, 228, 255}, SDL_Color{144, 80, 208, 255},
    SDL_Color{124, 60, 188, 255}, SDL_Color{104, 40, 168, 255},
    SDL_Color{84, 20, 148, 255}, SDL_Color{64, 0, 128, 255},
    SDL_Color{44, 0, 108, 255}, SDL_Color{24, 0, 88, 255},
    SDL_Color{160, 120, 200, 255}, SDL_Color{140, 100, 180, 255},
    SDL_Color{120, 80, 160, 255}, SDL_Color{100, 60, 140, 255},
    SDL_Color{80, 40, 120, 255}, SDL_Color{60, 20, 100, 255},
    SDL_Color{40, 0, 80, 255}, SDL_Color{20, 0, 60, 255},
    SDL_Color{100, 164, 228, 255}, SDL_Color{92, 156, 220, 255},
    SDL_Color{84, 148, 212, 255}, SDL_Color{76, 140, 204, 255},
    SDL_Color{68, 132, 196, 255}, SDL_Color{60, 124, 188, 255},
    SDL_Color{52, 116, 180, 255}, SDL_Color{44, 108, 172, 255},
    SDL_Color{36, 100, 164, 255}, SDL_Color{28, 92, 156, 255},
    SDL_Color{20, 84, 148, 255}, SDL_Color{12, 76, 140, 255},
    SDL_Color{4, 68, 132, 255}, SDL_Color{0, 60, 124, 255},
    SDL_Color{0, 52, 116, 255}, SDL_Color{0, 44, 108, 255},
    SDL_Color{228, 60, 0, 255}, SDL_Color{228, 84, 0, 255},
    SDL_Color{228, 108, 0, 255}, SDL_Color{228, 132, 0, 255},
    SDL_Color{228, 156, 0, 255}, SDL_Color{228, 180, 0, 255},
    SDL_Color{228, 204, 0, 255}, SDL_Color{228, 228, 0, 255},
    SDL_Color{228, 60, 0, 255}, SDL_Color{228, 84, 0, 255},
    SDL_Color{228, 108, 0, 255}, SDL_Color{228, 132, 0, 255},
    SDL_Color{228, 156, 0, 255}, SDL_Color{228, 180, 0, 255},
    SDL_Color{228, 204, 0, 255}, SDL_Color{228, 228, 0, 255},
    SDL_Color{228, 148, 124, 255}, SDL_Color{204, 132, 108, 255},
    SDL_Color{188, 112, 96, 255}, SDL_Color{172, 96, 80, 255},
    SDL_Color{224, 140, 92, 255}, SDL_Color{208, 128, 96, 255},
    SDL_Color{192, 120, 88, 255}, SDL_Color{176, 108, 76, 255},
    SDL_Color{112, 72, 72, 255}, SDL_Color{120, 80, 80, 255},
    SDL_Color{128, 88, 88, 255}, SDL_Color{136, 96, 96, 255},
    SDL_Color{144, 104, 104, 255}, SDL_Color{152, 112, 112, 255},
    SDL_Color{160, 120, 120, 255}, SDL_Color{168, 128, 128, 255},
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

    if (SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer) < 0) {
        return 2;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, 800, 600);

    Sint32 width;
    Sint32 height;
    SDL_RenderGetLogicalSize(renderer, &width, &height);

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
                    SDL_Color color;
                    Sint32 c;
                    Sint32 d;

                    d = data[((((frame - 1) * x) * y) + (i * x)) + j];
                    color = ourcolors[d];

                    rect.x = j * mult;
                    rect.y = i * mult;
                    rect.w = mult;
                    rect.h = mult;

                    c = SDL_MapRGB(pixie->format, color.r, color.g, color.b);
                    SDL_FillRect(pixie, &rect, c);
                }
            }

            // Draw palette
            for (i = 0; i < 32; ++i) {
                for (j = x; j < (x + 8); ++j) {
                    SDL_Rect rect;
                    SDL_Color color;
                    Sint32 c;
                    Sint32 d;

                    d = ((i * 8) + j) - x;
                    color = ourcolors[d];

                    rect.x = (x * mult) + (((j - x) * mult) * 2);
                    rect.y = (i * mult) * 2;
                    rect.w = mult * 2;
                    rect.h = mult * 2;

                    c = SDL_MapRGB(pixie->format, color.r, color.g, color.b);
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
