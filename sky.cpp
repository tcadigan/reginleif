/*
 * sky.cpp
 * 2006 Shamus Young
 *
 * Cheapo gradient sky. This is really lame. Seriously.
 */

#include "sky.hpp"

#include <SDL_opengl.h>

#include "math.hpp"
#include "world.hpp"

sky::sky(camera const &camera,
         ini_manager const &ini_mgr)
    : camera_(camera)
    , ini_mgr_(ini_mgr)
{
    sky_grid_ = ini_mgr_.get_int("Sky Settings", "sky_grid");

    grid_ = new sky_point*[sky_grid_];
    grid_[0] = new sky_point[sky_grid_ * sky_grid_];

    for(int i = 1; i < sky_grid_; ++i) {
        grid_[i] = grid_[i - 1] + sky_grid_;
    }
}

sky::~sky()
{
    delete[] grid_[0];
    delete[] grid_;
}

void sky::init(world const &world_object)
{
   int x;
    int y;
    gl_rgba top(0.0f, 0.0f, 1.0f);
    gl_rgba edge(0.0f, 1.0f, 1.0f);
    gl_rgba fog;
    float scale;
    float dist;
    float fade;
    
    fog = world_object.get_fog_color();

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

            grid_[x][y].get_position().set_x((float)(x - (sky_grid_ / 2)));
            grid_[x][y].get_position().set_y(1.0f - (scale * 1.5f));
            grid_[x][y].get_position().set_z((float)(y - (sky_grid_ / 2)));
            grid_[x][y].set_color(top);
            fade = math_smooth_step(scale, 0.0f, 0.6f);
            
            grid_[x][y].set_color(grid_[x][y].get_color().interpolate(edge, fade));

            fade = math_smooth_step(scale, 0.5f, 0.99f);
            grid_[x][y].set_color(grid_[x][y].get_color().interpolate(fog, fade));
        }
    }
}
 
void sky::update()
{
}

void sky::render(world const &world_object)
{
    gl_vector3 angle;
    gl_vector3 top;
    gl_vector3 bottom;
    gl_vector3 left;
    gl_vector3 right;
    gl_vector3 front;
    gl_vector3 back;
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
    angle = camera_.get_angle();
    glRotatef(angle.get_x(), 1.0f, 0.0f, 0.0f);
    glRotatef(angle.get_y(), 0.0f, 1.0f, 0.0f);
    glRotatef(angle.get_z(), 0.0f, 0.0f, 1.0f);
    top = gl_vector3(0.0f, 0.8f, 0.0f);
    bottom = gl_vector3(0.0f, -3.0f, 0.0f);
    left = gl_vector3(10.0f, 0.0f, 0.0f);
    right = gl_vector3(-10.0f, 0.0f, 0.0f);
    front = gl_vector3(0.0f, 0.0f, 10.0f);
    back = gl_vector3(0.0f, 0.0f, -10.0f);
    horizon = world_object.get_fog_color();
    sky = gl_rgba(0.6f, 0.7f, 0.9f);
    sky = world_object.get_fog_color();
    glClearColor(sky.get_red(), sky.get_green(), sky.get_blue(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    for(y = 0; y < (sky_grid_ - 1); ++y) {
        glBegin(GL_QUAD_STRIP);
        
        for(x = 0; x < sky_grid_; ++x) {
            glColor3fv(grid_[x][y].get_color().get_data());
            glVertex3fv(grid_[x][y].get_position().get_data());
            glColor3fv(grid_[x][y + 1].get_color().get_data());
            glVertex3fv(grid_[x][y + 1].get_position().get_data());
        }

        glEnd();
    }

    glPopMatrix();
    glPopAttrib();
    glDepthMask(true);
}

