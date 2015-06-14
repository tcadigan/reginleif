/*
 * win.cpp
 * 
 * 2006 Shamus Young
 *
 * Create the main windows and make it go.
 *
 */

#include "win.hpp"

#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>

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

static bool quit;

int WinWidth(void)
{
    return width;
}

void WinMousePosition(int *x, int *y)
{
}

int WinHeight(void)
{
    return height;
}

void WinTerm(void)
{
}

void AppQuit()
{
    quit = true;
}

void AppUpdate()
{
    CameraUpdate();
    EntityUpdate();
    WorldUpdate();
    TextureUpdate();
    VisibleUpdate();
    CarUpdate();
    RenderUpdate();
}

void AppInit(void)
{
    RandomInit(time(NULL));
    CameraInit();
    RenderInit();
    TextureInit();
    WorldInit();
}

void AppTerm(void)
{
    TextureTerm();
    WorldTerm();
    RenderTerm();
    CameraTerm();
    WinTerm();
}

int main(int argc, char *argv[])
{
    // glutInit(&argc, argv);
    // glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    // glutInitDisplayString("double rgba depth >= 16 rgba");
    // glutInitWindowSize(width, height);
    // glutInitWindowPosition(0, 0);
    // glutCreateWindows(APP_TITLE);
    // glutVisibilityFunc(visible);
    // glutReshapeFunc(resize);
    // glutKeyboardFunc(keyboard);
    // glutSpecialFunc(keyboard_s);

    AppInit();
    
    // glutMainLoop();
    
    AppTerm();

    return 0;
}    
