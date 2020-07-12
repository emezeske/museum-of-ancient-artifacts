#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>

#include "physics_obj.h"
#include "utilities.h"

using std::vector;

// To avoid allocating memory during the simulation cycle, we need
// exactly one temporary place to store each physics_obj, so we make
// this easy to keep track of with this class:
struct physics_obj_pair
{
	physics_obj
		*_cur,
		_next;

	physics_obj_pair() : _cur( 0 ), _next() {}

	physics_obj_pair( physics_obj *p ) : _cur( p ), _next() {}

	void update_cur()
	{ 
		*_cur = _next;
	}
};

typedef vector< physics_obj_pair > po_pair_vec;
typedef vector< physics_obj_pair >::iterator po_pair_vec_iter;

typedef vector< collision_desc > collision_vec;
typedef vector< collision_desc >::iterator collision_vec_iter;

class simulator
{
	static const wu_t A_SMIDGEON = 0.0001;

	po_pair_vec _physics_objs;

	collision_vec _collisions;

	vec_2d _global_acc;

	public:
		simulator() : _physics_objs(), _global_acc( 0.0, 0.0 ) {}

		collision_type detect_collisions();

		void solve_collisions();

		void update_all( wu_t delta );

		void add_physics_obj( physics_obj *p );

		void rm_physics_obj( physics_obj *p );
};

#endif // SIMULATOR_H
