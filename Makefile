NAME = terrain
CXXFLAGS = -Wall `sdl-config --cflags`
LDFLAGS = -lGL -lGLU `sdl-config --libs`
CXX = g++

HDRS = app.hpp camera.hpp console.hpp entity.hpp ini.hpp macro.hpp map.hpp \
	   map-texture.hpp math.hpp pointer.hpp render.hpp sky.hpp terrain.hpp \
	   texture.hpp win.hpp world.hpp gl-bbox.hpp gl-vector3.hpp gl-matrix.hpp \
	   gl-quat.hpp gl-rgba.hpp gl-vector2.hpp \

OBJS = app.o camera.o console.o entity.o gl-bbox.o gl-matrix.o gl-quat.o \
	   gl-rgba.o gl-vector2.o gl-vector3.o ini.o map.o map-texture.o math.o \
       pointer.o render.o sky.o terrain.o texture.o win.o world.o \

CPPFILES = add.cpp camera.cpp console.cpp entity.cpp gl-bbox.cpp gl-matrix.cpp \
	       gl-quat.cpp gl-rgba.cpp gl-vector2.cpp gl-vector3.cpp init.cpp \
	       map.cpp map-texture.cpp math.cpp pointer.cpp render.cpp sky.cpp \
		   terrain.cpp texture.cpp win.cpp world.cpp \

$(NAME): $(HDRS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS) 

clean:
	rm -f $(OBJS) $(NAME)

cppfiles: $(CPPFILES)
