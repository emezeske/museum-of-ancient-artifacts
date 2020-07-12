normalizebm_objects = main.o ltga.o libtarga.o vec3d.o
normalizebm_linkage = 

all: normalizebm 

normalizebm: $(normalizebm_objects)
	g++ -o normalizebm $(normalizebm_objects) $(normalizebm_linkage)

clean:
	rm -f *.o normalizebm

run: $(normalizebm_objects)
	g++ -o normalizebm $(normalizebm_objects) $(normalizebm_linkage)
	./normalizebm

	
