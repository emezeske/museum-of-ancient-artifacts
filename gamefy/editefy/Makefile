GL_LIBS = -lglut -lGL -lGLU 
OTHER_LIBS = -lm 

EDITEFY_OBJECTS = editefy.o ../shared/ltga.o ../shared/utils.o World.o Player.o Display.o DisplayOrtho.o DisplayPerspective.o selection.o ../shared/matrix.o Object3D.o

all: editefy

editefy:	$(EDITEFY_OBJECTS)
	g++ $(GL_LIBS) $(OTHER_LIBS) $(EDITEFY_OBJECTS) -g -o editefy 

World.o: World.cc
	g++ -c -g -o World.o World.cc

Object3D.o: Object3D.cc
	g++ -c -g -o Object3D.o Object3D.cc

../shared/ltga.o: ../shared/ltga.cc
	g++ -c -g -o ../shared/ltga.o ../shared/ltga.cc

../shared/utils.o: ../shared/utils.cc
	g++ -c -g -o ../shared/utils.o ../shared/utils.cc

Display.o: Display.cc
	g++ -c -g -o Display.o Display.cc

DisplayPerspective.o: DisplayPerspective.cc
	g++ -c -g -o DisplayPerspective.o DisplayPerspective.cc

DisplayOrtho.o: DisplayOrtho.cc
	g++ -c -g -o DisplayOrtho.o DisplayOrtho.cc

gWorldEditefy.o: gWorldEditefy.cc
	g++ -c -g -o gWorldEditefy.o gWorldEditefy.cc

editefy.o: editefy.cc
	g++ -c -g -o editefy.o editefy.cc

selection.o: selection.cc
	g++ -c -g -o selection.o selection.cc

../shared/matrix.o: ../shared/matrix.cc
	g++ -c -g -o ../shared/matrix.o ../shared/matrix.cc

run: all
	./editefy

clean:
	rm -rf *.o editefy editefy.tar editefy.tgz

package: clean
	tar -cvvf editefy.tar ./
	gzip -c editefy.tar > editefy.tgz

