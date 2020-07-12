#include <cmath>
#include "vec3d.h"

Vec3D Vec3D::operator-(const Vec3D& v) {
	return Vec3D(x-v.x, y-v.y, z-v.z);
} // end operator - ()

void Vec3D::Normalize() {
	float mag = std::sqrt(x*x+y*y+z*z);
	// Divide scalars by the magnitude to get a unit length vector
	x /= mag;
	y /= mag;
	z /= mag;
} // end Normalize()

void Vec3D::Clamp01() {
	Normalize();
	// Make sure no scalar is out of the range [-0.5, 0.5]
	x *= 0.5;
	y *= 0.5;
	z *= 0.5;
	// Clamp to the range [0.0, 1.0]
	x += 0.5;
	y += 0.5;
	z += 0.5;
} // end Clamp01()
