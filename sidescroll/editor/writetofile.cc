#include <vector>
#include <stdio.h>
#include <fstream>
#include "defines.h"
#include "worldobject.h"
#include "fixedworldobject.h"

extern int LEVEL_WIDTH;
extern int LEVEL_HEIGHT;
extern int COL_BLOCK_SIZE;

extern bool ** collisionBlocks;
extern std::vector<FixedWorldObject *> vectorFWO[2];

void writeToFile(char * curLevFile) {
	std::ofstream level_file;
	level_file.open(curLevFile);

	// write the fixed world objects to the file
	for(int which = 0; which < 2; ++which) {

		level_file << vectorFWO[which].size() << '\n';

		for(int i = 0; i < vectorFWO[which].size(); ++i) {
			level_file << vectorFWO[which][i]->x << '\n';
			level_file << vectorFWO[which][i]->y << '\n';
			level_file << vectorFWO[which][i]->width << '\n';
			level_file << vectorFWO[which][i]->height << '\n';
			level_file << vectorFWO[which][i]->textureNumber << '\n';
		} // end for

	} // end for

	// write the collisions array to the file
	level_file << LEVEL_WIDTH  << '\n';
	level_file << LEVEL_HEIGHT << '\n';
	level_file << COL_BLOCK_SIZE << '\n';
	for(int x = 0; x < LEVEL_WIDTH / COL_BLOCK_SIZE; ++x) 
		for(int y = 0; y < LEVEL_HEIGHT / COL_BLOCK_SIZE; ++y) 
			level_file << collisionBlocks[x][y] << '\n';

	level_file.close();
} // end writeToFile()

void readFromFile(char * curLevFile) {

	// clean up stuff that we are going to read from file
	for(int which = 0; which < 2; ++which) {

		if(!vectorFWO[which].empty()){
			for(int i = 0; i < vectorFWO[which].size(); ++i)
				delete vectorFWO[which][i];
			vectorFWO[which].clear();
		} 

	} // end for

	if(collisionBlocks){
		for(int i = 0; i < LEVEL_WIDTH / COL_BLOCK_SIZE; ++i)
			delete [] collisionBlocks[i];
		delete [] collisionBlocks;
	}

	// open file for reading
	std::ifstream new_level_file;
	new_level_file.open(curLevFile);

	// populate vectorFWO with objects from file
	int numFWO;
	for(int which = 0; which < 2; ++which) {

		new_level_file >> numFWO;

		for(int i = 0; i < numFWO; ++i) {
			vectorFWO[which].push_back(new FixedWorldObject() );
			new_level_file >> vectorFWO[which][i]->x;
			new_level_file >> vectorFWO[which][i]->y;
			new_level_file >> vectorFWO[which][i]->width;
			new_level_file >> vectorFWO[which][i]->height;
			new_level_file >> vectorFWO[which][i]->textureNumber;
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

