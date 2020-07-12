CXX := g++
COMMON_DIR := ../../common
NETWORKING_DIR := ../../networking
CFLAGS := -g -O2 -Wall -Wformat -Wunused -Wuninitialized -Wpointer-arith -Wconversion
LFLAGS := -lenet -lboost_program_options
INCLUDES := -I$(COMMON_DIR) -I$(NETWORKING_DIR)
BINARY := client
SOURCES := client.cc configuration.cc game_networking_client.cc game_networking_impl.cc utilities.cc
OBJECTS := $(SOURCES:.cc=.o)

vpath %.cc $(COMMON_DIR) $(NETWORKING_DIR)
vpath %.h $(COMMON_DIR) $(NETWORKING_DIR)

.PHONY: all

all: $(BINARY)

.cc.o:
	$(CXX) $(CFLAGS) $(INCLUDES) -c $<

$(BINARY): $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) -o $(BINARY)-sym $(LFLAGS)
	strip -o $(BINARY) $(BINARY)-sym

clean:
	rm -f *.o $(BINARY) *-sym
