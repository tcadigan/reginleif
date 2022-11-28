// Zardus 09/03/2002
// Edits pixie files
// Based on pixieread by Sean

#include <iomanip>
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

Sint32 num_pal_rows(Sint32 columns)
{
    Sint32 numcolors = sizeof(ourcolors) / sizeof(SDL_Color);
    Sint32 numrows = numcolors / columns;

    if ((numrows * columns) < numcolors) {
        ++numrows;
    }

    return numrows;
}

void draw_palette(SDL_Renderer *renderer, SDL_Rect const &pane)
{
    Sint32 columns = 10;
    Sint32 rows = num_pal_rows(columns);

    Sint32 xdelta = pane.w / columns;
    Sint32 ydelta = pane.h / rows;

    Sint32 index = 0;

    for (Sint32 y = 0; y < num_pal_rows(columns); ++y) {
        for (Sint32 x = 0; x < columns; ++x) {
            SDL_Color color = ourcolors[index];
            SDL_Rect rect = { (x * xdelta) + pane.x, (y * ydelta) + pane.y, xdelta, ydelta };
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &rect);

            ++index;
        }
    }
}

void draw_pix(SDL_Renderer *renderer,
              Uint8 *data, Uint8 frame, Uint8 num_rows, Uint8 num_columns,
              Uint8 logical_x, Uint8 logical_y, Uint8 zoom,
              SDL_Rect const &pane, SDL_Color const &bg_color)
{
    Sint32 columns = pane.w / 16;
    Sint32 rows = pane.h / 16;

    Sint32 xdelta = pane.w / columns;
    Sint32 ydelta = pane.h / rows;

    for (Sint32 y = 0; y < rows; ++y) {
        for (Sint32 x = 0; x < columns; ++x) {
            Sint32 d_x = (logical_x + x) / zoom;
            Sint32 d_y = (logical_y + y) / zoom;

            SDL_Color color = bg_color;

            if ((d_x < num_columns) && (d_y < num_rows)) {
                Sint32 frame_offset = (frame - 1) * (num_rows * num_columns);
                Uint8 d = data[frame_offset + ((d_y * num_columns) + d_x)];
                color = ourcolors[d];
            }

            SDL_Rect rect = { (x * xdelta) + pane.x, (y * ydelta) + pane.y, xdelta, ydelta };
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void print_pix(Uint8 *data, Uint32 num_frames, Uint32 num_rows, Uint32 num_columns)
{
    for (Uint32 f = 0; f < num_frames; ++f) {
        std::cout << "Frame: " << f << std::endl;

        for (Uint32 i = 0; i < num_rows; ++i) {
            for (Uint32 j = 0; j < num_columns; ++j) {
                if (j != 0) {
                    std::cout << " ";
                }

                Sint32 val = data[(i * num_columns) + j];
                std::cout << std::setw(3) << val;
            }

            std::cout << std::endl;
        }

        std::cout << std::endl;
    }
}

int main(int argc, char *argv[])
{
    SDL_RWops *file;

    Uint8 *data;
    Uint8 num_frames;
    Uint8 num_rows;
    Uint8 num_columns;

    Uint8 logical_x = 0;
    Uint8 logical_y = 0;
    Sint8 zoom = 1;

    SDL_Color bg_color = { 0, 0, 0, SDL_ALPHA_OPAQUE };

    Uint8 curcolor = 0;
    Sint32 frame = 1;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;
    bool done = false;
    bool redraw = true;
    bool leftclick = false;
    bool rightclick = false;

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

    SDL_RWread(file, &num_frames, 1, 1);
    SDL_RWread(file, &num_columns, 1, 1);
    SDL_RWread(file, &num_rows, 1, 1);

    data = new Uint8[(num_rows * num_columns) * num_frames];
    SDL_RWread(file, data, 1, (num_rows * num_columns) * num_frames);
    SDL_RWclose(file);

    std::cout << "=================== " << argv[1] << " ==================="
              << std::endl << "num frames: " << static_cast<Uint32>(num_frames)
              << std::endl << "num rows: " << static_cast<Uint32>(num_rows)
              << std::endl << "num columns: " << static_cast<Uint32>(num_columns) << std::endl;

    SDL_Init(SDL_INIT_VIDEO);

    if (SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer) < 0) {
        return 2;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    Sint32 width;
    Sint32 height;
    SDL_GetRendererOutputSize(renderer, &width, &height);

    Sint32 pix_width = (width / 4) * 3;
    Sint32 pal_width = (width / 4);

    SDL_Rect pix_pane = { 0, 0, pix_width, height };
    SDL_Rect pal_pane = { pix_width, 0, pal_width, height };

    print_pix(data, num_frames, num_rows, num_columns);
    draw_palette(renderer, pal_pane);

    while (!done) {
        if (redraw) {
            std::ostringstream buffer;
            buffer << "Frame " << static_cast<Uint32>(frame) << " at " << static_cast<Uint32>(zoom) << "x";
            SDL_SetWindowTitle(window, buffer.str().c_str());

            draw_pix(renderer,
                     data, frame, num_rows, num_columns,
                     logical_x, logical_y, zoom,
                     pix_pane, bg_color);
            SDL_RenderPresent(renderer);
            redraw = false;
        }

        SDL_WaitEvent(&event);

        switch (event.type) {
        case SDL_QUIT:
            done = true;

            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                done = true;

                break;
            case SDLK_b:
                bg_color.r = rand() % 256;
                bg_color.g = rand() % 256;
                bg_color.b = rand() % 256;
                redraw = true;

                break;
            case SDLK_j:
                if (frame > 1) {
                    --frame;
                    redraw = true;
                }

                break;
            case SDLK_l:
                if (frame < num_frames) {
                    ++frame;
                    redraw = true;
                }

                break;
            case SDLK_i:
                ++zoom;
                redraw = true;

                break;
            case SDLK_k:
                if (zoom > 1) {
                    --zoom;
                    redraw = true;
                }

                break;
            case SDLK_UP:
                if (logical_y > 0) {
                    --logical_y;
                    redraw = true;
                }

                break;
            case SDLK_DOWN:
                if ((logical_y / zoom) < num_rows) {
                    ++logical_y;
                    redraw = true;
                }

                break;
            case SDLK_LEFT:
                if (logical_x > 0) {
                    --logical_x;
                    redraw = true;
                }

                break;
            case SDLK_RIGHT:
                if ((logical_x / zoom) < num_columns) {
                    ++logical_x;
                    redraw = true;
                }

                break;
            case SDLK_p:
                print_pix(data, num_frames, num_rows, num_columns);

                break;
            case SDLK_s:
                // file = SDL_RWFromFile(argv[1], "w");
                // if (file == NULL) {
                //     std::cout << "error while trying to open " << argv[1] << std::endl;

                //     exit(1);
                // }

                // SDL_RWwrite(file, &numframes, 1, 1);
                // SDL_RWwrite(file, &x, 1, 1);
                // SDL_RWwrite(file, &y, 1, 1);
                // SDL_RWwrite(file, data, 1, (numframes * x) * y);
                // SDL_RWclose(file);

                break;
            default:
                break;
            }

            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                leftclick = true;
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                rightclick = true;
            }

            break;
        case SDL_MOUSEBUTTONUP:
        {
            SDL_Point point = { event.button.x, event.button.y };

            if (SDL_PointInRect(&point, &pix_pane)) {
                Sint32 columns = pix_pane.w / 16;
                Sint32 rows = pix_pane.h / 16;

                Sint32 col = (point.x - pix_pane.x) / (pix_pane.w / columns);
                Sint32 row = (point.y - pix_pane.y) / (pix_pane.h / rows);

                Sint32 d_x = (logical_x + col) / zoom;
                Sint32 d_y = (logical_y + row) / zoom;

                if ((d_x < num_columns) && (d_y < num_rows)) {
                    Sint32 frame_offset = (frame - 1) * (num_rows * num_columns);

                    if (leftclick) {
                        data[frame_offset + ((d_y * num_columns) + d_x)] = curcolor;
                        redraw = true;
                    } else if (rightclick) {
                        curcolor = data[frame_offset + ((d_y * num_columns) + d_x)];
                    }
                }
            } else if (SDL_PointInRect(&point, &pal_pane)) {
                Sint32 columns = 10;
                Sint32 rows = num_pal_rows(columns);

                Sint32 col = (point.x - pal_pane.x) / (pal_pane.w / columns);
                Sint32 row = (point.y - pal_pane.y) / (pal_pane.h / rows);

                if ((col < columns) && (row < rows)) {
                    if (leftclick) {
                        curcolor = (row * columns) + col;
                    } else if (rightclick) {
                        curcolor = 0;
                    }
                }
            }

            if (event.button.button == SDL_BUTTON_LEFT) {
                leftclick = false;
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                rightclick = false;
            }

            break;
        }
        default:
            break;
        }
    }

    SDL_Quit();

    delete[](data);

    return 0;
}
