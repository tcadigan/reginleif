NAME = openglad
CXXFLAGS = -Wall `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`

CPPFILES = button.cpp campaign_picker.cpp effect.cpp game.cpp glad.cpp \
           ouya_controller.cpp \

OBJS = button.o campaign_picker.o effect.o game.o glad.o \
       ouya_controller.o \

HDRS = base.hpp button.hpp campaign_picker.hpp colors.hpp effect.hpp \
       gloader-fwd.hpp gloader.hpp gparser.hpp graph.hpp guy-fwd.hpp guy.hpp \
       input.hpp io.hpp level_data.hpp level_picker.hpp living.hpp obmap.hpp \
       options.hpp ouya_controller.hpp pal32.hpp palettes.hpp picker.hpp \
       pixdefs.hpp pixie_data.hpp pixie-fwd.hpp pixie.hpp pixien.hpp radar.hpp \
       results_screen.hpp sai2x.hpp save_data.hpp screen-fwd.hpp screen.hpp \
       smooth.hpp soundob.hpp sounds.hpp stats.hpp text.hpp treasure.hpp \
       util.hpp version.hpp video-fwd.hpp video.hpp view-fwd.hpp view.hpp \
       view_sizes.hpp walker-fwd.hpp walker.hpp weap.hpp \

$(NAME): $(HDRS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS) $(INCLUDE)

clean:
	rm -f $(OBJS) $(NAME) *~
