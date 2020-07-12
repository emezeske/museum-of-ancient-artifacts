editor_objects = editor.o textdisplayer.o fixedworldobject.o input.o ltga.o mobileworldobject.o textures.o ui.o viewport.o worldobject.o writetofile.o

all: editor

editor: $(editor_objects)
	g++ -o editor $(editor_objects) -lglut

clean:
	rm -f *.o editor

go: $(editor_objects)
	g++ -o editor $(editor_objects) -lglut
	./editor
	
	
