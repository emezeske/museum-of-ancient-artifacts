#include <SDL/SDL_opengl.h>

#include "camera.h"
#include "utilities.h"

////////////////////////////////////////////////////////////////////////////////
// Member function definitions for Camera:
////////////////////////////////////////////////////////////////////////////////

Camera::Camera( Vector2Df position ) :
	position_( position ),
	velocity_( 0.0f, 0.0f ),
	accelerating_up_( false ),
	accelerating_down_( false ),
	accelerating_left_( false ),
	accelerating_right_( false )
{
}

void Camera::draw() const
{
    Vector2Di pixel_position = roundVector( position_ );

	glTranslatef( -pixel_position.x_, -pixel_position.y_, 0.0f );
}

void Camera::doOneStep( const double step_time )
{
    float dv = ACCELERATION * step_time;

    if ( !accelerating_up_ && !accelerating_down_ && !accelerating_left_ && !accelerating_right_ )
    {
        float v = velocity_.length();
        if ( v > 0.0f ) velocity_ *= std::max( v - dv, 0.0f ) / v;
    }
    else
    {
	    if ( accelerating_up_ )    velocity_ += Vector2Df(  0.0f,  dv   );
	    if ( accelerating_down_ )  velocity_ += Vector2Df(  0.0f, -dv   );
	    if ( accelerating_left_ )  velocity_ += Vector2Df( -dv,    0.0f );
	    if ( accelerating_right_ ) velocity_ += Vector2Df(  dv,    0.0f );
    }

    float v = velocity_.length();
    if ( v > MAX_SPEED ) velocity_ *= MAX_SPEED / v;

    position_ += velocity_ * step_time;
}

Vector2Di Camera::translateMouseCoordinates( const Vector2Di &mouse_coordinates ) const
{
    return roundVector( position_ ) + Vector2Di( mouse_coordinates.x_, mouse_coordinates.y_ );
}

