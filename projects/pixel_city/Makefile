PROG = pixelcity
CXX = clang++
CXXFLAGS = -Wall `sdl2-config --cflags` --std=c++17
LDFLAGS = -lGL -lGLU `sdl2-config --libs`

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif

SRCS = building.cpp camera.cpp car.cpp decoration.cpp entity.cpp gl-bbox.cpp \
       gl-font.cpp gl-rgba.cpp gl-vector2.cpp gl-vector3.cpp gl-vertex.cpp \
       hsl.cpp ini.cpp light.cpp math.cpp mesh.cpp random.cpp render.cpp \
       sky.cpp texture.cpp visible.cpp win.cpp world.cpp

OBJS = ${SRCS:.cpp=.o}

all: ${PROG}

# Test
${PROG}: depend ${OBJS}
	${CXX} ${CXXFLAGS} ${OBJS} -o ${.TARGET} ${LDFLAGS}

clean:
	rm -rf *~ *.o .depend ${PROG}

depend: ${SRCS}
	rm -rf .depend
	${CXX} ${CXXFLAGS} -MM ${SRCS} > .depend

-include .depend

.cpp.o:
	${BEAR} ${CXX} ${CXXFLAGS} -c ${.IMPSRC} -o ${.TARGET}
