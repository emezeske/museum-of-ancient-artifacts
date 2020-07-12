#ifndef __SHIP_H__
#define __SHIP_H__

#include "collisionobj.h"
#include "drawable.h"

class Ship : public CircleCollisionObj, public DrawableObj
{
	private:
		TexMap tex;

	public:
		Ship( double x, double y, double xV, double yV, double r, double m, const char *tex_fname ) :
			CircleCollisionObj( x, y, xV, yV, r, m ),
			tex( tex_fname ) // TODO: Deal with the fact that this might throw()
		{
		}

		virtual void draw();
};

#endif // __SHIP_H__

