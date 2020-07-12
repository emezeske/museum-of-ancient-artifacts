#include "drone.h"
#include "utilities.h"

////////////////////////////////////////////////////////////////////////////////
// Member function definitions for Drone:
////////////////////////////////////////////////////////////////////////////////

void Drone::attachToTile()
{
    map_.attachToTile( GameMap::absoluteToIndex( position_ ), this );
}

void Drone::doOneStep( const double step_time )
{
    float distance_to_target = target_position_.distance( position_ );
    
    if ( distance_to_target > FUZZ_FACTOR )
    {
        Vector2Df travel = target_position_ - position_;
    
        float 
            theta         = atan2( travel.y_, travel.x_ ),
            movement      = speed() * step_time,
            overshoot     = movement - distance_to_target,
            leftover_time = 0.0f;
    
        if ( overshoot > FUZZ_FACTOR )
        {
            movement = distance_to_target;
            leftover_time = overshoot / speed(); // TODO If speed() will ever be zero, this needs to change.
        }
    
        movement = ( movement <= distance_to_target ) ? movement : distance_to_target;
    
        position_.x_ += movement * cos( theta );
        position_.y_ += movement * sin( theta );
    
        if ( leftover_time > FUZZ_FACTOR ) doOneStep( leftover_time );
    }
    else
    {
        Vector2Di target_tile;

        if ( map_.nextStepToBase( GameMap::absoluteToIndex( position_ ), goal_, target_tile ) )
        {
            target_position_ = GameMap::centerOfTile( target_tile );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Static variable definitions for HoverDrone:
////////////////////////////////////////////////////////////////////////////////

const float HoverDrone::SIZE;

////////////////////////////////////////////////////////////////////////////////
// Member function definitions for HoverDrone:
////////////////////////////////////////////////////////////////////////////////

void HoverDrone::doOneStep( const double step_time )
{
    Drone::doOneStep( step_time );

    hover_position_ += hover_direction_ * HOVER_SPEED * step_time;

    if ( hover_position_ > HOVER_DISTANCE )
    {
        hover_position_ = HOVER_DISTANCE;
        hover_direction_ = -hover_direction_;
    }
    else if ( hover_position_ < 0.0f )
    {
        hover_position_ = 0.0f;
        hover_direction_ = -hover_direction_;
    }
}

void HoverDrone::draw()
{
    if ( highlight_ )
    {
        texture_.setColor( ColorRGBA( 0.3f, 1.0f, 0.3f, 1.0f ) );
    }
    else texture_.setColor( ColorRGBA( 1.0f, 1.0f, 1.0f, 1.0f ) );

    texture_.drawDimetric( position_, hover_position_ );
}

void HoverDrone::takeDamage( int damage )
{
    hit_points_ -= damage;
    VNOTIFY( VERBOSE, "Took %d damage, hit_points_ is now %d.", damage, hit_points_ );
}

bool HoverDrone::readyForCleanup()
{
    bool ready = false;

    if ( GameMap::absoluteToIndex( target_position_ ) == map_.basePosition( goal_ ) )
    {
        ready = target_position_.distance( position_ ) <= FUZZ_FACTOR;
    }

    return ready;
}

