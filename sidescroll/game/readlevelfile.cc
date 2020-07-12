#include <vector>
#include <fstream>
#include "defines.h"
#include "worldobject.h"
#include "fixedworldobject.h"

extern int LEVEL_WIDTH;
extern int LEVEL_HEIGHT;
extern int COL_BLOCK_SIZE;

extern bool ** collisionBlocks;
extern std::vector<FixedWorldObject *> pWorldObjectVector[2];

void readFromFile(void) {

	// open file for reading
	std::ifstream new_level_file;
	new_level_file.open("level1.lvl");

	// populate pWorldObjectVector with objects from file
	int numFWO;

	for(int which = 0; which < 2; ++which) {

		new_level_file >> numFWO;
		for(int i = 0; i < numFWO; ++i) {
			pWorldObjectVector[which].push_back(new FixedWorldObject() );
			new_level_file >> pWorldObjectVector[which][i]->x;
			new_level_file >> pWorldObjectVector[which][i]->y;
			new_level_file >> pWorldObjectVector[which][i]->width;
			new_level_file >> pWorldObjectVector[which][i]->height;
			new_level_file >> pWorldObjectVector[which][i]->textureNumber;
			++(pWorldObjectVector[which][i]->textureNumber);
		} // end for

	} // end for
	
	// read the collisions array from the file
	new_level_file >> LEVEL_WIDTH;
	new_level_file >> LEVEL_HEIGHT;
	new_level_file >> COL_BLOCK_SIZE;

	collisionBlocks = new bool*[LEVEL_WIDTH / COL_BLOCK_SIZE];
	for(int i = 0; i < LEVEL_WIDTH / COL_BLOCK_SIZE; ++i) 
		collisionBlocks[i] = new bool[LEVEL_HEIGHT / COL_BLOCK_SIZE];

	for(int x = 0; x < LEVEL_WIDTH / COL_BLOCK_SIZE; ++x) 
		for(int y = 0; y < LEVEL_HEIGHT / COL_BLOCK_SIZE; ++y) 
			new_level_file >> collisionBlocks[x][y];

	new_level_file.close();
	
} // end readFromFile()

