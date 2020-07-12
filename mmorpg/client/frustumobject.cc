///////////////////////////////////////////////////////////////////////////////
// Filename: frustumobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "frustumobject.h"


///////////////////////////////////////////////////////////////////////////////
// Function name: FrustumObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
FrustumObject::FrustumObject()
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: FrustumObject
// Purpose: Copy constructor.
// Inputs: other - The object this function is making a copy of.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
FrustumObject::FrustumObject(const FrustumObject& other)
{
}



///////////////////////////////////////////////////////////////////////////////
// Function name: ~FrustumObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
FrustumObject::~FrustumObject()
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ConstructFrustum
// Purpose: Builds the frustum to use for culling based on the current viewing
//          location.
// Inputs: None.
// Outputs: None.
// Details: Calculates the six planes to check for inclusion or exclusion of 
//          objects in.
///////////////////////////////////////////////////////////////////////////////
void FrustumObject::ConstructFrustum()
{
	float proj[16];
	float modl[16];
	float clip[16];
	float t;

	// Get the current projection matrix from opengl.
	glGetFloatv(GL_PROJECTION_MATRIX, proj);

	// Get the current modelview matrix from opengl.
	glGetFloatv(GL_MODELVIEW_MATRIX, modl);

	// Multiply the two matrices.
	clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

	// Calculate the planes.

	// Right plane.
	frustumPlanes[0][0] = clip[ 3] - clip[ 0];
	frustumPlanes[0][1] = clip[ 7] - clip[ 4];
	frustumPlanes[0][2] = clip[11] - clip[ 8];
	frustumPlanes[0][3] = clip[15] - clip[12];

	// Normalize it.
	t = (float)sqrt(frustumPlanes[0][0] * frustumPlanes[0][0] + 
		            frustumPlanes[0][1] * frustumPlanes[0][1] + 
					frustumPlanes[0][2] * frustumPlanes[0][2]);
	frustumPlanes[0][0] /= t;
	frustumPlanes[0][1] /= t;
	frustumPlanes[0][2] /= t;
	frustumPlanes[0][3] /= t;

	// Left plane.
	frustumPlanes[1][0] = clip[ 3] + clip[ 0];
	frustumPlanes[1][1] = clip[ 7] + clip[ 4];
	frustumPlanes[1][2] = clip[11] + clip[ 8];
	frustumPlanes[1][3] = clip[15] + clip[12];

	// Normalize it.
	t = (float)sqrt(frustumPlanes[1][0] * frustumPlanes[1][0] + 
					frustumPlanes[1][1] * frustumPlanes[1][1] + 
					frustumPlanes[1][2] * frustumPlanes[1][2]);
	frustumPlanes[1][0] /= t;
	frustumPlanes[1][1] /= t;
	frustumPlanes[1][2] /= t;
	frustumPlanes[1][3] /= t;

	// Bottom plane.
	frustumPlanes[2][0] = clip[ 3] + clip[ 1];
	frustumPlanes[2][1] = clip[ 7] + clip[ 5];
	frustumPlanes[2][2] = clip[11] + clip[ 9];
	frustumPlanes[2][3] = clip[15] + clip[13];

	// Normalize it.
	t = (float)sqrt(frustumPlanes[2][0] * frustumPlanes[2][0] + 
					frustumPlanes[2][1] * frustumPlanes[2][1] + 
					frustumPlanes[2][2] * frustumPlanes[2][2]);
	frustumPlanes[2][0] /= t;
	frustumPlanes[2][1] /= t;
	frustumPlanes[2][2] /= t;
	frustumPlanes[2][3] /= t;

	// Top plane.
	frustumPlanes[3][0] = clip[ 3] - clip[ 1];
	frustumPlanes[3][1] = clip[ 7] - clip[ 5];
	frustumPlanes[3][2] = clip[11] - clip[ 9];
	frustumPlanes[3][3] = clip[15] - clip[13];

	// Normalize it.
	t = (float)sqrt(frustumPlanes[3][0] * frustumPlanes[3][0] + 
					frustumPlanes[3][1] * frustumPlanes[3][1] + 
					frustumPlanes[3][2] * frustumPlanes[3][2]);
	frustumPlanes[3][0] /= t;
	frustumPlanes[3][1] /= t;
	frustumPlanes[3][2] /= t;
	frustumPlanes[3][3] /= t;

	// Far plane.
	frustumPlanes[4][0] = clip[ 3] - clip[ 2];
	frustumPlanes[4][1] = clip[ 7] - clip[ 6];
	frustumPlanes[4][2] = clip[11] - clip[10];
	frustumPlanes[4][3] = clip[15] - clip[14];

	// Normalize it.
	t = (float)sqrt(frustumPlanes[4][0] * frustumPlanes[4][0] + 
					frustumPlanes[4][1] * frustumPlanes[4][1] + 
					frustumPlanes[4][2] * frustumPlanes[4][2]);
	frustumPlanes[4][0] /= t;
	frustumPlanes[4][1] /= t;
	frustumPlanes[4][2] /= t;
	frustumPlanes[4][3] /= t;

	// Near plane.
	frustumPlanes[5][0] = clip[ 3] + clip[ 2];
	frustumPlanes[5][1] = clip[ 7] + clip[ 6];
	frustumPlanes[5][2] = clip[11] + clip[10];
	frustumPlanes[5][3] = clip[15] + clip[14];

	// Normalize it.
	t = (float)sqrt(frustumPlanes[5][0] * frustumPlanes[5][0] + 
					frustumPlanes[5][1] * frustumPlanes[5][1] + 
					frustumPlanes[5][2] * frustumPlanes[5][2]);
	frustumPlanes[5][0] /= t;
	frustumPlanes[5][1] /= t;
	frustumPlanes[5][2] /= t;
	frustumPlanes[5][3] /= t;

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: CheckPoint
// Purpose: Checks if a point is viewable inside the 6 planes.
// Inputs: x - The x position of the point.
//         y - The y position of the point.
//         z - The z position of the point.
// Outputs: A boolean value indicating if the point is inside or not.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool FrustumObject::CheckPoint(float x, float y, float z)
{
	int i;

	// Check each of the six planes to make sure the point is in frustum.
	for(i=0; i<6; i++)
	{
		if(frustumPlanes[i][0] * x + frustumPlanes[i][1] * y + 
		   frustumPlanes[i][2] * z + frustumPlanes[i][3] <= 0)
		{
			return false;
		}
	}
	
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: CheckCube
// Purpose: Checks if a cube is viewable inside the 6 planes.
// Inputs: xCenter - The x center point of the cube.
//         yCenter - The y center point of the cube.
//         zCenter - The z center point of the cube.
//         size - The radius of the cube.
// Outputs: A boolean value indicating if the cube can be seen or not.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool FrustumObject::CheckCube(float xCenter, float yCenter, float zCenter, 
							  float size)
{
	int i;

	// Check each of the six planes to see if the cube is inside the frustum.
	for(i=0; i<6; i++)
	{
		if(frustumPlanes[i][0] * (xCenter - size) + 
		   frustumPlanes[i][1] * (yCenter - size) + 
		   frustumPlanes[i][2] * (zCenter - size) + frustumPlanes[i][3] > 0)
		{
			continue;
		}

		if(frustumPlanes[i][0] * (xCenter + size) + 
		   frustumPlanes[i][1] * (yCenter - size) + 
		   frustumPlanes[i][2] * (zCenter - size) + frustumPlanes[i][3] > 0)
		{
			continue;
		}

		if(frustumPlanes[i][0] * (xCenter - size) + 
		   frustumPlanes[i][1] * (yCenter + size) + 
		   frustumPlanes[i][2] * (zCenter - size) + frustumPlanes[i][3] > 0)
		{
			continue;
		}

		if(frustumPlanes[i][0] * (xCenter + size) + 
		   frustumPlanes[i][1] * (yCenter + size) + 
		   frustumPlanes[i][2] * (zCenter - size) + frustumPlanes[i][3] > 0)
		{
			continue;
		}

		if(frustumPlanes[i][0] * (xCenter - size) + 
		   frustumPlanes[i][1] * (yCenter - size) + 
		   frustumPlanes[i][2] * (zCenter + size) + frustumPlanes[i][3] > 0)
		{
			continue;
		}

		if(frustumPlanes[i][0] * (xCenter + size) + 
		   frustumPlanes[i][1] * (yCenter - size) + 
		   frustumPlanes[i][2] * (zCenter + size) + frustumPlanes[i][3] > 0)
		{
			continue;
		}

		if(frustumPlanes[i][0] * (xCenter - size) + 
		   frustumPlanes[i][1] * (yCenter + size) + 
		   frustumPlanes[i][2] * (zCenter + size) + frustumPlanes[i][3] > 0)
		{
			continue;
		}

		if(frustumPlanes[i][0] * (xCenter + size) + 
		   frustumPlanes[i][1] * (yCenter + size) + 
		   frustumPlanes[i][2] * (zCenter + size) + frustumPlanes[i][3] > 0)
		{
			continue;
		}
		
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: CheckSphere
// Purpose: Checks if a sphere is viewable inside the 6 planes.
// Inputs: xCenter - The x center point of the sphere.
//         yCenter - The y center point of the sphere.
//         zCenter - The z center point of the sphere.
//         size - The radius of the sphere.
// Outputs: A boolean value indicating if the sphere can be seen or not.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool FrustumObject::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;

	// Check the six planes to see if the sphere is inside them or not.
	for(i=0; i<6; i++)
	{
		if(frustumPlanes[i][0] * xCenter + frustumPlanes[i][1] * yCenter + 
		   frustumPlanes[i][2] * zCenter + frustumPlanes[i][3] <= -radius)
		{
			return false;
		}
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: CheckRectangle
// Purpose: Checks if a rectangle is viewable inside the 6 planes.
// Inputs: xCenter - The x center point of the rectangle.
//         yCenter - The y center point of the rectangle.
//         zCenter - The z center point of the rectangle.
//         xSize - The size of the rectangle on the x-axis.
//         ySize - The size of the rectangle on the y-axis.
//         zSize - The size of the rectangle on the z-axis.
// Outputs: A boolean value indicating if the rectangle can be seen or not.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool FrustumObject::CheckRectangle(float xCenter, float yCenter, float zCenter, 
								   float xSize, float ySize, float zSize)
{
	int i;

	// Check each of the six planes to see if the rectangle is in the frustum
	// or not.
	for(i=0; i<6; i++)
	{
		if(frustumPlanes[i][0] * (xCenter - xSize) + 
		   frustumPlanes[i][1] * (yCenter - ySize) + 
		   frustumPlanes[i][2] * (zCenter - zSize) + frustumPlanes[i][3] >= 0.0f)
		{
			continue;
		}

		if(frustumPlanes[i][0] * (xCenter + xSize) + 
		   frustumPlanes[i][1] * (yCenter - ySize) + 
		   frustumPlanes[i][2] * (zCenter - zSize) + frustumPlanes[i][3] >= 0.0f)
		{
			continue;
		}

		if(frustumPlanes[i][0] * (xCenter - xSize) + 
		   frustumPlanes[i][1] * (yCenter + ySize) + 
		   frustumPlanes[i][2] * (zCenter - zSize) + frustumPlanes[i][3] >= 0.0f)
		{
			continue;
		}

		if(frustumPlanes[i][0] * (xCenter - xSize) + 
		   frustumPlanes[i][1] * (yCenter - ySize) + 
		   frustumPlanes[i][2] * (zCenter + zSize) + frustumPlanes[i][3] >= 0.0f)
		{
			continue;
		}

		if(frustumPlanes[i][0] * (xCenter + xSize) + 
		   frustumPlanes[i][1] * (yCenter + ySize) + 
		   frustumPlanes[i][2] * (zCenter - zSize) + frustumPlanes[i][3] >= 0.0f)
		{
			continue;
		}

		if(frustumPlanes[i][0] * (xCenter + xSize) + 
		   frustumPlanes[i][1] * (yCenter - ySize) + 
		   frustumPlanes[i][2] * (zCenter + zSize) + frustumPlanes[i][3] >= 0.0f)
		{
			continue;
		}

		if(frustumPlanes[i][0] * (xCenter - xSize) + 
		   frustumPlanes[i][1] * (yCenter + ySize) + 
		   frustumPlanes[i][2] * (zCenter + zSize) + frustumPlanes[i][3] >= 0.0f)
		{
			continue;
		}

		if(frustumPlanes[i][0] * (xCenter + xSize) + 
		   frustumPlanes[i][1] * (yCenter + ySize) + 
		   frustumPlanes[i][2] * (zCenter + zSize) + frustumPlanes[i][3] >= 0.0f)
		{
			continue;
		}
		
		return false;
	}
	
	return true;
}
