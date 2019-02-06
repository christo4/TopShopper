


#ifndef PHYSICSMANAGER_H_
#define PHYSICSMANAGER_H_

#include "PxPhysicsAPI.h"
#include <memory>
#include "objects/shoppingcartplayer.h"
#include "core/gamescene.h"

class Broker;



/////////////////////////////////////////////////////////////////////////////
// FRICTION STUFF...

enum DrivableSurfaceTypes {
	WAXED_FLOOR=0,
	NUMBER_OF_DRIVABLE_SURFACE_TYPES
};

enum TireTypes {
	NORMAL=0,
	NUMBER_OF_TIRE_TYPES
};

void initFrictionPairs();


/////////////////////////////////////////////////////////////////////////////
// COLLISION FILTERING STUFF...

// describes what each entity type / actors can collide with...
// each collision flag should be unique, but against flags can be duplicates
// can have at most 32 shift
enum CollisionFlags {
	COLLISION_FLAG_GROUND				= (1 << 0),
	COLLISION_FLAG_WHEEL				= (1 << 1),
	COLLISION_FLAG_CHASSIS				= (1 << 2),
	COLLISION_FLAG_OBSTACLE				= (1 << 3),
	COLLISION_FLAG_DRIVABLE_OBSTACLE	= (1 << 4),

	COLLISION_FLAG_GROUND_AGAINST				=													COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_WHEEL_AGAINST				=							COLLISION_FLAG_WHEEL |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE,
	COLLISION_FLAG_CHASSIS_AGAINST				= COLLISION_FLAG_GROUND |	COLLISION_FLAG_WHEEL |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_OBSTACLE_AGAINST				= COLLISION_FLAG_GROUND |	COLLISION_FLAG_WHEEL |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST	= COLLISION_FLAG_GROUND |							COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE
};

physx::PxFilterFlags VehicleFilterShader
(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
 physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
 physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);







class PhysicsManager {
public:
	PhysicsManager(Broker* broker);
	virtual ~PhysicsManager();
	void init();
	void updateMilliseconds(double deltaTime);
	void cleanup();

	/*
	// MAYBE MOVE THESE INTO A SPECIAL PHYSICS COMPONENT
	void disableShapeInContactTests(physx::PxShape *shape);
	void enableShapeInContactTests(physx::PxShape *shape);
	void disableShapeInSceneQueryTests(physx::PxShape *shape);
	void enableShapeInSceneQueryTests(physx::PxShape *shape);
	void setShapeTrigger(physx::PxShape *shape);
	void setShapeSolid(physx::PxShape *shape);
	*/

	// EXAMPLES:
	//physx::PxRigidStatic* createEnvironmentGeometryBase();
	//physx::PxRigidStatic* createGroundBase();
	//physx::PxRigidStatic* createPuddleBase();
	//physx::PxRigidDynamic* createShoppingCartBotBase();
	//physx::PxRigidDynamic* createShoppingCartObstacleBase();

	std::shared_ptr<ShoppingCartPlayer> instantiateShoppingCartPlayer();

	//physx::PxRigidStatic* createUIBase();

	void switchToScene1();


	std::shared_ptr<GameScene> getActiveScene() { return _activeScene; }


private:
	Broker* _broker = nullptr;

	std::shared_ptr<GameScene> _activeScene = nullptr;
	//std::vector<std::shared_ptr<Scene>> _scenes;
};





#endif // PHYSICSMANAGER_H_
