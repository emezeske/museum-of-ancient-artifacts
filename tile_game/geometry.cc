#include "geometry.h"

//////////////////////////////////////////////////////////////////////////////////
// Constant definitions:
//////////////////////////////////////////////////////////////////////////////////

const float TILE_SIZE = 32.0f; // TODO This seems like an odd place for this to be.  Is it?

//////////////////////////////////////////////////////////////////////////////////
// Function definitions:
//////////////////////////////////////////////////////////////////////////////////

Vector2Di roundVector( const Vector2Df &v )
{
    return Vector2Di( static_cast<int>( roundf( v.x_ ) ), static_cast<int>( roundf( v.y_ ) ) );
}

Vector2Di worldToDimetric( const Vector2Df &w )
{
    return roundVector( Vector2Df( TILE_SIZE * ( w.x_ + w.y_ ), TILE_SIZE * ( -w.x_ / 2 + w.y_ / 2 ) ) );
}

Vector2Df dimetricToWorld( const Vector2Di &d )
{
    return Vector2Df( ( d.x_ / 2 - d.y_ ) / TILE_SIZE, ( d.x_ / 2 + d.y_ ) / TILE_SIZE );
}

