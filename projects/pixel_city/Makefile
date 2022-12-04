PROG = pixelcity
CXX = clang++
CXXFLAGS = -Wall `sdl2-config --cflags`
LDFLAGS = -lGL -lGLU `sdl2-config --libs`

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif

SRCS = building.cpp camera.cpp car.cpp decoration.cpp entity.cpp ini.cpp \
       light.cpp math.cpp gl-matrix.cpp mesh.cpp random.cpp render.cpp \
       gl-rgba.cpp sky.cpp gl-bbox.cpp texture.cpp visible.cpp win.cpp \
       world.cpp gl-vector3.cpp gl-vector2.cpp gl-vertex.cpp

OBJS = ${SRCS:.cpp=.o}

all: ${PROG}

${PROG}: depend ${OBJS}
	${CXX} ${CXXFLAGS} ${OBJS} -o ${.TARGET} ${LDFLAGS}

clean:
	rm -rf *~ *.o .depend ${PROG}

depend: ${SRCS}
	rm -rf .depend
	${CXX} ${CFLAGS} -MM ${SRCS} > .depend

-include .depend

.cpp.o:
	${BEAR} ${CXX} ${CFLAGS} -c ${.IMPSRC} -o ${.TARGET}
