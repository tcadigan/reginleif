/*
 * mouse-pointer.cpp
 * 2006 Shamus Young
 *
 * This entity will take mouse positions and attempt to determined what part of
 * the terrain the user is pointing at. This is used as an anchor point when
 * rotating the camera with the left mouse button.
 *
 * This stuff is very slapdash and doesn't work very well. The big flaw is that
 * once it has a few squares in the selection buffer, it needs to sort by depth.
 * Right now if you are close to the ground it may grab real estate on the wrong
 * side of the hill.
 */

#include "mouse-pointer.hpp"
 
#include <SDL.h>
#include <cmath>
#include <cstring>

#include "world.hpp"

mouse_pointer::mouse_pointer(world const &world_object,
                             texture &texture_object)
    : entity()
    , world_(world_object)
    , texture_id_(texture_object.from_name("ring"))
{
    entity_type_ = "pointer";
    last_cell_.set_y(-1);
    last_cell_.set_x(-1);
}

mouse_pointer::~mouse_pointer()
{
}

void mouse_pointer::init(terrain_map const &terrain_map_entity,
                         camera const &camera_object,
                         ini_manager const &ini_mgr)
{
    map_ = &terrain_map_entity;
    camera_ = &camera_object;
    ini_mgr_ = &ini_mgr;

    pt_size_ = ini_mgr_->get_int("Pointer Settings", "pt_size");
}

void mouse_pointer::update()
{
    point p;
    GLint viewport[4];
    gl_vector3 pos;
    gl_vector3 angle;
    unsigned long t;

    t = SDL_GetTicks() % 3600;
    pulse_ = (GLfloat)sin(((GLfloat)t / 10.0f) * (GLfloat)(acos(-1) / 180)) * 1.0f;
    // WinMousePosition(&p.x, &p.y);

    if((last_mouse_.get_x() == p.get_x()) && (last_mouse_.get_y() == p.get_y())) {
        return;
    }

    last_mouse_ = p;

    // This sets the viewport[4] to the size and location
    // of the screen relative to the window
    // glViewport(0, 0, WinWidth(), WinHeight());
    glGetIntegerv(GL_VIEWPORT, viewport);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    // This creates a matrix that will zoom up to a small portion
    // of the screen
    gluPickMatrix((GLdouble)p.get_x(),
                  (GLdouble)(viewport[3] - p.get_y()),
                  2.0f,
                  2.0f,
                  viewport);

    // Apply the perspective matrix
    gluPerspective(45.0f,
                   (GLfloat)(viewport[2] - viewport[0]) / (GLfloat)(viewport[3] - viewport[1]),
                   0.1f,
                   1024.0f);

    // Select the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    pos = camera_->get_position();
    angle = camera_->get_angle();
    glRotatef(angle.get_x(), 1.0f, 0.0f, 0.0f);
    glRotatef(angle.get_y(), 0.0f, 1.0f, 0.0f);
    glRotatef(angle.get_z(), 0.0f, 0.0f, 1.0f);
    glTranslatef(-pos.get_x(), -pos.get_y(), -pos.get_z());
    last_cell_ = draw_grid();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void mouse_pointer::term()
{
}

void mouse_pointer::render()
{
    GLint cell_x;
    GLint cell_y;
    gl_vector3 p;
    gl_vector3 pos = camera_->get_position();

    glPushAttrib(GL_POLYGON_BIT | GL_LIGHTING_BIT | GL_FOG_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_FOG);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_ONE, GL_ONE);
    glLineWidth(3.5f);
    glColor3f(1.0f, 0.5f, 0.0f);
    cell_x = (GLint)(pos.get_x() - 0.5f) + (map_->get_size() / 2);
    cell_y = (GLint)(pos.get_z() - 0.5f) + (map_->get_size() / 2);
    cell_x = last_cell_.get_x();
    cell_y = last_cell_.get_y();

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    p = map_->get_position(cell_x - (pt_size_ / 2), cell_y - (pt_size_ / 2));
    p.set_x(p.get_x() - pulse_);
    p.set_y(p.get_y() + 2.0f);
    p.set_z(p.get_z() - pulse_);
    glVertex3fv(p.get_data());

    glTexCoord2f(0.0f, 1.0f);
    p = map_->get_position(cell_x - (pt_size_ / 2), cell_y + (pt_size_ / 2));
    p.set_x(p.get_x() - pulse_);
    p.set_y(p.get_y() + 2.0f);
    p.set_z(p.get_z() + pulse_);
    glVertex3fv(p.get_data());

    glTexCoord2f(1.0f, 1.0f);
    p = map_->get_position(cell_x + (pt_size_ / 2), cell_y + (pt_size_ / 2));
    p.set_x(p.get_x() + pulse_);
    p.set_y(p.get_y() + 2.0f);
    p.set_z(p.get_z() + pulse_);
    glVertex3fv(p.get_data());

    glTexCoord2f(1.0f, 0.0f);
    p = map_->get_position(cell_x + (pt_size_ / 2), cell_y - (pt_size_ / 2));
    p.set_x(p.get_x() + pulse_);
    p.set_y(p.get_y() + 2.0f);
    p.set_z(p.get_z() - pulse_);
    glVertex3fv(p.get_data());
    glEnd();
    glPopAttrib();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
}

point mouse_pointer::get_selected()
{
    return last_cell_;
}

point mouse_pointer::draw_grid()
{
    GLint x;
    GLint y;
    GLint block;
    GLint hits;
    
    // Set up a selection buffer
    GLuint buffer[512];
    gl_vector3 v1;
    gl_vector3 v2;
    gl_vector3 v3;
    gl_vector3 v4;
    point cell;

    ini_manager ini_mgr;
    GLint pt_size = ini_mgr.get_int("Pointer Settings", "pt_size");

    memset(buffer, 0, sizeof(buffer));

    // Tell OpenGL to use our array for selection
    glSelectBuffer(512, buffer);

    // Put OpenGL in selection mode.
    glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(0);
    block = 0;
    glDisable(GL_CULL_FACE);

    for(y = 0; y < map_->get_size(); y += pt_size) {
        for(x = 0; x < map_->get_size(); x += pt_size) {
            block = x + (y * map_->get_size());
            glLoadName(block);
            v1 = map_->get_position(x, y);
            v2 = map_->get_position(x, y + pt_size);
            v3 = map_->get_position(x + pt_size, y + pt_size);
            v4 = map_->get_position(x + pt_size, y);

            glBegin(GL_QUADS);
            glVertex3fv(v1.get_data());
            glVertex3fv(v2.get_data());
            glVertex3fv(v3.get_data());
            glVertex3fv(v4.get_data());
            glEnd();
        }
    }

    hits = glRenderMode(GL_RENDER);
    cell.set_y(-1);
    cell.set_x(-1);

    if(hits > 0) {
        block = buffer[3];
        cell.set_x((block % map_->get_size()) + (pt_size / 2));
        cell.set_y(((block - cell.get_x()) / map_->get_size()) + (pt_size / 2));
    }

    return cell;
}
