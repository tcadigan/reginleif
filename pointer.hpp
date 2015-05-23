#ifndef POINTER_HPP_
#define POINTER_HPP_

#include "entity.hpp"
#include "gl-vector3.hpp"

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

    GLvector3 position_;
    point last_mouse_;
    point last_cell_;
    float pulse_;
    int pt_size_;
};

#endif
