CXX = g++
OBJS := main.o utilities.o fracterrain.o
SOURCES := $(OBJS:.o=.cc)
LFLAGS := -lode -lglut -lm
CFLAGS := -g -Wall -Wunused -DDEBUG -I ../common
BIN := test
DEPFILE := .depend

vpath %.h ../common
vpath %.cc ../common

.PHONY: all run clean

.cc.o: 
	$(CXX) $(CFLAGS) -c $<

all: $(BIN)

run: all
	./$(BIN)

clean:
	rm -f $(OBJS) $(BIN) $(BIN)-sym $(DEPFILE)

tags:
	exuberant-ctags *.cc *.h

$(BIN): $(DEPFILE) $(OBJS)
	$(CXX) $(CFLAGS) -o $(BIN)-sym $(OBJS) $(LFLAGS)
	strip -o $(BIN) $(BIN)-sym

$(DEPFILE):
	touch $(DEPFILE)
	makedepend -Y -f $(DEPFILE) -- $(CFLAGS) -- $(SOURCES) &> /dev/null

include $(DEPFILE)
