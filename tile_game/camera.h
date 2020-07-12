#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "sdl_gl_interface.h"
#include "geometry.h"

////////////////////////////////////////////////////////////////////////////////
// The Camera class keeps track of the current viewing window.
////////////////////////////////////////////////////////////////////////////////

class Camera
{
	public:
		Camera( Vector2Df position );

		void draw() const;
		void doOneStep( const double step_time );

        Vector2Di translateMouseCoordinates( const Vector2Di &mouse_coordinates ) const;

		void moveUp( bool m ) { accelerating_up_ = m; }
		void moveDown( bool m ) { accelerating_down_ = m; }
		void moveLeft( bool m ) { accelerating_left_ = m; }
		void moveRight( bool m ) { accelerating_right_ = m; }

	private:
		static const float
			ACCELERATION      = 4000.0f,
			MAX_SPEED         = 1000.0f;

		Vector2Df
            position_,
		    velocity_;

		bool
			accelerating_up_,
			accelerating_down_,
			accelerating_left_,
			accelerating_right_;
};

#endif // __CAMERA_H__
