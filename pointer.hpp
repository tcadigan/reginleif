#ifdef ENTITY
#include "entity.hpp"
#endif

struct point {
    int x;
    int y;
};

class CPointer : public CEntity {
public:
    CPointer();
    
    void RenderFade(void);
    void Render(void);
    void Update(void);
    point Selected(void);

private:
    GLvector m_position;
    point m_last_mouse;
    point m_last_call;
    int m_texture;
    float m_pulse;
};
