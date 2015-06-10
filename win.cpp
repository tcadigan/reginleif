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

#include "types.hpp"

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

static int half_width;
static int half_height;
static bool lmb;
static bool rmb;
static bool mouse_forced;

static bool quit;

static void CenterCursor()
{
}

static void MoveCursor(int x, int y)
{
}

void WinPopup(char *message, ...)
{
    va_list marker;
    char buf[1024];

    va_start(marker, message);
    vsprintf(buf, message, marker);
    va_end(marker);

    // FIXME
    printf("%s\n", buf);
}

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

static void resize(int newWidth, int newHeight)
{
    width = newWidth;
    height = newHeight;
    
    IniIntSet("WindowWidth", width);
    IniIntSet("WindowHeight", height);
    
    RenderResize();
}

static void keyboard(unsigned char key, int x, int y)
{
    switch(key) {
    case 'R':
    case 'r':
        WorldReset();
        break;
    case 'W':
    case 'w':
        RenderWireframeToggle();
        break;
    case 'E':
    case 'e':
        RenderEffectCycle();
        break;
    case 'L':
    case 'l':
        RenderLetterboxToggle();
        break;
    case 'F':
    case 'f':
        RenderFPSToggle();
        break;
    case 'G':
    case 'g':
        RenderFogToggle();
        break;
    case 'T':
    case 't':
        RenderFlatToggle();
        break;
    case 'C':
    case 'c':
        CameraAutoToggle();
        break;
    case 'B':
    case 'b':
        CameraNextBehavior();
        break;
    default:
        return;
    }

    // glutPostRedisplay();
}

static void keyboard_s(int key, int x, int y)
{
    switch(key) {
    // case GLUT_KEY_F1:
    //     RenderHelpToggle();
    //     break;
    // case GLUT_KEY_F5:
    //     CameraReset();
    //     break;
    // case GLUT_KEY_UP:
    //     CameraMedial(1.0f);
    //     break;
    // case GLUT_KEY_DOWN:
    //     CameraMedial(-1.0f);
    //     break;
    // case GLUT_KEY_LEFT:
    //     CameraLateral(1.0f);
    //     break;
    // case GLUT_KEY_RIGHT:
    //     CameraLateral(-1.0f);
    //     break;
    // case GLUT_KEY_PAGE_UP:
    //     CameraVertical(1.0f);
    //     break;
    // case GLUT_KEY_PAGE_DOWN:
    //     CameraVertical(-1.0f);
    //     break;
    default:
        return;
    }

    // glutPostRedisplay();
}

static void idle()
{
    AppUpdate();
}

static void visible(int vis)
{
    // glutIdleFunc(vis == GLUT_VISIBLE ? idle : NULL);
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
