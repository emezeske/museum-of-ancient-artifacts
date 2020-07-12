#include <math.h>
#include <stdio.h>
#include "collisionobj.h"

// debug, for pause()
#include <unistd.h>

struct DebugThingie 
{
	double
		c1X, c1Y, c1XV, c1YV,
		c2X, c2Y, c2XV, c2YV;	
	double periodLen, t1, t2;
	void narbutoquify() { 
		printf( "c1X: %f, c1Y: %f, c1XV: %f, c1YV: %f, c2X: %f, c2Y: %f, c2XV: %f, c2YV: %f,	periodLen: %f, t1: %f, t2: %f\n",
				c1X, c1Y, c1XV, c1YV, c2X, c2Y, c2XV, c2YV,	periodLen, t1, t2 );
	}
};

DebugThingie zeThingie;

double CollisionObj::circleCircleTimeOfCollision( CircleCollisionObj &c1, CircleCollisionObj &c2, double periodLen )
{
	double collisionTime = NO_COLLISION;

	double
		dxV = c1.xV - c2.xV,
		dyV = c1.yV - c2.yV,
		dx = c1.x - c2.x,
		dy = c1.y - c2.y,
		tr = c1.r + c2.r,
		a = dxV * dxV + dyV * dyV,
		b = 2 * ( dxV * dx + dyV * dy ),
		c = dx * dx + dy * dy - tr * tr,
		bSqMin4AC = b * b - 4.0 * a * c;

	if ( bSqMin4AC >= 0 ) // Let's not call sqrt() without good reason
	{
		double
			rootTerm = sqrt( bSqMin4AC ),
			aTimes2Inv = 1 / ( a * 2.0 ),
			t1 = ( -b + rootTerm ) * aTimes2Inv,
			t2 = ( -b - rootTerm ) * aTimes2Inv;

		zeThingie.c1X = c1.x;
		zeThingie.c1Y = c1.y;
		zeThingie.c1XV = c1.xV;
		zeThingie.c1YV = c1.yV;
		zeThingie.c2X = c2.x;
		zeThingie.c2Y = c2.y;
		zeThingie.c2XV = c2.xV;
		zeThingie.c2YV = c2.yV;
		zeThingie.periodLen = periodLen;
		zeThingie.t1 = t1;
		zeThingie.t2 = t2;

		if ( t1 > 0 && t1 <= periodLen )
		{
			collisionTime = t1;
			if ( t2 > 0 && t2 < t1 ) collisionTime = t2;
		}
		else if ( t2 > 0 && t2 <= periodLen ) collisionTime = t2;
	}

	return collisionTime;
} // end func circleCircleTimeOfCollision()

inline double rad2deg( double r ) { return ( r / M_PI * 180 ); }
inline double mag( double x, double y ) { return sqrt( x*x+y*y ); }

void CollisionObj::circleCircleDoCollision( CircleCollisionObj &c1, CircleCollisionObj &c2, double when )
{
	double
		angle1To2 = atan2( c2.y - c1.y, c2.x - c1.x ),

		theCos = cos( angle1To2 ),
		theSin = sin( angle1To2 ),

		c1ColPlaneXVi = c1.xV * theCos - c1.yV * -theSin,
		c1ColPlaneYVf = c1.xV * -theSin + c1.yV * theCos,
 
		c2ColPlaneXVi = c2.xV * theCos - c2.yV * -theSin,
		c2ColPlaneYVf = c2.xV * -theSin + c2.yV * theCos,

		totalMassInv = 1 / ( c1.mass + c2.mass ),
		totalMassInvX2 = 2 * totalMassInv,

		c1ColPlaneXVf = c1ColPlaneXVi * ( c1.mass - c2.mass ) * totalMassInv +
						c2ColPlaneXVi * c2.mass * totalMassInvX2,

		c2ColPlaneXVf = c2ColPlaneXVi * ( c2.mass - c1.mass ) * totalMassInv +
						c1ColPlaneXVi * c1.mass * totalMassInvX2;

	// TODO: Figure out the cause of the fake collisions instead of ignoring them.
	if ( c2ColPlaneXVf - c1ColPlaneXVf < 0.0 )
	{
		printf( "Fake Collision Detected-----------------------------------------\n" );
		c1.dump(); c2.dump();
		printf( "c1ColPlaneXVi: %f\n", c1ColPlaneXVi );
		printf( "c2ColPlaneXVi: %f\n", c2ColPlaneXVi );
		printf( "c1ColPlaneXVf: %f\n", c1ColPlaneXVf );
		printf( "c2ColPlaneXVf: %f\n", c2ColPlaneXVf );
		printf( "c1ColPlaneYVf: %f\n", c1ColPlaneYVf );
		printf( "c2ColPlaneYVf: %f\n", c2ColPlaneYVf );
		printf( "zeThingie:\n" );
		zeThingie.narbutoquify();
		printf( "----------------------------------------------------------------\n\n" );
		return;
	}

	c1.xV = c1ColPlaneXVf * theCos - c1ColPlaneYVf * theSin;
	c1.yV = c1ColPlaneXVf * theSin + c1ColPlaneYVf * theCos;
	c2.xV = c2ColPlaneXVf * theCos - c2ColPlaneYVf * theSin;
	c2.yV = c2ColPlaneXVf * theSin + c2ColPlaneYVf * theCos;
}
