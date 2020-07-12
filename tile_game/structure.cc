#include "structure.h"
#include "game_map.h"

////////////////////////////////////////////////////////////////////////////////
// Member function definitions for Structure:
////////////////////////////////////////////////////////////////////////////////

Structure::Structure() :
    tile_position_(),
    highlight_mode_( highlight_normal )
{}

////////////////////////////////////////////////////////////////////////////////
// Member function definitions for Wall:
////////////////////////////////////////////////////////////////////////////////

void Wall::draw()
{
    switch ( highlight_mode_ ) // TODO This is dumb, replace with a map lookup.
    {
        case highlight_unplaced: texture_.setColor( ColorRGBA( 1.0f, 1.0f, 1.0f, 0.5f ) ); break;
        case highlight_normal:   texture_.setColor( ColorRGBA( 1.0f, 1.0f, 1.0f, 1.0f ) ); break;
    }

    texture_.drawDimetric( GameMap::centerOfTile( tile_position_ ) );
}

////////////////////////////////////////////////////////////////////////////////
// Member function definitions for TeslaTower:
////////////////////////////////////////////////////////////////////////////////

void TeslaTower::doOneStep( const double ) 
{
}

void TeslaTower::draw()
{
    switch ( highlight_mode_ ) // TODO This is dumb, replace with a map lookup.
    {
        case highlight_unplaced: texture_.setColor( ColorRGBA( 1.0f, 1.0f, 1.0f, 0.5f ) ); break;
        case highlight_normal:   texture_.setColor( ColorRGBA( 1.0f, 1.0f, 1.0f, 1.0f ) ); break;
    }

    texture_.drawDimetric( GameMap::centerOfTile( tile_position_ ) );
}
