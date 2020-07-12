OBJS := synth.o

SOURCES := $(OBJS:.o=.cc)
CFLAGS := -g -Wall -Wextra -Wunused -DDEBUG
LFLAGS := -lglut -lm
EXE := synth
DEPFILE := .depend

.PHONY: all run clean

all: $(EXE)

run: all
	./$(EXE)

clean:
	rm -f $(OBJS) $(EXE) $(EXE)-sym $(DEPFILE)

.cc.o: 
	$(CXX) $(CFLAGS) -c $<

$(EXE): $(DEPFILE) $(OBJS)
	$(CXX) $(CFLAGS) -o $(EXE) $(OBJS) $(LFLAGS)
	cp $(EXE) $(EXE)-sym
	strip $(EXE)

$(DEPFILE):
	touch $(DEPFILE)
	makedepend -Y -f $(DEPFILE) -- $(CFLAGS) -- $(SOURCES) &> /dev/null

include $(DEPFILE)
