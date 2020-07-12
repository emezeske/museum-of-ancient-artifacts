OBJS := main.o parser.o geometry.o\
		world_obj.o physics_obj.o\
		utilities.o simulator.o renderer.o

SOURCES := $(OBJS:.o=.cc)
CFLAGS := -g -Wall -Wunused -DDEBUG
LFLAGS := -lglut -lm
EXE := test
DEPFILE := .depend

.PHONY: all run clean

all: $(EXE)

run: all
	./$(EXE)

clean:
	rm -f $(OBJS) $(EXE) $(EXE)-sym $(DEPFILE)

tags:
	exuberant-ctags *.cc *.h

.cc.o: 
	$(CXX) $(CFLAGS) -c $<

$(EXE): $(DEPFILE) $(OBJS)
	$(CXX) $(CFLAGS) -o $(EXE) $(OBJS) $(LFLAGS)
	cp $(EXE) $(EXE)-sym
	strip $(EXE)

$(DEPFILE):
	touch $(DEPFILE)
	makedepend -Y -f $(DEPFILE) -- $(CFLAGS) -- $(SOURCES) &> /dev/null

include $(DEPFILE)
