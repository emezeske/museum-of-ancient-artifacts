#ifndef WORLD_OBJ_H
#define WORLD_OBJ_H

#include <vector>
#include <string>

using std::string;
using std::vector;

#include "drawable.h"
#include "physics_obj.h"

class world_obj : public drawable, public physics_obj
{
	public:
		world_obj( physics_obj po ) :
			drawable(),
			physics_obj( po )
		{}

		world_obj( const vector< string > &sv );

		virtual void draw();
		virtual bounding_box &get_bounding_box() { return _bb; }
};

#endif // WORLD_OBJ_H
