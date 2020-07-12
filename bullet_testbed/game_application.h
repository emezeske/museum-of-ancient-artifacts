#ifndef GAME_APPLICATION_H
#define GAME_APPLICATION_H

#include <vector>
#include <LinearMath/btVector3.h>

#include "sdl_gl_interface.h"
#include "camera.h"
#include "simulator.h"
#include "basic_rigid_body.h"

class GameWindow : public SDL_GL_Window
{
    public:
        GameWindow( const int w, const int h, const int bpp, const Uint32 flags, const std::string &title );
        virtual ~GameWindow() {}

        virtual void createWindow();

    protected:
        virtual void initGL();
};

class GameApplication : public SDL_GL_Interface
{
	public:
        GameApplication( SDL_GL_Window &initializer, const int fps );
		virtual ~GameApplication();

	protected:
		virtual void addBasicRigidBody( BasicRigidBody *body );
		virtual void removeBasicRigidBody( BasicRigidBody *body );

		virtual void handleKeyDownEvent( const int key, const int mod );
		virtual void handleKeyUpEvent( const int key, const int mod );
		virtual void handleMouseMotionEvent( const int button, const int x, const int y, const int xrel, const int yrel );
		virtual void handleMouseDownEvent( const int button, const int x, const int y, const int xrel, const int yrel );
		virtual void fireProjectile( float radius, float speed );
		virtual void cleanUpProjectiles();
		virtual void doOneStep( double stepTime );
		virtual void draw();

		Camera cam;
		Simulator simulator;

		typedef std::vector< BasicRigidBody *> BasicRigidBodyVec;

		BasicRigidBodyVec
			basicRigidBodies,
			projectileRigidBodies;
};

#endif // GAME_APPLICATION_H
