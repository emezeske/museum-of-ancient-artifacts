CXX = g++
OBJS := main.o simulator.o utilities.o sdl_gl_interface.o game_application.o camera.o basic_rigid_body.o trimesh_rep.o
SOURCES := $(OBJS:.o=.cc)
LFLAGS := -lSDL -lGL -lGLU -lglut -L../../old/lib/bullet-2.36a/out/linux/optimize/libs -lbulletdynamics -lbulletcollision -lbulletmath
INCLUDE := -I../common -I../../old/lib/bullet-2.36a/src
CFLAGS := -g -Wall -Wunused -DDEBUG $(INCLUDE)
BIN := test
DEPFILE := .depend

vpath %.h ../common
vpath %.cc ../common

.PHONY: all run clean

.cc.o: 
	$(CXX) $(CFLAGS) -c $<

all: $(BIN)

run: all
	env __GL_SYNC_TO_VBLANK=1 ./$(BIN)

clean:
	rm -f $(OBJS) $(BIN) $(BIN)-sym $(DEPFILE)

tags:
	ctags *.cc *.h

$(BIN): $(DEPFILE) $(OBJS)
	$(CXX) $(CFLAGS) -o $(BIN)-sym $(OBJS) $(LFLAGS)
	strip -o $(BIN) $(BIN)-sym

$(DEPFILE):
	touch $(DEPFILE)
	makedepend -Y -f $(DEPFILE) -- $(CFLAGS) -- $(SOURCES) &> /dev/null

include $(DEPFILE)
