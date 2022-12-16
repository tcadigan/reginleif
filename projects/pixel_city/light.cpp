/*
 * light.cpp
 *
 * 2006 Shamus Young
 *
 * This tracks and renders the light sources. (Note that they do not really
 * CAST light in the OpenGL sense of the world, these are just simple panels.)
 * These are NOT subclassed to entities because these are dynamic. Some lights
 * blink, and thus they can't go into the fixed render lists managed by
 * Entity.cpp.
 *
 */

#include "light.hpp"

#include <SDL2/SDL.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <list>

#include "camera.hpp"
#include "entity.hpp"
#include "gl-vector2.hpp"
#include "math.hpp"
#include "random.hpp"
#include "render.hpp"
#include "texture.hpp"
#include "visible.hpp"
#include "win.hpp"

static int constexpr MAX_SIZE = 5;

static std::array<std::array<gl_vector2, MAX_SIZE>, 360> angles;
static std::list<Light> lights;
static bool angles_done;

void light_clear() { lights.clear(); }

int light_count() { return lights.size(); }

void light_render() {
  if (!entity_ready()) {
    return;
  }

  if (!angles_done) {
    for (int size = 0; size < MAX_SIZE; ++size) {
      for (int i = 0; i < 360; ++i) {
        float radians = i * DEGREES_TO_RADIANS;
        angles.at(size).at(i).set_x(cosf(radians) * (size + 0.5f));
        angles.at(size).at(i).set_y(sinf(radians) * (size + 0.5f));
      }
    }
  }

  glDepthMask(false);
  glEnable(GL_BLEND);
  glDisable(GL_CULL_FACE);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D, texture_id(texture_t::light));
  glDisable(GL_CULL_FACE);
  glBegin(GL_QUADS);

  for (Light const &light : lights) {
    light.render();
  }

  glEnd();
  glDepthMask(true);
}

Light::Light(gl_vector3 pos, gl_rgba color, int size) {
  position_ = pos;
  color_ = color;
  size_ = std::clamp(size, 0, (MAX_SIZE - 1));
  vert_size_ = size_ + 0.5f;
  flat_size_ = vert_size_ + 0.5f;
  blink_ = false;
  cell_x_ = pos.get_x() / GRID_RESOLUTION;
  cell_z_ = pos.get_z() / GRID_RESOLUTION;
  lights.push_back(*this);
}

void Light::blink() {
  blink_ = true;

  // We don't want blinkers to by in sync, so have them blink at
  // slightly different rates. (Milliseconds)
  blink_interval_ = 1500 + random_val(500);
}

void Light::render() const {
  if (!visible(cell_x_, cell_z_)) {
    return;
  }

  gl_vector3 camera = camera_angle();
  gl_vector3 camera_pos = camera_position();

  if (fabs(camera_pos.get_x() - position_.get_x()) > render_fog_distance()) {
    return;
  }
  if (fabs(camera_pos.get_x() - position_.get_z()) > render_fog_distance()) {
    return;
  }
  if (blink_ && ((SDL_GetTicks() % blink_interval_) > 200)) {
    return;
  }

  int angle = math_angle(camera.get_y());
  gl_vector2 offset = angles.at(size_).at(angle);
  gl_vector3 pos = position_;

  glColor4fv(color_.get_rgba().data());

  glTexCoord2f(0, 0);
  glVertex3f(pos.get_x() + offset.get_x(), pos.get_y() - vert_size_,
             pos.get_z() + offset.get_y());

  glTexCoord2f(0, 1);
  glVertex3f(pos.get_x() - offset.get_x(), pos.get_y() - vert_size_,
             pos.get_z() - offset.get_y());

  glTexCoord2f(1, 1);
  glVertex3f(pos.get_x() - offset.get_x(), pos.get_y() + vert_size_,
             pos.get_z() - offset.get_y());

  glTexCoord2f(1, 0);
  glVertex3f(pos.get_x() + offset.get_x(), pos.get_y() + vert_size_,
             pos.get_z() + offset.get_y());
}
