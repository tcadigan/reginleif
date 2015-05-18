/*
 * render.cpp
 * 2006 Shamus Young
 *
 * This is the core of the gl rndering functions. This contains the main
 * rendering function RenderScene(), which initiates the various other
 * renders in the other modules.
 */

#define RENDER_DISTANCE 2048

#include "render.hpp"

#include <GL/gl.h>
#include <GL/glu.h>

#include "gl-rgba.hpp"
#include "camera.hpp"
#include "entity.hpp"
#include "world.hpp"

// static PIXELFORMATDESCRIPTOR pfd = {
//     sizeof(PIXELFORMATDESCRIPTOR),
//     1,                   // Version Number
//     PFD_DRAW_TO_WINDOW | // Format must support window
//     PFD_SUPPORT_OPENGL | // Format must support OpenGL
//     PFD_DOUBLE_BUFFER,   // Must support double buffering
//     PFD_TYPE_RGBA,       // Request an RGBA format
//     32,                  // Select our glRgbaDepth
//     0, 0, 0, 0, 0, 0,    // glRgbabits ignored
//     0,                   // No alpha buffer
//     0,                   // Shift bit ignored
//     0,                   // Accumulation buffer
//     0, 0, 0, 0,          // Accumulation bits ignored
//     16,                  // Z-buffer (depth buffer) bits
//     0,                   // Stencil Buffers
//     1,                   // Auxilliary buffers
//     PFD_MAIN_PLANE,      // Main drawing layer
//     0,                   // Reserved
//     0, 0, 0              // Layer masks ignored
// };

// static HDC hDc;
// static HGLRC hRC;
static int render_width;
static int render_height;
static float render_aspect;
static unsigned char *buffer;

void RenderResize(void)
{
    int left;
    int top;

    if(buffer != NULL) {
        delete[] buffer;
    }

    // render_width = WinWidth();
    // render_height = WinHeight();
    left = 0;
    top = 0;
    render_aspect = (float)render_width / (float)render_height;
    glViewport(left, top, render_width, render_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, render_aspect, 0.1f, RENDER_DISTANCE);
    glMatrixMode(GL_MODELVIEW);
    // buffer = new unsigned char[(WinWidth() * WinHeight()) * 4];
}

void RenderTerm(void)
{
    // if(!hRC) {
    //     return;
    // }

    // wglDeletecontext(hRC);
    // hRC = NULL;
}

void RenderInit(void)
{
    // HWND hWnd;
    // unsigned int PixelFormat;

    // hWnd = WinHwnd();
    // hDC = GetDC(hWnd);

    // if(!hDc) {
    //     // Did we get a device context?
    //     WinPopup("Can't create a GL device context.");
        
    //     return;
    // }

    // PixelFormat = ChoosePixelFormat(hDC, &pfd);

    // if(!PixelFormat) {
    //     // Did Windows find a matching pixel format?
    //     WinPopup("Can't find a suitable PixelFormat.");

    //     return;
    // }

    // if(!SetPixelFormat(hDC, PixelFormat, &pfd)) {
    //     // Are we able to set the pixel format?
    //     WinPopup("Can't set the PixelForma");

    //     return;
    // }

    // if(!wglMakeCurrent(hDC, hRC)) {
    //     // Try to activate the rendering context
    //     WinPopup("Can't activate the GL rendering context.");

    //     return;
    // }

    // glViewport(0, 0, WinWidth(), WinHeight());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // SwapBuffers(hDC);
    RenderResize();
}

void RenderUpdate(void)
{
    GLquat light_vector;
    GLrgba light_color;
    GLrgba ambient_color;
    GLrgba fog_color;
    GLvector3 pos;
    GLvector3 angle;

    light_vector = WorldLightQuat();
    light_color = WorldLightColor();
    fog_color = WorldFogColor();
    ambient_color = WorldAmbientColor();
    // glViewport(0, 0, WinWidth(), WinHeight());
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);

    // Enable this for fog
    // glEnable(GL_FOG);

    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 484.0f);
    glFogf(GL_FOG_END, 5880.0f);
    glFogfv(GL_FOG_COLOR, &light_color.red);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLoadIdentity();
    pos = CameraPosition();
    angle = CameraAngle();
    glRotatef(angle.x, 1.0f, 0.0f, 0.0f);
    glRotatef(angle.y, 0.0f, 1.0f, 0.0f);
    glRotatef(angle.x, 0.0f, 0.0f, 1.0f);
    glTranslatef(-pos.x, -pos.y, -pos.z);

    // This was part of a failed experiment. I made a system to allow stuff to
    // fade in over time, like in Grand Theft Auto. This is very effective,
    // but since the scene is drawn twice it is quite hard on framerate.
    if(0) {
        glDrawBuffer(GL_AUX0);
        glFogfv(GL_FOG_COLOR, &fog_color.red);
        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        EntityRenderFadeIn();

        glEnable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
        glReadBuffer(GL_AUX0);
        glDrawBuffer(GL_AUX0);
        // glReadPixels(0, 
        //              0,
        //              WinWidth(), 
        //              WinHeight(),
        //              GL_RGBA,
        //              GL_UNSIGNED_BYTE,
        //              buffer);

        glReadBuffer(GL_BACK);
        glDrawBuffer(GL_BACK);
        glColor4f(1.0f, 1.0f, 1.0f, 0.1f);
        glPixelTransferf(GL_ALPHA_SCALE, WorldFade());
        glDisable(GL_FOG);
        // glDrawPixels(WinWidth(), 
        //              WinHeight(), 
        //              GL_RGBA,
        //              GL_UNSIGNED_BYTE,
        //              buffer);

        glReadBuffer(GL_BACK);
        glDrawBuffer(GL_BACK);
        glPixelTransferf(GL_ALPHA_SCALE, 1.0f);
    }
    else {
        // This will just render everything once, with no fancy fade
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        EntityRender();
    }

    // SwapBuffers(hDC);
}
               
