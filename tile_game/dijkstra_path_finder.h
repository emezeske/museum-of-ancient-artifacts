#ifndef __DIJKSTRA_PATH_FINDER_H__
#define __DIJKSTRA_PATH_FINDER_H__

#include <vector>
#include <limits>
#include <list>

#include "geometry.h"

class GameMap;

class DijkstraPathFinder
{
    public:
        DijkstraPathFinder( GameMap &map, const Vector2Di &goal );

        void rebuild();
        bool shortestPath( const Vector2Di &start, Vector2DiList &shortest_path );
        bool nextStep( const Vector2Di &start, Vector2Di &next );

    private:
        typedef float Distance; // The type used to represent the distance from a vertex to the goal.
        typedef unsigned long VertexIndex; typedef std::vector<VertexIndex> IndexVector; 
        static const VertexIndex NO_INDEX = static_cast<VertexIndex>( -1 );
        static const Distance VERTEX_WEIGHT = 1.0f; // TODO This should be per-vertex (but at the moment all vertices have the same weight).

        struct Vertex
        {
            Vertex() {}

            Vertex( const Vector2Di &position ) :
                distance_( std::numeric_limits<Distance>::max() ),
                visited_( false ),
                position_( position ),
                predecessor_( 0 )
            {}

            bool is_max() const { return distance_ == std::numeric_limits<Distance>::max(); }
        
            Distance distance_;    // The best known distance from this vertex to the goal.
            bool visited_;         // Indicates whether this vertex has been visited on this pass.
            Vector2Di position_;
            Vertex *predecessor_;  // Points to the adjacent vertex that is closed to the goal.
            IndexVector adjacent_; // The indices of any adjacent vertices.
        };

        typedef std::vector<Vertex> VertexVector;
        typedef std::list<VertexVector::iterator> VertexVectorItList; // A list of iterators to vertices that may be nearest the goal.

        struct VertexDistanceCompare
        {
            bool operator()( const VertexVector::iterator a, const VertexVector::iterator b )
            {
                return a->distance_ < b->distance_;
            }
        };

        VertexVector::iterator closestVertexNeedingVisit( VertexVector &vertices, VertexVectorItList &needs_visit );
        void addVertexNeedingVisit( VertexVectorItList &needs_visit, VertexVector::iterator it );
        void addAdjacentVertex( Vertex &vertex, const int x_offset, const int y_offset );

        VertexIndex vertexIndex( const Vector2Di &pos ); // Returns the index of the vertex at 'pos' in 'vertices_', or NO_VERTEX.
        Vertex *vertexAt( const Vector2Di &pos ); // Returns the vertex at 'pos' or 0.

        GameMap &map_; // TODO Make this a template parameter so this class can be used in other contexts.
        Matrix2D<VertexIndex> tile_vertices_; // Maps tile positions to vertex indices for fast lookup.
        VertexVector vertices_;
        Vector2Di goal_;
};

#endif // __DIJKSTRA_PATH_FINDER_H__
