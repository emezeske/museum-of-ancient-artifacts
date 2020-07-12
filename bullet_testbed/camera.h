#ifndef CAMERA_H
#define CAMERA_H

#include "sdl_gl_interface.h"
#include "LinearMath/btVector3.h"

class Camera
{
	public:
		Camera( btVector3 position, btScalar sensitivity, btScalar p, btScalar y );

		void draw() const;
		void doOneStep( double stepTime );
		void handleMouseMotion( int xrel, int yrel );
		btVector3 &getDirection( btVector3 &dir );

		void fastMoveMode( bool m ) { movingFast = m; }
		void moveForward( bool m ) { movingForward = m; }
		void moveBackward( bool m ) { movingBackward = m; }
		void moveLeft( bool m ) { movingLeft = m; }
		void moveRight( bool m ) { movingRight = m; }
		void moveUp( bool m ) { movingUp = m; }
		void moveDown( bool m ) { movingDown = m; }

		btVector3 pos;

		btScalar
			mouseSensitivity,
			pitch,
			yaw;

	private:
		void moveForward( btScalar movementUnits );
		void strafe( btScalar movementUnits );

		static const float
			CAMERA_SPEED = 30.0f,
			CAMERA_FAST_MODE_FACTOR = 5.0f;

		bool
			movingFast,
			movingForward,
			movingBackward,
			movingLeft,
			movingRight,
			movingUp,
			movingDown;
};

#endif // CAMERA_H
