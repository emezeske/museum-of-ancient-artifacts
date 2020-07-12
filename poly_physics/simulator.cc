#include <algorithm>
#include <GL/glut.h>

using std::find;

#include "simulator.h"
#include "utilities.h"

#include "world_obj.h"
#include "renderer.h"

collision_type simulator::detect_collisions()
{
	collision_type col_type = COLLISION_NONE;
	bool found_overlap = false;
	_collisions.clear();
	unsigned n_objs = _physics_objs.size();
	
	for ( unsigned i = 0; i < n_objs && col_type != COLLISION_OVERLAP; ++i )
	{
		for ( unsigned z = i + 1; z < n_objs && col_type != COLLISION_OVERLAP; ++z )
		{
			collision_desc col;

			col_type = _physics_objs[ i ]._next.check_collision( _physics_objs[ z ]._next, col );

			if ( col_type == COLLISION_TOUCH ) _collisions.push_back( col ); 
			else if ( col_type == COLLISION_OVERLAP ) found_overlap = true;
		}
	}

	if ( !found_overlap && _collisions.size() > 0 ) col_type = COLLISION_TOUCH;

	return col_type;
}

void simulator::solve_collisions()
{
	for ( collision_vec_iter i = _collisions.begin(); i != _collisions.end(); ++i )
	{
		VNOTIFY( VERBOSE, "Collision between 0x%08lx and 0x%08lx.", i->_po1, i->_po2 );
		VNOTIFY( VERBOSE, "Where: x: %f, y: %f.", i->_where._x, i->_where._y );
		VNOTIFY( VERBOSE, "Norm: x: %f, y: %f.", i->_norm._x, i->_norm._y );
		physics_obj::handle_collision( *i );
	}
}

void simulator::update_all( wu_t delta )
{
	wu_t
		time_step = delta,
		time_advanced = 0.0;

	while ( time_advanced < delta - A_SMIDGEON )
	{
		collision_type col_type;

		for ( po_pair_vec_iter i = _physics_objs.begin(); i != _physics_objs.end(); ++i )
		{
			i->_next = i->_cur->get_next_pos( time_step, _global_acc );
			i->_next.update_world_poly();
		}
		
		if ( ( col_type = detect_collisions() ) != COLLISION_OVERLAP )
		{
			if ( col_type == COLLISION_TOUCH )
			{
				VNOTIFY( VERBOSE, "Detected Touch @ %f.", time_step );
				solve_collisions();

			}
			time_advanced += time_step;

			for ( po_pair_vec_iter i = _physics_objs.begin(); i != _physics_objs.end(); ++i )
			{
				i->update_cur();
			}

			if ( col_type == COLLISION_TOUCH ) time_step = delta - time_advanced;
		}
		else
		{
			VNOTIFY( VERBOSE, "Detected Overlap @ %f.", time_step );
			time_step /= 2;

			if ( time_step < A_SMIDGEON )
			{
				NOTIFY( VERBOSE, "Time step dropped below minimum threshold." );
				utilities::die( -1 );
			}
		}
	}
}

void simulator::add_physics_obj( physics_obj *p )
{
	_physics_objs.push_back( physics_obj_pair( p ) );
}

void simulator::rm_physics_obj( physics_obj *p )
{
	for ( po_pair_vec_iter i = _physics_objs.begin(); i != _physics_objs.end(); ++i )
	{
		if ( i->_cur == p ) _physics_objs.erase( i );
	}
}
