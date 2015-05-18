#ifndef ENTITY_HPP_
#define ENTITY_HPP_

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
    char const *entity_type_;

private:
    CEntity *next_;
};

void EntityUpdate(void);
void EntityInit(void);
CEntity *EntityFindType(char const *type, CEntity *start);
void EntityTerm(void);
void EntityRender(void);
void EntityRenderFadeIn(void);
void EntityFadeStart(void);

#endif
