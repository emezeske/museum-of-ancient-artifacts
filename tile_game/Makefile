#
# Makefile for tile_game
#

CXX := g++
# TODO fix no-unused
CWARNINGS := -Wall -W -Wshadow -Wpointer-arith -Wcast-qual -Wwrite-strings -Wconversion -Winline -Wredundant-decls -Wno-unused
CFLAGS := -g $(CWARNINGS)
COMMON_DIR := ../common
LFLAGS := -lSDL -lSDL_image -lGL -lGLU -lm
INCLUDE_DIRS := -I$(COMMON_DIR)
DEPEND_DIR := ./.depend
BINARY_NAME := tile_game
SOURCES := main.cc game_application.cc sdl_gl_interface.cc camera.cc utilities.cc drawing_utilities.cc game_map.cc \
		   drone.cc geometry.cc structure.cc path_finder.cc dijkstra_path_finder.cc
OBJECTS := $(SOURCES:.cc=.o)

vpath %.cc $(COMMON_DIR)

.PHONY: all clean common_tags tags all_tags

all: $(BINARY_NAME)

run: all
	env __GL_SYNC_TO_VBLANK=1 ./$(BINARY_NAME)

.cc.o:
	$(CXX) $(CFLAGS) $(INCLUDE_DIRS) -c $<

$(BINARY_NAME): $(OBJECTS)
	$(CXX) $(CFLAGS) -o $(BINARY_NAME)-sym $(OBJECTS) $(LFLAGS)
	strip -o $(BINARY_NAME) $(BINARY_NAME)-sym

clean:
	rm -f *.o $(BINARY_NAME) $(BINARY_NAME).sha1 *-sym core *~ .*~ a.out proxy/*~
	rm -rf $(DEPEND_DIR)

tags: *.cc *.h
	ctags $^

$(DEPEND_DIR):
	@mkdir $(DEPEND_DIR)

$(DEPEND_DIR)/%.d: %.cc $(DEPEND_DIR)
	@set -e; $(CC) -MM $(CFLAGS) $(INCLUDE_DIRS) $< | sed 's#\($*\)\.o[ :]*#\1.o $@ : #g' > $@; [ -s $@ ] || rm -f $@

-include $(patsubst %,$(DEPEND_DIR)/%,$(subst .cc,.d,$(SOURCES)))

