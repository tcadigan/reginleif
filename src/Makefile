NAME = openglad
CXXFLAGS = -Wall -std=c++17 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`

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
       editor_object_brush.hpp editor_terrain_brush.hpp effect.hpp game.hpp \
       glad.hpp gloader-fwd.hpp gloader.hpp gparser.hpp graphlib.hpp \
       guy-fwd.hpp guy.hpp help.hpp input.hpp intro.hpp io.hpp level_data.hpp \
       level_editor.hpp level_editor-fwd.hpp level_editor_data.hpp \
       level_picker.hpp living.hpp map.hpp object_type.hpp obmap.hpp \
       options.hpp pal32.hpp picker.hpp pixdefs.hpp \
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

base.hpp: pixie_data.hpp screen-fwd.hpp
	touch base.hpp
browser_entry.o: browser_entry.hpp level_picker.hpp screen.hpp
browser_entry.hpp: level_data.hpp radar.hpp screen-fwd.hpp
	touch browser_entry.hpp
button.o: button.hpp gloader.hpp gparser.hpp input.hpp io.hpp picker.hpp screen.hpp
button.hpp: gloader-fwd.hpp guy.hpp input.hpp obmap.hpp pixien.hpp screen-fwd.hpp text.hpp
	touch button.hpp
campaign_entry.o: campaign_entry.hpp base.hpp io.hpp graphlib.hpp pixie.hpp screen.hpp text.hpp util.hpp
campaign_entry.hpp: pixie-fwd.hpp pixie_data.hpp
	touch campaign_entry.hpp
campaign_picker.o: campaign_picker.hpp button.hpp campaign_entry.hpp graphlib.hpp guy-fwd.hpp io.hpp level_editor.hpp picker.hpp pixie-fwd.hpp screen.hpp text.hpp util.hpp
campaign_picker.hpp: save_data.hpp screen-fwd.hpp
	touch campaign_picker.hpp
command.o: command.hpp
damage_number.o: damage_number.hpp screen.hpp view.hpp
damage_number.hpp: view-fwd.hpp
	touch damage_number.hpp
editor_object_brush.o: editor_object_brush.hpp walker.hpp
editor_object_brush.hpp: walker-fwd.hpp
	touch editor_object_brush.hpp
editor_terrain_brush.o: editor_terrain_brush.hpp pixdefs.hpp
effect.o: effect.hpp base.hpp guy.hpp screen.hpp soundob.hpp util.hpp
effect.hpp: guy-fwd.hpp obmap.hpp screen-fwd.hpp stats.hpp walker.hpp
	touch effect.hpp
game.o: game.hpp campaign_picker.hpp guy.hpp picker.hpp screen.hpp smooth.hpp util.hpp view.hpp walker.hpp
game.hpp: screen.hpp
	touch game.hpp
glad.o: glad.hpp base.hpp colors.hpp game.hpp gparser.hpp guy.hpp help.hpp input.hpp intro.hpp io.hpp options.hpp pal32.hpp picker.hpp results_screen.hpp screen.hpp util.hpp view.hpp version.hpp walker.hpp
glad.hpp: screen.hpp walker-fwd.hpp
	touch glad.hpp
gloader.o: gloader.hpp effect.hpp graphlib.hpp gparser.hpp living.hpp picker.hpp stats.hpp treasure.hpp util.hpp walker.hpp weap.hpp
gloader.hpp: gloader-fwd.hpp pixien.hpp pixie_data.hpp screen-fwd.hpp walker-fwd.hpp
	touch gloader.hpp
gparser.o: gparser.hpp input.hpp io.hpp util.hpp version.hpp
graphlib.o: graphlib.hpp io.hpp pixdefs.hpp util.hpp
graphlib.hpp: pixie_data.hpp
	touch graphlib.hpp
guy.o: guy.hpp gloader.hpp screen.hpp walker.hpp
guy.hpp: guy-fwd.hpp screen-fwd.hpp stats.hpp walker-fwd.hpp
	touch guy.hpp
help.o: help.hpp input.hpp level_data.hpp util.hpp
help.hpp: screen.hpp
	touch help.hpp
input.o: input.hpp base.hpp gparser.hpp io.hpp screen.hpp util.hpp
input.hpp: screen-fwd.hpp video-fwd.hpp
	touch input.hpp
intro.o: intro.hpp base.hpp graphlib.hpp input.hpp pal32.hpp pixie.hpp screen.hpp util.hpp view.hpp
io.o: io.hpp input.hpp pixdefs.hpp util.hpp
level_data.o: level_data.hpp gloader.hpp graphlib.hpp io.hpp pixie.hpp screen.hpp smooth.hpp stats.hpp util.hpp view.hpp walker.hpp
level_data.hpp: base.hpp gloader-fwd.hpp pixdefs.hpp obmap.hpp pixie-fwd.hpp pixie_data.hpp pixien.hpp screen-fwd.hpp smooth.hpp stats.hpp walker-fwd.hpp
	touch level_data.hpp
level_editor-fwd.hpp: base.hpp object_type.hpp pixdefs.hpp
	touch level_editor-fwd.hpp
level_editor.o: level_editor.hpp input.hpp io.hpp object_type.hpp options.hpp pal32.hpp picker.hpp screen.hpp text.hpp util.hpp view.hpp walker.hpp
level_editor.hpp: level_editor-fwd.hpp level_data.hpp level_editor_data.hpp rectf.hpp screen-fwd.hpp simple_button.hpp walker-fwd.hpp
	touch level_editor.hpp
level_editor_data.o: level_editor_data.hpp gloader.hpp gparser.hpp input.hpp io.hpp level_editor.hpp level_picker.hpp picker.hpp rect.hpp screen.hpp util.hpp view.hpp walker.hpp
level_editor_data.hpp: campaign_picker.hpp editor_object_brush.hpp editor_terrain_brush.hpp level_data.hpp level_editor-fwd.hpp radar.hpp rectf.hpp selection_info.hpp screen-fwd.hpp simple_button.hpp walker-fwd.hpp
	touch level_editor_data.hpp
level_picker.o: level_picker.hpp browser_entry.hpp button.hpp guy.hpp io.hpp level_data.hpp picker.hpp radar.hpp screen.hpp stats.hpp text.hpp util.hpp walker.hpp
level_picker.hpp: level_data.hpp screen-fwd.hpp
	touch level_picker.hpp
living.o: living.hpp gparser.hpp guy.hpp screen.hpp smooth.hpp util.hpp
living.hpp: walker.hpp pixie_data.hpp
	touch living.hpp
map.o: map.hpp base.hpp screen.hpp walker.hpp
map.hpp: walker-fwd.hpp
	touch map.hpp
object_type.o: object_type.hpp
obmap.o: obmap.hpp base.hpp screen.hpp text.hpp util.hpp view.hpp walker.hpp
obmap.hpp: walker-fwd.hpp
	touch obmap.hpp
options.o: options.hpp io.hpp
pal32.o: pal32.hpp
picker.o: picker.hpp button.hpp campaign_picker.hpp glad.hpp gloader.hpp gparser.hpp graphlib.hpp guy.hpp help.hpp input.hpp io.hpp level_editor.hpp level_picker.hpp options.hpp pal32.hpp screen.hpp util.hpp version.hpp view.hpp walker.hpp
picker.hpp: button-fwd.hpp guy-fwd.hpp
	touch picker.hpp
pixie.o: pixie.hpp base.hpp pal32.hpp screen.hpp util.hpp view.hpp
pixie.hpp: pixie-fwd.hpp pixie_data.hpp view-fwd.hpp
	touch pixie.hpp
pixie_data.o: pixie_data.hpp
pixien.o: pixien.hpp
pixien.hpp: pixie.hpp
	touch pixien.hpp
radar.o: radar.hpp colors.hpp screen.hpp util.hpp view.hpp walker.hpp
radar.hpp: level_data.hpp view-fwd.hpp
	touch radar.hpp
rect.o: rect.hpp
rectf.o: rectf.hpp
results_screen.o: results_screen.hpp base.hpp button.hpp guy.hpp level_editor.hpp picker.hpp screen.hpp stats.hpp text.hpp troop_result.hpp util.hpp walker.hpp
results_screen.hpp: guy-fwd.hpp walker-fwd.hpp
	touch results_screen.hpp
sai2x.o: sai2x.hpp input.hpp util.hpp
save_data.o: save_data.hpp campaign_picker.hpp guy.hpp io.hpp picker.hpp util.hpp walker.hpp
save_data.hpp: guy-fwd.hpp walker-fwd.hpp
	touch save_data.hpp
screen.o: screen.hpp gloader.hpp gparser.hpp guy.hpp input.hpp io.hpp pal32.hpp results_screen.hpp smooth.hpp util.hpp view.hpp view_sizes.hpp walker.hpp
screen.hpp: screen-fwd.hpp gloader-fwd.hpp level_data.hpp obmap.hpp save_data.hpp smooth.hpp soundob.hpp text.hpp view.hpp
	touch screen.hpp
selection_info.o: selection_info.hpp walker.hpp
selection_info.hpp: level_data.hpp walker-fwd.hpp
	touch selection_info.hpp
simple_button.o: simple_button.hpp screen.hpp
simple_button.hpp: screen-fwd.hpp
	touch simple_button.hpp
smooth.o: smooth.hpp base.hpp pixdefs.hpp util.hpp
smooth.hpp: pixie_data.hpp
	touch smooth.hpp
sound.o: soundob.hpp io.hpp util.hpp
stats.o: stats.hpp guy.hpp input.hpp screen.hpp util.hpp view.hpp walker.hpp
stats.hpp: base.hpp command.hpp walker-fwd.hpp
	touch stats.hpp
text.o: text.hpp base.hpp graphlib.hpp input.hpp screen.hpp view.hpp
text.hpp: pixie_data.hpp screen-fwd.hpp view-fwd.hpp
treasure.o: treasure.hpp gloader.hpp guy.hpp input.hpp picker.hpp pixie_data.hpp screen.hpp util.hpp
treasure.hpp: walker.hpp
	touch treasure.hpp
troop_result.o: troop_result.hpp gloader.hpp guy.hpp picker.hpp results_screen.hpp screen.hpp view.hpp walker.hpp
troop_result.hpp: guy-fwd.hpp walker-fwd.hpp
	touch troop_result.hpp
util.o: util.hpp
video.o: video.hpp base.hpp gparser.hpp input.hpp io.hpp pal32.hpp sai2x.hpp util.hpp
video.hpp: video-fwd.hpp text.hpp
	touch video.hpp
view.o: view.hpp colors.hpp guy.hpp help.hpp input.hpp options.hpp pal32.hpp radar.hpp screen.hpp util.hpp version.hpp view_sizes.hpp walker.hpp
view.hpp: view-fwd.hpp level_data.hpp options.hpp radar.hpp
	touch view.hpp
walker.o: walker.hpp glad.hpp gloader.hpp gparser.hpp guy.hpp map.hpp pal32.hpp pixie_data.hpp screen.hpp stats.hpp util.hpp view.hpp weap.hpp
walker.hpp: walker-fwd.hpp damage_number.hpp guy-fwd.hpp obmap.hpp pixien.hpp stats.hpp
	touch walker.hpp
weap.o: weap.hpp screen.hpp util.hpp
weap.hpp: walker.hpp
	touch weap.hpp
