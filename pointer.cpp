/*
 * pointer.cpp
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

#define PT_SIZE 8
#define PT_HALF (PT_SIZE / 2)

#include "pointer.hpp"
 
#include <SDL.h>
#include <cstring>
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>

#include "camera.hpp"
#include "macro.hpp"
#include "map.hpp"
#include "texture.hpp"

void CPointer::Render()
{
    int cell_x;
    int cell_y;
    GLvector3 p;
    GLvector3 pos = CameraPosition();

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
    cell_x = (int)(pos.x - 0.5f) + (MapSize() / 2);
    cell_y = (int)(pos.z - 0.5f) + (MapSize() / 2);
    cell_x = last_cell_.x;
    cell_y = last_cell_.y;

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    p = MapPosition(cell_x - PT_HALF, cell_y - PT_HALF);
    p.x -= pulse_;
    p.y += 2.0f;
    p.z -= pulse_;
    glVertex3fv(&p.x);

    glTexCoord2f(0.0f, 1.0f);
    p = MapPosition(cell_x - PT_HALF, cell_y + PT_HALF);
    p.x -= pulse_;
    p.y += 2.0f;
    p.z += pulse_;
    glVertex3fv(&p.x);

    glTexCoord2f(1.0f, 1.0f);
    p = MapPosition(cell_x + PT_HALF, cell_y + PT_HALF);
    p.x += pulse_;
    p.y += 2.0f;
    p.z += pulse_;
    glVertex3fv(&p.x);

    glTexCoord2f(1.0f, 0.0f);
    p = MapPosition(cell_x + PT_HALF, cell_y - PT_HALF);
    p.x += pulse_;
    p.y += 2.0f;
    p.z -= pulse_;
    glVertex3fv(&p.x);
    glEnd();
    glPopAttrib();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
}

static point DrawGrid(void)
{
    int x;
    int y;
    int block;
    int hits;
    
    // Set up a selection buffer
    unsigned int buffer[512];
    GLvector3 v1;
    GLvector3 v2;
    GLvector3 v3;
    GLvector3 v4;
    point cell;

    memset(buffer, 0, sizeof(buffer));

    // Tell OpenGL to use our array for selection
    glSelectBuffer(512, buffer);

    // Put OpenGL in selection mode.
    glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(0);
    block = 0;
    glDisable(GL_CULL_FACE);

    for(y = 0; y < MapSize(); y += PT_SIZE) {
        for(x = 0; x < MapSize(); x += PT_SIZE) {
            block = x + (y * MapSize());
            glLoadName(block);
            v1 = MapPosition(x, y);
            v2 = MapPosition(x, y + PT_SIZE);
            v3 = MapPosition(x + PT_SIZE, y + PT_SIZE);
            v4 = MapPosition(x + PT_SIZE, y);

            glBegin(GL_QUADS);
            glVertex3fv(&v1.x);
            glVertex3fv(&v2.x);
            glVertex3fv(&v3.x);
            glVertex3fv(&v4.x);
            glEnd();
        }
    }

    hits = glRenderMode(GL_RENDER);
    cell.y = 01;
    cell.x = cell.y;

    if(hits > 0) {
        block = buffer[3];
        cell.x = (block % MapSize()) + PT_HALF;
        cell.y = ((block - cell.x) / MapSize()) + PT_HALF;
    }

    return cell;
}

void CPointer::Update()
{
    point p;
    int viewport[4];
    GLvector3 pos;
    GLvector3 angle;
    unsigned long t;

    t = SDL_GetTicks() % 3600;
    pulse_ = (float)sin(((float)t / 10.0f) * DEGREES_TO_RADIANS) * 1.0f;
    // WinMousePosition(&p.x, &p.y);

    if((last_mouse_.x == p.x) && (last_mouse_.y == p.y)) {
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
    gluPickMatrix((GLdouble)p.x,
                  (GLdouble)(viewport[3] - p.y),
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
    pos = CameraPosition();
    angle = CameraAngle();
    glRotatef(angle.x, 1.0f, 0.0f, 0.0f);
    glRotatef(angle.y, 0.0f, 1.0f, 0.0f);
    glRotatef(angle.z, 0.0f, 0.0f, 1.0f);
    glTranslatef(-pos.x, -pos.y, -pos.z);
    last_cell_ = DrawGrid();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

CPointer::CPointer()
    : CEntity()
    , texture_(TextureFromName("ring"))
{
    entity_type_ = "pointer";
    last_cell_.y = -1;
    last_cell_.x = last_cell_.y;
}

point CPointer::Selected()
{
    return last_cell_;
}
    
