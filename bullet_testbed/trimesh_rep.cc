#include "trimesh_rep.h"

/////////////////////////////////////////////////////////////////
// TrimeshRep instance member functions
/////////////////////////////////////////////////////////////////

TrimeshRep::~TrimeshRep() {}

/////////////////////////////////////////////////////////////////
// RandHeightfieldRep instance member functions
/////////////////////////////////////////////////////////////////

RandHeightfieldRep::RandHeightfieldRep( int w, int d, float wscale, float dscale ) :
	TrimeshRep(),
	width( w ),
	depth( d )
{
	int index = 0;

	numVertices = w * d;
	numTriangles = 2 * ( w - 1) * ( d - 1 );

	vertices = new btVector3[numVertices];
	indices = new int[numTriangles * 3];

	for ( int x = 0; x < width; ++x )
	{
		for ( int z = 0; z < depth; ++z )
		{
			float
				wl = 0.08f,
				height = 20.0f * sinf( wl * x ) * cosf( wl * z );
			vertices[x + z * width].setValue( ( x - width * 0.5f ) * wscale, height, ( z - depth * 0.5f ) * dscale );
		}
	}

	for ( int x = 0; x < width - 1; ++x )
	{
		for ( int z = 0; z < depth - 1; ++z )
		{
			indices[index++] = z * width + x;
			indices[index++] = z * width + x + 1;
			indices[index++] = ( z + 1 ) * width + x + 1;
                           
			indices[index++] = z * width + x;
			indices[index++] = ( z + 1 ) * width + x + 1;
			indices[index++] = ( z + 1 ) * width + x;
		}
	}
}

RandHeightfieldRep::~RandHeightfieldRep()
{
	delete indices;
	delete vertices;
}
