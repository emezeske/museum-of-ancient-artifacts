GL_LIBS = -lglut -lGL -lGLU 
OTHER_LIBS = -lm 

COMPILEFY_OBJECTS = compilefy.o ../shared/gWorld.o ../shared/utils.o ../shared/ltga.o gCWorld.o

all: compilefy

compilefy:	$(COMPILEFY_OBJECTS)
	g++ $(GL_LIBS) $(OTHER_LIBS) $(COMPILEFY_OBJECTS) -g -o compilefy 

../shared/gWorld.o: ../shared/gWorld.cc
	g++ -c -g -o ../shared/gWorld.o ../shared/gWorld.cc

../shared/ltga.o: ../shared/ltga.cc
	g++ -c -g -o ../shared/ltga.o ../shared/ltga.cc

gCWorld.o:	gCWorld.cc
	g++ -c -g -o gCWorld.o gCWorld.cc

compilefy.o: compilefy.cc
	g++ -c -g -o compilefy.o compilefy.cc

../shared/utils.o: ../shared/utils.cc
	g++ -c -g -o ../shared/utils.o ../shared/utils.cc

clean:
	rm *.o compilefy compilefy.tar compilefy.tgz

package: clean
	tar -cvvf compilefy.tar ./
	gzip -c compilefy.tar > compilefy.tgz

