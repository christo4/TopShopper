


#ifndef PHYSICSMANAGER_H_
#define PHYSICSMANAGER_H_


#include <PxPhysicsAPI.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <PxSimulationEventCallback.h>
#include <iostream>




class PhysicsManager {
public:
	PhysicsManager();
	virtual ~PhysicsManager();
	void init();
	void updateMilliseconds(double deltaTime);
	void createObject();
private:
	physx::PxDefaultAllocator defaultAlloc;
	physx::PxDefaultErrorCallback defaultError;
	physx::PxFoundation* test;
};




















#endif // PHYSICSMANAGER_H_
