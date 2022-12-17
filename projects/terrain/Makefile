PROG = terrain
CXX = clang++
CXXFLAGS = -Wall `sdl2-config --cflags` --std=c++17
LDFLAGS = -lGL -lGLU `sdl2-config --libs`

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif

SRCS = app.cpp camera.cpp console.cpp entity-manager.cpp gl-bbox.cpp \
       gl-matrix.cpp gl-quat.cpp gl-rgba.cpp gl-vector2.cpp gl-vector3.cpp \
       ini-manager.cpp terrain-map.cpp terrain-texture.cpp math.cpp \
       mouse-pointer.cpp render-entity.cpp sky.cpp terrain-entity.cpp \
       texture-manager.cpp win.cpp world.cpp cell.cpp ztexture.cpp \
       sky-point.cpp point.cpp sun.cpp texture-item.cpp entity-item.cpp

OBJS = ${SRCS:.cpp=.o}

all: ${PROG}

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
