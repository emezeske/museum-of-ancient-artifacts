#include "Frustum.h"
#include "../shared/gCWorld.h"

Frustum::Frustum() {
//	effRnearfar = effRbottom = effRtop = effRleft = effRright = 0.0;
} // end constructor Frustum()

PlaneResult Frustum::excelcior(Plane &pl, Vector3 &pt, double effR) {
	double distance = 
		pl.n.x*pt.x+
		pl.n.y*pt.y+
		pl.n.z*pt.z+
		pl.d;
	//printf("excelcior distance = %f\teffectiveRadius = %f\n", distance, effR);
	if(distance < -effR) return PR_OUT;
	if(distance > effR) return PR_IN;
	return PR_INTERSECT;
} // end function excelcior()

FrustumResult Frustum::IntersectionWithAaBox(Vector3 &center, unsigned char level) {
	double rn, rl, rr, rb, rt;
	rn = effRnearfar;
	rl = effRleft;
	rr = effRright;
	rb = effRbottom;
	rt = effRtop;
	for(int l=0; l<level-1; ++l) {
		rn	*=0.5;
		rl	*=0.5;
		rr	*=0.5;
		rb	*=0.5;
		rt	*=0.5;
	} // end for l
	bool insideAllSoFar = true;
	PlaneResult r;

	r =	excelcior(near, center, rn);
	if(r==PR_OUT) return FR_OUT;
	if(r==PR_INTERSECT) insideAllSoFar = false;
	r =	excelcior(far, center, rn);
	if(r==PR_OUT) return FR_OUT;
	if(r==PR_INTERSECT) insideAllSoFar = false;
	r =	excelcior(left, center, rl);
	if(r==PR_OUT) return FR_OUT;
	if(r==PR_INTERSECT) insideAllSoFar = false;
	r =	excelcior(right, center, rr);
	if(r==PR_OUT) return FR_OUT;
	if(r==PR_INTERSECT) insideAllSoFar = false;
	r =	excelcior(bottom, center, rb);
	if(r==PR_OUT) return FR_OUT;
	if(r==PR_INTERSECT) insideAllSoFar = false;
	r =	excelcior(top, center, rt);
	if(r==PR_OUT) return FR_OUT;
	if(r==PR_INTERSECT) insideAllSoFar = false;

	if(insideAllSoFar) return FR_IN;
	return FR_INTERSECT;

} // end function IntersectionWithAaBox()

double Frustum::calculateEffectiveRadius(Plane &pl, Vector3 pt[]) {

	double greatest, least, tmp;

	greatest = least =
		pl.distanceToPoint(pt[0]);
	tmp =	pl.distanceToPoint(pt[1]);
	least =		(tmp<least)?	tmp:least;
	greatest =	(tmp>greatest)?	tmp:greatest;
	tmp =	pl.distanceToPoint(pt[2]);
	least =		(tmp<least)?	tmp:least;
	greatest =	(tmp>greatest)?	tmp:greatest;
	tmp =	pl.distanceToPoint(pt[3]);
	least =		(tmp<least)?	tmp:least;
	greatest =	(tmp>greatest)?	tmp:greatest;
	tmp =	pl.distanceToPoint(pt[4]);
	least =		(tmp<least)?	tmp:least;
	greatest =	(tmp>greatest)?	tmp:greatest;
	tmp =	pl.distanceToPoint(pt[5]);
	least =		(tmp<least)?	tmp:least;
	greatest =	(tmp>greatest)?	tmp:greatest;
	tmp =	pl.distanceToPoint(pt[6]);
	least =		(tmp<least)?	tmp:least;
	greatest =	(tmp>greatest)?	tmp:greatest;
	tmp =	pl.distanceToPoint(pt[7]);
	least =		(tmp<least)?	tmp:least;
	greatest =	(tmp>greatest)?	tmp:greatest;

	return fabs((greatest-least)*0.5);

} // end function calculateEffectiveRadius()

void Frustum::calculateEffectiveRadii(AaBox &box) {
	Vector3 pts[8];
	unsigned char p=0;
	double size = box.xmax-box.xmin;
	for(char X=0; X<2; ++X)
	for(char Y=0; Y<2; ++Y)
	for(char Z=0; Z<2; ++Z)
		pts[p++] = Vector3(box.xmin+X*size, box.ymin+Y*size, box.zmin+Z*size);

/*	for(p=0; p<8; ++p)
		printf("size = %f\tp[%d] = (%f, %f, %f)\n", size, p, 
						pts[p].x,
						pts[p].y,
						pts[p].z);
						*/

	effRnearfar =	calculateEffectiveRadius(near,		pts);
	effRleft =	calculateEffectiveRadius(left,		pts);
	effRright =	calculateEffectiveRadius(right,		pts);
	effRbottom =	calculateEffectiveRadius(bottom,	pts);
	effRtop =	calculateEffectiveRadius(top,		pts);
	
} // end function calculateEffectiveRadii()
