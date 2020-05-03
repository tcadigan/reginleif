NAME = openglad
CXXFLAGS = -Wall -std=c++17 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lstdc++fs

CPPFILES = browser_entry.cpp button.cpp campaign_data.cpp campaign_entry.cpp \
           campaign_picker.cpp command.cpp damage_number.cpp \
           editor_object_brush.cpp editor_terrain_brush.cpp effect.cpp \
           game.cpp glad.cpp gloader.cpp gparser.cpp graphlib.cpp guy.cpp \
           help.cpp input.cpp intro.cpp io.cpp joy_data.cpp level_data.cpp \
           level_editor.cpp level_editor_data.cpp level_picker.cpp living.cpp \
           map.cpp menu_nav.cpp mouse_state.cpp object_type.cpp obmap.cpp \
           options.cpp pal32.cpp picker.cpp pixie.cpp pixie_data.cpp \
           pixien.cpp radar.cpp rect.cpp rectf.cpp results_screen.cpp \
           save_data.cpp screen.cpp selection_info.cpp simple_button.cpp \
           smooth.cpp sound.cpp stats.cpp text.cpp treasure.cpp \
           troop_result.cpp util.cpp video.cpp videoscreen.cpp view.cpp \
           virtual_button.cpp walker.cpp weap.cpp \

OBJS = browser_entry.o button.o campaign_data.o campaign_entry.o \
       campaign_picker.o command.o damage_number.o editor_object_brush.o \
       editor_terrain_brush.o effect.o game.o glad.o gloader.o gparser.o \
       graphlib.o guy.o help.o input.o intro.o io.o joy_data.o level_data.o \
       level_editor.o level_editor_data.o level_picker.o living.o map.o \
       menu_nav.o mouse_state.o object_type.o obmap.o options.o pal32.o \
       picker.o pixie.o pixie_data.o pixien.o radar.o rect.o rectf.o \
       results_screen.o save_data.o screen.o selection_info.o \
       simple_button.o smooth.o sound.o stats.o text.o treasure.o \
       troop_result.o util.o video.o video_screen.o view.o virtual_button.o \
       walker.o weap.o \

HDRS = base.hpp browser_entry.hpp button.hpp campaign_data.hpp \
       campaign_entry.hpp campaign_picker.hpp colors.hpp command.hpp \
       damage_number.hpp editor_object_brush.hpp editor_terrain_brush.hpp \
       effect.hpp game.hpp glad.hpp gloader.hpp gparser.hpp graphlib.hpp \
       guy.hpp help.hpp input.hpp intro.hpp io.hpp joy_data.hpp level_data.hpp \
       level_editor.hpp level_editor_data.hpp level_picker.hpp living.hpp \
       map.hpp menu_nav.hpp mouse_state.hpp object_type.hpp obmap.hpp \
       options.hpp pal32.hpp picker.hpp pixdefs.hpp pixie_data.hpp pixie.hpp \
       pixien.hpp radar.hpp  rect.hpp rectf.hpp results_screen.hpp \
       save_data.hpp screen.hpp selection_info.hpp simple_button.hpp \
       smooth.hpp soundob.hpp sounds.hpp stats.hpp text.hpp treasure.hpp \
       troop_result.hpp util.hpp version.hpp video.hpp video_screen.hpp \
       view.hpp view_sizes.hpp virtual_button.hpp walker.hpp walker.hpp \
       weap.hpp \

$(NAME): $(HDRS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS) $(INCLUDE)

clean:
	rm -f $(OBJS) $(NAME) *~

browser_entry.o: browser_entry.hpp video_screen.hpp
browser_entry.hpp: level_data.hpp radar.hpp video_screen.hpp
	touch browser_entry.hpp
button.o: button.hpp input.hpp pixien.hpp virtual_button.hpp
button.hpp: menu_nav.hpp
	touch button.hpp
campaign_data.o: campaign_data.hpp graphlib.hpp io.hpp util.hpp
campaign_data.hpp: pixie.hpp pixie_data.hpp
	touch campaign_data.hpp
campaign_entry.o: campaign_entry.hpp base.hpp io.hpp graphlib.hpp pixie.hpp text.hpp util.hpp video_screen.hpp
campaign_entry.hpp: pixie.hpp pixie_data.hpp
	touch campaign_entry.hpp
campaign_picker.o: campaign_picker.hpp button.hpp campaign_entry.hpp graphlib.hpp guy.hpp input.hpp io.hpp level_editor.hpp mouse_state.hpp picker.hpp pixie.hpp text.hpp util.hpp video_screen.hpp
campaign_picker.hpp: save_data.hpp
	touch campaign_picker.hpp
command.o: command.hpp
damage_number.o: damage_number.hpp video_screen.hpp
editor_object_brush.o: editor_object_brush.hpp walker.hpp
editor_object_brush.hpp: walker.hpp
	touch editor_object_brush.hpp
editor_terrain_brush.o: editor_terrain_brush.hpp pixdefs.hpp
effect.o: effect.hpp base.hpp guy.hpp soundob.hpp util.hpp video_screen.hpp
effect.hpp: pixie_data.hpp walker.hpp
	touch effect.hpp
game.o: game.hpp campaign_picker.hpp guy.hpp picker.hpp  smooth.hpp util.hpp view.hpp walker.hpp
game.hpp: video_screen.hpp
	touch game.hpp
glad.o: glad.hpp base.hpp colors.hpp game.hpp gparser.hpp guy.hpp help.hpp input.hpp intro.hpp io.hpp joy_data.hpp mouse_state.hpp options.hpp pal32.hpp picker.hpp results_screen.hpp util.hpp version.hpp view.hpp walker.hpp
glad.hpp: video_screen.hpp walker.hpp
	touch glad.hpp
gloader.o: gloader.hpp effect.hpp graphlib.hpp gparser.hpp guy.hpp living.hpp picker.hpp stats.hpp treasure.hpp util.hpp video_screen.hpp walker.hpp weap.hpp
gloader.hpp: base.hpp pixien.hpp pixie_data.hpp
	touch gloader.hpp
gparser.o: gparser.hpp input.hpp io.hpp util.hpp version.hpp
graphlib.o: graphlib.hpp io.hpp pixdefs.hpp util.hpp
graphlib.hpp: pixie_data.hpp
	touch graphlib.hpp
guy.o: guy.hpp picker.hpp
guy.hpp: base.hpp
	touch guy.hpp
help.o: help.hpp campaign_data.hpp input.hpp util.hpp
help.hpp: video_screen.hpp
	touch help.hpp
input.o: input.hpp gparser.hpp io.hpp joy_data.hpp mouse_state.hpp picker.hpp util.hpp video_screen.hpp
intro.o: intro.hpp base.hpp graphlib.hpp input.hpp pal32.hpp pixie.hpp util.hpp video_screen.hpp view.hpp
io.o: io.hpp input.hpp pixdefs.hpp util.hpp
joy_data.o: joy_data.hpp
joy_data.hpp: input.hpp
	touch joy_data.hpp
level_data.o: level_data.hpp graphlib.hpp io.hpp screen.hpp util.hpp view.hpp walker.hpp
level_data.hpp: gloader.hpp pixdefs.hpp obmap.hpp pixie.hpp pixie_data.hpp pixien.hpp smooth.hpp walker.hpp
	touch level_data.hpp
level_editor.o: level_editor.hpp input.hpp io.hpp joy_data.hpp level_editor_data.hpp mouse_state.hpp object_type.hpp options.hpp pal32.hpp picker.hpp text.hpp util.hpp view.hpp walker.hpp
level_editor.hpp: level_data.hpp level_editor_data.hpp rectf.hpp simple_button.hpp video_screen.hpp walker.hpp
	touch level_editor.hpp
level_editor_data.o: level_editor_data.hpp gloader.hpp gparser.hpp input.hpp io.hpp level_editor.hpp level_picker.hpp mouse_state.hpp picker.hpp rect.hpp util.hpp view.hpp walker.hpp
level_editor_data.hpp: campaign_data.hpp campaign_picker.hpp editor_object_brush.hpp editor_terrain_brush.hpp level_data.hpp radar.hpp rectf.hpp selection_info.hpp simple_button.hpp video_screen.hpp walker.hpp
	touch level_editor_data.hpp
level_picker.o: level_picker.hpp browser_entry.hpp button.hpp input.hpp io.hpp level_editor.hpp mouse_state.hpp picker.hpp text.hpp util.hpp
living.o: living.hpp gparser.hpp guy.hpp smooth.hpp util.hpp video_screen.hpp
living.hpp: walker.hpp pixie_data.hpp
	touch living.hpp
map.o: map.hpp base.hpp video_screen.hpp walker.hpp
map.hpp: walker.hpp
	touch map.hpp
menu_nav.o: menu_nav.hpp
mouse_state.o: mouse_state.hpp input.hpp
object_type.o: object_type.hpp
obmap.o: obmap.hpp base.hpp text.hpp util.hpp video_screen.hpp view.hpp walker.hpp
obmap.hpp: walker.hpp
	touch obmap.hpp
options.o: options.hpp io.hpp
pal32.o: pal32.hpp
picker.o: picker.hpp button.hpp campaign_picker.hpp glad.hpp gloader.hpp gparser.hpp graphlib.hpp guy.hpp help.hpp input.hpp io.hpp level_editor.hpp level_picker.hpp mouse_state.hpp options.hpp pal32.hpp util.hpp version.hpp video_screen.hpp view.hpp virtual_button.hpp walker.hpp
picker.hpp: button.hpp guy.hpp
	touch picker.hpp
pixie.o: pixie.hpp base.hpp pal32.hpp util.hpp video_screen.hpp view.hpp
pixie.hpp: pixie_data.hpp
	touch pixie.hpp
pixie_data.o: pixie_data.hpp
pixien.o: pixien.hpp
pixien.hpp: pixie.hpp
	touch pixien.hpp
radar.o: radar.hpp colors.hpp level_data.hpp util.hpp video_screen.hpp walker.hpp
radar.hpp: level_data.hpp walker.hpp
	touch radar.hpp
rect.o: rect.hpp
rectf.o: rectf.hpp
results_screen.o: results_screen.hpp base.hpp button.hpp guy.hpp input.hpp level_editor.hpp mouse_state.hpp picker.hpp screen.hpp stats.hpp text.hpp troop_result.hpp util.hpp walker.hpp
results_screen.hpp: guy.hpp walker.hpp
	touch results_screen.hpp
save_data.o: save_data.hpp campaign_picker.hpp guy.hpp io.hpp picker.hpp util.hpp walker.hpp
save_data.hpp: guy.hpp walker.hpp
	touch save_data.hpp
screen.o: screen.hpp input.hpp util.hpp
selection_info.o: selection_info.hpp walker.hpp
selection_info.hpp: level_data.hpp walker.hpp
	touch selection_info.hpp
simple_button.o: simple_button.hpp
simple_button.hpp: video_screen.hpp
	touch simple_button.hpp
smooth.o: smooth.hpp base.hpp pixdefs.hpp util.hpp
smooth.hpp: pixie_data.hpp
	touch smooth.hpp
sound.o: soundob.hpp io.hpp util.hpp
stats.o: stats.hpp guy.hpp input.hpp util.hpp video_screen.hpp view.hpp
stats.hpp: base.hpp command.hpp
	touch stats.hpp
text.o: text.hpp base.hpp graphlib.hpp input.hpp video_screen.hpp view.hpp
text.hpp: pixie_data.hpp
	touch text.hpp
treasure.o: treasure.hpp gloader.hpp guy.hpp input.hpp picker.hpp pixie_data.hpp util.hpp video_screen.hpp
treasure.hpp: walker.hpp
	touch treasure.hpp
troop_result.o: troop_result.hpp gloader.hpp guy.hpp picker.hpp results_screen.hpp video_screen.hpp view.hpp walker.hpp
troop_result.hpp: guy.hpp walker.hpp
	touch troop_result.hpp
util.o: util.hpp
video.o: video.hpp base.hpp gparser.hpp input.hpp io.hpp pal32.hpp screen.hpp util.hpp
video.hpp: text.hpp
	touch video.hpp
video_screen.o: video_screen.hpp gloader.hpp gparser.hpp guy.hpp input.hpp io.hpp pal32.hpp results_screen.hpp smooth.hpp util.hpp view.hpp view_sizes.hpp walker.hpp
video_screen.hpp: base.hpp level_data.hpp save_data.hpp soundob.hpp video.hpp view.hpp
	touch video_screen.hpp
view.o: view.hpp colors.hpp guy.hpp help.hpp input.hpp joy_data.hpp options.hpp pal32.hpp radar.hpp  util.hpp version.hpp video_screen.hpp view_sizes.hpp walker.hpp
view.hpp: options.hpp radar.hpp
	touch view.hpp
virtual_button.o: virtual_button.hpp gparser.hpp input.hpp io.hpp level_editor.hpp mouse_state.hpp picker.hpp text.hpp video_screen.hpp
virtual_button.hpp: button.hpp pixien.hpp
	touch virtual_button.hpp
walker.o: walker.hpp glad.hpp gloader.hpp gparser.hpp glad.hpp guy.hpp map.hpp pal32.hpp picker.hpp pixie_data.hpp stats.hpp util.hpp video_screen.hpp view.hpp weap.hpp
walker.hpp: base.hpp damage_number.hpp guy.hpp pixien.hpp stats.hpp
	touch walker.hpp
weap.o: weap.hpp util.hpp video_screen.hpp
weap.hpp: walker.hpp
	touch weap.hpp
