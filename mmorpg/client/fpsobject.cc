///////////////////////////////////////////////////////////////////////////////
// Filename: fpsobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "fpsobject.h"


///////////////////////////////////////////////////////////////////////////////
// Function name: FpsObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: Initializes the member variables.
///////////////////////////////////////////////////////////////////////////////
FpsObject::FpsObject()
{
	fps = 0;
	count = 0;
	frameTime = startTime = glutGet(GLUT_ELAPSED_TIME);
}


///////////////////////////////////////////////////////////////////////////////
// Function name: FpsObject
// Purpose: Copy constructor.
// Inputs: other - The object this function is making a copy of.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
FpsObject::FpsObject(const FpsObject& other)
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ~FpsObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
FpsObject::~FpsObject()
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Frame
// Purpose: Increments the frame per second count.
// Inputs: None.
// Outputs: Returns true if we should draw a frame, false if we should skip.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool FpsObject::Frame()
{
	// Lock the graphics MAX_FPS.
	if(LOCK_TO_FPS)
	{
		if(glutGet(GLUT_ELAPSED_TIME) < (frameTime + (1000 / MAX_FPS)) )
		{
			// if it's too soon to draw a frame, ABORT!!!
			return false;
		}
		frameTime = glutGet(GLUT_ELAPSED_TIME);
	}

	// Increment the frames per second count.
	count++;

	// If it has been one second then store the count and start over.
	if(glutGet(GLUT_ELAPSED_TIME) >= (startTime + 1000))
	{
		fps = count;
		count = 0;
		
		startTime = glutGet(GLUT_ELAPSED_TIME);
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetFps
// Purpose: Gives the calling function the frame per second count for the last
//          second.
// Inputs: None.
// Outputs: fps - The frame per second count.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
int FpsObject::GetFps()
{
	return fps;
}
