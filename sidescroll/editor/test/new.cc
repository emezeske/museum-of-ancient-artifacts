#include <iostream>
#include "poop.h"

void turdBurgle(void);

Poop poop;

Poop::Poop(void) {

	Roger = 22;
	Herb  = 33;
	pRoger = &Roger;

}; // end Poop()

int main(void) {


turdBurgle();
poop.pRoger = &poop.Herb;
turdBurgle();

} // end main()
