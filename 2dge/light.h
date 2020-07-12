#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "vec3d.h"

// Simple struct that contains information about where
// a light is and it's color and intensity.  Note that when
// a Light is instantiated it is automatically added to the
// renderer's list of lights
struct Light {
	Vec3D pos;
	float r, g, b, i;
	// Ctor gets passed a position vector, then it's r, g, b,
	// and intensity values
	Light(Vec3D, float, float, float, float);
	~Light();
};
#endif
