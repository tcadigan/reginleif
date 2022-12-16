#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "cube.hpp"
#include "fan.hpp"
#include "gl-vertex.hpp"
#include "quad-strip.hpp"

#include <vector>

class Mesh {
public:
  Mesh();
  ~Mesh();

  void vertex_add(gl_vertex const &v);
  int vertex_count();
  int poly_count() const;
  void cube_add(Cube const &c);
  void quad_strip_add(QuadStrip const &qs);
  void fan_add(Fan const &f);
  void render() const;
  void compile();

private:
  unsigned int list_;
  int poly_count_;
  std::vector<gl_vertex> vertex_;
  std::vector<Cube> cube_;
  std::vector<QuadStrip> quad_strip_;
  std::vector<Fan> fan_;
  bool compiled_;
};

#endif /* __MESH_HPP__ */
