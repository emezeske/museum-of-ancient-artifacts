# Makefile for the 2DGE experiment.  

# List of the objects needed to create the final executable
2DGE_objects = 	main.o \
		ltga.o \
		vec3d.o \
		isdrawable.o \
		utils.o \
		renderer.o \
		background.o \
		light.o \
		glmanager.o

# Linking directives for ld
2DGE_linkage = -lGLEW -lglut -lGL -lGLU

# The default make target, makes a working 2DGE binary
all: $(2DGE_objects)
	g++ -g -o 2DGE $(2DGE_objects) $(2DGE_linkage)

# Get rid of all the object files and the 2DGE binary.  This is useful when
# a header dependancy is not known about by make -- need to figure out how to
# get make to depend on *.h files
clean:
	rm -f *.o 2DGE

# This is just a convenience target that runs the 2DGE binary if compilation
# succeeds, useful for testing
run: all
	./2DGE
