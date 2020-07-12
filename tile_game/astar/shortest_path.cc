#include <boost/graph/astar_search.hpp>

#include "shortest_path.h"
#include "game_map.h"
#include "utilities.h"

using namespace boost;

////////////////////////////////////////////////////////////////////////////////
// Helper templates for ShortestPathFinder:
////////////////////////////////////////////////////////////////////////////////

template <class Graph, class CostType>
class distance_heuristic : public astar_heuristic<Graph, CostType>
{
    public:
        typedef typename graph_traits<Graph>::vertex_descriptor Vertex;

        distance_heuristic( const ShortestPathFinder::PositionVector &positions, Vertex goal ) : 
            positions_( positions ), 
            goal_( goal ) 
        {}

        CostType operator()( Vertex u )
        {
            // The A* algorithm is much faster if the distance heuristic can provide an accurate estimate
            // of the shortest distance to the goal.  This accomplishes that for adjacent tiles.
            return ( positions_[goal_].x_ - positions_[u].x_ ) + ( positions_[goal_].y_ - positions_[u].y_ );
        }

    private:
        const ShortestPathFinder::PositionVector &positions_;
        Vertex goal_;
};

struct found_goal {}; // This exception class is used to indicate that the shortest path search may be terminated.

template <class Vertex>
class astar_goal_visitor : public boost::default_astar_visitor
{
    public:
        astar_goal_visitor( Vertex goal ) : goal_( goal ) {}

        template <class Graph>
        void examine_vertex( Vertex u, Graph & ) { if( u == goal_ ) throw found_goal(); }

    private:
        Vertex goal_;
};

////////////////////////////////////////////////////////////////////////////////
// Member function definitions for ShortestPathFinder:
////////////////////////////////////////////////////////////////////////////////

void ShortestPathFinder::rebuild( GameMap &map )
{
    tile_positions_.reserve( map.numTiles() );
    tile_positions_.clear();

    graph_ = TileGraph( map.numTiles() ); // Note that this is an over-estimate of the number of vertices in the graph.
    weight_map_ = get( edge_weight, graph_ );

    predecessors_ = std::vector<TileGraph::vertex_descriptor>( num_vertices( graph_ ) );
    distance_approximations_ = CostVector( num_vertices( graph_ ) );

    for ( int x = 0; x < map.sizeX(); ++x )
    {
        for ( int y = 0; y < map.sizeY(); ++y )
        {
            Tile &t = map.tileAt( x, y );

            if ( !t.impasse_ )
            {
                t.topology_index_ = tile_positions_.size();
                tile_positions_.push_back( Vector2Di( x, y ) );

                buildEdge( map, x,     y - 1, t.topology_index_ );
                buildEdge( map, x - 1, y    , t.topology_index_ );
            }
            else t.topology_index_ = NO_VERTEX;
        }
    }
}

bool ShortestPathFinder::findShortestPath( Vertex start_index, Vertex goal_index, Vector2DiList &shortest_path )
{
    bool found = false;

    shortest_path.clear();

    try
    {
        // Perform the A* search.
        astar_search( graph_, start_index,
                      distance_heuristic<TileGraph, Cost>( tile_positions_, goal_index ),
                      predecessor_map( &predecessors_[0] ).
                      distance_map( &distance_approximations_[0] ).
                      visitor( astar_goal_visitor<Vertex>( goal_index ) ) );
    } 
    catch ( found_goal fg )
    {
        found = true;

        for ( Vertex v = goal_index;; v = predecessors_[v] )
        {
            shortest_path.push_back( tile_positions_[v] );
            if( predecessors_[v] == v ) break;
        }
    }

    return found;
}

void ShortestPathFinder::buildEdge( GameMap &map, const int x, const int y, const Vertex index )
{
    if ( map.isTileAt( x, y ) )
    {
        Tile &t = map.tileAt( x,y );
        
        if ( !t.impasse_ )
        {
            TileGraph::edge_descriptor e; bool inserted;
            tie( e, inserted ) = add_edge( index, t.topology_index_, graph_ );
            weight_map_[e] = 1.0f; // NOTE: This assumes that all tiles are equally difficult to cross.
        }
    }
}

