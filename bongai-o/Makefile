bongai_objects = main.o ltga.o spacecraft.o flame.o rocketlauncher.o network.o

all: bongai 

bongai: $(bongai_objects)
	g++ -o bongai $(bongai_objects) -lglut -lpthread -lGL -lGLU

clean:
	rm -f *.o bongai

run: $(bongai_objects)
	g++ -o bongai $(bongai_objects) -lglut -lpthread -lGL -lGLU
	./bongai

	
