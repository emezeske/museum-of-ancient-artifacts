#ifndef __DEPTHMAP_H__
#define __DEPTHMAP_H__

#include <cstdio>
#include <float.h>
#include <math.h>

#include "utils.h"
#include "vector.h"
#include "ltga.h"

class Depthmap {
	public:
		static const int width = 512;
		static const int height = 512;
		float maxDepth;
		float minDepth;
		Plane nearPlane;		
		Plane farPlane;		
		float depths[width][height];

		Depthmap(const char *);
		void LoadFromTGAFile(const char *);
		Vector3 RayIntersection(Vector3, Vector3);
}; // end class Depthmap

#endif
