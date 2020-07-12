#ifndef SIMULATION_BODY_H
#define SIMULATION_BODY_H

#include <vector>
#include <ode/ode.h>

class SimulationBody
{
	virtual ~SimulationBody() = 0;

	private:

		dBodyID body;
		std::vector< dGeomID > geom;
};

class SphereBody
{
};

#endif // SIMULATION_BODY_H
