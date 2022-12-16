/*
 * sky.cpp
 *
 * 2009 Shamus Young
 *
 * Did this need to be written as a class? It did not. There will never be
 * more than one sky in play, so the whole class structure here is superflous,
 * but harmless.
 *
 */

#include "sky.hpp"

#include <cmath>
#include <memory>

#include "camera.hpp"
#include "gl-vertex.hpp"
#include "math.hpp"
#include "random.hpp"
#include "render.hpp"
#include "sky.hpp"
#include "texture.hpp"
#include "world.hpp"

static int constexpr SKYPOINTS = 24;

static std::unique_ptr<Sky> sky;

void sky_render() {
  if (sky && !render_flat()) {
    sky->render();
  }
}

void sky_clear() { sky = nullptr; }

void Sky::render() {
  if (!texture_ready()) {
    return;
  }

  glDepthMask(false);
  glPushAttrib(GL_POLYGON_BIT | GL_FOG_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_CULL_FACE);
  glDisable(GL_FOG);
  glPushMatrix();
  glLoadIdentity();
  gl_vector3 angle = camera_angle();
  gl_vector3 position = camera_position();
  glRotatef(angle.get_x(), 1.0f, 0.0f, 0.0f);
  glRotatef(angle.get_y(), 0.0f, 1.0f, 0.0f);
  glRotatef(angle.get_z(), 0.0f, 0.0f, 1.0f);
  glTranslatef(0.0f, -position.get_y() / 100.0f, 0.0f);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture_id(texture_t::sky));
  glCallList(list_);
  glPopMatrix();
  glPopAttrib();
  glDepthMask(true);
  glEnable(GL_COLOR_MATERIAL);
}

Sky::Sky() {
  std::array<gl_vertex, SKYPOINTS> circle;

  float size = 10.0f;
  for (int i = 0; i < SKYPOINTS; ++i) {
    float angle = static_cast<float>(i) / (SKYPOINTS - 1);
    angle *= 360;
    angle *= DEGREES_TO_RADIANS;
    circle.at(i).get_position().set_x(sinf(angle) * size);
    circle.at(i).get_position().set_y(0.1f);
    circle.at(i).get_position().set_z(cosf(angle) * size);
    circle.at(i).get_uv().set_x((static_cast<float>(i) / (SKYPOINTS - 1)) *
                                5.0f);
    circle.at(i).get_uv().set_y(0.5f);
    float rad =
        (static_cast<float>(i) / (SKYPOINTS - 1)) * 180.f * DEGREES_TO_RADIANS;
    float lum = sinf(rad);
    lum = pow(lum, 5);
    circle.at(i).set_color(gl_rgba(lum * 255, lum * 255, lum * 255));
  }

  list_ = glGenLists(1);
  glNewList(list_, GL_COMPILE);
  glColor3f(1, 1, 1);

  glBegin(GL_QUAD_STRIP);
  for (int i = 0; i < SKYPOINTS; ++i) {
    glTexCoord2f(circle.at(i).get_uv().get_x(), 0.0f);
    glVertex3fv(circle.at(i).get_position().get_data().data());
    gl_vector3 pos = circle.at(i).get_position();
    pos.set_y(size / 3.5f);
    glTexCoord2f(circle.at(i).get_uv().get_x(), 1.0f);
    glVertex3fv(pos.get_data().data());
  }
  glEnd();
  glEndList();

  sky.reset(this);
}
