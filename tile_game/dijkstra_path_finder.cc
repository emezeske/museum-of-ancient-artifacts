#include <iostream>
#include <iomanip>
#include <algorithm>

#include "dijkstra_path_finder.h"
#include "utilities.h"
#include "sdl_utilities.h"
#include "game_map.h"

using namespace std;

DijkstraPathFinder::DijkstraPathFinder( GameMap &map, const Vector2Di &goal ) :
    map_( map ),
    tile_vertices_( map_.size() ),
    vertices_( 2 * map_.numTiles() ), // This may reserve more vertices than necessary, but never less.
    goal_( goal )
{}

DijkstraPathFinder::VertexIndex DijkstraPathFinder::vertexIndex( const Vector2Di &pos )
{
    return ( tile_vertices_.includes( pos ) ? tile_vertices_.at( pos ) : NO_INDEX );
}

DijkstraPathFinder::Vertex *DijkstraPathFinder::vertexAt( const Vector2Di &pos )
{
    VertexIndex i = vertexIndex( pos );
    return ( i == NO_INDEX ? 0 : &vertices_[i] );
}

bool DijkstraPathFinder::shortestPath( const Vector2Di &start, Vector2DiList &shortest_path )
{
    bool success = false;

    shortest_path.clear();

    Vertex *next_vertex = vertexAt( start );
    
    while ( !success && next_vertex )
    {
        shortest_path.push_back( next_vertex->position_ );
    
        if ( next_vertex->position_ == goal_ ) success = true;
    
        next_vertex = next_vertex->predecessor_;
    }

    return success;
}

bool DijkstraPathFinder::nextStep( const Vector2Di &start, Vector2Di &next )
{
    bool success = false;

    Vertex *start_vertex = vertexAt( start );

    if ( start_vertex && start_vertex->predecessor_ ) 
    {
        next = start_vertex->predecessor_->position_;
        success = true;
    }

    return success;
}

DijkstraPathFinder::VertexVector::iterator DijkstraPathFinder::closestVertexNeedingVisit( VertexVector &vertices, VertexVectorItList &vertices_needing_visit )
{
    // Find the vertex that is nearest the goal but still has not been visited.
    VertexVector::iterator min_it = vertices.end();
    VertexVectorItList::iterator min_it_it = min_element( vertices_needing_visit.begin(), vertices_needing_visit.end(), VertexDistanceCompare() );

    if ( min_it_it != vertices_needing_visit.end() )
    {
        // We are visiting this node right now, so remove it from the "needing visit" list and mark it as visited.
        vertices_needing_visit.erase( min_it_it );
        min_it = *min_it_it;
        min_it->visited_ = true;
    }

    return min_it;
}

void DijkstraPathFinder::addVertexNeedingVisit( VertexVectorItList &vertices_needing_visit, VertexVector::iterator it )
{
    // Only allow one copy of 'it' in the 'vertices_needing_visit' list.
    if ( find( vertices_needing_visit.begin(), vertices_needing_visit.end(), it ) == vertices_needing_visit.end() )
    {
        vertices_needing_visit.push_back( it );
    }
}

void DijkstraPathFinder::addAdjacentVertex( Vertex &vertex, const int x_offset, const int y_offset )
{
    VertexIndex adjacent_index = vertexIndex( Vector2Di( vertex.position_.x_ + x_offset, vertex.position_.y_ + y_offset ) );

    if ( adjacent_index != NO_INDEX ) vertex.adjacent_.push_back( adjacent_index );
}

void DijkstraPathFinder::rebuild()
{
    SCOPE_TIMER_BEGIN

    // Keep a linked list of the vertices that need to be visited.  This information is redundant with Vertex.visited_,
    // but the list is faster for looking up the next vertex that needs to be visited, and the member variable is faster
    // for checking if adjacent vertices have been visited.
    VertexVectorItList vertices_needing_visit;

    vertices_.clear();

    // Build the vertex list from the GameMap topology.
    for ( int x = 0; x < map_.sizeX(); ++x )
    {
        for ( int y = 0; y < map_.sizeY(); ++y )
        {
            if ( !map_.tileAt( x, y ).impasse_ )
            {
                tile_vertices_.at( x, y ) = vertices_.size();

                vertices_.push_back( Vertex( Vector2Di( x, y ) ) );

                if ( x == goal_.x_ && y == goal_.y_ ) 
                {
                    vertices_.back().distance_ = 0; // The shortest path from the start to itself has length zero.
                    addVertexNeedingVisit( vertices_needing_visit, vertices_.end() - 1 );
                }
            }
            else tile_vertices_.at( x, y ) = NO_INDEX;
        }
    }

    // Each vertex needs a list of its adjacent neighbors.
    for ( VertexVector::iterator vit = vertices_.begin(); vit != vertices_.end(); ++vit )
    {
        addAdjacentVertex( *vit,  0,  1 );
        addAdjacentVertex( *vit,  1,  0 );
        addAdjacentVertex( *vit,  0, -1 );
        addAdjacentVertex( *vit, -1,  0 );
    }

    // Begin Dijkstra's Algorithm:
    for ( VertexIndex c = 0; c < vertices_.size(); ++c )
    {
        VertexVector::iterator min_it = closestVertexNeedingVisit( vertices_, vertices_needing_visit );

        if ( min_it != vertices_.end() ) // This should not happen, but better safe than sorry.
        {
            if ( !min_it->is_max() ) // The closest vertex will have a max distance if no path exists from it to the goal; skip these disconnected vertices.
            {
                // Check to see if we have found a new shortest path for any of this vertex' adjacent neighbors.
                for ( IndexVector::iterator index_it = min_it->adjacent_.begin(); index_it != min_it->adjacent_.end(); ++index_it )
                {
                    VertexVector::iterator adjacent_it = vertices_.begin() + *index_it;

                    if ( !adjacent_it->visited_ )
                    {
                        Distance distance_estimate = min_it->distance_ + VERTEX_WEIGHT;
                        addVertexNeedingVisit( vertices_needing_visit, adjacent_it );

                        if ( distance_estimate < adjacent_it->distance_ ) // We've found a better path for this neighbor.
                        {
                            adjacent_it->distance_ = distance_estimate;
                            adjacent_it->predecessor_ = &(*min_it);
                        }
                    }
                }
            }
        }
    }

    SCOPE_TIMER_END

    // DEBUG: Print each vertex' distance from the goal in a grid:
    //
    for ( int y = map_.sizeY() - 1; y >= 0; --y )
    {
        for ( int x = 0; x < map_.sizeX(); ++x )
        {
            Vertex *v = vertexAt( Vector2Di( x, y ) );

            if ( v )
            {
                if ( !v->is_max() )
                {
                    cout << setw( 3 ) << v->distance_;
                }
                else cout << "???";
            }
            else cout << "---";

            cout << " ";
        }

        cout << endl;
    }
}

