#ifdnef TYPES
#include "glTypes.hpp"
#endif

#ifndef ENTITY
#define ENTITY

class CEntity {
public:
    CEntity(void);
    class CEntity *Next(void);
    virtual void Render(void);
    virtual void RenderFadeIn(void);
    virtual void FadeStart(void);
    virtual void Update(void);
    virtual char *Type(void);

protected:
    char *m_entity_type;

private:
    CEntity *next;
};

void EntityUpdate(void);
void EntityInit(void);
CEntity *EntityFindTryp(char *type, CEntity *start);
void EntityTerm(void);
void EntityRender(void);
void EntityRenderFadeIn(void);
void EntityFadeStart(void);

#endif
