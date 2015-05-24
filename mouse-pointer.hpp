#ifndef POINTER_HPP_
#define POINTER_HPP_

#include "entity.hpp"
#include "point.hpp"
#include "gl-vector-3d.hpp"

class mouse_pointer : public entity {
public:
    mouse_pointer();
    virtual ~mouse_pointer();
    
    void render_fade(void);
    void render(void);
    void update(void);
    point selected(void);

private:
    int texture_;

    gl_vector_3d position_;
    point last_mouse_;
    point last_cell_;
    float pulse_;
    int pt_size_;
};

#endif
