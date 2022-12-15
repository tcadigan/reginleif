/*
 * app.cpp
 * 2006 Shamus Young
 *
 * This module contains the core of the program.
 */

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "ini-manager.hpp"
#include "win.hpp"
// #include "camera.hpp"
// #include "console.hpp"
// #include "entity.hpp"
// #include "map.hpp"
// #include "mapTexture.hpp"
// #include "render.hpp"
// #include "texture.hpp"
// #include "win.hpp"
// #include "world.hpp"

SDL_Window *window = nullptr;

void win_term(SDL_Window *window)
{
    SDL_DestroyWindow(window);
}

void app_init()
{
    SDL_Init(SDL_INIT_VIDEO);

    unsigned int window_flags = SDL_WINDOW_OPENGL;

    window = SDL_CreateWindow("Terrain", 0, 0, 640, 480, window_flags);
}

void app_term(SDL_Window *window)
{
    win_term(window);
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

    bool quit = false;
    
    while (!quit) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = true;

                    break;
                default:
                    break;
                }
            } else if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);

    app_term(window);

    return EXIT_SUCCESS;
}
