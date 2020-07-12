#include <GL/gl.h>
#include <math.h>
#include "Player.h"

editefyPlayer::editefyPlayer(float X, float Y, float Z) {
	x = X; y = Y; z = Z;
	xv = yv = zv = 0.0;
	pitch = yaw = 0.0;
	speed = 0.00004;
	strafeSpeed = 0.00002;
	altSpeed = 0.00002;
	feetOnGround = movinForward = movinBackward = movinLeft = movinRight = movinDown = movinUp = jumpin = false;
	gravity = 0.000008;
} // end constructor editefyPlayer()

void editefyPlayer::adjustVelocity( GLfloat adjustVmag, GLfloat adjustVangle, int ms ) {
	GLfloat actualAngleInRadians( ( yaw + adjustVangle ) * M_PI / 180.0 );
	xv += -sin( actualAngleInRadians ) * adjustVmag * ms;
	zv += -cos( actualAngleInRadians ) * adjustVmag * ms;
} // end function adjustVelocity()

void editefyPlayer::adjustAltitudeVelocity(float spd, int ms) {
	yv += spd*ms;
} // end function adjustAltitudeVelocity()

void editefyPlayer::Reset() {
	xv = yv = zv = 0.0;
} // end function Reset()

void editefyPlayer::move( int ms ) {

	// slow down
	xv *= 1.0-(0.002*ms);
	yv *= 1.0-(0.002*ms);
	zv *= 1.0-(0.002*ms);

	// are we movin?
	if(movinForward)
		adjustVelocity(speed, 0.0, ms);
	if(movinBackward)
		adjustVelocity(speed, 180.0, ms);
	if(movinLeft)
		adjustVelocity(strafeSpeed, 90.0, ms);
	if(movinRight)
		adjustVelocity(strafeSpeed, 270.0, ms);
	if(movinUp)
		adjustAltitudeVelocity(altSpeed, ms);
	if(movinDown)
		adjustAltitudeVelocity(-altSpeed, ms);

	// decelerate if touching the ground
	if(feetOnGround) { 	xv *= 1-(0.005*ms);
				zv *= 1-(0.005*ms);}

	// move player
	x += xv*ms;
	y += yv*ms;
	z += zv*ms;

} // end function move()

void editefyPlayer::moveForward(bool movin) {
	movinForward = movin;
} // end function moveForward()

void editefyPlayer::moveBackward(bool movin) {
	movinBackward = movin;
} // end function moveBackward()

void editefyPlayer::moveLeft(bool movin) {
	movinLeft = movin;
} // end function moveLeft()

void editefyPlayer::moveRight(bool movin) {
	movinRight = movin;
} // end function moveRight()

void editefyPlayer::moveUp(bool movin) {
	movinUp = movin;
} // end function moveRight()

void editefyPlayer::moveDown(bool movin) {
	movinDown = movin;
} // end function moveRight()

void editefyPlayer::jump(bool jumpin) {
	this->jumpin = jumpin;
} // end function jump()
