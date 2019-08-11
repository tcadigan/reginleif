/*
 * sky.cpp
 * 2006 Shamus Young
 *
 * Cheapo gradient sky. This is really lame. Seriously.
 */

#include "sky.hpp"

#include <SDL2/SDL_opengl.h>

#include "math.hpp"
#include "world.hpp"

sky::sky(world const &world_object)
    : world_(world_object)
{
}

sky::~sky()
{
    term();
}

void sky::init(camera const &camera_object,
               ini_manager const &ini_mgr)
{
    camera_ = &camera_object;
    ini_mgr_ = &ini_mgr;

    gl_rgba top(0.0f, 0.0f, 1.0f);
    gl_rgba edge(0.0f, 1.0f, 1.0f);
    gl_rgba fog;
    GLfloat scale;
    GLfloat dist;
    GLfloat fade;

    sky_grid_ = ini_mgr_->get_int("Sky Settings", "sky_grid");
    
    fog = world_.get_fog_color();

    grid_.reserve(sky_grid_ * sky_grid_);
    
    for(GLint col = 0; col < sky_grid_; ++col) {
        for(GLint row = 0; row < sky_grid_; ++row) {
            dist = math_distance((GLfloat)row,
                                 (GLfloat)col,
                                 (GLfloat)(sky_grid_ / 2),
                                 (GLfloat)(sky_grid_ / 2));
            
            if((dist / (sky_grid_ / 2)) < 0.0f) {
                scale = 0.0f;
            }
            else if((dist / (sky_grid_ / 2)) > 1.0f) {
                scale = 1.0f;
            }
            else {
                scale = dist / (sky_grid_ / 2);
            }

            sky_point *s_point = new sky_point();

            s_point->get_position().set_x((GLfloat)(row - (sky_grid_ / 2)));
            s_point->get_position().set_y(1.0f - (scale * 1.5f));
            s_point->get_position().set_z((GLfloat)(col - (sky_grid_ / 2)));
            s_point->set_color(top);
            fade = math_smooth_step(scale, 0.0f, 0.6f);
            
            s_point->set_color(s_point->get_color().interpolate(edge, fade));
            
            fade = math_smooth_step(scale, 0.5f, 0.99f);
            s_point->set_color(s_point->get_color().interpolate(fog, fade));

            grid_[(row * sky_grid_) + col] = s_point;
        }
    }
}

void sky::update()
{
}

void sky::term()
{
    std::vector<sky_point *>::iterator itr;

    for(itr = grid_.begin(); itr != grid_.begin(); ++itr) {
        delete *itr;
    }
}

void sky::render()
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

    glDepthMask(false);
    glPushAttrib(GL_POLYGON_BIT | GL_LIGHTING_BIT | GL_FOG_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glLoadIdentity();
    angle = camera_->get_angle();
    glRotatef(angle.get_x(), 1.0f, 0.0f, 0.0f);
    glRotatef(angle.get_y(), 0.0f, 1.0f, 0.0f);
    glRotatef(angle.get_z(), 0.0f, 0.0f, 1.0f);
    top = gl_vector3(0.0f, 0.8f, 0.0f);
    bottom = gl_vector3(0.0f, -3.0f, 0.0f);
    left = gl_vector3(10.0f, 0.0f, 0.0f);
    right = gl_vector3(-10.0f, 0.0f, 0.0f);
    front = gl_vector3(0.0f, 0.0f, 10.0f);
    back = gl_vector3(0.0f, 0.0f, -10.0f);
    horizon = world_.get_fog_color();
    sky = gl_rgba(0.6f, 0.7f, 0.9f);
    sky = world_.get_fog_color();
    glClearColor(sky.get_red(), sky.get_green(), sky.get_blue(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    for(GLint col = 0; col < (sky_grid_ - 1); ++col) {
        glBegin(GL_QUAD_STRIP);
        
        for(GLint row = 0; row < sky_grid_; ++row) {
            sky_point *s_point = grid_[(row * sky_grid_) + col];

            glColor3fv(s_point->get_color().get_data());
            glVertex3fv(s_point->get_position().get_data());

            sky_point *next_s_point = grid_[(row * sky_grid_) + (col + 1)];
            glColor3fv(next_s_point->get_color().get_data());
            glVertex3fv(next_s_point->get_position().get_data());
        }

        glEnd();
    }

    glPopMatrix();
    glPopAttrib();
    glDepthMask(true);
}

