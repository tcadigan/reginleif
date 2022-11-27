NAME = PixelCity
CXXFLAGS = -Wall `sdl-config --cflags`
LDFLAGS = -lGL -lGLU `sdl-config --libs`

HDRS = building.hpp camera.hpp decoration.hpp entity.hpp ini.hpp light.hpp \
	   macro.hpp math.hpp mesh.hpp random.hpp render.hpp sky.hpp texture.hpp \
	   visible.hpp win.hpp world.hpp gl-bbox.hpp gl-vector3.hpp \
	   gl-vector2.hpp gl-rgba.hpp gl-matrix.hpp gl-vertex.hpp \

OBJS = building.o camera.o car.o decoration.o entity.o gl-bbox.o ini.o \
	   light.o math.o gl-matrix.o mesh.o random.o render.o gl-rgba.o \
	   sky.o texture.o visible.o win.o world.o gl-vector3.o gl-vector2.o \
	   gl-vertex.o \

CPPFILES = buildingBox.cpp build.cpp camera.cpp car.cpp decoration.cpp \
	       entity.cpp ini.cpp light.cpp math.cpp gl-matrix.cpp mesh.cpp \
	       random.cpp render.cpp gl-rgba.cpp sky.cpp gl-bbox.cpp \
	       texture.cpp visible.cpp win.cpp world.cpp gl-vector3.cpp \
	       gl-vector2.cpp gl-vertex.cpp \

${NAME}: $(HDRS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

clean:
	rm -rf $(OBJS) $(NAME)

cppfiles: $(CPPFILES)
