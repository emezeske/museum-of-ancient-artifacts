#ifndef __STRUCTURE_H__
#define __STRUCTURE_H__

#include "drawing_utilities.h"
#include "geometry.h"

////////////////////////////////////////////////////////////////////////////////
// The Drone class provides an abstract base class for autonomous moving units.
////////////////////////////////////////////////////////////////////////////////

class Structure : public Drawable
{
    public:
        enum HighlightMode
        {
            highlight_unplaced,
            highlight_normal
        };

        Structure();
        virtual ~Structure() {}

        virtual void setTilePosition( const Vector2Di& p ) { tile_position_ = p; }
        virtual bool impassable() { return true; }

        virtual void doOneStep( const double step_time ) = 0;
        virtual void draw() = 0;

        virtual void setHighlightMode( const HighlightMode mode ) { highlight_mode_ = mode; }

    protected:
        Vector2Di tile_position_;
        HighlightMode highlight_mode_;
};

class Wall : public Structure
{
    public:
        Wall() : 
            texture_( "wall.png" )
        {}

        virtual void doOneStep( const double ) {}
        virtual void draw();

    protected:
        TextureMap texture_;
};

class TeslaTower : public Structure
{
    public:
        TeslaTower() : 
            texture_( "tesla_tower.png" )
        {}

        virtual void doOneStep( const double step_time );
        virtual void draw();

    protected:
        TextureMap texture_;
};

#endif // __STRUCTURE_H__

