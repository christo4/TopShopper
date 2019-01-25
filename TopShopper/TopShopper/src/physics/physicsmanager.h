


#ifndef PHYSICSMANAGER_H_
#define PHYSICSMANAGER_H_

#include <PxPhysicsAPI.h>

class Broker;


class PhysicsManager {
public:
	PhysicsManager(Broker* broker);
	virtual ~PhysicsManager();
	void init();
	void updateMilliseconds(double deltaTime);
	void cleanup();

	// MAYBE MOVE THESE INTO A SPECIAL PHYSICS COMPONENT
	void disableShapeInContactTests(physx::PxShape *shape);
	void enableShapeInContactTests(physx::PxShape *shape);
	void disableShapeInSceneQueryTests(physx::PxShape *shape);
	void enableShapeInSceneQueryTests(physx::PxShape *shape);
	void setShapeTrigger(physx::PxShape *shape);
	void setShapeSolid(physx::PxShape *shape);




	physx::PxRigidStatic* createStaticActor();
	physx::PxRigidDynamic* createDynamicActor();




	physx::PxPhysics* getFactory();

private:
	Broker* _broker = nullptr;

	physx::PxDefaultAllocator _defaultAlloc;
	physx::PxDefaultErrorCallback _defaultError;
	physx::PxFoundation *_foundation = nullptr;
	physx::PxPhysics *_factory = nullptr; // this is the factory we use to create new PhysX objects
	physx::PxCooking *_cooker = nullptr;
	physx::PxTolerancesScale _simulationScale;
	//physx::PxPvd *_pvd;
};





#endif // PHYSICSMANAGER_H_
