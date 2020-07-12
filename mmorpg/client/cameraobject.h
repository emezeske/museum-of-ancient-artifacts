///////////////////////////////////////////////////////////////////////////////
// Filename: cameraobject.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERAOBJECT_H_
#define _CAMERAOBJECT_H_


//////////////
// INCLUDES //
//////////////
#include <GL/glut.h>


///////////////////////////////////////////////////////////////////////////////
// Class name: CameraObject
///////////////////////////////////////////////////////////////////////////////
class CameraObject
{
public:
	CameraObject();
	CameraObject(const CameraObject&);
	~CameraObject();

	void Render();
	void SetPosition(float, float, float, float);

private:
	float posX, posY, posZ, yRot;
};

#endif
