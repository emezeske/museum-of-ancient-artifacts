#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#include "drawable.h"
#include "utilities.h"

using std::vector;

typedef vector< drawable * > drawable_vec;
typedef vector< drawable * >::iterator drawable_vec_iter;

class renderer
{
	drawable_vec _draw_list;

	public: 
		void draw_all();
		void add_drawable( drawable *d );
		void rm_drawable( drawable *d );
};

#endif // RENDERER_H
