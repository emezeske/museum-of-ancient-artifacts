///////////////////////////////////////////////////////////////////////////////
// Filename: vertexbufferobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "vertexbufferobject.h"


///////////////////////////////////////////////////////////////////////////////
// Function name: VertexBufferObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: Initializes member variables.
///////////////////////////////////////////////////////////////////////////////
VertexBufferObject::VertexBufferObject()
{
	// Initialize the position in the vertex buffer.
	endPos = 0;

	// Set the vertex buffer to null until it is created.
	polyBuffer = 0;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: VertexBufferObject
// Purpose: Copy constructor.
// Inputs: other - The VertexBufferObject that is being copied.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
VertexBufferObject::VertexBufferObject(const VertexBufferObject& other)
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ~VertexBufferObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: Frees allocated memory.
///////////////////////////////////////////////////////////////////////////////
VertexBufferObject::~VertexBufferObject()
{
	// Delete the vertex buffer if it is still not deleted.
	if(polyBuffer)
	{
		delete [] polyBuffer;
		polyBuffer = 0;
	}
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Create
// Purpose: Creates the vertex buffer.
// Inputs: numVerts - The size to make the vertex buffer.
// Outputs: A boolean variable indicating whether the vertex buffer was created
//          or not.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool VertexBufferObject::Create(unsigned int numVerts)
{
	// Initialize the position in the vertex buffer.
	endPos = 0;

	// Create the vertex buffer.
	polyBuffer = new PolyType[numVerts];

	// If it could not create it due to lack of memory then return false.
	if(!polyBuffer)
	{
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Free
// Purpose: De-allocates memory assigned to the vertex buffer.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void VertexBufferObject::Free()
{
	// If memory is assigned to the vertex buffer then delete it.
	if(polyBuffer)
	{
		delete polyBuffer;
		polyBuffer = 0;
	}
	
	// Reset the position in the vertex buffer.
	endPos = 0;

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Reset
// Purpose: Resets the position in the vertex buffer to the beginning.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void VertexBufferObject::Reset()
{
	endPos = 0;
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: AddData
// Purpose: Adds data to the vertex buffer.
// Inputs: x - The x vertex.
//         y - The y vertex.
//         z - The z vertex.
//         tu - The u texture coordinate.
//         tv - The v texture coordinate.
//         nx - The x normal.
//         ny - The y normal.
//         nz - The z normal.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void VertexBufferObject::AddData(float x, float y, float z, 
								 float tu, float tv, 
								 float nx, float ny, float nz)
{
	// Add vertex data to the end of the array.
	polyBuffer[endPos].tu = tu;
	polyBuffer[endPos].tv = tv;
	polyBuffer[endPos].nx = nx;
	polyBuffer[endPos].ny = ny;
	polyBuffer[endPos].nz = nz;
	polyBuffer[endPos].x = x;
	polyBuffer[endPos].y = y;
	polyBuffer[endPos].z = z;

	// Increment the array position.
	endPos++;

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Render
// Purpose: Draws the vertex buffer.
// Inputs: numPolysToDraw - The number of polygons to draw.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void VertexBufferObject::Render(unsigned int numPolysToDraw)
{
	glInterleavedArrays(GL_T2F_N3F_V3F, 0, polyBuffer);
	glDrawArrays(GL_TRIANGLES, 0, numPolysToDraw * 3);
	return;
}
