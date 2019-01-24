

#ifndef PHYSICSMANAGER_H_
#define PHYSICSMANAGER_H_


#include <PxPhysicsAPI.h>
//#include <extensions/PxDefaultAllocator.h> 
//#include <extensions/PxDefaultErrorCallback.h>
//#include <PxFoundation.h>

//class PxFoundation;

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
	physx::PxFoundation* test;
};





#endif // PHYSICSMANAGER_H_
