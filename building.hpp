#ifndef BUILDING_HPP_
#define BUILDING_HPP_

#include "entity.hpp"

#include <SDL_opengl.h>

#include "gl-rgba.hpp"
#include "mesh.hpp"

enum {
    BUILDING_SIMPLE,
    BUILDING_BLOCKY,
    BUILDING_MODERN,
    BUILDING_TOWER
};

class Building: public Entity {
public:
    Building(GLint type, 
             GLint x, 
             GLint y,
             GLint height,
             GLint width,
             GLint depth,
             GLint seed,
             gl_rgba color);
    ~Building();

    void render(void);
    GLint poly_count();
    void render_flat(GLboolean colored);
    GLuint texture();

private:
    GLint x_;
    GLint y_;
    GLint width_;
    GLint depth_;
    GLint height_;
    GLint texture_type_;
    GLint seed_;
    GLint roof_tiers_;
    gl_rgba color_;
    gl_rgba trim_color_;
    Mesh *mesh_;
    Mesh *mesh_flat_;
    GLboolean have_lights_;
    GLboolean have_trim_;
    GLboolean have_logo_;

    void create_simple();
    void create_blocky();
    void create_modern();
    void create_tower();

    GLfloat construct_wall(GLint start_x,
                           GLint start_y,
                           GLint start_z,
                           GLint direction,
                           GLint length,
                           GLint height,
                           GLint window_groups,
                           GLfloat uv_start,
                           GLboolean blank_corners);

    void construct_spike(GLint left,
                         GLint right,
                         GLint front,
                         GLint back,
                         GLint bottom,
                         GLint top);

    void construct_cube(GLint left,
                        GLint right,
                        GLint front,
                        GLint back,
                        GLint bottom,
                        GLint top);

    void construct_cube(GLfloat left,
                        GLfloat right,
                        GLfloat front,
                        GLfloat back,
                        GLfloat bottom,
                        GLfloat top);

    void construct_roof(GLfloat left,
                        GLfloat right,
                        GLfloat front,
                        GLfloat back,
                        GLfloat bottom);
};

#endif /* BUILDING_HPP_ */
