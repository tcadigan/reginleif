NAME = terrain
CXXFLAGS = -Wall `sdl-config --cflags`
LDFLAGS = -lGL -lGLU `sdl-config --libs`
CXX = g++

HDRS = app.hpp camera.hpp console.hpp entity.hpp ini.hpp map.hpp cell.hpp \
	   map-texture.hpp math.hpp mouse-pointer.hpp render.hpp sky.hpp \
	   terrain.hpp texture.hpp win.hpp world.hpp gl-bbox.hpp gl-vector-3d.hpp \
	   gl-matrix.hpp gl-quat.hpp gl-rgba.hpp gl-vector-2d.hpp ztexture.hpp \
	   sky-point.hpp point.hpp \

OBJS = app.o camera.o console.o entity.o gl-bbox.o gl-matrix.o gl-quat.o \
	   gl-rgba.o gl-vector-2d.o gl-vector-3d.o ini.o map.o map-texture.o \
	   math.o mouse-pointer.o render.o sky.o terrain.o texture.o win.o \
	   world.o cell.o ztexture.o sky-point.o point.o \

CPPFILES = add.cpp camera.cpp console.cpp entity.cpp gl-bbox.cpp gl-matrix.cpp \
	       gl-quat.cpp gl-rgba.cpp gl-vector-2d.cpp gl-vector-3d.cpp init.cpp \
	       map.cpp map-texture.cpp math.cpp mouse-pointer.cpp render.cpp \
	       sky.cpp terrain.cpp texture.cpp win.cpp world.cpp cell.cpp \
	       ztexture.cpp sky-point.cpp point.cpp \

$(NAME): $(HDRS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS) 

clean:
	rm -f $(OBJS) $(NAME)

cppfiles: $(CPPFILES)
