#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include "gl-vector3.hpp"

class Entity {
public:
    Entity(void);
    virtual ~Entity();
    virtual void Render(void);
    virtual void RenderFlat(bool wireframe);
    virtual unsigned int Texture();
    virtual void Update(void);
    virtual bool Alpha();
    virtual int PolyCount();
    gl_vector3 Center();

protected:
    gl_vector3 center_;
};

void EntityClear();
int EntityCount(void);
float EntityProgress();
bool EntityReady();
void EntityRender(void);
void EntityUpdate(void);
int EntityPolyCount(void);

#endif /* ENTITY_HPP_ */
