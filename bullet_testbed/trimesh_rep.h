#ifndef TRIMESH_REP_H
#define TRIMESH_REP_H

#include <btBulletDynamicsCommon.h>

class TrimeshRep
{
	public:
		TrimeshRep() {};
		virtual ~TrimeshRep() = 0;

		virtual int getNumVertices() const = 0;
		virtual int getNumTriangles() const = 0;
		virtual btVector3 *getVertices() const = 0;
		virtual int *getIndices() const = 0;
};

class RandHeightfieldRep : public TrimeshRep
{
	public:
		RandHeightfieldRep( int w, int d, float wscale, float dscale );
		virtual ~RandHeightfieldRep();

		virtual int getNumVertices() const { return numVertices; }
		virtual int getNumTriangles() const { return numTriangles; }
		virtual btVector3 *getVertices() const { return vertices; }
		virtual int *getIndices() const { return indices; }

	private:
		btVector3 *vertices;
		
		int
			*indices,
			width,
			depth,
			numVertices,
			numTriangles;
};

#endif // TRIMESH_REP_H
