sidescroll_objects = sidescroll.o fixedworldobject.o input.o ltga.o mobileworldobject.o textures.o camera.o worldobject.o readlevelfile.o hero.o

all: sidescroll

sidescroll: $(sidescroll_objects)
	g++ -o sidescroll $(sidescroll_objects) -lglut -lGL -lGLU

clean:
	rm -f *.o sidescroll

go: $(sidescroll_objects)
	g++ -o sidescroll $(sidescroll_objects) -lglut -lGL -lGLU
	./sidescroll

	
