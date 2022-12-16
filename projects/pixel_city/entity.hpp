#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__

#include <memory>

#include "gl-vector3.hpp"

static int constexpr MAX_VBUFFER = 256;

class Entity {
public:
  Entity();

  // Rule of five
  virtual ~Entity() = default;
  Entity(Entity const &other) = delete;
  Entity(Entity &&other) = delete;
  Entity &operator=(Entity const &other) = delete;
  Entity &operator=(Entity &&other) = delete;

  virtual void render() const = 0;
  virtual void render_flat(bool wireframe) const = 0;
  virtual unsigned int texture() const = 0;
  virtual void update() = 0;
  virtual bool alpha() const = 0;
  virtual int poly_count() const = 0;

  gl_vector3 center() const;

protected:
  gl_vector3 center_;
  int poly_count_;
};

void entity_clear();
int entity_count();
float entity_progress();
bool entity_ready();
void entity_render();
void entity_update();
int entity_poly_count();

#endif /* __ENTITY_HPP__ */
