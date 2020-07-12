FRACTAL_OBJECTS = fractal.o SocketClient.o
l = -lglut -lpthread -lm -O3 -funroll-all-loops -finline-functions -funsafe-math-optimizations -fprefetch-loop-arrays -falign-loops -falign-floats -falign-functions -fno-math-errno -ffast-math

all: $(FRACTAL_OBJECTS) 
	g++ -o fractalclient $(FRACTAL_OBJECTS) $(l)

fractal.o: fractal.cc
	g++ -g -c -o fractal.o fractal.cc

SocketClient.o: SocketClient.cc
	g++ -g -c -o SocketClient.o SocketClient.cc

run: all
	./fractalclient

clean:
	rm $(FRACTAL_OBJECTS) fractalclient	
