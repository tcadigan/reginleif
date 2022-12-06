#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include "gl-vector3.hpp"

static int const MAX_VBUFFER = 256;

class Entity {
public:
    Entity();
    virtual ~Entity();
    virtual void render();
    virtual void render_flat(bool wireframe);
    virtual unsigned int texture();
    virtual void update();
    virtual bool alpha();
    virtual int poly_count();
    gl_vector3 center();

protected:
    gl_vector3 center_;
};

void entity_clear();
int entity_count();
float entity_progress();
bool entity_ready();
void entity_render();
void entity_update();
int entity_poly_count();

#endif /* ENTITY_HPP_ */
