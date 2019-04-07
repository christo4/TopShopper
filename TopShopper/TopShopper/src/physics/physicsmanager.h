#ifndef PHYSICSMANAGER_H_
#define PHYSICSMANAGER_H_

#include "PxPhysicsAPI.h"
#include <memory>
#include "core/gamescene.h"

#include "objects/shoppingcartplayer.h"
#include "objects/ground.h"
#include "objects/roof.h"
#include "objects/milk.h"
#include "objects/water.h"
#include "objects/cola.h"
#include "objects/apple.h"
#include "objects/watermelon.h"
#include "objects/banana.h"
#include "objects/carrot.h"
#include "objects/eggplant.h"
#include "objects/broccoli.h"
#include "objects/sparechange.h"
#include "objects/obstacle1.h"
#include "objects/obstacle2.h"
#include "objects/obstacle3.h"
#include "objects/obstacle4.h"
#include "objects/obstacle5.h"
#include "objects/obstacle6.h"
#include "objects/obstacle7.h"
#include "objects/cookie.h"
#include "objects/mysterybag.h"


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
// can have at most 32 shifts (0-31)
enum CollisionFlags {
	COLLISION_FLAG_GROUND				= (1 << 0),
	COLLISION_FLAG_WHEEL				= (1 << 1),
	COLLISION_FLAG_CHASSIS				= (1 << 2),
	COLLISION_FLAG_OBSTACLE				= (1 << 3),
	COLLISION_FLAG_DRIVABLE_OBSTACLE	= (1 << 4),
	COLLISION_FLAG_PICKUP				= (1 << 5),
	//COLLISION_FLAG_LOOKAT_SHAPE			= (1 << 6),
	//COLLISION_FLAG_CAMERA_SHAPE		    = (1 << 7),

	COLLISION_FLAG_GROUND_AGAINST				=						  COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE | COLLISION_FLAG_PICKUP,
	COLLISION_FLAG_WHEEL_AGAINST				= COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE |	COLLISION_FLAG_DRIVABLE_OBSTACLE | COLLISION_FLAG_PICKUP,
	COLLISION_FLAG_CHASSIS_AGAINST				= COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE | COLLISION_FLAG_PICKUP,
	COLLISION_FLAG_OBSTACLE_AGAINST				= COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE | COLLISION_FLAG_PICKUP,
	COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST	= COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE | COLLISION_FLAG_PICKUP,
	COLLISION_FLAG_PICKUP_AGAINST				= COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE | COLLISION_FLAG_PICKUP
	//COLLISION_FLAG_LOOKAT_SHAPE_AGAINST = 0,
	//COLLISION_FLAG_CAMERA_SHAPE_AGAINST = 0
};



physx::PxFilterFlags CustomFilterShader
(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);

class CustomSimulationEventCallback : public physx::PxSimulationEventCallback {
public:
	void onAdvance(const physx::PxRigidBody *const *bodyBuffer, const physx::PxTransform *poseBuffer, const physx::PxU32 count) override;
	void onConstraintBreak(physx::PxConstraintInfo *constraints, physx::PxU32 count) override;
	void onContact(const physx::PxContactPairHeader &pairHeader, const physx::PxContactPair *pairs, physx::PxU32 nbPairs) override;
	void onSleep(physx::PxActor **actors, physx::PxU32 count) override;
	void onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count) override;
	void onWake(physx::PxActor **actors, physx::PxU32 count) override;
};




struct ContactCollision {

	enum ContactCollisionTypes {
		ENTER,
		EXIT
	};

	ContactCollision(ContactCollisionTypes collisionType, std::shared_ptr<BehaviourScript> caller, physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) 
		: _collisionType(collisionType), _caller(caller), _localShape(localShape), _otherShape(otherShape), _otherEntity(otherEntity), _contacts(contacts), _nbContacts(nbContacts)
	{

	}
	
	ContactCollisionTypes _collisionType;
	std::shared_ptr<BehaviourScript> _caller = nullptr;
	physx::PxShape *_localShape = nullptr;
	physx::PxShape *_otherShape = nullptr;
	Entity *_otherEntity = nullptr;
	physx::PxContactPairPoint *_contacts = nullptr;
	physx::PxU32 _nbContacts;

};



struct TriggerCollision {

	enum TriggerCollisionTypes {
		ENTER,
		EXIT
	};

	TriggerCollision(TriggerCollisionTypes collisionType, std::shared_ptr<BehaviourScript> caller, physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity)
		: _collisionType(collisionType), _caller(caller), _localShape(localShape), _otherShape(otherShape), _otherEntity(otherEntity)
	{

	}

	TriggerCollisionTypes _collisionType;
	std::shared_ptr<BehaviourScript> _caller = nullptr;
	physx::PxShape *_localShape = nullptr;
	physx::PxShape *_otherShape = nullptr;
	Entity *_otherEntity = nullptr;

};







class PhysicsManager {
public:
	PhysicsManager(Broker *broker);
	virtual ~PhysicsManager();
	void init();
	void updateSeconds(double fixedDeltaTime);
	void cleanup();

	void loadScene1();
	void cleanupScene1();

	std::shared_ptr<Entity> instantiateEntity(EntityTypes type, physx::PxTransform transform, const char *name);
	//void switchToScene1();
	std::shared_ptr<GameScene> getActiveScene() { return _activeScene; }


	

	/*
	// MAYBE MOVE THESE INTO A SPECIAL PHYSICS COMPONENT
	void disableShapeInContactTests(physx::PxShape *shape);
	void enableShapeInContactTests(physx::PxShape *shape);
	void disableShapeInSceneQueryTests(physx::PxShape *shape);
	void enableShapeInSceneQueryTests(physx::PxShape *shape);
	void setShapeTrigger(physx::PxShape *shape);
	void setShapeSolid(physx::PxShape *shape);
	*/

	physx::PxShape** getAllShapes();
	physx::PxU32 getNbShapes();


	// returns true if any touching/blocking hit was found
	bool raycast(const physx::PxVec3 &origin, const physx::PxVec3 &unitDir, const physx::PxReal distance, physx::PxRaycastCallback &hitCall);




private:
	Broker *_broker = nullptr;

	std::shared_ptr<GameScene> _activeScene = nullptr;


	physx::PxShape* createSphereCollider(physx::PxReal radius, physx::PxMaterial *material, const physx::PxFilterData& simData, const physx::PxFilterData& qryData, bool isExclusive, physx::PxShapeFlags shapeFlags);
	physx::PxShape* createBoxCollider(physx::PxReal xSize, physx::PxReal ySize, physx::PxReal zSize, physx::PxMaterial *material, const physx::PxFilterData& simData, const physx::PxFilterData& qryData, bool isExclusive, physx::PxShapeFlags shapeFlags);
	physx::PxShape* createTriMeshCollider(const std::vector<physx::PxVec3>& verts, const std::vector<physx::PxU32>& indices, physx::PxMaterial *material, const physx::PxFilterData& simData, const physx::PxFilterData& qryData, bool isExclusive, physx::PxShapeFlags shapeFlags);



};














#endif // PHYSICSMANAGER_H_
