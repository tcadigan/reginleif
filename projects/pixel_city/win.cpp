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
#include "macro.hpp"
#include "random.hpp"
#include "render.hpp"
#include "texture.hpp"
#include "visible.hpp"
#include "win.hpp"
#include "world.hpp"

#define MOUSE_MOVEMENT 0.5f

// HACK
static int width = 640;
static int height = 480;
static bool quit = false;
SDL_Window *window = nullptr;

void AppUpdate()
{
    camera_update();
    EntityUpdate();
    WorldUpdate();
    TextureUpdate();
    VisibleUpdate();
    CarUpdate();
    RenderUpdate();
}

void appInit(void)
{
    SDL_Init(SDL_INIT_VIDEO);

    unsigned int windowFlags = SDL_WINDOW_OPENGL;

    window = SDL_CreateWindow(APP_TITLE, 0, 0, width, height, windowFlags);

    randomInit(time(NULL));
    camera_init();
    RenderInit();
    TextureInit();
    WorldInit();
}

void appQuit()
{
    quit = true;
}

void winTerm(SDL_Window *window)
{
    SDL_DestroyWindow(window);
}

void appTerm(SDL_Window *window)
{
    TextureTerm();
    WorldTerm();
    RenderTerm();
    camera_term();
    winTerm(window);
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    appInit();

    if (window == nullptr) {
        std::cerr << "Unable to create window" << std::endl;

        return EXIT_FAILURE;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);

    while (!quit) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    appQuit();
                    break;
                default:
                    break;
                }
            } else if (event.type == SDL_QUIT) {
                appQuit();
            }
        }

        glViewport(0, 0, width, height);
        glClearColor(1.0f, 1.0f, 1.0f, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);

    appTerm(window);

    return EXIT_SUCCESS;
}
