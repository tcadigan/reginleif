#ifndef TERRAIN_MAP_HPP_
#define TERRAIN_MAP_HPP_

#include "terrain-map-fwd.hpp"

#include "entity-item.hpp"

#include <SDL_opengl.h>
#include <string>

#include "camera-fwd.hpp"
#include "cell.hpp"
#include "gl-vector3.hpp"
#include "gl-rgba.hpp"
#include "ini-manager.hpp"
#include "sun.hpp"
#include "world-fwd.hpp"

class terrain_map : public entity_item {
public:
    terrain_map(world const &world_object);
    virtual ~terrain_map();

    void init(camera const &camera_object,
              sun const &sun_object,
              ini_manager const &ini_mgr);
    void term();

    void build();

    void save();
    GLboolean load();

    GLint get_size() const;
    GLfloat get_elevation(GLint x, GLint y) const;
    GLfloat get_elevation(GLfloat x, GLfloat y) const;
    GLfloat get_layer(GLint x, GLint y, GLint layer) const;
    GLfloat get_distance(GLint x, GLint y) const;
    gl_rgba get_light(GLint x, GLint y) const;
    gl_vector3 get_position(GLint x, GLint y) const;

    virtual void update();
    virtual void render();

private:
    gl_vector3 do_normal(GLfloat north,
                         GLfloat south,
                         GLfloat east,
                         GLfloat west) const;

    GLushort rgb_sample(GLshort, GLint shift, GLint numbits) const;

    world const &world_;
    camera const *camera_;
    sun const *sun_;
    ini_manager const *ini_mgr_;

    cell **terrain_data_;

    std::string map_image_;
    std::string map_file_;

    // HDC dc;
    // HPEN pen;

    GLint bits_;
    GLint scan_y_;
    GLint zone_grid_;
    GLint map_area_;
    GLint blend_range_;
    GLint map_update_time_;
    GLint far_view_;
    GLint terrain_scale_;
    GLint force_rebuild_;
};

#endif
