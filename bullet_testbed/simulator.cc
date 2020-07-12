#include "simulator.h"

/////////////////////////////////////////////////////////////////
// Instance member functions
/////////////////////////////////////////////////////////////////

Simulator::Simulator()
{
	timeAccumulator = 0.0f;

	dispatcher = new btCollisionDispatcher( );
	overlappingPairCache = new btSimpleBroadphase;
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld( dispatcher, overlappingPairCache, solver );
	dynamicsWorld = new btDiscreteDynamicsWorld();

	dynamicsWorld->setGravity( btVector3( 0.0f, -10.0f, 0.0f ) );
}

Simulator::~Simulator()
{
	delete dynamicsWorld;
	delete solver;
	delete overlappingPairCache;
	delete dispatcher;
}

void Simulator::addRigidBody( btRigidBody *body )
{
	dynamicsWorld->addRigidBody( body );
}

void Simulator::removeRigidBody( btRigidBody *body )
{
	dynamicsWorld->removeRigidBody( body );
}

void Simulator::resetCollisionObjects()
{
	dynamicsWorld->stepSimulation( 1.0f / 60.0f, 0 );

	int numObjects = dynamicsWorld->getNumCollisionObjects();
	
	for ( int i = 0; i < numObjects; i++ )
	{
		btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody *body = btRigidBody::upcast( colObj );

		if ( body && body->getMotionState() )
		{
			btDefaultMotionState *myMotionState = ( btDefaultMotionState *)body->getMotionState();
			myMotionState->m_graphicsWorldTrans = myMotionState->m_startWorldTrans;
			colObj->setWorldTransform( myMotionState->m_graphicsWorldTrans );
			colObj->setInterpolationWorldTransform( myMotionState->m_startWorldTrans );
			colObj->activate();

			dynamicsWorld->getBroadphase()->cleanProxyFromPairs( colObj->getBroadphaseHandle() ); // Removed cached contact points

			btRigidBody *body = btRigidBody::upcast( colObj );

			if ( body && !body->isStaticObject() )
			{
				btRigidBody::upcast( colObj )->setLinearVelocity( btVector3( 0, 0, 0 ) );
				btRigidBody::upcast( colObj )->setAngularVelocity( btVector3( 0, 0, 0 ) );
			}
		}
	}
}

void Simulator::doOneStep( double stepTime )
{
	timeAccumulator += stepTime;

	if ( timeAccumulator >= SIMULATION_STEP_SIZE )
	{
		dynamicsWorld->stepSimulation( timeAccumulator, 2, 1.0f / 120.0f );
		timeAccumulator = 0.0f;
	}
}
