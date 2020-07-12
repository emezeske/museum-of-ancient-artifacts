FRACTAL_OBJECTS = fractal.o libtarga.o
l = -lglut -lm -O3 -funroll-all-loops -finline-functions -funsafe-math-optimizations -fprefetch-loop-arrays -falign-loops -falign-floats -falign-functions -fno-math-errno -ffast-math

all: $(FRACTAL_OBJECTS) 
	g++ -o fractal $(FRACTAL_OBJECTS) $(l)

run: all
	./fractal

clean:
	rm fractal.o fractal	
