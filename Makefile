NAME = terrain
CXXFLAGS = -Wall `sdl-config --cflags`
LDFLAGS = -lGL -lGLU `sdl-config --libs`
CXX = g++

HDRS = app.hpp camera.hpp console.hpp entity.hpp ini-manager.hpp \
	   terrain-map.hpp cell.hpp terrain-texture.hpp math.hpp mouse-pointer.hpp \
	   render.hpp sky.hpp terrain.hpp texture.hpp win.hpp world.hpp \
	   gl-bbox.hpp gl-vector3.hpp gl-matrix.hpp gl-quat.hpp gl-rgba.hpp \
	   gl-vector2.hpp ztexture.hpp sky-point.hpp point.hpp enums.hpp \
	   camera-fwd.hpp terrain-map-fwd.hpp world-fwd.hpp sky-fwd.hpp \

OBJS = app.o camera.o console.o entity.o gl-bbox.o gl-matrix.o gl-quat.o \
	   gl-rgba.o gl-vector2.o gl-vector3.o ini-manager.o terrain-map.o \
	   terrain-texture.o math.o mouse-pointer.o render.o sky.o terrain.o \
	   texture.o win.o world.o cell.o ztexture.o sky-point.o point.o \

CPPFILES = add.cpp camera.cpp console.cpp entity.cpp gl-bbox.cpp gl-matrix.cpp \
	       gl-quat.cpp gl-rgba.cpp gl-vector2.cpp gl-vector3.cpp \
	       ini-manager.cpp terrain-map.cpp terrain-texture.cpp math.cpp \
	       mouse-pointer.cpp render.cpp sky.cpp terrain.cpp texture.cpp \
	       win.cpp world.cpp cell.cpp ztexture.cpp sky-point.cpp point.cpp \

$(NAME): $(HDRS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS) 

clean:
	rm -f $(OBJS) $(NAME)

cppfiles: $(CPPFILES)
