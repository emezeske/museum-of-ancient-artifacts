#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__
#include <list>

typedef float coord_t;

class point_2d
{
	public:
		coord_t x, y;

		point_2d() : x( 0.0f ), y( 0.0f ) { }
		point_2d( const point_2d &rhs ) : x( rhs.x ), y( rhs.y ) { }
		point_2d( const coord_t new_x, const coord_t new_y) : x( new_x ), y( new_y ) { }

		virtual ~point_2d() { }
}; // end class point_2d

class line_2d
{
	public:
		point_2d	begin,	// begin is the start point of this line
					vec; 	// vec can either be a displacement vector
							// or a direction vector

		line_2d() : begin(), vec() { }
		line_2d( const line_2d &rhs ) : begin( rhs.begin ), vec( rhs.vec ) { }
		line_2d( const point_2d &new_b, const point_2d &new_e ) :
				begin( new_b ), vec( new_e ) { }
		line_2d( const coord_t bX, const coord_t bY, const coord_t eX, const coord_t eY ) :
				begin( bX , bY ), vec( eX, eY ) { }

		inline coord_t slope() const { return (vec.y / vec.x ); }

		inline coord_t y_intercept() const { return ( begin.y - slope() * begin.x ); }

		inline bool is_point_above( const point_2d &p ) const
		{ 
				coord_t m = slope();
				return ( m * p.x - p.y + ( begin.y - slope() * begin.x ) > 0);
		}
}; // end class line_2d

class polygon 
{
	public:
		// The last point in the vertices list is connected to the first point
		std::list< point_2d > vertices;
		typedef std::list< point_2d >::iterator vertices_iter;
}; // end class polygon
#endif
