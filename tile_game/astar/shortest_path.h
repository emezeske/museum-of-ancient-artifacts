#ifndef __SHORTEST_PATH__
#define __SHORTEST_PATH__

#include <vector>
#include <boost/graph/adjacency_list.hpp>

#include "geometry.h"

class GameMap;

////////////////////////////////////////////////////////////////////////////////
// The ShortestPathFinder class provides an efficient way to find the shortest
// path from one tile on a GameMap to another.  It contains a graph-based
// representation of the tile topology that must be rebuilt when any tile's
// impassability status changes.
////////////////////////////////////////////////////////////////////////////////

class ShortestPathFinder
{
    public:
        typedef std::size_t Vertex;
        static const Vertex NO_VERTEX = static_cast<Vertex>( -1 );

        void rebuild( GameMap &map );
        bool findShortestPath( Vertex start_index, Vertex goal_index, Vector2DiList &shortest_path );

    private:
        typedef float Cost;
        typedef std::pair<std::size_t, std::size_t> Edge;
        typedef std::vector<Vector2Di> PositionVector;
        typedef std::vector<Edge> EdgeVector;
        typedef std::vector<Cost> CostVector;
        typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, Cost> > TileGraph;
        typedef boost::property_map<TileGraph, boost::edge_weight_t>::type WeightMap;

        void buildEdge( GameMap &map, const int x, const int y, const Vertex index );

        PositionVector tile_positions_;
        EdgeVector edges_;
        TileGraph graph_;
        WeightMap weight_map_;
        std::vector<TileGraph::vertex_descriptor> predecessors_;
        CostVector distance_approximations_;
};

#endif // __SHORTEST_PATH__

