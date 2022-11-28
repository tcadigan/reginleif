NAME = terrain
CXXFLAGS = -Wall `sdl2-config --cflags`
LDFLAGS = -lGL -lGLU -lstdc++ -lm `sdl2-config --libs`

HDRS = app.hpp camera.hpp console.hpp entity-manager.hpp ini-manager.hpp \
       terrain-map.hpp cell.hpp terrain-texture.hpp math.hpp mouse-pointer.hpp \
       render-entity.hpp sky.hpp terrain-entity.hpp texture-manager.hpp \
       win.hpp world.hpp gl-bbox.hpp gl-vector3.hpp gl-matrix.hpp gl-quat.hpp \
       gl-rgba.hpp gl-vector2.hpp ztexture.hpp sky-point.hpp point.hpp \
       enums.hpp camera-fwd.hpp terrain-map-fwd.hpp world-fwd.hpp sky-fwd.hpp \
       sun.hpp texture-item.hpp entity-item.hpp \

OBJS = app.o camera.o console.o entity-manager.o gl-bbox.o gl-matrix.o \
       gl-quat.o gl-rgba.o gl-vector2.o gl-vector3.o ini-manager.o \
       terrain-map.o terrain-texture.o math.o mouse-pointer.o render-entity.o \
       sky.o terrain-entity.o texture-manager.o win.o world.o cell.o \
       ztexture.o sky-point.o point.o sun.o texture-item.o entity-item.o \

CPPFILES = add.cpp camera.cpp console.cpp entity-manager.cpp gl-bbox.cpp \
           gl-matrix.cpp gl-quat.cpp gl-rgba.cpp gl-vector2.cpp gl-vector3.cpp \
           ini-manager.cpp terrain-map.cpp terrain-texture.cpp math.cpp \
           mouse-pointer.cpp render-entity.cpp sky.cpp terrain-entity.cpp \
           texture-manager.cpp win.cpp world.cpp cell.cpp ztexture.cpp \
           sky-point.cpp point.cpp sun.cpp texture-item.cpp entity-item.cpp \

$(NAME): $(HDRS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(NAME)

cppfiles: $(CPPFILES)
