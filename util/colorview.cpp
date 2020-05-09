// Jonathan Dearborn 10/24/2013
// Shows which colors you're dealing with

#include <iostream>
#include <SDL2/SDL.h>

// Color cycling colors indices
#define WATER_START 208
#define WATER_END 223
#define ORANGE_START 224
#define ORANGE_END 231

Sint32 columns = 16;
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

Sint32 num_rows()
{
    Sint32 numcolors = sizeof(ourcolors) / sizeof(SDL_Color);
    Sint32 numrows = numcolors / columns;

    if ((numrows * columns) < numcolors) {
        ++numrows;
    }

    return numrows;
}

// void draw_palette_to_surface(SDL_Surface *surface)
void draw_palette_to_surface(SDL_Renderer *renderer)
{
    Sint32 width;
    Sint32 height;
    SDL_GetRendererOutputSize(renderer, &width, &height);

    Sint32 xdelta = width / columns;
    Sint32 ydelta = height / num_rows();


    Sint32 x = 0;
    Sint32 y = 0;
    Sint32 index = 0;

    while (index < 256) {
        SDL_Color color = ourcolors[index];

        SDL_Rect rect = { x, y, xdelta, ydelta };

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);

        ++index;
        x += xdelta;
        if (x >= (columns * xdelta)) {
            x = 0;
            y += ydelta;
        }
    }
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_Window *window;
    SDL_Renderer *renderer;

    if (SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer) < 0) {
        return 2;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    Sint32 width;
    Sint32 height;
    SDL_GetRendererOutputSize(renderer, &width, &height);

    draw_palette_to_surface(renderer);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    bool done = false;

    while (!done) {
        while (SDL_PollEvent(&event)) {
            if ((event.type == SDL_QUIT)
                || ((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_ESCAPE))) {
                done = true;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                Sint32 column_index = event.button.x / (width / columns);
                Sint32 row_index = event.button.y / (height / num_rows());
                Sint32 index = (columns * row_index) + column_index;

                if ((index >= 0) && (index < 256)) {
                    std::cout << "Color: " << index;

                    if ((WATER_START <= index) && (index <= WATER_END)) {
                        std::cout << " (water cycle)";
                    }

                    if ((ORANGE_START <= index) && (index <= ORANGE_END)) {
                        std::cout << " (orange cycle)";
                    }

                    std::cout << std::endl;
                }
            }
        }

        SDL_Delay(10);
    }

    SDL_Quit();

    return 0;
}
