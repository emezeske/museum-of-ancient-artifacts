#include "physics_obj.h"
#include "utilities.h"

physics_obj physics_obj::get_next_pos( wu_t delta, vec_2d acc )
{
	physics_obj np = physics_obj( *this );

	vec_2d acc_times_dt = acc * delta;
	np._vel += acc_times_dt;
	np._pos += np._vel * delta + acc_times_dt * delta / 2.0;

	wu_t racc_times_dt = _racc * delta;
	np._vrot += racc_times_dt;
	np._rot += np._vrot * delta + racc_times_dt * delta / 2.0;

	return np;
}

void physics_obj::update_world_poly()
{
	_world_poly = _shape;
	_world_poly.rotate( _rot );
	_world_poly.translate( _pos );

	_bb = bounding_box( _world_poly );
}

void physics_obj::precalculate()
{
	wu_t max_dist = 0.0;

	for ( ray_2d_vector_citer i = _shape._edges.begin(); i != _shape._edges.end(); ++i )
	{
		const wu_t d = ( *i )._origin.mag();

		if ( d > max_dist ) max_dist = d;
	}

	_radius = max_dist;
}

collision_type physics_obj::check_collision( physics_obj &po, collision_desc &col )
{
	collision_type col_type = COLLISION_NONE;

	wu_t r_sum =_radius + po._radius;
	
	// First approximation is a circle test
	if ( ( _pos - po._pos ).mag_sq() < r_sum * r_sum )
	{
		// Then an axis-aligned bounding box test
		if ( _bb.overlaps( po._bb ) )
		{
			// Then a full-blown edge vs edge test
			if ( _world_poly.overlaps( po._world_poly ) )
			{
				col_type = COLLISION_OVERLAP;
			}
			else
			{
				if ( _world_poly.touches( po._world_poly, col._where, col._norm ) )
				{
					vec_2d 
						v1 = point_velocity( col._where ),
						v2 = po.point_velocity( col._where ),
						c2c = ( _pos - po._pos ).normalize();

					wu_t relative_vel = c2c * ( v1 - v2 );

					if ( relative_vel < 0 )
					{
						col._po1 = this;
						col._po2 = &po;
						col._rel_vel = relative_vel;
						col_type = COLLISION_TOUCH;
						VNOTIFY( VERBOSE, "rel vel: %f", relative_vel );
					}
				}
			}
		}
	}

	return col_type;
}

vec_2d physics_obj::point_velocity( const vec_2d &v ) const
{
	return _vel + ( ( v - _pos ).normal() * _vrot );
}

/* static */ void physics_obj::handle_collision( collision_desc &col )
{
	physics_obj 
		&po1 = *col._po1,
		&po2 = *col._po2;

	vec_2d
		norm = col._norm,
		where = col._where,
		r1 = ( where - po1._pos ).normal(),
		r2 = ( where - po2._pos ).normal(),
		v1 = po1.point_velocity( where ),
		v2 = po2.point_velocity( where );

	wu_t
		relative_vel = norm * ( v1 - v2 ),
		numerator = -( 1 + po1._elasticity * po2._elasticity ) * relative_vel,
		nr1 = r1 * norm,
		nr2 = r2 * norm,
		trm1 = 1 / po1._mass,
		trm2 = 1 / po2._mass,
		trm3 = ( nr1 * nr1 ) / po1._mo_inertia,
		trm4 = ( nr2 * nr2 ) / po2._mo_inertia,
		impulse = numerator / ( trm1 + trm2 + trm3 + trm4 );

		VNOTIFY( VERBOSE, "impulse: %f", impulse );
		VNOTIFY( VERBOSE, "po1._vrot: %f po2._vrot: %f", po1._vrot, po2._vrot );

		po1._vel += norm * ( impulse / po1._mass );
		po1._vrot += nr1 * ( impulse  / po1._mo_inertia );

		po2._vel -= norm * ( impulse / po2._mass );
		po2._vrot -= nr2 * ( impulse / po2._mo_inertia );

		VNOTIFY( VERBOSE, ":po1._vrot: %f po2._vrot: %f", po1._vrot, po2._vrot );
}
