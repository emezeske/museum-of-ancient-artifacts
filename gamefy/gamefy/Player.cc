#include <GL/gl.h>
#include <stdio.h>
#include <math.h>
#include "Player.h"

Player::Player(float X, float Y, float Z) {
	x = X; y = Y; z = Z;
	xv = yv = zv = 0.0;
	pitch = yaw = 0.0;
	speed = 0.00004;
	strafeSpeed = 0.00002;
	movinForward = movinBackward = movinLeft = movinRight = jumpin = false;
	feetOnGround = true;
	gravity = 0.000008;
} // end constructor Player()

void Player::adjustVelocity( GLfloat adjustVmag, GLfloat adjustVangle, int ms ) {
	GLfloat actualAngleInRadians( ( yaw + adjustVangle ) * M_PI / 180.0 );
	xv += -sin( actualAngleInRadians ) * adjustVmag * ms;
	zv += -cos( actualAngleInRadians ) * adjustVmag * ms;
} // end function adjustVelocity()

void Player::move( int ms ) {
	// gravity pulls down
	yv -= gravity*ms;

	// are feet on the ground?
	if(feetOnGround = (y < 1.21)) {
		yv = 0.0;
		if(jumpin) 
			yv = +0.0018;
	} // end if

	// are we movin?
	if(movinForward)
		adjustVelocity(speed, 0.0, ms);
	if(movinBackward)
		adjustVelocity(speed, 180.0, ms);
	if(movinLeft)
		adjustVelocity(strafeSpeed, 90.0, ms);
	if(movinRight)
		adjustVelocity(strafeSpeed, 270.0, ms);

	// slow if touching the ground
	if(feetOnGround) { 	xv *= 1-(0.005*ms);
				zv *= 1-(0.005*ms);}

	// move player
	x += xv*ms;
	y += yv*ms;
	z += zv*ms;

} // end function move()

void Player::moveForward(bool movin) {
	movinForward = movin;
} // end function moveForward()

void Player::moveBackward(bool movin) {
	movinBackward = movin;
} // end function moveBackward()

void Player::moveLeft(bool movin) {
	movinLeft = movin;
} // end function moveLeft()

void Player::moveRight(bool movin) {
	movinRight = movin;
} // end function moveRight()

void Player::jump(bool jumpin) {
	this->jumpin = jumpin;
} // end function jump()
