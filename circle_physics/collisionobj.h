#ifndef __COLLISIONOBJ_H__
#define __COLLISIONOBJ_H__

#include <assert.h>
#include <stdio.h>

extern const int SCREEN_W, SCREEN_H;

class CircleCollisionObj;

class CollisionObj
{
	public:
		virtual void dump() = 0;
		// How long until these objects next collide?  Returns -1
		// if they don't collide during periodLen ms
		virtual void move( double dt ) = 0;

		virtual double timeOfCollision( CollisionObj &c, double when ) = 0;

		virtual double timeOfCollisionCircle( CircleCollisionObj &c, double periodLen ) = 0;
		static double circleCircleTimeOfCollision( CircleCollisionObj &c1, CircleCollisionObj &c2, double periodLen );

		virtual void doCollision( CollisionObj &c, double when ) = 0;

		virtual void doCollisionCircle( CircleCollisionObj &c, double when ) = 0;
		static void circleCircleDoCollision( CircleCollisionObj &c1, CircleCollisionObj &c2, double when );

		static const double NO_COLLISION = -1;
};

class PredictedCollision
{
	public:
		PredictedCollision( CollisionObj *co1, CollisionObj *co2, double t ) :
			c1( co1 ), c2( co2 ), timeOfCollision( t )
		{
		}
		CollisionObj *c1, *c2;
		double timeOfCollision;
		void doCollision(){ assert( c1 && c2 ); c1->doCollision( *c2, timeOfCollision ); }
};

class CircleCollisionObj : public CollisionObj 
{
	public:
		CircleCollisionObj( double X, double Y, double XV, double YV, double R, double M, double T = 0.0 ) :
			x( X ), y( Y ), xV( XV ), yV( YV ), r( R ), mass( M ), theta( T )
		{
		}

		virtual double timeOfCollision( CollisionObj &c, double periodLen )
		{
			return c.timeOfCollisionCircle( *this, periodLen );
		}

		virtual void doCollision( CollisionObj &c, double when )
		{
			c.doCollisionCircle( *this, when );
		}

		virtual double timeOfCollisionCircle( CircleCollisionObj &c, double periodLen )
		{
			return CollisionObj::circleCircleTimeOfCollision( *this, c, periodLen );
		}

		virtual void doCollisionCircle( CircleCollisionObj &c, double when )
		{
			CollisionObj::circleCircleDoCollision( *this, c, when );
		}

		virtual void move( double dt )
		{
			x += xV * dt;
			y += yV * dt;

			if ( x < -SCREEN_W/2 )
			{
				xV = -xV;
				x = -SCREEN_W/2;
			}
			else if ( x > SCREEN_W/2 )
			{
				xV = -xV;
				x = SCREEN_W/2;
			}

			if ( y < -SCREEN_H/2 )
			{
				yV = -yV;
				y = -SCREEN_H/2;
			}
			else if ( y > SCREEN_H/2 )
			{
				yV = -yV;
				y = SCREEN_H/2;
			}
		}
		double x, y, xV, yV, r, mass, theta;
		virtual void dump() { printf( "x: %f, y: %f, xV: %f, yV: %f, r: %f, mass: %f, theta: %f\n", x, y, xV, yV, r, mass, theta ); }
};

#endif // __COLLISIONOBJ_H__
