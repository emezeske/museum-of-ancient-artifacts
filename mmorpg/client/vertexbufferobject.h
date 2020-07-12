///////////////////////////////////////////////////////////////////////////////
// Filename: vertexbufferobject.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _VERTEXBUFFEROBJECT_H_
#define _VERTEXBUFFEROBJECT_H_


//////////////
// INCLUDES //
//////////////
#include <GL/glut.h>


///////////////////////////////////////////////////////////////////////////////
// Class name: VertexBufferObject
///////////////////////////////////////////////////////////////////////////////
class VertexBufferObject
{
private:
    struct PolyType
	{ 
		float tu, tv;
		float nx, ny, nz;
		float x, y, z;
	};

public:
	VertexBufferObject();
	VertexBufferObject(const VertexBufferObject&);
	~VertexBufferObject();

	bool Create(unsigned int);
	void Free();
	void Reset();
	void AddData(float, float, float, float, float, float, float, float);
	
	void Render(unsigned int);
	
private:
	unsigned int endPos;
	PolyType* polyBuffer;
};

#endif
