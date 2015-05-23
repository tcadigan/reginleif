/*
 * sky.cpp
 * 2006 Shamus Young
 *
 * Cheapo gradient sky. This is really lame. Seriously.
 */

#include "sky.hpp"

#include <SDL_opengl.h>

#include "camera.hpp"
#include "ini.hpp"
#include "macro.hpp"
#include "math.hpp"
#include "world.hpp"

void CSky::Render()
{
    GLvector3 angle;
    GLvector3 top;
    GLvector3 bottom;
    GLvector3 left;
    GLvector3 right;
    GLvector3 front;
    GLvector3 back;
    GLrgba horizon;
    GLrgba sky;
    int x;
    int y;

    glDepthMask(false);
    glPushAttrib(GL_POLYGON_BIT | GL_LIGHTING_BIT | GL_FOG_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glLoadIdentity();
    angle = CameraAngle();
    glRotatef(angle.x, 1.0f, 0.0f, 0.0f);
    glRotatef(angle.y, 0.0f, 1.0f, 0.0f);
    glRotatef(angle.z, 0.0f, 0.0f, 1.0f);
    top = glVector(0.0f, 0.8f, 0.0f);
    bottom = glVector(0.0f, -3.0f, 0.0f);
    left = glVector(10.0f, 0.0f, 0.0f);
    right = glVector(-10.0f, 0.0f, 0.0f);
    front = glVector(0.0f, 0.0f, 10.0f);
    back = glVector(0.0f, 0.0f, -10.0f);
    horizon = WorldFogColor();
    sky = glRgba(0.6f, 0.7f, 0.9f);
    sky = WorldFogColor();
    glClearColor(sky.red, sky.green, sky.blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    for(y = 0; y < (sky_grid_ - 1); ++y) {
        glBegin(GL_QUAD_STRIP);
        
        for(x = 0; x < sky_grid_; ++x) {
            glColor3fv(&grid_[x][y].color.red);
            glVertex3fv(&grid_[x][y].position.x);
            glColor3fv(&grid_[x][y + 1].color.red);
            glVertex3fv(&grid_[x][y + 1].position.x);
        }

        glEnd();
    }

    glPopMatrix();
    glPopAttrib();
    glDepthMask(true);
}

void CSky::Update()
{
}

CSky::CSky()
{
    IniManager ini_mgr;

    sky_grid_ = ini_mgr.get_int("Sky Settings", "sky_grid");

    grid_ = new sky_point*[sky_grid_];
    grid_[0] = new sky_point[sky_grid_ * sky_grid_];

    for(int i = 1; i < sky_grid_; ++i) {
        grid_[i] = grid_[i - 1] + sky_grid_;
    }

    int x;
    int y;
    GLrgba top = { 0.0f, 0.0f, 1.0f };
    GLrgba edge = { 0.0f, 1.0f, 1.0f };
    GLrgba fog;
    float scale;
    float dist;
    float fade;
    
    fog = WorldFogColor();

    for(y = 0; y < sky_grid_; ++y) {
        for(x = 0; x < sky_grid_; ++x) {
            dist = MathDistance((float)x,
                                (float)y,
                                (float)(sky_grid_ / 2),
                                (float)(sky_grid_ / 2));

            if((dist / (sky_grid_ / 2)) < 0.0f) {
                scale = 0.0f;
            }
            else if((dist / (sky_grid_ / 2)) > 1.0f) {
                scale = 1.0f;
            }
            else {
                scale = dist / (sky_grid_ / 2);
            }

            grid_[x][y].position.x = (float)(x - (sky_grid_ / 2));
            grid_[x][y].position.y = 1.0f - (scale * 1.5f);
            grid_[x][y].position.z = (float)(y - (sky_grid_ / 2));
            grid_[x][y].color = top;
            fade = MathSmoothStep(scale, 0.0f, 0.6f);
            grid_[x][y].color =
                glRgbaInterpolate(grid_[x][y].color, edge, fade);

            fade = MathSmoothStep(scale, 0.5f, 0.99f);
            grid_[x][y].color = 
                glRgbaInterpolate(grid_[x][y].color, fog, fade);
        }
    }
}

CSky::~CSky()
{
    delete[] grid_[0];
    delete[] grid_;
}
