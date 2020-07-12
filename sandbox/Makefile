# Makefile for test

CXX := g++
# For memory debugging:
#CXX := yamd-g++
CWARNINGS := -Wall -W -Wpointer-arith -Wcast-qual -Wwrite-strings -Wconversion -Winline
CFLAGS := -g $(CWARNINGS)
LFLAGS := -l boost_system -l boost_regex
BINARY_NAME := test
INCLUDE_DIRS :=
DEPEND_DIR := ./.depend

SOURCES := test.cc

OBJECTS := $(SOURCES:.cc=.o)

vpath %.cc
vpath %.h

.PHONY: all clean common_tags tags all_tags

.cc.o:
	$(CXX) $(CFLAGS) $(INCLUDE_DIRS) -c $<

all: $(OBJECTS)
	$(CXX) -o $(BINARY_NAME)-sym $(OBJECTS) $(LFLAGS)
	strip -o $(BINARY_NAME) $(BINARY_NAME)-sym

clean:
	rm -f *.o $(BINARY_NAME) *-sym core *~ .*~ a.out proxy/*~
	rm -rf $(DEPEND_DIR)

tags: *.cc *.h
	ctags $^

$(DEPEND_DIR):
	@mkdir $(DEPEND_DIR)

$(DEPEND_DIR)/%.d: %.cc $(DEPEND_DIR)
	@set -e; $(CC) -MM $(CFLAGS) $(INCLUDE_DIRS) $< | sed 's#\($*\)\.o[ :]*#\1.o $@ : #g' > $@; [ -s $@ ] || rm -f $@

-include $(patsubst %,$(DEPEND_DIR)/%,$(subst .cc,.d,$(SOURCES)))

