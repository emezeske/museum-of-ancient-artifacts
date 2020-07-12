GL_LIBS = -lglut -lGL -lGLU 
OTHER_LIBS = -lm 

OBJECTS = gamefy.o ../shared/ltga.o ../shared/utils.o gCWorld.o Player.o Frustum.o

all: gamefy

gamefy:	$(OBJECTS)
	g++ $(GL_LIBS) $(OTHER_LIBS) $(OBJECTS) -o gamefy 

gCWorld.o: gCWorld.cc
	g++ -c -g -o gCWorld.o gCWorld.cc

Player.o: Player.cc
	g++ -c -g -o Player.o Player.cc

Frustum.o: Frustum.cc
	g++ -c -g -o Frustum.o Frustum.cc

../shared/ltga.o: ../shared/ltga.cc
	g++ -c -g -o ../shared/ltga.o ../shared/ltga.cc

gamefy.o: gamefy.cc
	g++ -c -g -o gamefy.o gamefy.cc

../shared/utils.o: ../shared/utils.cc
	g++ -c -g -o ../shared/utils.o ../shared/utils.cc

clean:
	rm -rf ../shared/*.o *.o gamefy

run: all
	./gamefy

package: clean
	tar -cvvf gamefy.tar ./
	gzip -c gamefy.tar > gamefy.tgz

tags:
	ctags *

