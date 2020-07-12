///////////////////////////////////////////////////////////////////////////////
// Filename: frustumobject.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _FRUSTUMOBJECT_H_
#define _FRUSTUMOBJECT_H_


//////////////
// INCLUDES //
//////////////
#include <GL/glut.h>
#include <math.h>


///////////////////////////////////////////////////////////////////////////////
// Class name: FrustumObject
///////////////////////////////////////////////////////////////////////////////
class FrustumObject
{
public:
	FrustumObject();
	FrustumObject(const FrustumObject&);
	~FrustumObject();

	bool Initialize();
	bool Shutdown();

	void ConstructFrustum();

	bool CheckPoint(float, float, float);
	bool CheckCube(float, float, float, float);
	bool CheckSphere(float, float, float, float);
	bool CheckRectangle(float, float, float, float, float, float);

private:
	float frustumPlanes[6][4];
};

#endif
