#include <cmath>
#include "vec3d.h"
using std::sqrt;

Vec3D Vec3D::operator-(const Vec3D& v) {
	return Vec3D(x-v.x, y-v.y, z-v.z);
} // end operator - ()

void Vec3D::Normalize() {
	float mag = sqrt(x*x+y*y+z*z);
	x /= mag;
	y /= mag;
	z /= mag;
} // end Normalize()

void Vec3D::Clamp01() {
	Normalize();
	x *= 0.5;
	y *= 0.5;
	z *= 0.5;
	x += 0.5;
	y += 0.5;
	z += 0.5;
} // end Clamp01()
