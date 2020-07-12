#ifndef __GAME_MAP__
#define __GAME_MAP__

#include <set>
#include <memory>

#include "drawing_utilities.h"
#include "geometry.h"
#include "path_finder.h"
#include "structure.h"

enum Base
{
    BASE_A,
    BASE_B
};

////////////////////////////////////////////////////////////////////////////////
// The Tile class contains the representation for a map tile.
////////////////////////////////////////////////////////////////////////////////

class Tile
{
    public:
        Tile() : 
            impasse_( false ),
            structure_( 0 )
        {}

        ~Tile() { removeStructure(); }

        void setStructure( Structure *s );
        void removeStructure();
        void attach( Drawable *d ) { fleeting_occupants_.insert( d ); }
        void draw();

        bool impasse_;

    private:
        std::set<Drawable *> fleeting_occupants_;
        Structure *structure_;
};

////////////////////////////////////////////////////////////////////////////////
// The GameMap structure contains the representation for the playing field.  It
// provides the graphical representation and also keeps track of the map tiles.
////////////////////////////////////////////////////////////////////////////////

class GameMap : public Drawable
{
    public:
        GameMap( const std::string bg_filename, const Vector2Di &size );

        static int absoluteToIndex( const float a );
        static Vector2Di absoluteToIndex( const Vector2Df &p );
        static float centerOfTile( const int i );
        static Vector2Df centerOfTile( const Vector2Di &p );

        Tile &tileAt( const int x, const int y );
        Tile &tileAt( const Vector2Di &p );
        Tile &tileAt( const Vector2Df &p );

        bool isTileAt( const int x, const int y ) const;
        bool isTileAt( const Vector2Di &p ) const;
        bool isTileAt( const Vector2Df &p ) const;
        bool findPathToBase( const Vector2Di &start, const Base base, Vector2DiList &shortest_path );
        bool nextStepToBase( const Vector2Di &start, Vector2Di &next );
        bool nextStepToBase( const Vector2Di &start, const Base base, Vector2Di &next );
        bool impasseChangeIsOK( const Vector2Di &p, const bool new_impasse );

        bool setImpasse( const Vector2Di &p, bool new_impasse );
        bool setStructure( const Vector2Di &p, Structure *s );
        bool removeStructure( const Vector2Di &p );

        void attachToTile( const Vector2Di &p, Drawable *d );
        Vector2Di basePosition( const Base base ) const;

        Vector2Di size() const { return tiles_.size(); }
        int sizeX() const { return size().x_; }
        int sizeY() const { return size().y_; }
        int numTiles() const { return sizeX() * sizeY(); }

        virtual void draw();

    private:
        static const int 
            TILE_HEIGHT = 32,
            TILE_WIDTH  = 64;

        void forceSetStructure( const Vector2Di &p, Structure *s );

        TextureMap background_texture_;
        Vector2DiList base_to_base_path_;

        Matrix2D<Tile> tiles_;
        TextureMap path_debug_texture_;
        std::auto_ptr<PathFinder> base_b_path_finder_;
};

#endif // __GAME_MAP__

