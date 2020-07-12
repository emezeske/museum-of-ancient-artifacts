#include "geometry.h"
#include "utilities.h"

inline bool between( wu_t n, wu_t bound1, wu_t bound2 )
{
	return ( ( n > bound1 && n < bound2 ) || ( n > bound2 && n < bound1 ) );
}

bool ray_2d::overlaps( const ray_2d &r ) const
{
	// TODO: Robustify this.  Currently has bad behavior when
	// a line is vertical or horizontal.
	wu_t 
		ms = _displacement._y / _displacement._x,
		mr = r._displacement._y / r._displacement._x,
		x = ( ms * _origin._x - _origin._y - mr * r._origin._x + r._origin._y ) / ( ms - mr );

	return ( 	between( x, _origin._x, _origin._x + _displacement._x ) &&
				between( x, r._origin._x, r._origin._x + r._displacement._x ) );
}

// Determine whether the endpoint of one ray touches another ray.
bool endpoint_touches_line( const ray_2d &e, const ray_2d &l, vec_2d &where, vec_2d &norm )
{
	bool touches = false;

	wu_t 
		m = l._displacement.slope(),

		x = l._origin._x,
		xh = l._origin._x + l._displacement._x,
		y = l._origin._y,

		xe = e._origin._x,
		ye = e._origin._y;
	
	if ( between( xe, x, xh ) && geom::fuzzy_cmp( 0.0, m * ( x - xe ) + ye - y ) )
	{
		touches = true;
	}
	else 
	{
		xe += e._displacement._x;
		ye += e._displacement._y;

		if ( between( xe, x, xh ) && geom::fuzzy_cmp( 0.0, m * ( x - xe ) + ye - y ) )
		{
			touches = true;
		}
	}

	if ( touches ) 
	{
		where = vec_2d( xe, ye );
		norm = l._displacement.normal().normalize();
	}

	return touches;
}

bool ray_2d::touches( const ray_2d &r, vec_2d &where, vec_2d &norm ) const
{
	return ( 	endpoint_touches_line( *this, r, where, norm ) ||
				endpoint_touches_line( r, *this, where, norm ) );
}

void poly_2d::add_vertex( const vec_2d &v )
{
	_edges.push_back( ray_2d( v, vec_2d() ) );
}

void poly_2d::precalculate()
{
	unsigned n_edges = _edges.size();

	for ( unsigned i = 0; i < n_edges; ++i )
	{
		_edges[ i ]._displacement = vec_2d( ( _edges[ i + 1 < n_edges ? i + 1 : 0 ] )._origin - _edges[ i ]._origin );
	}
}

bool poly_2d::overlaps( const poly_2d &p ) const
{
	bool overlap = false;

	for ( unsigned i = 0; i < _edges.size() && !overlap; ++i )
	{
		for ( unsigned z = 0; z < p._edges.size(); ++z )
		{
			if ( _edges[ i ].overlaps( p._edges[ z ] ) )
			{
				overlap = true;
				break;
			}
		}
	}

	return overlap;
}

bool poly_2d::touches( const poly_2d &p, vec_2d &where, vec_2d &norm ) const
{
	bool touches = false;

	for ( unsigned i = 0; i < _edges.size() && !touches; ++i )
	{
		for ( unsigned z = 0; z < p._edges.size(); ++z )
		{
			if ( _edges[ i ].touches( p._edges[ z ], where, norm ) )
			{
				touches = true;
				break;
			}
		}
	}
	
	return touches;
}

void poly_2d::rotate( wu_t theta )
{
	matrix2x2 rm = matrix2x2::get_rotation_matrix( theta );

	for ( ray_2d_vector_iter i = _edges.begin(); i != _edges.end(); ++i )
	{
		( *i )._origin = rm * ( *i )._origin; 
	}

	precalculate();
}

void poly_2d::translate( vec_2d v )
{
	for ( ray_2d_vector_iter i = _edges.begin(); i != _edges.end(); ++i )
	{
		( *i )._origin = ( *i )._origin + v; 
	}
}

bounding_box::bounding_box( const poly_2d &p )
{
	bool begun = false;

	for ( ray_2d_vector_citer i = p._edges.begin(); i != p._edges.end(); ++i )
	{
		const wu_t x = ( *i )._origin._x;
		const wu_t y = ( *i )._origin._y;

		if ( !begun )
		{
			_ll._x = x; _ll._y = y;
			_ur._x = x; _ur._y = y;
			begun = true;
		}
		else
		{
			if ( x < _ll._x ) _ll._x = x;
			if ( y < _ll._y ) _ll._y = y;
			if ( x > _ur._x ) _ur._x = x;
			if ( y > _ur._y ) _ur._y = y;
		}
	}
}

bool bounding_box::overlaps( const bounding_box &b ) const
{
	return	(	between( _ll._x, b._ll._x, b._ur._x ) &&
 				between( _ll._y, b._ll._y, b._ur._y ) )
				||
			(	between( _ur._x, b._ll._x, b._ur._x ) &&
 				between( _ur._y, b._ll._y, b._ur._y ) )
				||
			(	between( _ll._x, b._ll._x, b._ur._x ) &&
 				between( _ur._y, b._ll._y, b._ur._y ) )
				||
			(	between( _ur._x, b._ll._x, b._ur._x ) &&
 				between( _ll._y, b._ll._y, b._ur._y ) );
}
