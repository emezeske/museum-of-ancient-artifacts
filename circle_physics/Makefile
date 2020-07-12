OBJS := main.o collisionobj.o ltga.o ball.o drawable.o ship.o
CFLAGS := -g -Wall
LFLAGS := -lglut -lm -lGL -lGLU
EXE := test

all: $(EXE)

run: all
	./$(EXE)

clean:
	rm -f $(OBJS) $(EXE)

.cc.o: 
	$(CXX) $(CFLAGS) -c $<

$(EXE): $(OBJS)
	$(CXX) $(CFLAGS) -o $(EXE) $(OBJS) $(LFLAGS)

