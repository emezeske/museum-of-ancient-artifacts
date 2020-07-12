#ifndef PHYSICS_OBJ_H
#define PHYSICS_OBJ_H

#include "geometry.h"
#include "utilities.h"

enum collision_type 
{
	COLLISION_NONE,
	COLLISION_TOUCH,
	COLLISION_OVERLAP
};

struct collision_desc;

class physics_obj
{
	protected:
		poly_2d
			_shape,
			_world_poly;

		bounding_box _bb;

		vec_2d
			_pos,
			_vel,
			_acc;

		wu_t 
			_rot,
			_vrot,
			_racc,
			_mass,
			_mo_inertia,
			_radius,
			_elasticity;

	public:
		physics_obj() :
			_shape(),
			_bb(),
			_pos(),
			_vel(),
			_acc(),
			_rot( 0.0 ),
			_vrot( 0.0 ),
			_racc( 0.0 ),
			_mass( 10.0 ),
			_mo_inertia( 10000.0 ),
			_radius( 0.0 ), 
			_elasticity( 0.5 )
		{}

		virtual ~physics_obj() {}

		physics_obj get_next_pos( wu_t delta, vec_2d acc );

		void update_world_poly();

		collision_type check_collision( physics_obj &po, collision_desc &col );

		void precalculate();

		void stop() { _vel = vec_2d(); _vrot = 0.0; } 

		vec_2d point_velocity( const vec_2d &v ) const;

		static void handle_collision( collision_desc &col );
};

struct collision_desc
{
	physics_obj 
		*_po1,
		*_po2;

	vec_2d
		_where,
		_norm;

	wu_t _rel_vel;
};

#endif // PHYSICS_OBJ_H
