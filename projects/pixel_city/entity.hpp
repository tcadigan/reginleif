#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <memory>

#include "gl-vector3.hpp"

static int const MAX_VBUFFER = 256;

class Entity {
public:
    Entity();

    // Rule of five
    virtual ~Entity() = default;
    Entity(Entity const &other) = delete;
    Entity(Entity  &&other) = delete;
    Entity &operator =(Entity const &other) = delete;
    Entity &operator =(Entity &&other) = delete;

    virtual void render() const;
    virtual void render_flat(bool wireframe) const;
    virtual unsigned int texture() const;
    virtual void update();
    virtual bool alpha() const;
    virtual int poly_count() const;
    gl_vector3 center() const;

protected:
    gl_vector3 center_;
    int poly_count_;
};

class EntityCompare {
public:
    bool operator()(std::shared_ptr<Entity> const &lhs, std::shared_ptr<Entity> const &rhs) const;
};

void entity_clear();
int entity_count();
float entity_progress();
bool entity_ready();
void entity_render();
void entity_update();
int entity_poly_count();

#endif /* ENTITY_HPP_ */
