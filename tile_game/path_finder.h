#ifndef __PATH_FINDER_H__
#define __PATH_FINDER_H__

#include "geometry.h"

class GameMap;
class DijkstraPathFinder;

class PathFinder
{
    public:
        PathFinder( GameMap &map, const Vector2Di &goal );
        ~PathFinder();

        void rebuild();
        bool shortestPath( const Vector2Di &start, Vector2DiList &shortest_path );
        bool nextStep( const Vector2Di &start, Vector2Di &next );

    private:
        DijkstraPathFinder *impl_;
};

#endif // __PATH_FINDER_H__
