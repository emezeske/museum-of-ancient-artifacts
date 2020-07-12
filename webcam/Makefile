OBJS := main.o utilities.o camera.o edge_detector.o

SOURCES := $(OBJS:.o=.cc)
CFLAGS := -g -Wall -Wunused -DDEBUG
IFLAGS := -I/usr/X11R6/include -I../common
LFLAGS := -lglut -lm
EXE := test
DEPFILE := .depend

vpath %h ../common
vpath %cc ../common

.PHONY: all run clean

all: $(EXE)

run: all
	./$(EXE)

clean:
	rm -f $(OBJS) $(EXE) $(EXE)-sym $(DEPFILE)

.cc.o: 
	$(CXX) $(CFLAGS) $(IFLAGS) -c $<

$(EXE): $(DEPFILE) $(OBJS)
	$(CXX) $(CFLAGS) -o $(EXE) $(OBJS) $(LFLAGS)
	cp $(EXE) $(EXE)-sym
	strip $(EXE)

$(DEPFILE):
	touch $(DEPFILE)
	makedepend -Y -f $(DEPFILE) -- $(CFLAGS) -- $(SOURCES) &> /dev/null

include $(DEPFILE)
