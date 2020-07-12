FRACTAL_OBJECTS = fractal.o ltga.o
l = -lglut -O3 -funroll-all-loops -finline-functions -funsafe-math-optimizations -fprefetch-loop-arrays -falign-loops -falign-floats -falign-functions

all: $(FRACTAL_OBJECTS) 
	g++ -o fractal $(FRACTAL_OBJECTS) $(l)

run: all
	./fractal

