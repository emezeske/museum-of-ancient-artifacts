#include <GL/gl.h>
#include <GL/glut.h>
#include <algorithm>

#include "utilities.h"
#include "game_application.h"

//////////////////////////////////////////////////////////////////////////////////
// Function definitions for GameWindow:
//////////////////////////////////////////////////////////////////////////////////

GameWindow::GameWindow( const int w, const int h, const int bpp, const Uint32 flags, const std::string &title ) : 
    SDL_GL_Window( w, h, bpp, flags, title )
{
}

void GameWindow::createWindow()
{
    SDL_GL_Window::createWindow();

    SDL_ShowCursor( SDL_DISABLE );
    SDL_WM_GrabInput( SDL_GRAB_ON );
}

void GameWindow::initGL()
{
    char *fake_argv[] = { "Fake", NULL };
    int fake_argc = 1;

    SDL_GL_Window::initGL();

    glutInit( &fake_argc, fake_argv ); // FIXME This is really, really dumb!

    GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
  
    glLightfv( GL_LIGHT0, GL_AMBIENT, light_ambient );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, light_diffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular );
    glLightfv( GL_LIGHT0, GL_POSITION, light_position );
  
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
 
    glShadeModel( GL_SMOOTH );
	glEnable( GL_COLOR_MATERIAL );
}

//////////////////////////////////////////////////////////////////////////////////
// Function definitions for GameApplication:
//////////////////////////////////////////////////////////////////////////////////

GameApplication::GameApplication( SDL_GL_Window &initializer, const int fps ) :
	SDL_GL_Interface( initializer, fps ),
	cam( btVector3( 50.0f, 70.0f, 50.0f ), 0.15, -45.0f, 45.0f ),
	simulator()
{
	btTransform trans;
	trans.setIdentity();
	trans.setOrigin( btVector3( 0.0f, -10.0f, 0.0f ) );
	addBasicRigidBody( new RigidBox( 0.0f, btVector3( 10.0f, 10.0f, 10.0f ), trans ) ); 

	trans.setIdentity();
	trans.setOrigin( btVector3( 0.0f, -50.0f, 0.0f ) );
	addBasicRigidBody( new RigidBox( 0.0f, btVector3( 30.0f, 30.0f, 30.0f ), trans ) ); 

	trans.setIdentity();
	trans.setOrigin( btVector3( 0.0f, -40.0f, -125.0f ) );
	addBasicRigidBody( new RigidBox( 0.0f, btVector3( 120.0f, 50.0f, 5.0f ), trans ) ); 

	trans.setIdentity();
	trans.setOrigin( btVector3( 0.0f, -40.0f, 125.0f ) );
	addBasicRigidBody( new RigidBox( 0.0f, btVector3( 120.0f, 50.0f, 5.0f ), trans ) ); 

	trans.setIdentity();
	trans.setOrigin( btVector3( -125.0f, -40.0f, 0.0f ) );
	addBasicRigidBody( new RigidBox( 0.0f, btVector3( 5.0f, 50.0f, 120.0f ), trans ) ); 

	trans.setIdentity();
	trans.setOrigin( btVector3( +125.0f, -40.0f, 0.0f ) );
	addBasicRigidBody( new RigidBox( 0.0f, btVector3( 5.0f, 50.0f, 120.0f ), trans ) ); 

	trans.setIdentity();
	trans.setOrigin( btVector3( 0.0f, -40.0f, 0.0f ) );
	addBasicRigidBody( new RigidTrimesh( 0.0f, trans, new RandHeightfieldRep( 50, 50, 5.0f, 5.0f ) ) );

	for ( int i = 0; i < 30; ++i )
	{
		trans.setIdentity();
		float angle = i / 30.0f * 2 * M_PI;
		trans.setOrigin( btVector3( 20.0f * sin( angle ), ( i + 1 ) * 3.0f, 20.0f * cos( angle ) ) );
		addBasicRigidBody( new RigidSphere( 1.0f, 1.0f, trans ) );
	}

	for ( int i = 0; i < 60; ++i )
	{
		trans.setIdentity();
		btScalar angle = i / 60.0f * 2 * M_PI;
		trans.setOrigin( btVector3( 25.0f * sin( angle ), -20.0f + 3.0f, 25.0f * cos( angle ) ) );
		trans.setRotation( btQuaternion( angle, 0.0f, 0.0f ) );
		addBasicRigidBody( new RigidBox( 1.0f, btVector3( 0.3f, 3.0f, 1.0f ), trans ) );
	}

	for ( int i = 0; i < 60; ++i )
	{
		trans.setIdentity();
		btScalar angle = i / 60.0f * 2 * M_PI;
		trans.setOrigin( btVector3( 25.0f * sin( angle ), -20.0f + 3.0f + 6.0f, 25.0f * cos( angle ) ) );
		trans.setRotation( btQuaternion( angle, 0.0f, 0.0f ) );
		addBasicRigidBody( new RigidBox( 1.0f, btVector3( 0.3f, 3.0f, 1.0f ), trans ) );
	}

	for ( int i = 0; i < 40; ++i )
	{
		trans.setIdentity();
		trans.setOrigin( btVector3( 0.0f, i * 2.0f + 1.0f, 0.0f ) );
		addBasicRigidBody( new RigidBox( 1.0f, btVector3( 1.0f, 1.0f, 1.0f ), trans ) );
	}

	for ( int i = 0; i < 6; ++i )
	{
		for ( int j = 0; j < 5; ++j )
		{
			trans.setIdentity();
			btScalar angle = j / 5.0f * 2 * M_PI + ( i & 1 ? 0.0f : 72.0f );
			trans.setOrigin( btVector3( 5.0f * sin( angle ), i * 4.0f + 2.0f, 5.0f * cos( angle ) ) );
			trans.setRotation( btQuaternion( angle, 0.0f, 0.0f ) );
			addBasicRigidBody( new RigidBox( 1.0f, btVector3( 2.0f, 2.0f, 1.0f ), trans ) );
		}
	}
}

GameApplication::~GameApplication()
{
	for ( BasicRigidBodyVec::iterator i = basicRigidBodies.begin(); i != basicRigidBodies.end(); ++i ) delete *i;
}

void GameApplication::addBasicRigidBody( BasicRigidBody *body )
{
	basicRigidBodies.push_back( body );
	simulator.addRigidBody( body->getBtRigidBody() );
}

void GameApplication::removeBasicRigidBody( BasicRigidBody *body )
{
	BasicRigidBodyVec::iterator i = find( basicRigidBodies.begin(), basicRigidBodies.end(), body );
	if ( i != basicRigidBodies.end() ) basicRigidBodies.erase( i );

	simulator.removeRigidBody( body->getBtRigidBody() );
}

void GameApplication::handleKeyDownEvent( const int key, const int mod )
{
	bool handled = true;

	switch ( key )
	{
		case SDLK_LSHIFT:
			cam.fastMoveMode( true );
			break;

		case SDLK_w:
			cam.moveForward( true );
			break;

		case SDLK_s:
			cam.moveBackward( true );
			break;

		case SDLK_a:
			cam.moveLeft( true );
			break;

		case SDLK_d:
			cam.moveRight( true );
			break;

		case SDLK_e:
			cam.moveUp( true );
			break;

		case SDLK_q:
			cam.moveDown( true );
			break;

		case SDLK_r:
			cleanUpProjectiles();
			simulator.resetCollisionObjects();
			break;

		case SDLK_SPACE:
			printf( "Camera: pitch: %f, yaw: %f\n", cam.pitch, cam.yaw );
			break;

		case SDLK_ESCAPE:
			exit(0);
			break;

		default:
			handled = false;
			break;
	}

	if ( !handled ) SDL_GL_Interface::handleKeyDownEvent( key, mod );
}

void GameApplication::handleKeyUpEvent( const int key, const int mod )
{
	switch ( key )
	{
		case SDLK_LSHIFT:
			cam.fastMoveMode( false );
			break;

		case SDLK_w:
			cam.moveForward( false );
			break;

		case SDLK_s:
			cam.moveBackward( false );
			break;

		case SDLK_a:
			cam.moveLeft( false );
			break;

		case SDLK_d:
			cam.moveRight( false );
			break;

		case SDLK_e:
			cam.moveUp( false );
			break;

		case SDLK_q:
			cam.moveDown( false );
			break;
	}
}

void GameApplication::handleMouseMotionEvent( const int button, const int x, const int y, const int xrel, const int yrel )
{
	cam.handleMouseMotion( xrel, yrel );
}

void GameApplication::handleMouseDownEvent( const int button, const int x, const int y, const int xrel, const int yrel )
{
	switch ( button )
	{
		case SDL_BUTTON_LEFT: fireProjectile( 0.3f, 50.0f ); break;
		case SDL_BUTTON_RIGHT: fireProjectile( 6.0f, 180.0f ); break;
	}
}

void GameApplication::fireProjectile( float radius, float speed )
{
	btVector3 velocity;
	btTransform trans;

	trans.setIdentity();
	trans.setOrigin( cam.pos );

	RigidSphere *s = new RigidSphere( radius, radius, trans ); // For now, 'Something' refers to a ball.

	s->getBtRigidBody()->setLinearVelocity( speed * cam.getDirection( velocity ) );
	projectileRigidBodies.push_back( s );
	addBasicRigidBody( s );
}

void GameApplication::cleanUpProjectiles()
{
	for ( BasicRigidBodyVec::iterator i = projectileRigidBodies.begin(); i != projectileRigidBodies.end(); ++i )
	{
		removeBasicRigidBody( *i );
		delete *i;
	}

	projectileRigidBodies.clear();
}

void GameApplication::doOneStep( double stepTime )
{
	simulator.doOneStep( stepTime );
	cam.doOneStep( stepTime );
}

void GameApplication::draw()
{
	cam.draw(); // Perform camera based translation and rotation.

	for ( BasicRigidBodyVec::iterator i = basicRigidBodies.begin(); i != basicRigidBodies.end(); ++i )
	{
		( *i )->drawDebug();
	}
}
