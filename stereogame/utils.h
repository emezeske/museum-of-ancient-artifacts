#ifndef __UTILS_H__
#define __UTILS_H__

#include "cstdlib"
#include "cstdio"
#include "vector.h"

class Plane {
	public:
		float a, b, c, d;
		Plane(): a(0.0), b(0.0), c(0.0), d(0.0) { };
		Plane(float A, float B, float C, float D): a(A), b(B), c(C), d(D) { };
}; // end class Plane

class Vector2 {
	public:
		float x, y;
		Vector2(): x(0.0), y(0.0) { };
		Vector2(float X, float Y): x(X), y(Y) { };
}; // end class Vector2

extern void bailout(const char *);
extern Vector3 planeRayIntersect(Plane, Vector3, Vector3);

#endif
