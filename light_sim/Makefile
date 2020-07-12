CFLAGS = -Wall -g
LFLAGS = -lglut
BINARY = light_sim

OBJECTS = main.o light_beam.o solid.o

.cc.o:
	g++ -c $(CFLAGS) $<

.PHONY: all clean tags

all: $(OBJECTS)
	g++ -o $(BINARY) $(OBJECTS) -lglut

clean: 
	rm -rf $(OBJECTS) $(BINARY)

tags:
	ctags *.cc *.h
