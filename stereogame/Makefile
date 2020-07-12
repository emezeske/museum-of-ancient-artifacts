STEREOGAME_OBJECTS = main.o ltga.o depthmap.o utils.o

all: $(STEREOGAME_OBJECTS)
	g++ -g -o stereogame $(STEREOGAME_OBJECTS) -lglut

main.o: main.cc
	g++ -c -g -o main.o main.cc
depthmap.o: depthmap.cc
	g++ -c -g -o depthmap.o depthmap.cc
utils.o: utils.cc
	g++ -c -g -o utils.o utils.cc
ltga.o: ltga.cc
	g++ -c -g -o ltga.o ltga.cc

clean: 
	rm -f stereogame *.o

run: all
	./stereogame
