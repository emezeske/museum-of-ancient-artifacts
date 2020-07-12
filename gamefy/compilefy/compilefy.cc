#include <math.h>
#include "../shared/utils.h"
#include "../shared/gWorld.h"
#include "../shared/gCWorld.h"

Tri *triangle;			// for both file in and out
gWorld *world;

void die(const char *msg) {
	printf(msg);
	exit(1);
} // end function die()

int main(int argc, char **argv) {
	FILE *fileIn;
	char input_filename[256] = "./outworld";
	char aline[256];
	int numtextures = 0;

	if(argc<2)				die("Usage:\n\tcompilefy source.map\n\tTry again.\n");
	strcpy(input_filename, argv[1]);
	fileIn = fopen(input_filename, "rt");
	if(fileIn == NULL)			die("Could not open %s for reading.\n\tTry again.\n");

	world = new gWorld(input_filename);
	triangle = new Tri[world->sectors->polygons * 2];
	numtextures = world->numtextures;

	// bust quads into 2 triangles
	wQuad *quad = world->sectors->quads;

	unsigned int t = 0;
	while(quad != NULL) {
		triangle[t].vertex[0] = quad->vertex[1];
		triangle[t].vertex[1] = quad->vertex[3];
		triangle[t].vertex[2] = quad->vertex[0];
		triangle[t].texture   = quad->texture;
		++t;
		triangle[t].vertex[0] = quad->vertex[3];
		triangle[t].vertex[1] = quad->vertex[1];
		triangle[t].vertex[2] = quad->vertex[2];
		triangle[t].texture   = quad->texture;
		++t;
		quad = quad->next;
	} // end for q

	gCWorld *oL = new gCWorld(numtextures, t, triangle);

	return 0;
} // end function main()

