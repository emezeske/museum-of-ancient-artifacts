#ifndef __DRONE_H__
#define __DRONE_H__

#include "drawing_utilities.h"
#include "geometry.h"
#include "game_map.h"

////////////////////////////////////////////////////////////////////////////////
// The Drone class provides an abstract base class for autonomous moving units.
////////////////////////////////////////////////////////////////////////////////

class Drone : public Drawable
{
    public:
        Drone( GameMap &map, const Vector2Df &position, const Base goal ) : 
            map_( map ),
            position_( position ),
            target_position_( position ),
            goal_( goal ),
            highlight_( false )
        {}

        virtual ~Drone() {}

        virtual void attachToTile();
        virtual void doOneStep( const double step_time );
        virtual void draw() = 0;

        virtual void takeDamage( int damage ) = 0;
        virtual bool readyForCleanup() = 0;
        virtual float speed() = 0;

    protected:
        GameMap &map_;

        Vector2Df 
            position_,
            target_position_;

        Base goal_;

        bool highlight_; // Highlight this Drone (debug).
};

////////////////////////////////////////////////////////////////////////////////
// The HoverDrone class is a kind of Drone that hovers above the ground.
////////////////////////////////////////////////////////////////////////////////

class HoverDrone : public Drone
{
    public:
        HoverDrone( GameMap &map, const Vector2Df &position, const Base goal ) : 
            Drone( map, position, goal ),
            hover_position_( 0.0f ),
            hover_direction_( 1 ),
            hit_points_( 100 ),
            texture_( "drone_e.png" )
        {}

        virtual void doOneStep( const double step_time );
        virtual void draw();

        virtual void takeDamage( int damage );
        virtual bool readyForCleanup();
        virtual float speed() { return SPEED; }

    protected:
        static const float 
            SIZE           = 32.0f,
            SPEED          = 2.0f,
            HOVER_DISTANCE = 3.0f,
            HOVER_SPEED    = 9.0f;

        float hover_position_;
        int 
            hover_direction_,
            hit_points_;

        TextureMap texture_;
};

#endif // __DRONE_H__

