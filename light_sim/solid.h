#ifndef __SOLID_H__
#define __SOLID_H__

#include "geometry.h"

struct material 
{
		float diffuse,
			  reflect,
			  absorb,
			  refraction_indx,
			  color_R,
			  color_G,
			  color_B;
}; // end class material

class solid
{
	public:
		point_2d pos;
		float rot;
		material mat;
		polygon bounds;
	
	solid( point_2d &new_pos, float new_rot, material &new_mat, polygon &new_bounds ):
			pos( new_pos ), rot( new_rot ), mat( new_mat ), bounds( new_bounds ) { }

	void draw( void );
}; // end class solid
#endif
