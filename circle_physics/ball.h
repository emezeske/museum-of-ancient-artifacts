#ifndef __BALL_H__
#define __BALL_H__

#include "collisionobj.h"
#include "drawable.h"

class Ball : public CircleCollisionObj, public DrawableObj
{
	private:
		TexMap ballTex;

	public:
		Ball( double x, double y, double xV, double yV, double r, double m, const char *tex_fname ) :
			CircleCollisionObj( x, y, xV, yV, r, m ),
			ballTex( tex_fname ) // TODO: Deal with the fact that this might throw()
		{
		}

		virtual void draw();
};

#endif // __BALL_H__
