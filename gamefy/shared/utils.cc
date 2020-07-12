#include <stdio.h>

#include "vector.h"

void readString(FILE *f, char string[255]) {
	do{
		fgets(string, 255, f);
	} while(string[0] == '#');
} // end function readString()

