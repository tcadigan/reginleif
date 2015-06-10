#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include "types.hpp"

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
    GLvector Center();

protected:
    GLvector center_;
};

void EntityClear();
int EntityCount(void);
float EntityProgress();
bool EntityReady();
void EntityRender(void);
void EntityUpdate(void);
int EntityPolyCount(void);

#endif /* ENTITY_HPP_ */
