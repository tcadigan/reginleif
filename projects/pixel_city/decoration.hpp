#ifndef __DECORATION_HPP__
#define __DECORATION_HPP__

#include "entity.hpp"

#include <array>
#include <memory>

#include "gl-rgba.hpp"
#include "gl-vector2.hpp"
#include "gl-vector3.hpp"
#include "mesh.hpp"

class Decoration : public Entity {
public:
    Decoration();

    void create_logo(gl_vector2 start,
                     gl_vector2 end,
                     float base,
                     int seed,
                     gl_rgba color);

    void create_light_strip(float x,
                            float z,
                            float width,
                            float depth,
                            float height,
                            gl_rgba color);

    void create_light_trim(std::array<gl_vector3, MAX_VBUFFER> &chain,
                           int count,
                           float height,
                           int seed,
                           gl_rgba color);

    void create_radio_tower(gl_vector3 pos, float height);

    void render() const override;
    void render_flat(bool colored) const override;
    unsigned int texture() const override;
    void update() override;
    bool alpha() const override;
    int poly_count() const override;

private:
    gl_rgba color_;
    std::unique_ptr<Mesh> mesh_;
    unsigned int texture_;
    bool use_alpha_;
};

#endif /* __DECORATION_HPP__ */
