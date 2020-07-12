#ifndef __VEC3D_H__
#define __VEC3D_H__

class Vec3D {
	public:
	float x, y, z;
	Vec3D() : x(0), y(0), z(0) { }
	Vec3D(const Vec3D& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) { }
	Vec3D(float X, float Y, float Z) : x(X), y(Y), z(Z) { }
	Vec3D operator-(const Vec3D&);
	void Normalize();
	void Clamp01();
};

#endif
