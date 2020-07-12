#include "depthmap.h"

Depthmap::Depthmap(const char *filename) {
	maxDepth = 423;
	minDepth = 0;
	LoadFromTGAFile(filename);
} // end constructor Depthmap()

void Depthmap::LoadFromTGAFile(const char *filename) {
	LTGA *depthTGA = new LTGA(filename);

	short tgaW  = depthTGA->GetImageWidth();
	short tgaH = depthTGA->GetImageHeight();
	if(tgaW != width || tgaH != height)
		bailout("Depthmap image incorrect size...\n");

	unsigned char *depthBytes = (unsigned char *)depthTGA->GetPixels();

	float nearestDepthValue = -FLT_MAX;
	for(int x=0; x<tgaW; ++x)
		for(int y=0; y<tgaH; ++y)
		{
			depths[x][y] = 255.0 - (float)(depthBytes[x+y*width] / 5.0) - maxDepth;
			if(depths[x][y] > nearestDepthValue)
				nearestDepthValue = depths[x][y];
		}
	farPlane  = Plane(0.0, 0.0, 1.0, maxDepth);
	nearPlane = Plane(0.0, 0.0, 1.0, -nearestDepthValue);
} // end function LoadFromTGAFile()

Vector3 Depthmap::RayIntersection(Vector3 dir, Vector3 orig) {
	// Narrow down the points we have to check 
	Vector3 nearIntersect = planeRayIntersect(nearPlane, dir, orig);
	Vector3 farIntersect  = planeRayIntersect(farPlane,  dir, orig);
	int minX, maxX;
	if(nearIntersect.x > farIntersect.x)
	{
		minX 	= (int)floor(farIntersect.x);
		maxX	= (int)ceil(nearIntersect.x);
	} else {
		minX 	= (int)floor(nearIntersect.x);
		maxX	= (int)ceil(farIntersect.x);
	}
	if(maxX >= width) 
		maxX = width - 1;
	if(minX < 0)
		minX = 0;

	int y = (int)orig.y;
	// Interpolated Intersection with Height Map
	Vector3 iIHM;
	for(int x = minX; x < maxX; ++x) 
	{
		// Find intersection of two lines a and b
		Vector2 a1(nearIntersect.x, nearIntersect.z),
			a2(farIntersect.x, farIntersect.z),
			b1(x, depths[x][y]),
			b2(x+1, depths[x+1][y]);
		float uN = (b2.x - b1.x)*(a1.y - b1.y) - (b2.y - b1.y)*(a1.x - b1.x),
	              uD = (b2.y - b1.y)*(a2.x - a1.x) - (b2.x - b1.x)*(a2.y - a1.y);
		if(uD == 0)
			continue;
		float u = uN/uD,
		      intersectX = a1.x + u*(a2.x - a1.x),
		      intersectY = a1.y + u*(a2.y - a1.y);
		if( (intersectY >= b1.y && intersectY <= b2.y) || (intersectY <= b1.y && intersectY >= b2.y) )
			if( (intersectX >= b1.x && intersectX <= b2.x) || (intersectX <= b1.x && intersectX >= b2.x) )
			{
				iIHM = Vector3(intersectX, y, intersectY);
				break;
			}
	} // end for
	return iIHM;
} // end function RayIntersection()
