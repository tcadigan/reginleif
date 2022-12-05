#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include "gl-vector3.hpp"

#define MAX_VBUFFER 256

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

void EntityClear();
int EntityCount();
float EntityProgress();
bool EntityReady();
void EntityRender();
void EntityUpdate();
int EntityPolyCount();

#endif /* ENTITY_HPP_ */
