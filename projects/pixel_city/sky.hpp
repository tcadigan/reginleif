#ifndef __SKY_HPP__
#define __SKY_HPP__

#include "gl-rgba.hpp"
#include "gl-vector3.hpp"

static int constexpr SKY_GRID = 21;
static int constexpr SKY_HALF = SKY_GRID / 2;

class Sky {
public:
    Sky();
    void render();

private:
    int list_;
    std::array<std::array<std::pair<gl_rgba, gl_vector3>, SKY_GRID>, SKY_GRID> grid_;
};

void sky_render();
void sky_clear();

#endif /* __SKY_HPP__ */
