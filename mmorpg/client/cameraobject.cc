///////////////////////////////////////////////////////////////////////////////
// Filename: cameraobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "cameraobject.h"


///////////////////////////////////////////////////////////////////////////////
// Function name: CameraObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: Initializes the position of the camera to zero.
///////////////////////////////////////////////////////////////////////////////
CameraObject::CameraObject()
{
	posX = posY = posZ = yRot = 0.0f;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: CameraObject
// Purpose: Copy constructor.
// Inputs: other - The camera object this function is making a copy of.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
CameraObject::CameraObject(const CameraObject& other)
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ~CameraObject
// Purpose: Destructor
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
CameraObject::~CameraObject()
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Render
// Purpose: Positions the viewer at the location of the camera.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void CameraObject::Render()
{
	// Rotate around the origin to the proper camera angle first.
	glRotatef((360.0f - yRot), 0.0f, 1.0f, 0.0f);

	// Then translate to the position of the camera.
	glTranslatef(-posX, -posY, -posZ);

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetPosition
// Purpose: Sets the position of the camera to the input values.
// Inputs: x - The x position of the camera.
//         y - The y position of the camera.
//         z - The z position of the camera.
//         r - The rotation of the camera.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void CameraObject::SetPosition(float x, float y, float z, float r)
{
	posX = x;
	posY = y;
	posZ = z;
	
	yRot = r;

	return;
}
