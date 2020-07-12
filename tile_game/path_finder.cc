#include "path_finder.h"
#include "dijkstra_path_finder.h"
#include "game_map.h"

PathFinder::PathFinder( GameMap &map, const Vector2Di &goal ) :
    impl_( new DijkstraPathFinder( map, goal ) )
{}

PathFinder::~PathFinder()
{
    delete impl_;
}

void PathFinder::rebuild()
{
    impl_->rebuild();
}

bool PathFinder::shortestPath( const Vector2Di &start, Vector2DiList &shortest_path )
{
    return impl_->shortestPath( start, shortest_path );
}

bool PathFinder::nextStep( const Vector2Di &start, Vector2Di &next )
{
    return impl_->nextStep( start, next );
}

