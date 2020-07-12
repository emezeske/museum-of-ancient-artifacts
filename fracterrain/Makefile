CFLAGS = -Wall -g 
LFLAGS = -lglut -lm -lGL -lGLU
OBJECTS = main.o fracterrain.o
EXE = ft

all: $(EXE)

.cc.o:
	g++ $(CFLAGS) -c $<

$(EXE): $(OBJECTS) 
	g++ -o $(EXE) $(OBJECTS) $(LFLAGS) 

clean:
	rm -rf $(OBJECTS) $(EXE)
