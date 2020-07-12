#include <math.h>
#include <algorithm>

#include "game_map.h"
#include "utilities.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Member function definitions for Tile:
////////////////////////////////////////////////////////////////////////////////

void Tile::setStructure( Structure *s )
{
    removeStructure();
    structure_ = s;
}

void Tile::removeStructure()
{
    if ( structure_ )
    {
        delete structure_;
        structure_ = 0;
    }
}

void Tile::draw()
{
    for ( set<Drawable *>::const_iterator i = fleeting_occupants_.begin(); i != fleeting_occupants_.end(); ++i )
    {
        (*i)->draw();
    }

    fleeting_occupants_.clear();

    if ( structure_ ) structure_->draw();
}

////////////////////////////////////////////////////////////////////////////////
// Static function definitions for GameMap:
////////////////////////////////////////////////////////////////////////////////

int GameMap::absoluteToIndex( const float a )
{
    return static_cast<int>( floor( a ) );
}

Vector2Di GameMap::absoluteToIndex( const Vector2Df &p )
{
    return Vector2Di( absoluteToIndex( p.x_ ), absoluteToIndex( p.y_ ) );
}

float GameMap::centerOfTile( const int i )
{
    return i + 1.0f / 2.0f;
}

Vector2Df GameMap::centerOfTile( const Vector2Di &p )
{
    return Vector2Df( centerOfTile( p.x_ ), centerOfTile( p.y_ ) );
}

////////////////////////////////////////////////////////////////////////////////
// Member function definitions for GameMap:
////////////////////////////////////////////////////////////////////////////////

GameMap::GameMap( const string bg_filename, const Vector2Di &size ) :
    background_texture_( bg_filename ),
    tiles_( size ),
    path_debug_texture_( "path_tile.png" ),
    base_b_path_finder_( new PathFinder( *this, basePosition( BASE_B ) ) )
{
    for ( int i = 1; i < 28; i += 4 )
    {
        for ( int x = 0; x < sizeX() - 1; ++x )
        {
            forceSetStructure( Vector2Di( x, i ), new Wall() );
        }

        for ( int x = 1; x < sizeX(); ++x )
        {
            forceSetStructure( Vector2Di( x, i + 2 ), new Wall() );
        }
    }

    base_b_path_finder_->rebuild();
}

Tile &GameMap::tileAt( const int x, const int y )
{
    return tiles_.at( x, y );
}

Tile &GameMap::tileAt( const Vector2Di &p )
{
    return tileAt( p.x_, p.y_ );
}

Tile &GameMap::tileAt( const Vector2Df &p )
{
    return tileAt( absoluteToIndex( p ) );
}

bool GameMap::isTileAt( const int x, const int y ) const
{
    return ( x >= 0 && y >= 0 && x < sizeX() && y < sizeY() );
}

bool GameMap::isTileAt( const Vector2Di &p ) const
{
    return isTileAt( p.x_, p.y_ );
}

bool GameMap::isTileAt( const Vector2Df &p ) const
{
    return isTileAt( absoluteToIndex( p.x_ ), absoluteToIndex( p.y_ ) );
}

bool GameMap::findPathToBase( const Vector2Di &start, const Base base, Vector2DiList &shortest_path )
{
    bool found = false;

    switch ( base )
    {
        case BASE_B: 
            found = base_b_path_finder_->shortestPath( start, shortest_path );
            break;

        default:
            VNOTIFY( FAULT, "No PathFinder has been built with goal of base: %d", base );
    }

    return found;
}

bool GameMap::nextStepToBase( const Vector2Di &start, const Base base, Vector2Di &next )
{
    bool found = false;

    switch ( base )
    {
        case BASE_B: 
            found = base_b_path_finder_->nextStep( start, next );
            break;

        default:
            VNOTIFY( FAULT, "No PathFinder has been built with goal of base: %d", base );
    }

    return found;
}

bool GameMap::impasseChangeIsOK( const Vector2Di &p, const bool new_impasse )
{
    bool ok = true;

    // TODO Maybe the rebuild should always be attempted, so that stranded dones can be caught...

    if ( !new_impasse || base_to_base_path_.empty() || std::find( base_to_base_path_.begin(), base_to_base_path_.end(), p ) != base_to_base_path_.end() )
    {
        Vector2DiList trial_path;
        auto_ptr<PathFinder> trial_path_finder( new PathFinder( *this, basePosition( BASE_B ) ) );

        trial_path_finder->rebuild();

        if ( trial_path_finder->shortestPath( basePosition( BASE_A ), trial_path ) )
        {
            base_b_path_finder_ = trial_path_finder;
            base_to_base_path_ = trial_path;
        }
        else ok = false;
    }

    return ok;
}

bool GameMap::setImpasse( const Vector2Di &p, bool new_impasse )
{
    bool success = false;

    if ( isTileAt( p ) )
    {
        Tile &t = tileAt( p );
        
        if ( t.impasse_ != new_impasse )
        {
            t.impasse_ = new_impasse;
            
            if ( !impasseChangeIsOK( p, new_impasse ) )
            {
                t.impasse_ = !new_impasse; // Revert this tile back to its initial state.
                if ( !new_impasse ) NOTIFY( FAULT, "WARNING: removing an impasse broke a path!?" );
            }
            else success = true;
        }
    }

    return success;
}

bool GameMap::setStructure( const Vector2Di &p, Structure *s )
{
    bool success = setImpasse( p, s->impassable() );

    if ( success )
    {
        s->setTilePosition( p );
        tileAt( p ).setStructure( s ); // This transfers ownership of s to the Tile object.
    }
    else delete s;

    return success;
}

bool GameMap::removeStructure( const Vector2Di &p )
{
    bool success = setImpasse( p, false );

    if ( success ) tileAt( p ).removeStructure();

    return success;
}

void GameMap::attachToTile( const Vector2Di &p, Drawable *d )
{
    if ( isTileAt( p ) ) tileAt( p ).attach( d );
}

Vector2Di GameMap::basePosition( const Base base ) const
{ 
    Vector2Di position = Vector2Di( 0, 0 );

    switch ( base )
    {
        case BASE_A: position = Vector2Di( 0, 0 ); break;
        case BASE_B: position = Vector2Di( sizeX() - 1, sizeY() - 1 ); break;
        default: VNOTIFY( FAULT, "position for unknown base '%d' requested.", base );
    }

    return position;
}

void GameMap::draw()
{
    background_texture_.rasterize( Vector2Di( 0, 0 ), Vector2Di( 0, -512 ) );

    for ( Vector2DiList::const_iterator i = base_to_base_path_.begin(); i != base_to_base_path_.end(); ++i )
    {
        path_debug_texture_.drawDimetric( centerOfTile( *i ) );
    }

    for ( int y = sizeY() - 1; y >= 0; --y )
    {
        for ( int x = 0; x < sizeX(); ++x )
        {
            tileAt( x, y ).draw();
        }
    }
}

void GameMap::forceSetStructure( const Vector2Di &p, Structure *s )
{
    Tile &t = tileAt( p );
    s->setTilePosition( p );
    t.impasse_ = s->impassable();
    t.setStructure( s ); // This transfers ownership of s to the Tile object.
}

