#ifndef __BUILDING_HPP__
#define __BUILDING_HPP__

#include "entity.hpp"

#include <SDL2/SDL_opengl.h>
#include <memory>

#include "gl-rgba.hpp"
#include "mesh.hpp"
#include "world.hpp"

enum class building_t { simple, blocky, modern, tower };

class Building : public Entity {
public:
  Building(building_t type, int x, int y, int height, int width, int depth,
           int seed, gl_rgba color);
  ~Building();

  void render() const override;
  void render_flat(bool wireframe) const override;
  unsigned int texture() const override;
  void update() override;
  bool alpha() const override;
  int poly_count() const override;

private:
  int x_;
  int y_;
  int width_;
  int depth_;
  int height_;
  int texture_type_;
  int seed_;
  int roof_tiers_;
  gl_rgba color_;
  gl_rgba trim_color_;

  std::unique_ptr<Mesh> mesh_;
  std::unique_ptr<Mesh> mesh_flat_;
  bool have_lights_;
  bool have_trim_;
  bool have_logo_;

  void create_simple();
  void create_blocky();
  void create_modern();
  void create_tower();

  float construct_wall(int start_x, int start_y, int start_z,
                       direction_t direction, int length, int height,
                       int window_groups, float uv_start, bool blank_corners);

  void construct_spike(int left, int right, int front, int back, int bottom,
                       int top);

  void construct_cube(int left, int right, int front, int back, int bottom,
                      int top);

  void construct_cube_float(float left, float right, float front, float back,
                            float bottom, float top);

  void construct_roof(float left, float right, float front, float back,
                      float bottom);
};

#endif /* __BUILDING_HPP__ */
