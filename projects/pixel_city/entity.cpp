/*
 * entity.cpp
 *
 * Copyright (c) 2005 Shamus Young
 * All Rights Reserved
 *
 * An entity is any renderable stationary object in the world. This is an
 * abstract class. This module gathers up the Entities, sorts them by
 * texture use and location, and then stores them in OpenGL render lists
 * for faster rendering.
 *
 */

#include "entity.hpp"

#include <set>

#include <SDL2/SDL.h>
#include <cmath>
#include <cstdlib>

#include "camera.hpp"
#include "cell.hpp"
#include "math.hpp"
#include "render.hpp"
#include "texture.hpp"
#include "visible.hpp"
#include "win.hpp"
#include "world.hpp"

// Changing texture is pretty expensive, and thus sorting the entities
// so that they are grouped by texture used can really improve
// framerate.
auto entity_compare = [](std::shared_ptr<Entity> lhs,
                         std::shared_ptr<Entity> rhs) {
  if (lhs->alpha() && !rhs->alpha()) {
    return 1;
  }

  if (!lhs->alpha() && rhs->alpha()) {
    return -1;
  }

  if (lhs->texture() > rhs->texture()) {
    return 1;
  }

  if (lhs->texture() < rhs->texture()) {
    return -1;
  }

  return 0;
};

static std::array<std::array<Cell, GRID_SIZE>, GRID_SIZE> cell_list;
static std::set<std::shared_ptr<Entity>, decltype(entity_compare)>
    entity_list(entity_compare);
static bool compiled;
static int poly_count;
static int compile_x;
static int compile_y;
static int compile_count;
static int compile_end;

void add(std::shared_ptr<Entity> const &b) {
  entity_list.insert(b);

  poly_count = 0;
}

void do_compile() {
  if (compiled) {
    return;
  }

  int x = compile_x;
  int y = compile_y;

  // Not group entities on the grid
  // Make a list for the textured objects in this region
  if (!cell_list.at(x).at(y).list_textured) {
    cell_list.at(x).at(y).list_textured = glGenLists(1);
  }

  glNewList(cell_list.at(x).at(y).list_textured, GL_COMPILE);
  cell_list.at(x).at(y).pos =
      gl_vector3(x * GRID_RESOLUTION, 0.0f, y * GRID_RESOLUTION);

  for (std::shared_ptr<Entity> const &entity : entity_list) {
    gl_vector3 pos = entity->center();
    if (((pos.get_x() / GRID_RESOLUTION) == x) &&
        ((pos.get_z() / GRID_RESOLUTION) == y) && !entity->alpha()) {
      glBindTexture(GL_TEXTURE_2D, entity->texture());
      entity->render();
    }
  }
  glEndList();

  // Make a list of flat-color stuff (A/C units, ledges, roofs, etc.)
  if (!cell_list.at(x).at(y).list_flat) {
    cell_list.at(x).at(y).list_flat = glGenLists(1);
  }

  glNewList(cell_list.at(x).at(y).list_flat, GL_COMPILE);
  glEnable(GL_CULL_FACE);
  cell_list.at(x).at(y).pos =
      gl_vector3(x * GRID_RESOLUTION, 0.0f, y * GRID_RESOLUTION);

  for (std::shared_ptr<Entity> const &entity : entity_list) {
    gl_vector3 pos = entity->center();
    if (((pos.get_x() / GRID_RESOLUTION) == x) &&
        ((pos.get_z() / GRID_RESOLUTION) == y) && !entity->alpha()) {
      entity->render_flat(false);
    }
  }
  glEndList();

  // Now a list of flat-colored stuff that will be wireframe friendly
  if (!cell_list.at(x).at(y).list_flat_wireframe) {
    cell_list.at(x).at(y).list_flat_wireframe = glGenLists(1);
  }

  glNewList(cell_list.at(x).at(y).list_flat_wireframe, GL_COMPILE);
  glEnable(GL_CULL_FACE);
  cell_list.at(x).at(y).pos =
      gl_vector3(x * GRID_RESOLUTION, 0.0f, y * GRID_RESOLUTION);

  for (std::shared_ptr<Entity> const &entity : entity_list) {
    gl_vector3 pos = entity->center();
    if (((pos.get_x() / GRID_RESOLUTION) == x) &&
        ((pos.get_z() / GRID_RESOLUTION) == y) && !entity->alpha()) {
      entity->render_flat(true);
    }
  }
  glEndList();

  // Now a list of stuff to be alpha-blended, and thus rendered last
  if (!cell_list.at(x).at(y).list_alpha) {
    cell_list.at(x).at(y).list_alpha = glGenLists(1);
  }

  glNewList(cell_list.at(x).at(y).list_alpha, GL_COMPILE);
  cell_list.at(x).at(y).pos =
      gl_vector3(x * GRID_RESOLUTION, 0.0f, y * GRID_RESOLUTION);
  glDepthMask(false);
  glEnable(GL_BLEND);
  glDisable(GL_CULL_FACE);
  for (std::shared_ptr<Entity> const &entity : entity_list) {
    gl_vector3 pos = entity->center();
    if (((pos.get_x() / GRID_RESOLUTION) == x) &&
        ((pos.get_z() / GRID_RESOLUTION) == y) && entity->alpha()) {
      glBindTexture(GL_TEXTURE_2D, entity->texture());
      entity->render();
    }
  }
  glDepthMask(true);
  glEndList();

  // Now walk the grid
  compile_x++;
  if (compile_x == GRID_SIZE) {
    compile_x = 0;
    compile_y++;
    if (compile_y == GRID_SIZE) {
      compiled = true;
    }

    compile_end = SDL_GetTicks();
  }

  compile_count++;
}

bool entity_ready() { return compiled; }

float entity_progress() {
  return static_cast<float>(compile_count) / (GRID_SIZE * GRID_SIZE);
}

void entity_update() {
  unsigned int stop_time;

  if (!texture_ready()) {
    return;
  }

  // We want to do several cells at once. Enough to get things done, but
  // not so many that they program is unresponsive.
  if (LOADING_SCREEN) {
    // If we're using a loading screen, we want to build as
    // fast as possible
    stop_time = SDL_GetTicks() + 100;
    while (!compiled && (SDL_GetTicks() < stop_time)) {
      do_compile();
    }
  } else {
    // Take it slow
    do_compile();
  }
}

void entity_render() {
  std::array<int, 2> polymode;
  bool wireframe;
  int elapsed;

  // Draw all textured objects
  glGetIntegerv(GL_POLYGON_MODE, &polymode.at(0));
  wireframe = (polymode.at(0) != GL_FILL);
  if (render_flat()) {
    glDisable(GL_TEXTURE_2D);
  }

  // If we're not using a loading screen, make the wireframe fade out via fog
  if (!LOADING_SCREEN && wireframe) {
    elapsed = 6000 - world_scene_elapsed();
    if ((elapsed >= 0) && (elapsed <= 6000)) {
      render_fog_fx(elapsed / 6000.0f);
    } else {
      return;
    }
  }

  for (int x = 0; x < GRID_SIZE; ++x) {
    for (int y = 0; y < GRID_SIZE; ++y) {
      if (visible(x, y)) {
        glCallList(cell_list.at(x).at(y).list_textured);
      }
    }
  }

  // Draw all flat colored objects
  glBindTexture(GL_TEXTURE_2D, 0);
  glColor3f(0, 0, 0);
  for (int x = 0; x < GRID_SIZE; ++x) {
    for (int y = 0; y < GRID_SIZE; ++y) {
      if (visible(x, y)) {
        if (wireframe) {
          glCallList(cell_list.at(x).at(y).list_flat_wireframe);
        } else {
          glCallList(cell_list.at(x).at(y).list_flat);
        }
      }
    }
  }

  // Draw all alpha-blended objects
  glBindTexture(GL_TEXTURE_2D, 0);
  glColor3f(0, 0, 0);
  glEnable(GL_BLEND);
  for (int x = 0; x < GRID_SIZE; ++x) {
    for (int y = 0; y < GRID_SIZE; ++y) {
      if (visible(x, y)) {
        glCallList(cell_list.at(x).at(y).list_alpha);
      }
    }
  }
}

void entity_clear() {
  compile_x = 0;
  compile_y = 0;
  compile_count = 0;
  compiled = false;

  for (int x = 0; x < GRID_SIZE; ++x) {
    for (int y = 0; y < GRID_SIZE; ++y) {
      glNewList(cell_list.at(x).at(y).list_textured, GL_COMPILE);
      glEndList();
      glNewList(cell_list.at(x).at(y).list_alpha, GL_COMPILE);
      glEndList();
      glNewList(cell_list.at(x).at(y).list_flat_wireframe, GL_COMPILE);
      glEndList();
      glNewList(cell_list.at(x).at(y).list_flat, GL_COMPILE);
      glEndList();
    }
  }
}

int entity_count() { return entity_list.size(); }

int entity_poly_count() {
  if (poly_count) {
    return poly_count;
  }

  for (std::shared_ptr<Entity> const &entity : entity_list) {
    poly_count += entity->poly_count();
  }

  return poly_count;
}

Entity::Entity() { add(std::shared_ptr<Entity>(this)); }

gl_vector3 Entity::center() const { return center_; }

int Entity::poly_count() const { return 0; }
