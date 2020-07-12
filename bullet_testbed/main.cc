#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

#include "utilities.h"
#include "game_application.h"
#include "exceptions.h"

const int
	SCREEN_WIDTH  = 1280,
	SCREEN_HEIGHT = 1024,
	SCREEN_BPP    = 32;

const Uint32 SCREEN_FLAGS = SDL_HWSURFACE | SDL_OPENGL;

int main( int argc, char **argv )
{
	int result = -1;

    try
    {
        GameWindow game_window( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SCREEN_FLAGS, "Physics Game" );
        GameApplication game( game_window, 60 );
        game.mainLoop();
        result =  0;
    }
    catch ( LoadingError e ) { VNOTIFY( FAULT, "LoadingError: %s.", e.what().c_str() ); }

	return result;
}
