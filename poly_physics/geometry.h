#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <cmath>

typedef float wu_t; // World Unit Type

struct vec_2d
{
	wu_t
		_x,
		_y;

	vec_2d() : _x( 0.0 ), _y( 0.0 ) {};

	vec_2d( wu_t x, wu_t y ) : _x( x ), _y( y ) {};

	vec_2d( const vec_2d &rhs ) 
	{
		_x = rhs._x;
		_y = rhs._y;
	}

	wu_t mag_sq() const
	{
		return _x * _x + _y * _y;
	}

	wu_t mag() const
	{
		return sqrt( mag_sq() );
	}

	wu_t slope() const
	{
		return _y / _x;
	}

	vec_2d normalize()
	{
		wu_t len = mag();
		_x /= len;
		_y /= len;
		return *this;
	}

	vec_2d normal() const
	{
		return vec_2d( -_y, _x );
	}
	
	vec_2d operator - ( const vec_2d &rhs ) const
	{
		return vec_2d( _x - rhs._x, _y - rhs._y );
	}
	
	vec_2d operator -= ( const vec_2d &rhs )
	{
		return ( *this = *this - rhs );
	}

	vec_2d operator + ( const vec_2d &rhs ) const
	{
		return vec_2d( _x + rhs._x, _y + rhs._y );
	}

	vec_2d operator += ( const vec_2d &rhs )
	{
		return ( *this = *this + rhs );
	}

	vec_2d operator * ( const wu_t s ) const
	{
		return vec_2d( _x * s, _y * s );
	}

	// Dot Product
	wu_t operator * ( const vec_2d &rhs ) const
	{
		return  _x * rhs._x + _y * rhs._y;
	}

	vec_2d operator / ( const wu_t s ) const
	{
		return vec_2d( _x / s, _y / s );
	}
};

struct ray_2d
{
	vec_2d
		_origin,
		_displacement;

	ray_2d() : _origin(), _displacement() {};

	ray_2d( vec_2d origin, vec_2d terminant ) :
		_origin( origin ),
		_displacement( terminant - origin ) {};

	ray_2d( const ray_2d &rhs )
	{
		_origin = rhs._origin;
		_displacement = rhs._displacement;
	}

	bool overlaps( const ray_2d &r ) const;
	bool touches( const ray_2d &r, vec_2d &where, vec_2d &norm ) const;
};

typedef std::vector< ray_2d > ray_2d_vector;
typedef std::vector< ray_2d >::iterator ray_2d_vector_iter;
typedef std::vector< ray_2d >::const_iterator ray_2d_vector_citer;

struct poly_2d
{
	ray_2d_vector _edges;

	poly_2d() : _edges() {};
	
	poly_2d( const poly_2d &rhs ) :
		_edges( rhs._edges )
	{}

	void add_vertex( const vec_2d &v );
	void precalculate();

	bool overlaps( const poly_2d &p ) const;
	bool touches( const poly_2d &p, vec_2d &where, vec_2d &norm ) const;

	void rotate( wu_t theta );
	void translate( vec_2d v );
};

typedef std::vector< poly_2d * > poly_2d_vec;
typedef std::vector< poly_2d * >::iterator poly_2d_vec_iter;

struct bounding_box
{
	vec_2d 
		_ll,
		_ur;

	bounding_box() : _ll(), _ur() {}

	bounding_box( const poly_2d &p );

	bool overlaps( const bounding_box &b ) const;
};

struct matrix2x2
{
	wu_t 
		_r0c0, _r0c1,
		_r1c0, _r1c1;

	matrix2x2( wu_t r0c0, wu_t r0c1, wu_t r1c0, wu_t r1c1 ) :
		_r0c0( r0c0 ), _r0c1( r0c1 ), _r1c0( r1c0 ), _r1c1( r1c1 )
	{}

	vec_2d operator * ( const vec_2d &v )
	{
		return vec_2d( v._x * _r0c0 + v._y * _r1c0, v._x * _r0c1 + v._y * _r1c1 );
	}

	static matrix2x2 get_rotation_matrix( wu_t theta )
	{
		// TODO: Figure out why theta has to be inverted...
		return matrix2x2( 	cos( -theta ), -sin( -theta ),
							sin( -theta ),  cos( -theta ) );
	}
};

namespace geom 
{
	
const wu_t
	deg_per_rad = 180.0 / M_PI,
	fuzziness = 0.01; 	// This value affects how many times a particular simulation step
						// must be broken down to find a collision.  The lower it is, the
						// more steps, which means more accuracy and more clock cycles.
	
inline bool fuzzy_cmp( wu_t lhs, wu_t rhs )
{
	return lhs > rhs - fuzziness && lhs < rhs + fuzziness;
}

inline wu_t distance( vec_2d a, vec_2d b )
{
	wu_t 
		dx = a._x - b._x,
		dy = a._y - b._y;

	return sqrt( dx * dx + dy * dy );
}

inline wu_t dist_orig( vec_2d v )
{
	return sqrt( v._x * v._x + v._y * v._y  );
}

} // end namespace geom

#endif // GEOMETRY_H
