FRACTAL_OBJECTS = fractal.o
LINKER_FLAGS = -lglut -O3 -funroll-all-loops -finline-functions -funsafe-math-optimizations -fprefetch-loop-arrays -falign-loops -falign-floats -falign-functions

all: $(FRACTAL_OBJECTS) 
	g++ -o fractal $(FRACTAL_OBJECTS) $(LINKER_FLAGS)

run: all
	./fractal

clean:
	rm -rf $(FRACTAL_OBJECTS)
