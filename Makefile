NAME = PixelCity
CXXFLAGS = -Wall `sdl-config --cflags`
LDFLAGS = -lGL -lGLU `sdl-config --libs`

HDRS = building.hpp camera.hpp decoration.hpp entity.hpp ini.hpp light.hpp \
	   macro.hpp math.hpp mesh.hpp random.hpp render.hpp sky.hpp texture.hpp \
	   types.hpp visible.hpp win.hpp world.hpp gl-bbox.hpp gl-vector3.hpp \
	   gl-vector2.hpp \

OBJS = building.o camera.o car.o decoration.o entity.o gl-bbox.o ini.o \
	   light.o math.o matrix.o mesh.o quaternion.o random.o render.o rgba.o \
	   sky.o texture.o visible.o win.o world.o gl-vector3.o gl-vector2.o \

CPPFILES = buildingBox.cpp build.cpp camera.cpp car.cpp decoration.cpp \
	       entity.cpp ini.cpp light.cpp math.cpp matrix.cpp mesh.cpp \
	       quaternion.cpp random.cpp render.cpp rgba.cpp sky.cpp gl-bbox.cpp \
	       texture.cpp visible.cpp win.cpp world.cpp gl-vector3.cpp \
	       gl-vector2.cpp \

${NAME}: $(HDRS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS) 

clean:
	rm -rf $(OBJS) $(NAME)

cppfiles: $(CPPFILES)
