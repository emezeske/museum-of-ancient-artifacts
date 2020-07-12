FRACTAL_OBJECTS = fractal.o libtarga.o SocketServer.o
l = -lGL -lGLU -lglut -lpthread
il = #-Wall

all: $(FRACTAL_OBJECTS) 
	g++ -g -o fractalserver $(FRACTAL_OBJECTS) $(l)

fractal.o: fractal.cc
	g++ -c -g -o fractal.o fractal.cc $(il)

libtarga.o: libtarga.cc
	g++ -c -g -o libtarga.o libtarga.cc $(il)

SocketServer.o: SocketServer.cc
	g++ -c -g -o SocketServer.o SocketServer.cc $(il)

run: all
	./fractalserver

clean:
	rm $(FRACTAL_OBJECTS) fractalserver	
