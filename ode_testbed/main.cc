#include <ode/ode.h>
#include <GL/glut.h>
#include <float.h>

#include "utilities.h"
#include "timer.h"
#include "fracterrain.h"

const int NUM = 2; // Number of boxes

const dReal
	MASS = 1.0, // Mass of a box.
	SIDE = 1.1732f; // Sphere radius.

const dReal STEP_SIZE = 0.02;

const int
	SCREEN_W = 800,
	SCREEN_H = 600;

// Dynamics and collision objects
dWorldID world;
dSpaceID space;
dBodyID body[NUM];
dJointGroupID contactGroup;
dGeomID geom[NUM];

Fracterrain *terrain;

void nearCallback( void *, dGeomID o1, dGeomID o2 )
{
	dBodyID b1,b2;
	dContact contact;
	
	b1 = dGeomGetBody( o1 );
	b2 = dGeomGetBody( o2 );

	// Exit without doing anything if the two bodies are connected by a joint.
	if ( b1 && b2 && dAreConnected( b1,b2 ) ) return;
	
	contact.surface.mode = 0;
	//contact.surface.mode = dContactBounce | dContactSoftERP;
	contact.surface.bounce = 0.1;
	contact.surface.soft_erp = 0.1;
	contact.surface.mu = 1111110.5;
	contact.surface.mu2 = 0;

	if ( dCollide( o1, o2, 0, &contact.geom, sizeof( dContactGeom ) ) )
	{
		dJointID c = dJointCreateContact( world, contactGroup, &contact );
		dJointAttach( c, b1, b2 );
	}
}

void idle( void )
{
	static Timer tmr;
	static dReal leftoverTime = 0.0;
	dReal elapsedTime = leftoverTime + 0.000001 * tmr.getElapsedTime();

	if ( elapsedTime > STEP_SIZE )
	{
		tmr.reset();

		while ( elapsedTime > STEP_SIZE )
		{
			dSpaceCollide( space, 0, &nearCallback );
			dWorldStep( world, STEP_SIZE );
			dJointGroupEmpty( contactGroup ); // Remove all contact joints.

			elapsedTime -= STEP_SIZE;
		}

		leftoverTime = elapsedTime;
	} 
	glutPostRedisplay();
}

void set_matrix (const float pos[3], const float R[12]) {
}

void display( void )
{
	glClear( GL_COLOR_BUFFER_BIT );

	const dReal *poscam = dBodyGetPosition( body[0] );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 45, (GLdouble)SCREEN_W/(GLdouble)SCREEN_H, 1, 100 );
	gluLookAt( 10, 10, 10, poscam[0], poscam[1], poscam[2], 0.0, 1.0, 0.0 );

	glMatrixMode( GL_MODELVIEW );
	terrain->draw();

	for ( int i = 0; i < NUM; i++ )
	{
		const dReal *pos = dBodyGetPosition( body[i] );
		const dReal *rot = dBodyGetRotation( body[i] );
		float m[16];

m[0] = rot[0];
m[1] = rot[4];
m[2] = rot[8];
m[3] = 0;
m[4] = rot[1];
m[5] = rot[5];
m[6] = rot[9];
m[7] = 0;
m[8] = rot[2];
m[9] = rot[6];
m[10] = rot[10];
m[11] = 0;
m[12] = pos[0];
m[13] = pos[1];
m[14] = pos[2];
m[15] = 1;

		glPushMatrix();
		glMultMatrixf( m );
		glColor3f( 0.9, 0.1, 0.6 );
		//glTranslatef( pos[0], pos[1], pos[2] );
		glutSolidCube( SIDE );
		glPopMatrix();
	}

	glutSwapBuffers();
}

void keyDown( unsigned char k, int, int )
{
	switch ( k )
	{
		case 27:
			exit( 0 );
			break;

		default:
			break;
	}
}

void keyUp( unsigned char, int, int )
{
}

void initWindow( int argc, char **argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	//glutGameModeString( "1024x768:32@60" );
	//glutEnterGameMode();

	glutCreateWindow( "wacky" );
	glutReshapeWindow( SCREEN_W, SCREEN_H );

	glutIgnoreKeyRepeat( 1 );
	
	glutDisplayFunc( display );
	glutKeyboardFunc( keyDown );
	glutKeyboardUpFunc( keyUp );
	glutIdleFunc( idle );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 45, (GLdouble)SCREEN_W/(GLdouble)SCREEN_H, 1, 100 );
	gluLookAt( 10.0, 10.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
	glViewport( 0, 0, SCREEN_W, SCREEN_H );
	glMatrixMode( GL_MODELVIEW );

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );
	glEnable( GL_TEXTURE_2D );
	glutSetCursor( GLUT_CURSOR_NONE );
	glClearColor( 0.0, 0.0, 0.0, 1.0 );

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glutSwapBuffers();
}

dReal getHeight( void *, int x, int y )
{
	return terrain->height( x, y );
}

int main( int argc, char **argv )
{
	initWindow( argc, argv);

	world = dWorldCreate();
	space = dHashSpaceCreate( 0 );
	contactGroup = dJointGroupCreate( 1000000 );

	dWorldSetGravity( world, 0.0, -9.81, 1.0 );
	dWorldSetERP( world, 0.2 ); // Error correction factor.
	dWorldSetCFM( world, 10e-5 ); // Error correction factor.
	dCreatePlane( space, 0, 1, 0, 0 );

	for ( int i = 0; i < NUM; i++ )
	{
		dMass m;
		body[i] = dBodyCreate( world );
		dBodySetPosition( body[i], 0, 5 + 2 * i * SIDE, 0 );
		dMassSetBox( &m, 1, SIDE, SIDE, SIDE );
		dMassAdjust( &m, MASS );
		dBodySetMass( body[i], &m );
		geom[i] = dCreateBox( space, SIDE, SIDE, SIDE );
		dGeomSetBody( geom[i], body[i] );
	}

	terrain = new Fracterrain( time( NULL ), 10, 0.54 );
	int width = Fracterrain::TER_W;

	dHeightfieldDataID heightfieldDataID = dGeomHeightfieldDataCreate();
	//dGeomHeightfieldDataBuildSingle( heightfieldDataID, terrain->data(), 0, width, width, width, width, 0, 0, 0, 0 );
	dGeomHeightfieldDataBuildCallback( heightfieldDataID, 0, &getHeight, width, width, width, width, 1, 0, 0, 300.1 );
	dGeomID heightfieldGeomID = dCreateHeightfield( space, heightfieldDataID, 0 );

	glutMainLoop();

	dJointGroupDestroy( contactGroup );
	dWorldDestroy( world );
	dCloseODE();

	return 0;
}
