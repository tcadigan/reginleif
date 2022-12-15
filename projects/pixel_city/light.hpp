#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

#include "gl-rgba.hpp"
#include "gl-vector3.hpp"

class Light {
public:
    Light(gl_vector3 pos, gl_rgba color, int size);
    Light *next_;
    void render() const;
    void blink();

private:
    gl_vector3 position_;
    gl_rgba color_;
    int size_;
    float vert_size_;
    float flat_size_;
    bool blink_;
    unsigned blink_interval_;
    int cell_x_;
    int cell_z_;
};

void light_render();
void light_clear();
int light_count();

#endif /* __LIGHT_HPP__ */
