


#ifndef PHYSICSMANAGER_H_
#define PHYSICSMANAGER_H_


#include <PxPhysicsAPI.h>



class PhysicsManager {
public:
	PhysicsManager();
	virtual ~PhysicsManager();
	void init();
	void updateMilliseconds(double deltaTime);
	void cleanup();
private:
	physx::PxDefaultAllocator _defaultAlloc;
	physx::PxDefaultErrorCallback _defaultError;
	physx::PxFoundation *_foundation = nullptr;
	physx::PxPhysics *_factory = nullptr; // this is the factory we use to create new PhysX objects
	physx::PxCooking *_cooker = nullptr;
	physx::PxTolerancesScale _simulationScale;
	//physx::PxPvd *_pvd;
};





#endif // PHYSICSMANAGER_H_
