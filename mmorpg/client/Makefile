mmorpg_objects = cameraobject.o       fontobject.o          fpsobject.o           \
		 gameobject.o         lightobject.o         frustumobject.o       \
		 mouseobject.o        networkobject.o       openglobject.o        \
		 otherplayerobject.o  playerobject.o        quadtreeobject.o      \
		 socketobject.o       staticmodelobject.o   terrainobject.o       \
		 textureobject.o      userinterfaceobject.o vertexbufferobject.o  \
		 main.o		      ltga.o

all: mmorpg

mmorpg: $(mmorpg_objects)
	g++ -o mmorpg $(mmorpg_objects) -lglut

clean:
	rm -f *.o mmorpg

go: $(mmorpg_objects)
	g++ -o mmorpg $(mmorpg_objects) -lglut
	./mmorpg
	
	
