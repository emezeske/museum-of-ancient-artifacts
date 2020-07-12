#include "../shared/plane.h"
#include "../shared/gCWorld.h"

enum FrustumResult {
	FR_OUT,
	FR_IN,
	FR_INTERSECT
};

enum PlaneResult {
	PR_OUT,
	PR_IN,
	PR_INTERSECT
};

class Matrix4x4 {
	public:
	float	_11, _21, _31, _41,
		_12, _22, _32, _42,
		_13, _23, _33, _43,
		_14, _24, _34, _44;
	void Transpose() {
		float tmp;
		tmp = _12;
		_12 = _21;
		_21 = tmp;
		tmp = _23;
		_23 = _32;
		_32 = tmp;
		tmp = _34;
		_34 = _43;
		_43 = tmp;
		tmp = _13;
		_13 = _31;
		_31 = tmp;
		tmp = _24;
		_24 = _42;
		_42 = tmp;
		tmp = _14;
		_14 = _41;
		_41 = tmp;
	} // end function Transpose()
}; // end struct Matrix4x4

class Frustum {
	public:

	Plane near;
	Plane far;
	Plane bottom;
	Plane top;
	Plane left;
	Plane right;
	double effRbottom;
	double effRtop;
	double effRleft;
	double effRright;
	double effRnearfar;
	Frustum();
	PlaneResult excelcior(Plane &pl, Vector3 &pt, double effR);
	FrustumResult IntersectionWithAaBox(Vector3 &center, unsigned char level);
	double calculateEffectiveRadius(Plane &pl, Vector3 pt[]);
	void calculateEffectiveRadii(AaBox &box);
}; // end class Frustum

