#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <math.h>

class Vector3 {
	public:
	double x, y, z;

	Vector3(double X, double Y, double Z) {
		x=X;
		y=Y;
		z=Z;
	}

	Vector3() {
		x = y = z = 0.0;
	}

	Vector3 operator + (const Vector3 &v) {
		Vector3 result(x+v.x, y+v.y, z+v.z);
		return result;
	} // end operator +

	Vector3 operator - (const Vector3 &v) {
		Vector3 result(x-v.x, y-v.y, z-v.z);
		return result;
	} // end operator -

	// cross product
	Vector3 operator * (const Vector3 &v) {
		Vector3 result(
				y*v.z - z*v.y,
				z*v.x - x*v.z,
				x*v.y - y*v.x
			      );
		return result;
	} // end operator *

	// multiplication by scalar
	Vector3 operator * (const double &n) {
		Vector3 result(
				x*n,
				y*n,
				z*n
			      );
		return result;
	} // end operator *

	// dot product
	double operator / (const Vector3 &v) {
		return	x*v.x+
			y*v.y+
			z*v.z;
	} // end operator /

	// make magnitude 1.0
	void Normalize() {
		double mag = Magnitude();
		x /= mag;
		y /= mag;
		z /= mag;
	} // end function Normalize()

	double Magnitude() {
		return sqrt(x*x+y*y+z*z);
	} // end function Magnitude()

}; // end class Vector3

#endif
