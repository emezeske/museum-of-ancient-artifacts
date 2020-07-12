#include <GL/gl.h>

#include "camera.h"

Camera::Camera( btVector3 position, btScalar sensitivity, btScalar p, btScalar y ) :
	pos( position ),
	mouseSensitivity( sensitivity ),
	pitch( p ),
	yaw( y ),
	movingFast( false ),
	movingForward( false ),
	movingBackward( false ),
	movingLeft( false ),
	movingRight( false ),
	movingUp( false ),
	movingDown( false )
{
}

void Camera::draw() const
{
	glRotatef( -pitch, 1.0f, 0.0f, 0.0f );
	glRotatef( -yaw, 0.0f, 1.0f, 0.0f );
	glTranslatef( -pos.x(), -pos.y(), -pos.z() );
}


void Camera::doOneStep( double stepTime )
{
	btScalar movementUnits = stepTime * CAMERA_SPEED;

	if ( movingFast ) movementUnits *= CAMERA_FAST_MODE_FACTOR;

	if ( movingForward ) moveForward( movementUnits );
	if ( movingBackward ) moveForward( -movementUnits );
	if ( movingLeft ) strafe( movementUnits );
	if ( movingRight ) strafe( -movementUnits );
	if ( movingUp ) pos += btVector3( 0.0f, movementUnits, 0.0f );
	if ( movingDown ) pos -= btVector3( 0.0f, movementUnits, 0.0f );
}

void Camera::handleMouseMotion( int xrel, int yrel )
{
	pitch -= mouseSensitivity * yrel;
	yaw -= mouseSensitivity * xrel;

	if ( yaw > 360.0f || yaw < -360.0f ) yaw = 0;
	if ( pitch > 90.0f ) pitch = 90.0f;
	else if ( pitch < -90.0f ) pitch = -90.f;
}

btVector3 &Camera::getDirection( btVector3 &dir )
{
	double
		pitchRadians = pitch * M_PI / 180.0f,
		yawRadians = yaw * M_PI / 180.0f,
		yd = sin( pitchRadians ),
		forwardFactor = cos( pitchRadians ),
		xd = -sin( yawRadians ) * forwardFactor,
		zd = -cos( yawRadians ) * forwardFactor;

	return ( dir = btVector3( xd, yd, zd ).normalize() );
}

void Camera::moveForward( btScalar movementUnits )
{
	btVector3 dir;
	pos += getDirection( dir ) * movementUnits;
}

void Camera::strafe( btScalar movementUnits )
{
	double
		yawRadians = yaw * M_PI / 180.0f + 0.5f * M_PI,
		xd = movementUnits * -sin( yawRadians ),
		zd = movementUnits * -cos( yawRadians );

	pos += btVector3( xd, 0.0f, zd );
}
