NAME = terrain
CXXFLAGS = -Wall `sdl-config --cflags`
LDFLAGS = -lGL -lGLU `sdl-config --libs`
CXX = g++

HDRS = app.hpp camera.hpp console.hpp entity.hpp glTypes.hpp ini.hpp \
	   macro.hpp map.hpp mapTexture.hpp math.hpp pointer.hpp \
	   render.hpp sky.hpp terrain.hpp texture.hpp win.hpp world.hpp \

OBJS = app.o camera.o console.o entity.o glBbox.o glMatrix.o glQuat.o \
	   glRgba.o glVector2.o glVector3.o ini.o map.o mapTexture.o math.o \
       pointer.o render.o sky.o terrain.o texture.o win.o world.o \

CPPFILES = add.cpp camera.cpp console.cpp entity.cpp glBbox.cpp glMatrix.cpp \
	       glQuat.cpp glRgba.cpp glVector2.cpp glVector3.cpp init.cpp \
	       map.cpp mapTexture.cpp math.cpp pointer.cpp render.cpp sky.cpp \
		   terrain.cpp texture.cpp win.cpp world.cpp \

$(NAME): $(HDRS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS) 

clean:
	rm -f $(OBJS) $(NAME)

cppfiles: $(CPPFILES)

# gcc -Wall test.cpp -o test -lGL -lGLU `sdl-config --cflags --libs`