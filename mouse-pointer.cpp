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
#include <SDL_opengl.h>
#include <cmath>
#include <cstring>

#include "camera.hpp"
#include "ini.hpp"
#include "map.hpp"
#include "texture.hpp"

void mouse_pointer::render()
{
    int cell_x;
    int cell_y;
    gl_vector_3d p;
    gl_vector_3d pos = camera_position();

    glPushAttrib(GL_POLYGON_BIT | GL_LIGHTING_BIT | GL_FOG_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_FOG);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_ONE, GL_ONE);
    glLineWidth(3.5f);
    glColor3f(1.0f, 0.5f, 0.0f);
    cell_x = (int)(pos.x_ - 0.5f) + (map_size() / 2);
    cell_y = (int)(pos.z_ - 0.5f) + (map_size() / 2);
    cell_x = last_cell_.x_;
    cell_y = last_cell_.y_;

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    p = map_position(cell_x - (pt_size_ / 2), cell_y - (pt_size_ / 2));
    p.x_ -= pulse_;
    p.y_ += 2.0f;
    p.z_ -= pulse_;
    glVertex3fv(&p.x_);

    glTexCoord2f(0.0f, 1.0f);
    p = map_position(cell_x - (pt_size_ / 2), cell_y + (pt_size_ / 2));
    p.x_ -= pulse_;
    p.y_ += 2.0f;
    p.z_ += pulse_;
    glVertex3fv(&p.x_);

    glTexCoord2f(1.0f, 1.0f);
    p = map_position(cell_x + (pt_size_ / 2), cell_y + (pt_size_ / 2));
    p.x_ += pulse_;
    p.y_ += 2.0f;
    p.z_ += pulse_;
    glVertex3fv(&p.x_);

    glTexCoord2f(1.0f, 0.0f);
    p = map_position(cell_x + (pt_size_ / 2), cell_y - (pt_size_ / 2));
    p.x_ += pulse_;
    p.y_ += 2.0f;
    p.z_ -= pulse_;
    glVertex3fv(&p.x_);
    glEnd();
    glPopAttrib();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
}

static point draw_grid(void)
{
    int x;
    int y;
    int block;
    int hits;
    
    // Set up a selection buffer
    unsigned int buffer[512];
    gl_vector_3d v1;
    gl_vector_3d v2;
    gl_vector_3d v3;
    gl_vector_3d v4;
    point cell;

    ini_manager ini_mgr;
    int pt_size = ini_mgr.get_int("Pointer Settings", "pt_size");

    memset(buffer, 0, sizeof(buffer));

    // Tell OpenGL to use our array for selection
    glSelectBuffer(512, buffer);

    // Put OpenGL in selection mode.
    glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(0);
    block = 0;
    glDisable(GL_CULL_FACE);

    for(y = 0; y < map_size(); y += pt_size) {
        for(x = 0; x < map_size(); x += pt_size) {
            block = x + (y * map_size());
            glLoadName(block);
            v1 = map_position(x, y);
            v2 = map_position(x, y + pt_size);
            v3 = map_position(x + pt_size, y + pt_size);
            v4 = map_position(x + pt_size, y);

            glBegin(GL_QUADS);
            glVertex3fv(&v1.x_);
            glVertex3fv(&v2.x_);
            glVertex3fv(&v3.x_);
            glVertex3fv(&v4.x_);
            glEnd();
        }
    }

    hits = glRenderMode(GL_RENDER);
    cell.y_ = 01;
    cell.x_ = cell.y_;

    if(hits > 0) {
        block = buffer[3];
        cell.x_ = (block % map_size()) + (pt_size / 2);
        cell.y_ = ((block - cell.x_) / map_size()) + (pt_size / 2);
    }

    return cell;
}

void mouse_pointer::update()
{
    point p;
    int viewport[4];
    gl_vector_3d pos;
    gl_vector_3d angle;
    unsigned long t;

    t = SDL_GetTicks() % 3600;
    pulse_ = (float)sin(((float)t / 10.0f) * (float)(acos(-1) / 180)) * 1.0f;
    // WinMousePosition(&p.x, &p.y);

    if((last_mouse_.x_ == p.x_) && (last_mouse_.y_ == p.y_)) {
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
    gluPickMatrix((GLdouble)p.x_,
                  (GLdouble)(viewport[3] - p.y_),
                  2.0f,
                  2.0f,
                  viewport);

    // Apply the perspective matrix
    gluPerspective(45.0f,
                   (float)(viewport[2] - viewport[0]) / (float)(viewport[3] - viewport[1]),
                   0.1f,
                   1024.0f);

    // Select the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    pos = camera_position();
    angle = camera_angle();
    glRotatef(angle.x_, 1.0f, 0.0f, 0.0f);
    glRotatef(angle.y_, 0.0f, 1.0f, 0.0f);
    glRotatef(angle.z_, 0.0f, 0.0f, 1.0f);
    glTranslatef(-pos.x_, -pos.y_, -pos.z_);
    last_cell_ = draw_grid();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

mouse_pointer::mouse_pointer()
    : entity()
    , texture_(texture_from_name("ring"))
{
    ini_manager ini_mgr;
    pt_size_ = ini_mgr.get_int("Pointer Settings", "pt_size");

    entity_type_ = "pointer";
    last_cell_.y_ = -1;
    last_cell_.x_ = last_cell_.y_;
}

point mouse_pointer::selected()
{
    return last_cell_;
}
    
mouse_pointer::~mouse_pointer()
{
}
