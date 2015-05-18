#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include "glTypes.hpp"

class CEntity {
public:
    CEntity(void);
    virtual ~CEntity(void);

    CEntity *Next(void);
    virtual void Render(void);
    virtual void RenderFadeIn(void);
    virtual void FadeStart(void);
    virtual void Update(void);
    virtual char const *Type(void);

protected:
    char const *m_entity_type;

private:
    CEntity *next;
};

void EntityUpdate(void);
void EntityInit(void);
CEntity *EntityFindType(char const *type, CEntity *start);
void EntityTerm(void);
void EntityRender(void);
void EntityRenderFadeIn(void);
void EntityFadeStart(void);

#endif
