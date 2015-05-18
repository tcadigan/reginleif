#ifndef POINTER_HPP_
#define POINTER_HPP_

#include "glTypes.hpp"
#include "entity.hpp"

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
    int texture_;

    GLvector position_;
    point last_mouse_;
    point last_cell_;
    float pulse_;
};

#endif
