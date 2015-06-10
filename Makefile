NAME = PixelCity
CXXFLAGS = -Wall `sdl-config --cflags`
LDFLAGS = -lGL -lGLU `sdl-config --libs`

HDRS = building.hpp camera.hpp decoration.hpp entity.hpp ini.hpp light.hpp \
	   macro.hpp math.hpp mesh.hpp random.hpp render.hpp sky.hpp texture.hpp \
	   types.hpp visible.hpp win.hpp world.hpp \

OBJS = boundingBox.o building.o camera.o car.o decoration.o entity.o ini.o \
	   light.o math.o matrix.o mesh.o quaternion.o random.o render.o rgba.o \
	   sky.o texture.o vector2.o vector3.o visible.o win.o world.o \

CPPFILES = buildingBox.cpp build.cpp camera.cpp car.cpp decoration.cpp \
	       entity.cpp ini.cpp light.cpp math.cpp matrix.cpp mesh.cpp \
	       quaternion.cpp random.cpp render.cpp rgba.cpp sky.cpp \
	       texture.cpp vector2.cpp vector3.cpp visible.cpp win.cpp world.cpp \

${NAME}: $(HDRS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS) 

clean:
	rm -rf $(OBJS) $(NAME)

cppfiles: $(CPPFILES)
