#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <vector>

#include "geometry.h"

using std::vector;

class drawable
{
	public:
		virtual void draw() = 0;
		virtual bounding_box &get_bounding_box() = 0;
		virtual ~drawable() {};
};

#endif // DRAWABLE_H
