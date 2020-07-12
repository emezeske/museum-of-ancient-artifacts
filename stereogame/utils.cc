#include "utils.h"
using namespace std;

void bailout(const char *m) {
	printf(m);
	exit(1);
} // end function bailout()

Vector3 planeRayIntersect(Plane p, Vector3 dir, Vector3 orig) {
	float 	tN =	-(p.a*orig.x + p.b*orig.y + p.c*orig.z + p.d),
		tD =	 (p.a*dir.x + p.b*dir.y + p.c*dir.z);
	if(tD == 0)
		bailout("divide by zero error\n");
	float t = tN/tD;
	Vector3 intersect(orig.x + t*dir.x,
			  orig.y + t*dir.y,
			  orig.z + t*dir.z);
	return intersect;
} // end function planeRayIntersect()
