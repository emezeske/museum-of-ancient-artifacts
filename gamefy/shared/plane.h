#ifndef __PLANE_H__
#define __PLANE_H__

#include <math.h>
#include "vector.h"

class Plane {
	public:
	Vector3 n;
	double 	d;

	Plane(double X, double Y, double Z, double D) : n(X, Y, Z), d(D){
	}

	Plane(): n(), d(0.0) {
	}

	// cross product
	Vector3 operator * (const Vector3 &v) {
		Vector3 result(n*v);
		return result;
	} // end operator *

	// dot product
	double operator / (const Vector3 &v) {
		return	n.x*v.x+
			n.y*v.y+
			n.z*v.z;
	} // end operator /

	// make magnitude 1.0
	void Normalize() {
		double mag = Magnitude();
		n.x/=mag;
		n.y/=mag;
		n.z/=mag;
		d/=mag;
	} // end function Normalize()

	double Magnitude() {
		return n.Magnitude();
	} // end function Magnitude()

	double distanceToPoint(const Vector3 &p) {
		return 
			n.x*p.x + 
			n.y*p.y + 
			n.z*p.z + 
			d;
	} // end function distanceToPoint()

}; // end class Plane

#endif
