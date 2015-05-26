#ifndef TERRAIN_HPP_
#define TERRAIN_HPP_

#include "camera.hpp"
#include "entity.hpp"
#include "gl-vector3.hpp"
#include "gl-vector2.hpp"
#include "ini-manager.hpp"
#include "terrain-map.hpp"
#include "terrain-texture.hpp"

class terrain : public entity {
public:
    terrain(world const &world_object);
    ~terrain();

    void init(terrain_texture &terrain_texture,
              terrain_map const &map,
              camera const &camera,
              ini_manager const &ini_mgr);

    void update();
    void term();

    void render();
    void render_fade_in();
    void fade_start();
    
private:

    world const &world_;
    GLshort stage_;
    gl_vector3 viewpoint_;

    terrain_texture *terrain_texture_;
    terrain_map const *map_;
    camera const *camera_;
    ini_manager const *ini_mgr_;

    GLint map_size_;
    GLint map_half_;
    GLint zone_size_;
    GLshort *boundary_;
    GLboolean *point_;

    GLfloat tolerance_;
    GLint update_time_;
    GLint do_wireframe_;
    GLint do_solid_;
    GLint zone_grid_;

    GLuint list_front_;
    GLuint list_back_;

    gl_vector2 *zone_uv_;
    GLint x_;
    GLint y_;
    GLint layer_;
    GLint zone_;
    GLint zone_origin_x_;
    GLint zone_origin_y_;
    GLboolean compile_back_;
    GLboolean use_color_;
    GLboolean fade_;
    GLuint triangles_;
    GLuint vertices_;
    GLuint build_start_;
    GLuint build_time_;
    GLuint compile_time_;

    void compile(void);
    void compile_block(GLint x, GLint y, GLint size);
    void compile_triangle(GLint x1, GLint y1, GLint x2, GLint y2, GLint x3, GLint y3);
    void compile_vertex(GLint x, GLint y);
    void compile_strip(GLint x1,
                       GLint y1,
                       GLint x2,
                       GLint y2,
                       GLint x3,
                       GLint y3,
                       GLint x4,
                       GLint y4);

    void compile_fan(GLint x1,
                     GLint y1,
                     GLint x2,
                     GLint y2,
                     GLint x3,
                     GLint y3,
                     GLint x4,
                     GLint y4,
                     GLint x5,
                     GLint y5);

    void compile_fan(GLint x1,
                     GLint y1,
                     GLint x2,
                     GLint y2,
                     GLint x3,
                     GLint y3,
                     GLint x4,
                     GLint y4,
                     GLint x5,
                     GLint y5,
                     GLint x6,
                     GLint y6);

    void grid_step(void);
    void do_quad(GLint x, GLint y, GLint size);
    void point_activate(GLint x, GLint y);
};
    
#endif
