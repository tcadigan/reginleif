/*
 * sky.cpp
 * 2006 Shamus Young
 *
 * Cheapo gradient sky. This is really lame. Seriously.
 */

#include <math.h>
#include <GL/gl.h>

#include "camera.hpp"
#include "console.hpp"
#include "macro.hpp"
#include "math.hpp"
#include "sky.hpp"
#include "glTypes.hpp"
#include "world.hpp"

void CSky::Render()
{
    GLvector angle;
    GLvector top;
    GLvector bottom;
    GLvector left;
    GLvector right;
    GLvector front;
    GLvector back;
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
    
    for(y = 0; y < (SKY_GRID - 1); ++y) {
        glBegin(GL_QUAD_STRIP);
        
        for(x = 0; x < SKY_GRID; ++x) {
            glColor3fv(&m_grid[x][y].color.red);
            glVertex3fv(&m_grid[x][y].position.x);
            glColor3fv(&m_grid[x][y + 1].color.red);
            glVertex3fv(&m_grid[x][y + 1].position.x);
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
    int x;
    int y;
    GLrgba top = { 0.0f, 0.0f, 1.0f };
    GLrgba edge = { 0.0f, 1.0f, 1.0f };
    GLrgba fog;
    float scale;
    float dist;
    float fade;
    
    fog = WorldFogColor();

    for(y = 0; y < SKY_GRID; ++y) {
        for(x = 0; x < SKY_GRID; ++x) {
            dist = MathDistance((float)x,
                                (float)y,
                                (float)SKY_HALF,
                                (float)SKY_HALF);

            scale = CLAMP((dist / SKY_HALF), 0.0f, 1.0f);
            m_grid[x][y].position.x = (float)(x - SKY_HALF);
            m_grid[x][y].position.y = 1.0f - (scale * 1.5f);
            m_grid[x][y].position.z = (float)(y - SKY_HALF);
            m_grid[x][y].color = top;
            fade = MathSmoothStep(scale, 0.0f, 0.6f);
            m_grid[x][y].color =
                glRgbaInterpolate(m_grid[x][y].color, edge, fade);

            fade = MathSmoothStep(scale, 0.5f, 0.99f);
            m_grid[x][y].color = 
                glRgbaInterpolate(m_grid[x][y].color, fog, fade);
        }
    }
}
