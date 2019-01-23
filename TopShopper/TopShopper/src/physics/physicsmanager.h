


#ifndef PHYSICSMANAGER_H_
#define PHYSICSMANAGER_H_


#include <PxPhysicsAPI.h>
#include <iostream>


class PhysicsManager {
public:
	PhysicsManager();
	virtual ~PhysicsManager();
	void init();
	void updateMilliseconds(double deltaTime);
	void createObject();
private:
	static physx::PxDefaultAllocator defaultAlloc;
	static physx::PxDefaultErrorCallback defaultError;
	PxFoundation* test;
};




















#endif // PHYSICSMANAGER_H_
