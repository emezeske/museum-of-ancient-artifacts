rts_objects = main.o textdisplayer.o ltga.o

all: rts 

rts: $(rts_objects)
	g++ -o rts $(rts_objects) -lglut

clean:
	rm -f *.o rts

go: $(rts_objects)
	g++ -o rts $(rts_objects) -lglut
	./rts

	
