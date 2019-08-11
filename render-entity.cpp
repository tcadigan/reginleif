/*
 * render-entity.cpp
 * 2006 Shamus Young
 *
 * This is the core of the gl rndering functions. This contains the main
 * rendering function RenderScene(), which initiates the various other
 * renders in the other modules.
 */

#include "render-entity.hpp"

#include "gl-rgba.hpp"
#include "world.hpp"

render_entity::render_entity(world const &world_object)
    : world_(world_object)
    , render_width_(0)
    , render_height_(0)
    , render_distance_(0)
    , render_aspect_(0)
{
}

render_entity::~render_entity()
{
}

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

void render_entity::init(entity_manager &entity_mgr,
                         camera const &camera_object,
                         sun const &sun_object,
                         ini_manager const &ini_mgr)
{
    camera_ = &camera_object;
    sun_ = &sun_object;
    entity_mgr_ = &entity_mgr;
    ini_mgr_ = &ini_mgr;

    render_distance_ = ini_mgr_->get_int("Render Settings", "render distance");

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
    resize();
}

void render_entity::term()
{
    // if(!hRC) {
    //     return;
    // }

    // wglDeletecontext(hRC);
    // hRC = NULL;
}

void render_entity::resize()
{
    GLint left;
    GLint top;

    if(buffer_ != NULL) {
        delete[] buffer_;
    }

    // render_width = WinWidth();
    // render_height = WinHeight();
    left = 0;
    top = 0;
    render_aspect_ = (GLfloat)render_width_ / (GLfloat)render_height_;
    glViewport(left, top, render_width_, render_height_);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    perspectiveGL(45.0f, render_aspect_, 0.1f, render_distance_);
    glMatrixMode(GL_MODELVIEW);
    // buffer = new unsigned char[(WinWidth() * WinHeight()) * 4];
}

void render_entity::perspectiveGL(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    const GLdouble pi = 3.1415926535897932384626433832795;
    GLdouble fW, fH;

    fH = tan(fovY / 360 * pi) * zNear;
    fW = fH * aspect;

    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

void render_entity::update()
{
    gl_vector3 pos;
    gl_vector3 angle;

    gl_quat light_vector = sun_->get_position_quat();
    gl_rgba fog_color = world_.get_fog_color();
    gl_rgba ambient_color = world_.get_ambient_color();
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
    glFogfv(GL_FOG_COLOR, sun_->get_color().get_data());
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLoadIdentity();
    pos = camera_->get_position();
    angle = camera_->get_angle();
    glRotatef(angle.get_x(), 1.0f, 0.0f, 0.0f);
    glRotatef(angle.get_y(), 0.0f, 1.0f, 0.0f);
    glRotatef(angle.get_z(), 0.0f, 0.0f, 1.0f);
    glTranslatef(-pos.get_z(), -pos.get_y(), -pos.get_z());

    // This was part of a failed experiment. I made a system to allow stuff to
    // fade in over time, like in Grand Theft Auto. This is very effective,
    // but since the scene is drawn twice it is quite hard on framerate.
    if(0) {
        glDrawBuffer(GL_AUX0);
        glFogfv(GL_FOG_COLOR, fog_color.get_data());
        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        entity_mgr_->fade_in();

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
        glPixelTransferf(GL_ALPHA_SCALE, world_.get_fade());
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
        entity_mgr_->render();
    }

    // SwapBuffers(hDC);
}

void render_entity::render()
{
    // no-op
}
