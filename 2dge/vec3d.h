#ifndef __VEC3D_H__
#define __VEC3D_H__

struct Vec3D {
	float x, y, z;
	// Default ctor
	Vec3D(float X = 0.0F, float Y = 0.0F, float Z = 0.0F) : x(X), y(Y), z(Z) { }
	// Copy ctor
	Vec3D(const Vec3D& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) { }
	// Subtract scalar components
	Vec3D operator-(const Vec3D&);
	// Take a vector of arbitrary length and make it unit length
	void Normalize();
	// Pack the scalar values to [0,1] with [0,0.5) being negative and
	// (0.5, 1.0] being positive, and 0.5 being zero.  This is the format
	// that the GL_DOT3_RGB texture combiner setting expects.
	void Clamp01();
}; // end struct Vec3D
#endif
