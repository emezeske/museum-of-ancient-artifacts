#include <GL/gl.h>
#include <GL/glut.h>

#include "basic_rigid_body.h"

/////////////////////////////////////////////////////////////////
// BasicRigidBody static member functions
/////////////////////////////////////////////////////////////////

btRigidBody *BasicRigidBody::createBtRigidBody( const btScalar mass, btDefaultMotionState &motionState, btCollisionShape &shape )
{
	btVector3 localInertia( 0, 0, 0 );

	if ( mass != 0.0f ) shape.calculateLocalInertia( mass, localInertia ); // A rigid body is dynamic if and only if mass is non zero, otherwise it's static.

	shape.setMargin( COLLISION_MARGIN );

	// Using motionstate provides interpolation capabilities, and only synchronizes 'active' objects
	return new btRigidBody( mass, &motionState, &shape, localInertia );
}

/////////////////////////////////////////////////////////////////
// BasicRigidBody instance member functions
/////////////////////////////////////////////////////////////////

BasicRigidBody::~BasicRigidBody() {}

/////////////////////////////////////////////////////////////////
// RigidSphere instance member functions
/////////////////////////////////////////////////////////////////

RigidSphere::RigidSphere( const btScalar mass, const btScalar radius, const btTransform &startTransform ) :
	motionState( startTransform ),
	sphereShape( radius )
{
	body = createBtRigidBody( mass, motionState, sphereShape );
}

RigidSphere::~RigidSphere()
{
	delete body;
}

void RigidSphere::drawDebug()
{
	float m[16];
	motionState.m_graphicsWorldTrans.getOpenGLMatrix( m );
	
	glPushMatrix();
		glMultMatrixf( m );
		glColor3f( 0.3f, 0.3f, 1.0f );
		glutSolidSphere( sphereShape.getRadius(), 30, 30 );
	glPopMatrix();
}

/////////////////////////////////////////////////////////////////
// RigidBox instance member functions
/////////////////////////////////////////////////////////////////

RigidBox::RigidBox( const btScalar mass, const btVector3 boxHalfExtents, const btTransform &startTransform ) :
	motionState( startTransform ),
	boxShape( boxHalfExtents )
{
	body = createBtRigidBody( mass, motionState, boxShape );
}

RigidBox::~RigidBox()
{
	delete body;
}

void RigidBox::drawDebug()
{
	float m[16];
	motionState.m_graphicsWorldTrans.getOpenGLMatrix( m );
	
	GLfloat
		x = boxShape.getHalfExtents().x(),
		y = boxShape.getHalfExtents().y(),
		z = boxShape.getHalfExtents().z();

	glPushMatrix();
		glMultMatrixf( m );
		glColor3f( 0.8f, 0.4f, 0.0f );
		#define V(a,b,c) glVertex3d( a, b, c );
		#define N(a,b,c) glNormal3d( a, b, c );
		glBegin( GL_QUADS );
		    N( 1, 0, 0 );  V( +x, -y, +z ); V( +x, -y, -z ); V( +x, +y, -z ); V( +x, +y, +z );
		    N( 0, 1, 0 );  V( +x, +y, +z ); V( +x, +y, -z ); V( -x, +y, -z ); V( -x, +y, +z );
		    N( 0, 0, 1 );  V( +x, +y, +z ); V( -x, +y, +z ); V( -x, -y, +z ); V( +x, -y, +z );
		    N( -1, 0, 0 ); V( -x, -y, +z ); V( -x, +y, +z ); V( -x, +y, -z ); V( -x, -y, -z );
		    N( 0, -1, 0 ); V( -x, -y, +z ); V( -x, -y, -z ); V( +x, -y, -z ); V( +x, -y, +z );
		    N( 0, 0, -1 ); V( -x, -y, -z ); V( -x, +y, -z ); V( +x, +y, -z ); V( +x, -y, -z );
		glEnd( );
		#undef V
		#undef N
	glPopMatrix();
}

/////////////////////////////////////////////////////////////////
// RigidTrimesh instance member functions
/////////////////////////////////////////////////////////////////

RigidTrimesh::RigidTrimesh( const btScalar mass, const btTransform &startTransform, TrimeshRep *trimeshRep, bool takeOwnershipTrimeshRep /* = true */ ) :
	motionState( startTransform ),
	rep( trimeshRep )
{
	float *vertices = reinterpret_cast< float* >( rep->getVertices() );
	int *indices = rep->getIndices();

	int
		numVertices = rep->getNumVertices(),
		numTriangles = rep->getNumTriangles(),
		vertexStride = sizeof( btVector3 ),
		indexStride = 3 * sizeof( int );

	indexVertexArray = new btTriangleIndexVertexArray( numTriangles, indices, indexStride, numVertices, vertices, vertexStride );
	meshShape = new btBvhTriangleMeshShape( indexVertexArray );

	body = createBtRigidBody( mass, motionState, *meshShape );
}

RigidTrimesh::~RigidTrimesh()
{
	if ( ownTrimeshRep ) delete rep;
	delete indexVertexArray;
	delete meshShape;
	delete body;
}

void RigidTrimesh::drawDebug()
{
	float m[16];
	motionState.m_graphicsWorldTrans.getOpenGLMatrix( m );

	glPushMatrix();
		glMultMatrixf( m );

		btVector3 aabbMax( 1e30f, 1e30f, 1e30f );
		btVector3 aabbMin( -1e30f, -1e30f, -1e30f);
		
		meshShape->processAllTriangles( this, aabbMin, aabbMax );
	glPopMatrix();
}

void RigidTrimesh::processTriangle( btVector3* triangle, int partId, int triangleIndex )
{
	btVector3 normal = ( triangle[1] - triangle[2] ).cross( triangle[0] - triangle[1] );
	normal.normalize();

	glBegin( GL_TRIANGLES );
		glColor3f( 0.3f, 0.4f, 0.3f );
		glNormal3d( normal.x(), normal.y(), normal.z() );
		glVertex3d( triangle[0].getX(), triangle[0].getY(), triangle[0].getZ() );
		glVertex3d( triangle[1].getX(), triangle[1].getY(), triangle[1].getZ() );
		glVertex3d( triangle[2].getX(), triangle[2].getY(), triangle[2].getZ() );
	glEnd();
}
