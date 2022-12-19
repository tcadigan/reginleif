PROG = openglad
CXX = clang++
CXXFLAGS = -Wall `sdl2-config --cflags` -std=c++17
LDFLAGS = -lSDL2_mixer `sdl2-config --libs`

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif

SRCS = browser_entry.cpp button.cpp campaign_data.cpp campaign_entry.cpp \
       campaign_result.cpp command.cpp damage_number.cpp \
       editor_object_brush.cpp editor_terrain_brush.cpp effect.cpp \
       game.cpp glad.cpp gloader.cpp gparser.cpp graphlib.cpp guy.cpp \
       help.cpp input.cpp intro.cpp io.cpp joy_data.cpp level_data.cpp \
       level_editor.cpp level_editor_data.cpp level_picker.cpp living.cpp \
       map.cpp menu_nav.cpp mouse_state.cpp object_type.cpp obmap.cpp \
       options.cpp pal32.cpp picker.cpp pixie.cpp pixie_data.cpp \
       pixien.cpp radar.cpp rect.cpp rectf.cpp results_screen.cpp \
       save_data.cpp screen.cpp selection_info.cpp simple_button.cpp \
       smooth.cpp sound.cpp stats.cpp text.cpp treasure.cpp \
       troop_result.cpp util.cpp video.cpp video_screen.cpp view.cpp \
       virtual_button.cpp walker.cpp weap.cpp

OBJS = ${SRCS:.cpp=.o}

all: ${PROG}

${PROG}: depend ${OBJS}
	${CXX} ${CXXFLAGS} ${OBJS} -o ${.TARGET} ${LDFLAGS}

clean:
	rm -rf *~ *.o .depend ${PROG}

depend: ${SRCS}
	${CXX} ${CXXFLAGS} -MM ${SRCS} > .depend

-include .depend

.cpp.o:
	${BEAR} ${CXX} ${CXXFLAGS} -c ${.IMPSRC} -o ${.TARGET}
