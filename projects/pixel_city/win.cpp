/*
 * win.cpp
 *
 * 2006 Shamus Young
 *
 * Create the main windows and make it go.
 *
 */

#include "win.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <ctime>
#include <iostream>

#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>

#include "camera.hpp"
#include "car.hpp"
#include "entity.hpp"
#include "ini.hpp"
#include "random.hpp"
#include "render.hpp"
#include "texture.hpp"
#include "version.hpp"
#include "visible.hpp"
#include "win.hpp"
#include "world.hpp"

// HACK
static int width = 640;
static int height = 480;
static bool quit = false;
SDL_Window *window = nullptr;

void app_update()
{
    camera_update();
    entity_update();
    world_update();
    texture_update();
    visible_update();
    car_update();
    render_update();
}

void app_init()
{
    SDL_Init(SDL_INIT_VIDEO);

    unsigned int window_flags = SDL_WINDOW_OPENGL;

    window = SDL_CreateWindow(APP_TITLE.c_str(), 0, 0, width, height, window_flags);

    load_ini();
    random_init(time(NULL));
    camera_init();
    render_init();
    texture_init();
    world_init();
}

void app_quit()
{
    quit = true;
}

void win_term(SDL_Window *window)
{
    SDL_DestroyWindow(window);
}

void app_term(SDL_Window *window)
{
    texture_term();
    world_term();
    render_term();
    camera_term();
    win_term(window);
    write_ini();
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    app_init();

    if (window == nullptr) {
        std::cerr << "Unable to create window\n";

        return EXIT_FAILURE;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);

    while (!quit) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    app_quit();
                    break;
                default:
                    break;
                }
            } else if (event.type == SDL_QUIT) {
                app_quit();
            }
        }

        glViewport(0, 0, width, height);
        glClearColor(1.0f, 1.0f, 1.0f, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);

    app_term(window);

    return EXIT_SUCCESS;
}
