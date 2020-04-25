NAME = openglad
CXXFLAGS = -Wall -std=c++17 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL_mixer

CPPFILES = browser_entry.cpp button.cpp campaign_entry.cpp campaign_picker.cpp \
           command.cpp damage_number.cpp editor_object_brush.cpp \
           editor_terrain_brush.cpp effect.cpp game.cpp glad.cpp gloader.cpp \
           gparser.cpp graphlib.cpp guy.cpp help.cpp input.cpp intro.cpp \
           io.cpp level_data.cpp level_editor.cpp level_editor_data.cpp \
           level_picker.cpp living.cpp map.cpp object_type.cpp obmap.cpp \
           options.cpp pal32.cpp picker.cpp pixie.cpp pixie_data.cpp \
           pixien.cpp radar.cpp rect.cpp rectf.cpp results_screen.cpp \
           sai2x.cpp save_data.cpp screen.cpp selection_info.cpp \
           simple_button.cpp smooth.cpp sound.cpp stats.cpp text.cpp \
           treasure.cpp troop_result.cpp util.cpp video.cpp view.cpp \
           walker.cpp weap.cpp \

OBJS = browser_entry.o button.o campaign_entry.o campaign_picker.o command.o \
       damage_number.o editor_object_brush.o editor_terrain_brush.o effect.o \
       game.o glad.o gloader.o gparser.o graphlib.o guy.o help.o input.o \
       intro.o io.o level_data.o level_editor.o level_editor_data.o \
       level_picker.o living.o map.o object_type.o obmap.o options.o \
       pal32.o picker.o pixie.o pixie_data.o pixien.o radar.o rect.o rectf.o \
       results_screen.o sai2x.o save_data.o screen.o selection_info.o \
       simple_button.o smooth.o sound.o stats.o text.o treasure.o \
       troop_result.o util.o video.o view.o walker.o weap.o \

HDRS = base.hpp browser_entry.hpp button.hpp button-fwd.hpp campaign_entry.hpp \
       campaign_picker.hpp colors.hpp command.hpp damage_number.hpp \
       editor_object_brush.hpp editor_terrain_brush.hpp effect.hpp glad.hpp \
       gloader-fwd.hpp gloader.hpp gparser.hpp graph.hpp graphlib.hpp \
       guy-fwd.hpp guy.hpp help.hpp input.hpp intro.hpp io.hpp level_data.hpp \
       level_editor.hpp level_editor-fwd.hpp level_editor_data.hpp \
       level_picker.hpp living.hpp map.hpp object_type.hpp obmap.hpp \
       options.hpp pal32.hpp palettes.hpp picker.hpp pixdefs.hpp \
       pixie_data.hpp pixie-fwd.hpp pixie.hpp pixien.hpp radar.hpp  rect.hpp \
       rectf.hpp results_screen.hpp sai2x.hpp save_data.hpp selection_info.hpp \
       screen-fwd.hpp screen.hpp simple_button.hpp smooth.hpp soundob.hpp \
       sounds.hpp stats.hpp text.hpp treasure.hpp troop_result.hpp util.hpp \
       version.hpp video-fwd.hpp video.hpp view-fwd.hpp view.hpp \
       view_sizes.hpp walker-fwd.hpp walker.hpp walker.hpp weap.hpp \

$(NAME): $(HDRS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS) $(INCLUDE)

clean:
	rm -f $(OBJS) $(NAME) *~
