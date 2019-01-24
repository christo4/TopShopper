


#include "physicsmanager.h"

#include <iostream>


using namespace physx;

PhysicsManager::PhysicsManager() {

}

PhysicsManager::~PhysicsManager() {

}


void PhysicsManager::init() {
	_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, _defaultAlloc, _defaultError);
	if (!_foundation) {
		std::cout << "PxCreateFoundation failed!" << std::endl;
		exit(EXIT_FAILURE);
	}

	_simulationScale.length = 1.0f; // 1 meter by default
	_simulationScale.speed = 9.81f; // 9.81 m/s by default (speed reached after falling for 1 second under 9.81m/s^2 gravity)
	
	bool recordMemoryAllocations = true;

	// WITH PVD...
	/*
	_pvd = PxCreatePvd(*_foundation);
	PxPvdTransport *transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	_pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	_factory = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, _simulationScale, recordMemoryAllocations, _pvd);
	*/

	// W/O PVD...
	_factory = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, _simulationScale, recordMemoryAllocations);
	if (!_factory) {
		std::cout << "PxCreatePhysics failed!" << std::endl;
		exit(EXIT_FAILURE);
	}



	_cooker = PxCreateCooking(PX_PHYSICS_VERSION, *_foundation, PxCookingParams(_simulationScale));
	if (!_cooker) {
		std::cout << "PxCreateCooking failed!" << std::endl;
		exit(EXIT_FAILURE);
	}




}


void PhysicsManager::updateMilliseconds(double deltaTime) {

}



void PhysicsManager::cleanup() {
	_factory->release();
	_foundation->release();
}




/*
NOTE: https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/Startup.html

SHUTTING DOWN...

To dispose of any PhysX object, call its release() method. 
This will destroy the object, and all contained objects. 
The precise behavior depends on the object type being released, so refer to the reference guide for details. 
To shut down the extensions library, call the function PxCloseExtensions(). 
To shut down physics, call release() on the PxPhysics object, and this will clean up all of the physics objects:

mPhysics->release(); // for us, _factory->release()

Do not forget to release the foundation object as well, but only after all other PhysX modules have been released:

mFoundation->release(); // for us _foundation->release()

*/