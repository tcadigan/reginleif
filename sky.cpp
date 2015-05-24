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
#include "math.hpp"
#include "world.hpp"

void sky::render()
{
    gl_vector_3d angle;
    gl_vector_3d top;
    gl_vector_3d bottom;
    gl_vector_3d left;
    gl_vector_3d right;
    gl_vector_3d front;
    gl_vector_3d back;
    gl_rgba horizon;
    gl_rgba sky;
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
    angle = camera_angle();
    glRotatef(angle.x_, 1.0f, 0.0f, 0.0f);
    glRotatef(angle.y_, 0.0f, 1.0f, 0.0f);
    glRotatef(angle.z_, 0.0f, 0.0f, 1.0f);
    top = gl_vector_3d(0.0f, 0.8f, 0.0f);
    bottom = gl_vector_3d(0.0f, -3.0f, 0.0f);
    left = gl_vector_3d(10.0f, 0.0f, 0.0f);
    right = gl_vector_3d(-10.0f, 0.0f, 0.0f);
    front = gl_vector_3d(0.0f, 0.0f, 10.0f);
    back = gl_vector_3d(0.0f, 0.0f, -10.0f);
    horizon = world_fog_color();
    sky = gl_rgba(0.6f, 0.7f, 0.9f);
    sky = world_fog_color();
    glClearColor(sky.red_, sky.green_, sky.blue_, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    for(y = 0; y < (sky_grid_ - 1); ++y) {
        glBegin(GL_QUAD_STRIP);
        
        for(x = 0; x < sky_grid_; ++x) {
            glColor3fv(&grid_[x][y].color_.red_);
            glVertex3fv(&grid_[x][y].position_.x_);
            glColor3fv(&grid_[x][y + 1].color_.red_);
            glVertex3fv(&grid_[x][y + 1].position_.x_);
        }

        glEnd();
    }

    glPopMatrix();
    glPopAttrib();
    glDepthMask(true);
}

void sky::update()
{
}

sky::sky()
{
    ini_manager ini_mgr;

    sky_grid_ = ini_mgr.get_int("Sky Settings", "sky_grid");

    grid_ = new sky_point*[sky_grid_];
    grid_[0] = new sky_point[sky_grid_ * sky_grid_];

    for(int i = 1; i < sky_grid_; ++i) {
        grid_[i] = grid_[i - 1] + sky_grid_;
    }

    int x;
    int y;
    gl_rgba top(0.0f, 0.0f, 1.0f);
    gl_rgba edge(0.0f, 1.0f, 1.0f);
    gl_rgba fog;
    float scale;
    float dist;
    float fade;
    
    fog = world_fog_color();

    for(y = 0; y < sky_grid_; ++y) {
        for(x = 0; x < sky_grid_; ++x) {
            dist = math_distance((float)x,
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

            grid_[x][y].position_.x_ = (float)(x - (sky_grid_ / 2));
            grid_[x][y].position_.y_ = 1.0f - (scale * 1.5f);
            grid_[x][y].position_.z_ = (float)(y - (sky_grid_ / 2));
            grid_[x][y].color_ = top;
            fade = math_smooth_step(scale, 0.0f, 0.6f);
            grid_[x][y].color_ =
                gl_rgba_interpolate(grid_[x][y].color_, edge, fade);

            fade = math_smooth_step(scale, 0.5f, 0.99f);
            grid_[x][y].color_ = 
                gl_rgba_interpolate(grid_[x][y].color_, fog, fade);
        }
    }
}

sky::~sky()
{
    delete[] grid_[0];
    delete[] grid_;
}
