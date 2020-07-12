#ifndef BASIC_RIGID_BODY_H
#define BASIC_RIGID_BODY_H

#include <btBulletDynamicsCommon.h>

#include "trimesh_rep.h"

class BasicRigidBody
{
	static const btScalar COLLISION_MARGIN = 0.05f;

	public:
 		static btRigidBody *createBtRigidBody( const btScalar mass, btDefaultMotionState &motionState, btCollisionShape &shape );

		virtual ~BasicRigidBody() = 0;

		virtual btRigidBody *getBtRigidBody() const = 0;
		virtual void drawDebug() = 0;
};

class RigidSphere : public BasicRigidBody
{
	btRigidBody *body;
	btDefaultMotionState motionState;
	btSphereShape sphereShape;

	public:
		RigidSphere( const btScalar mass, const btScalar radius, const btTransform &startTransform );
		~RigidSphere();

		virtual btRigidBody *getBtRigidBody() const { return body; }
		virtual void drawDebug();
};

class RigidBox : public BasicRigidBody
{
	btRigidBody *body;
	btDefaultMotionState motionState;
	btBoxShape boxShape;

	public:
		RigidBox( const btScalar mass, const btVector3 boxHalfExtents, const btTransform &startTransform );
		~RigidBox();

		virtual btRigidBody *getBtRigidBody() const { return body; }
		virtual void drawDebug();
};

class RigidTrimesh : public BasicRigidBody, public btTriangleCallback
{
	btRigidBody *body;
	btDefaultMotionState motionState;
	btTriangleIndexVertexArray *indexVertexArray;
	btBvhTriangleMeshShape *meshShape;

	TrimeshRep *rep;

	bool ownTrimeshRep;

	public:
		RigidTrimesh( const btScalar mass, const btTransform &startTransform, TrimeshRep *trimeshRep, bool takeOwnershipTrimeshRep = true );
		~RigidTrimesh();

		virtual btRigidBody *getBtRigidBody() const { return body; }
		virtual void drawDebug();

		virtual void processTriangle( btVector3* triangle, int partId, int triangleIndex );
};

#endif // BASIC_RIGID_BODY_H
