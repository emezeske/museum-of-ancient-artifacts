#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <btBulletDynamicsCommon.h>

class Simulator
{
	public:
		Simulator();
		virtual ~Simulator();

		virtual void addRigidBody( btRigidBody *body );
		virtual void removeRigidBody( btRigidBody *body );
		virtual void resetCollisionObjects();

		virtual void doOneStep( double stepTime );

	protected:
		static const float SIMULATION_STEP_SIZE = 1.0f / 60.0f;
		
		btDynamicsWorld *dynamicsWorld;
		btOverlappingPairCache *overlappingPairCache;
		btCollisionDispatcher *dispatcher;
		btConstraintSolver *solver;
		
		double timeAccumulator;
};

#endif // SIMULATOR_H
