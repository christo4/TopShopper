//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2018 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

// ****************************************************************************
// This snippet illustrates simple use of PxVehicleDrive4W.
//
// It creates a vehicle on a plane and then controls the vehicle so that it performs a 
// number of choreographed manoeuvres such as accelerate, reverse, brake, handbrake, and turn.

// It is a good idea to record and playback with pvd (PhysX Visual Debugger).
// ****************************************************************************



#define PVD_HOST "127.0.0.1" // Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to. DEFAULT = LOCALHOST


#include <ctype.h>

#include "physicsmanager.h"
#include <iostream>

#include "vehicle/PxVehicleUtil.h"
#include "vehicle/snippetvehiclecommon/SnippetVehicleSceneQuery.h"
#include "vehicle/snippetvehiclecommon/SnippetVehicleCreate.h"
#include "vehicle/VehicleShoppingCart.h"

#include "utility/utility.h"

#include "core/broker.h"
#include "rendering/Geometry.h"



using namespace physx;
using namespace snippetvehicle;


PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation*			gFoundation = NULL;
PxPhysics*				gPhysics = NULL;

PxDefaultCpuDispatcher*	gDispatcher = NULL;

PxCooking*				gCooking = NULL;

#ifdef PVD_ENABLED
PxPvd*                  gPvd = NULL;
#endif // PVD_ENABLED

VehicleSceneQueryData*	gVehicleSceneQueryData = NULL;
PxBatchQuery*			gBatchQuery = NULL;

PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

CustomSimulationEventCallback gSimEventCallback;



///////////
// TEMPORARY: for Milestone 2 only
//std::vector<PxTransform> gSpawnPoints = { PxTransform(40.0f, 2.0f, 40.0f, PxQuat(PxIdentity)), PxTransform(-40.0f, 2.0f, 40.0f, PxQuat(PxIdentity)), PxTransform(40.0f, 2.0f, -40.0f, PxQuat(PxIdentity)), PxTransform(-40.0f, 2.0f, -40.0f, PxQuat(PxIdentity)) };
//int gSpawnID = 0;



std::vector<ContactCollision> gContactCollisions;
std::vector<TriggerCollision> gTriggerCollisions;


/////////////////////////////////////////////////////////////////////////////
// FRICTION STUFF...

void initFrictionPairs() {
	// array of drivable surface types whose IDS get set to enum values
	PxVehicleDrivableSurfaceType surfaceTypes[DrivableSurfaceTypes::NUMBER_OF_DRIVABLE_SURFACE_TYPES];
	surfaceTypes[DrivableSurfaceTypes::WAXED_FLOOR].mType = DrivableSurfaceTypes::WAXED_FLOOR;

	// array of materials associated with each surface type
	const PxMaterial* surfaceMaterials[DrivableSurfaceTypes::NUMBER_OF_DRIVABLE_SURFACE_TYPES];
	surfaceMaterials[DrivableSurfaceTypes::WAXED_FLOOR] = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	// alloc memeory to hold info about each combo
	PxVehicleDrivableSurfaceToTireFrictionPairs *surfaceTirePairs = PxVehicleDrivableSurfaceToTireFrictionPairs::allocate(TireTypes::NUMBER_OF_TIRE_TYPES, DrivableSurfaceTypes::NUMBER_OF_DRIVABLE_SURFACE_TYPES);

	// setup the data to be all together
	surfaceTirePairs->setup(TireTypes::NUMBER_OF_TIRE_TYPES, DrivableSurfaceTypes::NUMBER_OF_DRIVABLE_SURFACE_TYPES, surfaceMaterials, surfaceTypes);

	// FOR EACH COMBO, set the friction coefficient
	surfaceTirePairs->setTypePairFriction(DrivableSurfaceTypes::WAXED_FLOOR, TireTypes::NORMAL, 1.0f); // friction coefficient = 1.0f


	// init the global variable to reflect our work
	gFrictionPairs = surfaceTirePairs;
}



/////////////////////////////////////////////////////////////////////////////
// COLLISION FILTERING STUFF...


// the basic usage of the filter shader, and it will ensure that SampleSubmarine::onContact() is called for all interesting pairs.
// Callback for every collision pair??? (i guess it applies to each shape?)
// will be called for all pairs of shapes that come near each other -- more precisely: for all pairs of shapes whose axis aligned bounding boxes in world space are found to intersect for the first time. All behavior beyond that is determined by what SampleSubmarineFilterShader() returns.
PxFilterFlags CustomFilterShader
(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);

	// if these 2 shapes are not flagged as a symmetric collision pair...
	if ((0 == (filterData0.word0 & filterData1.word1)) || (0 == (filterData1.word0 & filterData0.word1)))
		return PxFilterFlag::eSUPPRESS; // ignore collision

	// get here if these 2 shapes are a collision pair...

	// if either (or both) shape(s) is a trigger...
	// NOTE: since PhysX 3.5, Trigger-Trigger collisions are deprecated (can still set pairflags here, but the onTrigger callback will be blocked)
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1)) {
		pairFlags = PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST | PxPairFlag::eDETECT_DISCRETE_CONTACT;
	}
	else { // if both shapes are solid...
		pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST | PxPairFlag::eNOTIFY_CONTACT_POINTS | PxPairFlag::eDETECT_DISCRETE_CONTACT;
	}
	return PxFilterFlag::eDEFAULT;

}



/*
CALLBACK INFO
- callback are called from fetchresults() rather than on the simulation thread

- OnTrigger
- OnContact

- NOTE: for these 2 events to be received, must set a flag in the filter shader callback for ALL PAIRS of interacting objects for which events are required

- use PxActor.GetType and then be able to downcast to subclass Static or DYnamic

*/


// UNUSED EVENTS...
void CustomSimulationEventCallback::onAdvance(const physx::PxRigidBody *const *bodyBuffer, const physx::PxTransform *poseBuffer, const physx::PxU32 count) {}
void CustomSimulationEventCallback::onConstraintBreak(physx::PxConstraintInfo *constraints, physx::PxU32 count) {}
void CustomSimulationEventCallback::onSleep(physx::PxActor **actors, physx::PxU32 count) {}
void CustomSimulationEventCallback::onWake(physx::PxActor **actors, physx::PxU32 count) {}



// calls appropriate BahviourScript::onCollisionEnter(), BehaviourScript::onCollisionStay(), BehaviourScript::onCollisionExit()
void CustomSimulationEventCallback::onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs) {
	
	for (PxU32 i = 0; i < nbPairs; i++) { // foreach pair of SOLID-SOLID shapes...
		
		// IGNORE THIS PAIR WHEN...

		// 1. if either/both shapes have been removed from their respective actors or from the scene...
		if (pairs[i].flags & (PxContactPairFlag::eREMOVED_SHAPE_0 | PxContactPairFlag::eREMOVED_SHAPE_1)) continue;

		PxShape *shape0 = pairs[i].shapes[0];
		PxShape *shape1 = pairs[i].shapes[1];

		Entity *entity0 = static_cast<Entity*>(shape0->getActor()->userData);
		Entity *entity1 = static_cast<Entity*>(shape1->getActor()->userData);

		// 2. if either/both entities have been flagged to be destroyed...
		if (entity0->getDestroyFlag() || entity1->getDestroyFlag()) continue;

		// get contact info...
		const PxU32 BUFFER_SIZE = 64; // at most get 64 contacts
		PxContactPairPoint contacts[BUFFER_SIZE];
		PxU32 nbContacts = pairs[i].extractContacts(contacts, BUFFER_SIZE);

		// NOW CALLBACK PROPER BEHAVIOURSCRIPT METHOD FOR BOTH ENTITIES...

		if (pairs[i].flags & PxContactPairFlag::eACTOR_PAIR_HAS_FIRST_TOUCH) {
			// 1. check if entity0 has a BehaviourScript (will be assigned as a childScript)
			std::shared_ptr<Component> entity0Comp = entity0->getComponent(ComponentTypes::BEHAVIOUR_SCRIPT);
			if (entity0Comp != nullptr) {
				std::shared_ptr<BehaviourScript> entity0Script = std::static_pointer_cast<BehaviourScript>(entity0Comp);
				// 2. if so call onCollisionEnter(PxShape* localShape = shape0, PxShape* otherShape = shape1, collision stuff)
				//entity0Script->onCollisionEnter(shape0, shape1, entity1, contacts, nbContacts);
				ContactCollision collision(ContactCollision::ContactCollisionTypes::ENTER, entity0Script, shape0, shape1, entity1, contacts, nbContacts);
				gContactCollisions.push_back(collision);
			}

			// 3. check if entity1 has a BehaviourScript (will be assigned as a childScript)
			std::shared_ptr<Component> entity1Comp = entity1->getComponent(ComponentTypes::BEHAVIOUR_SCRIPT);
			if (entity1Comp != nullptr) {
				std::shared_ptr<BehaviourScript> entity1Script = std::static_pointer_cast<BehaviourScript>(entity1Comp);
				// 4. if so call onCollisionEnter(PxShape* localShape = shape1, PxShape* otherShape = shape0, collision stuff to figure out)
				//entity1Script->onCollisionEnter(shape1, shape0, entity0, contacts, nbContacts);
				ContactCollision collision(ContactCollision::ContactCollisionTypes::ENTER, entity1Script, shape1, shape0, entity0, contacts, nbContacts);
				gContactCollisions.push_back(collision);
			}
		}
		else if (pairs[i].flags & PxContactPairFlag::eACTOR_PAIR_LOST_TOUCH) {
			// 1. check if entity0 has a BehaviourScript (will be assigned as a childScript)
			std::shared_ptr<Component> entity0Comp = entity0->getComponent(ComponentTypes::BEHAVIOUR_SCRIPT);
			if (entity0Comp != nullptr) {
				std::shared_ptr<BehaviourScript> entity0Script = std::static_pointer_cast<BehaviourScript>(entity0Comp);
				// 2. if so call onCollisionExit(PxShape* localShape = shape0, PxShape* otherShape = shape1, collision stuff to figure out)
				//entity0Script->onCollisionExit(shape0, shape1, entity1, contacts, nbContacts);
				ContactCollision collision(ContactCollision::ContactCollisionTypes::EXIT, entity0Script, shape0, shape1, entity1, contacts, nbContacts);
				gContactCollisions.push_back(collision);
			}

			// 3. check if entity1 has a BehaviourScript (will be assigned as a childScript)
			std::shared_ptr<Component> entity1Comp = entity1->getComponent(ComponentTypes::BEHAVIOUR_SCRIPT);
			if (entity1Comp != nullptr) {
				std::shared_ptr<BehaviourScript> entity1Script = std::static_pointer_cast<BehaviourScript>(entity1Comp);
				// 4. if so call onCollisionExit(PxShape* localShape = shape1, PxShape* otherShape = shape0, collision stuff to figure out)
				//entity1Script->onCollisionExit(shape1, shape0, entity0, contacts, nbContacts);
				ContactCollision collision(ContactCollision::ContactCollisionTypes::EXIT, entity1Script, shape1, shape0, entity0, contacts, nbContacts);
				gContactCollisions.push_back(collision);
			}
		}
	}
}






// I think this gets called ONCE for all pairs that a collision was reported for during this frame...
//https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/RigidBodyCollision.html
//The code above iterates through all pairs of overlapping shapes that involve a trigger shape.If it is found that the treasure has been touched by the submarine then the flag gTreasureFound is set true.

// I think this will get called ONCE per pair of shapes
// SINCE PHYSX 3.5, THIS WILL ON GET CALLED FOR SOLID-TRIGGER PAIRS!!!
// if NOTIFY_TOUCH_FOUND and NOTIFY_TOUCH_LOST are both defined, then onTrigger will get called ONCE for BOTH (so I'm gonna add in a test here to determine if it was ENTER or EXIT and then call proper one)
// ****NOTE: FROM DOCS: Trigger shapes will no longer send notification events for interactions with other trigger shapes. For PhysX 3.4 there is the option to re-enable the reports by raising PxSceneFlag::eDEPRECATED_TRIGGER_TRIGGER_REPORTS. In PhysX 3.5 there will be no support for these reports any longer. See the 3.4 migration guide for more information.



// calls appropriate BehaviourScript::onTriggerEnter(), BehaviourScript::onTriggerExit()
// passes in useful collision data
// NOTE: semantically, the other shape CAUSED the trigger event
void CustomSimulationEventCallback::onTrigger(PxTriggerPair *pairs, PxU32 count) {

	for (PxU32 i = 0; i < count; i++) { // foreach pair of SOLID-TRIGGER shapes...

		// IGNORE THIS PAIR WHEN...

		// 1. if either/both shapes have been removed from their respective actors or from the scene... 
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER)) continue;

		Entity *triggerEntity = static_cast<Entity*>(pairs[i].triggerActor->userData);
		Entity *otherEntity = static_cast<Entity*>(pairs[i].otherActor->userData);

		// 2. if either/both entities have been flagged to be destroyed...
		if (triggerEntity->getDestroyFlag() || otherEntity->getDestroyFlag()) continue;

		// NOW CALLBACK PROPER BEHAVIOURSCRIPT METHOD FOR THE TRIGGER ENTITY...

		if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_FOUND) {
			// 1. check if triggerEntity has a BehaviourScript (will be assigned as a childScript)
			std::shared_ptr<Component> triggerComp = triggerEntity->getComponent(ComponentTypes::BEHAVIOUR_SCRIPT);
			if (triggerComp != nullptr) {
				std::shared_ptr<BehaviourScript> triggerScript = std::static_pointer_cast<BehaviourScript>(triggerComp);
				// 2. if so call onTriggerEnter(PxShape* localShape = triggerShape, PxShape* otherShape, Entity *otherEntity)
				//triggerScript->onTriggerEnter(pairs[i].triggerShape, pairs[i].otherShape, otherEntity);
				TriggerCollision collision(TriggerCollision::TriggerCollisionTypes::ENTER, triggerScript, pairs[i].triggerShape, pairs[i].otherShape, otherEntity);
				gTriggerCollisions.push_back(collision);
			}
		}
		else if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_LOST) {
			// 1. check if triggerEntity has a BehaviourScript (will be assigned as a childScript)
			std::shared_ptr<Component> triggerComp = triggerEntity->getComponent(ComponentTypes::BEHAVIOUR_SCRIPT);
			if (triggerComp != nullptr) {
				std::shared_ptr<BehaviourScript> triggerScript = std::static_pointer_cast<BehaviourScript>(triggerComp);
				// 2. if so call onTriggerExit(PxShape* localShape = triggerShape, PxShape* otherShape, Entity *otherEntity)
				//triggerScript->onTriggerExit(pairs[i].triggerShape, pairs[i].otherShape, otherEntity);
				TriggerCollision collision(TriggerCollision::TriggerCollisionTypes::EXIT, triggerScript, pairs[i].triggerShape, pairs[i].otherShape, otherEntity);
				gTriggerCollisions.push_back(collision);
			}	
		}
		else {
			std::cout << "ERROR: PhysicsManager.cpp | onTrigger() callback has invalid enum" << std::endl;
			return;
		}
	}
}



/////////////////////////////////////////////////////////////////////////////
// PHYSICSMANAGER STUFF...


PhysicsManager::PhysicsManager(Broker *broker)
	: _broker(broker)
{

}

PhysicsManager::~PhysicsManager() {

}


void PhysicsManager::init() {
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	if (!gFoundation) {
		std::cout << "PxCreateFoundation failed!" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	#ifdef PVD_ENABLED
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);
	#endif // PVD_ENABLED

	PxTolerancesScale simScale;
	simScale.length = 1.0f; // 1 meter by default
	simScale.speed = 98.1f; // 98.1 m/s by default (speed reached after falling for 1 second under 98.1m/s^2 gravity)

	bool recordMemoryAllocations = true;
	
	#ifdef PVD_ENABLED
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, simScale, recordMemoryAllocations, gPvd); // WITH PVD
	#else
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, simScale, recordMemoryAllocations); // W/O PVD
	#endif // PVD_ENABLED

	if (!gPhysics) {
		std::cout << "PxCreatePhysics failed!" << std::endl;
		exit(EXIT_FAILURE);
	}

	PxCookingParams params(simScale);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);
	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, params);
	if (!gCooking) {
		std::cout << "PxCreateCooking failed!" << std::endl;
		exit(EXIT_FAILURE);
	}

	// SETUP VEHICLE SDK
	PxInitVehicleSDK(*gPhysics);
	PxVehicleSetBasisVectors(PxVec3(0.0f, 1.0f, 0.0f), PxVec3(0.0f, 0.0f, 1.0f)); // up = y, forward = z
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

}



void PhysicsManager::loadScene1(int numPlayers) {
	// init vehicle stuff, create entities / add their actors into PxScene, position them at starting transforms, etc.....
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -98.1f, 0.0f);

	PxU32 numWorkers = 1; // 1 thread off the main thread (~~~~~~~~MAYBE CHANGE TO 0, to run on main thread?)
	gDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = CustomFilterShader; // TODO: change this later to use a finished CustomFilterShader
	sceneDesc.simulationEventCallback = &gSimEventCallback;

	PxScene *physxScene = gPhysics->createScene(sceneDesc);

	#ifdef PVD_ENABLED
	PxPvdSceneClient *pvdClient = physxScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	#endif // PVD_ENABLED

	//Create the batched scene queries for the suspension raycasts.
	PxU32 maxNumVehicles = 6; // 6 players
	PxU32 maxNumWheelsPerVehicle = 4;
	gVehicleSceneQueryData = VehicleSceneQueryData::allocate(maxNumVehicles, maxNumWheelsPerVehicle, 1, maxNumVehicles, WheelSceneQueryPreFilterBlocking, NULL, gAllocator); // setup for 1 batch (batch ID=0 ???)
	gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, physxScene);

	//Create the friction table for each combination of tire and surface type.
	initFrictionPairs();


	// ENTITY INIT...

	_activeScene = std::make_shared<GameScene>(physxScene);


	// GROUND:
	std::shared_ptr<Ground> ground = std::dynamic_pointer_cast<Ground>(instantiateEntity(EntityTypes::GROUND, PxTransform(0.0f, 0.0f, 0.0f, PxQuat(PxIdentity)), "ground"));

	// ROOF:
	std::shared_ptr<Roof> roof = std::dynamic_pointer_cast<Roof>(instantiateEntity(EntityTypes::ROOF, PxTransform(0.0f, 0.0f, 0.0f, PxQuat(PxIdentity)), "roof"));

	// BLUE WALL BOTTOM:
	std::shared_ptr<Obstacle1> blueWallBot = std::dynamic_pointer_cast<Obstacle1>(instantiateEntity(EntityTypes::OBSTACLE1, PxTransform(18.0f, 0.0f, -146.0f, PxQuat(PxIdentity)), "blueWallBot"));

	// BLUE WALL MIDDLE:
	std::shared_ptr<Obstacle2> blueWallMid = std::dynamic_pointer_cast<Obstacle2>(instantiateEntity(EntityTypes::OBSTACLE2, PxTransform(-101.0f, 0.0f, -168.0f, PxQuat(PxIdentity)), "blueWallMid"));

	// BLUE WALL TOP:
	std::shared_ptr<Obstacle3> blueWallTop = std::dynamic_pointer_cast<Obstacle3>(instantiateEntity(EntityTypes::OBSTACLE3, PxTransform(-141.0f, 0.0f, -61.0f, PxQuat(PxIdentity)), "blueWallTop"));

	// GREEN WALL BOTTOMS:
	std::shared_ptr<Obstacle4> greenWallBot1 = std::dynamic_pointer_cast<Obstacle4>(instantiateEntity(EntityTypes::OBSTACLE4, PxTransform(-105.0f, 0.0f, 63.0f, PxQuat(PxIdentity)), "greenWallBot1"));
	std::shared_ptr<Obstacle4> greenWallBot2 = std::dynamic_pointer_cast<Obstacle4>(instantiateEntity(EntityTypes::OBSTACLE4, PxTransform(4.0f, 0.0f, 125.0f, PxQuat(PxIdentity)), "greenWallBot2"));

	// GREEN WALL TOP:
	std::shared_ptr<Obstacle5> greenWallTop = std::dynamic_pointer_cast<Obstacle5>(instantiateEntity(EntityTypes::OBSTACLE5, PxTransform(-89.0f, 0.0f, 161.0f, PxQuat(PxIdentity)), "greenWallTop"));

	// RED WALL BOTTOM:
	std::shared_ptr<Obstacle6> redWallBot = std::dynamic_pointer_cast<Obstacle6>(instantiateEntity(EntityTypes::OBSTACLE6, PxTransform(159.0f, 0.0f, 75.0f, PxQuat(PxIdentity)), "redWallBot"));

	// RED WALL TOP:
	std::shared_ptr<Obstacle7> redWallTop = std::dynamic_pointer_cast<Obstacle7>(instantiateEntity(EntityTypes::OBSTACLE7, PxTransform(160.0f, 0.0f, -72.0f, PxQuat(PxIdentity)), "redWallTop"));


	// only cases:
	// numPlayers = 1 -> 1,-1,-2,-3,-4,-5
	// numPlayers = 2 -> 1, 2, -1, -2, -3, -4
	// numPlayers = 3 -> 1, 2, 3, -1, -2, -3
	// numPlayers = 4 -> 1, 2, 3, 4, -1, -2

	// NOTE: I'm specifying starting angle in range [-pi, pi]
	std::vector<PxTransform> vehicleSpawnTransforms;
	vehicleSpawnTransforms.push_back(PxTransform(-237.0f, 5.0f, -20.0f, PxQuat(1.5708f, PxVec3(0.0f, 1.0f, 0.0f))));
	vehicleSpawnTransforms.push_back(PxTransform(-237.0f, 5.0f, 20.0f, PxQuat(1.5708f, PxVec3(0.0f, 1.0f, 0.0f))));
	vehicleSpawnTransforms.push_back(PxTransform(101.18f, 5.0f, 215.25f, PxQuat(-2.6180f, PxVec3(0.0f, 1.0f, 0.0f))));
	vehicleSpawnTransforms.push_back(PxTransform(135.82f, 5.0f, 195.25f, PxQuat(-2.6180f, PxVec3(0.0f, 1.0f, 0.0f))));
	vehicleSpawnTransforms.push_back(PxTransform(135.82f, 5.0f, -195.25f, PxQuat(-0.5236f, PxVec3(0.0f, 1.0f, 0.0f))));
	vehicleSpawnTransforms.push_back(PxTransform(101.18f, 5.0f, -215.25f, PxQuat(-0.5236f, PxVec3(0.0f, 1.0f, 0.0f))));

	std::random_shuffle(vehicleSpawnTransforms.begin(), vehicleSpawnTransforms.end());


	// VEHICLE 0:
	std::shared_ptr<ShoppingCartPlayer> vehicle0 = std::dynamic_pointer_cast<ShoppingCartPlayer>(instantiateEntity(EntityTypes::SHOPPING_CART_PLAYER, vehicleSpawnTransforms.at(0), "vehicle0"));
	std::shared_ptr<PlayerScript> vehicle0Script = std::static_pointer_cast<PlayerScript>(vehicle0->getComponent(ComponentTypes::PLAYER_SCRIPT));
	vehicle0Script->_playerType = PlayerScript::PlayerTypes::HUMAN;
	vehicle0Script->_inputID = 1;

	// VEHICLE 1:
	std::shared_ptr<ShoppingCartPlayer> vehicle1 = std::dynamic_pointer_cast<ShoppingCartPlayer>(instantiateEntity(EntityTypes::SHOPPING_CART_PLAYER, vehicleSpawnTransforms.at(1), "vehicle1"));
	std::shared_ptr<PlayerScript> vehicle1Script = std::static_pointer_cast<PlayerScript>(vehicle1->getComponent(ComponentTypes::PLAYER_SCRIPT));
	vehicle1Script->_playerType = PlayerScript::PlayerTypes::BOT;
	vehicle1Script->_inputID = numPlayers > 1 ? 2 : -1;

	// VEHICLE 2:
	std::shared_ptr<ShoppingCartPlayer> vehicle2 = std::dynamic_pointer_cast<ShoppingCartPlayer>(instantiateEntity(EntityTypes::SHOPPING_CART_PLAYER, vehicleSpawnTransforms.at(2), "vehicle2"));
	std::shared_ptr<PlayerScript> vehicle2Script = std::static_pointer_cast<PlayerScript>(vehicle2->getComponent(ComponentTypes::PLAYER_SCRIPT));
	vehicle2Script->_playerType = PlayerScript::PlayerTypes::BOT;
	vehicle2Script->_inputID = numPlayers > 2 ? 3 : (numPlayers - 3);

	// VEHICLE 3:
	std::shared_ptr<ShoppingCartPlayer> vehicle3 = std::dynamic_pointer_cast<ShoppingCartPlayer>(instantiateEntity(EntityTypes::SHOPPING_CART_PLAYER, vehicleSpawnTransforms.at(3), "vehicle3"));
	std::shared_ptr<PlayerScript> vehicle3Script = std::static_pointer_cast<PlayerScript>(vehicle3->getComponent(ComponentTypes::PLAYER_SCRIPT));
	vehicle3Script->_playerType = PlayerScript::PlayerTypes::BOT;
	vehicle3Script->_inputID = numPlayers > 3 ? 4 : (numPlayers - 4);

	// VEHICLE 4:
	std::shared_ptr<ShoppingCartPlayer> vehicle4 = std::dynamic_pointer_cast<ShoppingCartPlayer>(instantiateEntity(EntityTypes::SHOPPING_CART_PLAYER, vehicleSpawnTransforms.at(4), "vehicle4"));
	std::shared_ptr<PlayerScript> vehicle4Script = std::static_pointer_cast<PlayerScript>(vehicle4->getComponent(ComponentTypes::PLAYER_SCRIPT));
	vehicle4Script->_playerType = PlayerScript::PlayerTypes::BOT;
	vehicle4Script->_inputID = numPlayers - 5;
	
	// VEHICLE 5:
	std::shared_ptr<ShoppingCartPlayer> vehicle5 = std::dynamic_pointer_cast<ShoppingCartPlayer>(instantiateEntity(EntityTypes::SHOPPING_CART_PLAYER, vehicleSpawnTransforms.at(5), "vehicle5"));
	std::shared_ptr<PlayerScript> vehicle5Script = std::static_pointer_cast<PlayerScript>(vehicle5->getComponent(ComponentTypes::PLAYER_SCRIPT));
	vehicle5Script->_playerType = PlayerScript::PlayerTypes::BOT;
	vehicle5Script->_inputID = numPlayers - 6;
}


void PhysicsManager::cleanupScene1() {

	std::vector<std::shared_ptr<Entity>> entitiesCopy = _activeScene->_entities;
	for (std::shared_ptr<Entity> &entity : entitiesCopy) {
		_activeScene->removeEntity(entity);
	}

	//std::cout << _activeScene->_entities.size() << std::endl;

	gBatchQuery->release();
	gBatchQuery = NULL;
	gVehicleSceneQueryData->free(gAllocator);
	gVehicleSceneQueryData = NULL;
	gFrictionPairs->release();
	gFrictionPairs = NULL;

	_activeScene->_physxScene->release();
	_activeScene->_physxScene = nullptr;
	_activeScene = nullptr;

	gDispatcher->release();
	gDispatcher = NULL;
}


/*
void PhysicsManager::switchToScene1() {
	// TODO: cleanup scene we just transitioned from...

	// init vehicle stuff, create entities / add their actors into PxScene, position them at starting transforms, etc.....
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -98.1f, 0.0f);

	PxU32 numWorkers = 1; // 1 thread off the main thread (~~~~~~~~MAYBE CHANGE TO 0, to run on main thread?)
	gDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = CustomFilterShader; // TODO: change this later to use a finished CustomFilterShader
	sceneDesc.simulationEventCallback = &gSimEventCallback;

	PxScene *physxScene = gPhysics->createScene(sceneDesc);

	#ifdef PVD_ENABLED
	PxPvdSceneClient *pvdClient = physxScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	#endif // PVD_ENABLED

	//Create the batched scene queries for the suspension raycasts.
	PxU32 maxNumVehicles = 12; // 6 players + 6 dumb shopping cart obstacles???
	PxU32 maxNumWheelsPerVehicle = 4;
	gVehicleSceneQueryData = VehicleSceneQueryData::allocate(maxNumVehicles, maxNumWheelsPerVehicle, 1, maxNumVehicles, WheelSceneQueryPreFilterBlocking, NULL, gAllocator); // setup for 1 batch (batch ID=0 ???)
	gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, physxScene);

	//Create the friction table for each combination of tire and surface type.
	initFrictionPairs();


	// ENTITY INIT...

	_activeScene = std::make_shared<GameScene>(physxScene);

	// GROUND:
	std::shared_ptr<Ground> ground = std::dynamic_pointer_cast<Ground>(instantiateEntity(EntityTypes::GROUND, PxTransform(0.0f, 0.0f, 0.0f, PxQuat(PxIdentity)), "ground"));

	// ROOF:
	std::shared_ptr<Roof> roof = std::dynamic_pointer_cast<Roof>(instantiateEntity(EntityTypes::ROOF, PxTransform(0.0f, 0.0f, 0.0f, PxQuat(PxIdentity)), "roof"));

	// BLUE WALL BOTTOM:
	std::shared_ptr<Obstacle1> blueWallBot = std::dynamic_pointer_cast<Obstacle1>(instantiateEntity(EntityTypes::OBSTACLE1, PxTransform(18.0f, 0.0f, -146.0f, PxQuat(PxIdentity)), "blueWallBot"));

	// BLUE WALL MIDDLE:
	std::shared_ptr<Obstacle2> blueWallMid = std::dynamic_pointer_cast<Obstacle2>(instantiateEntity(EntityTypes::OBSTACLE2, PxTransform(-101.0f, 0.0f, -168.0f, PxQuat(PxIdentity)), "blueWallMid"));

	// BLUE WALL TOP:
	std::shared_ptr<Obstacle3> blueWallTop = std::dynamic_pointer_cast<Obstacle3>(instantiateEntity(EntityTypes::OBSTACLE3, PxTransform(-141.0f, 0.0f, -61.0f, PxQuat(PxIdentity)), "blueWallTop"));

	// GREEN WALL BOTTOMS:
	std::shared_ptr<Obstacle4> greenWallBot1 = std::dynamic_pointer_cast<Obstacle4>(instantiateEntity(EntityTypes::OBSTACLE4, PxTransform(-105.0f, 0.0f, 63.0f, PxQuat(PxIdentity)), "greenWallBot1"));
	std::shared_ptr<Obstacle4> greenWallBot2 = std::dynamic_pointer_cast<Obstacle4>(instantiateEntity(EntityTypes::OBSTACLE4, PxTransform(4.0f, 0.0f, 125.0f, PxQuat(PxIdentity)), "greenWallBot2"));

	// GREEN WALL TOP:
	std::shared_ptr<Obstacle5> greenWallTop = std::dynamic_pointer_cast<Obstacle5>(instantiateEntity(EntityTypes::OBSTACLE5, PxTransform(-89.0f, 0.0f, 161.0f, PxQuat(PxIdentity)), "greenWallTop"));

	// RED WALL BOTTOM:
	std::shared_ptr<Obstacle6> redWallBot = std::dynamic_pointer_cast<Obstacle6>(instantiateEntity(EntityTypes::OBSTACLE6, PxTransform(159.0f, 0.0f, 75.0f, PxQuat(PxIdentity)), "redWallBot"));

	// RED WALL TOP:
	std::shared_ptr<Obstacle7> redWallTop = std::dynamic_pointer_cast<Obstacle7>(instantiateEntity(EntityTypes::OBSTACLE7, PxTransform(160.0f, 0.0f, -72.0f, PxQuat(PxIdentity)), "redWallTop"));



	// VEHICLE 1:
	std::shared_ptr<ShoppingCartPlayer> vehicle1 = std::dynamic_pointer_cast<ShoppingCartPlayer>(instantiateEntity(EntityTypes::SHOPPING_CART_PLAYER, PxTransform(-200.0f, 5.0f, 0.0f, PxQuat(PxIdentity)), "vehicle1"));
	std::shared_ptr<PlayerScript> player1Script = std::static_pointer_cast<PlayerScript>(vehicle1->getComponent(ComponentTypes::PLAYER_SCRIPT));
	player1Script->_playerType = PlayerScript::PlayerTypes::HUMAN;
	player1Script->_inputID = 1;

	// VEHICLE 2: (COMMENT OUT WHEN 2 CONTROLLERS ARE NOT PLUGGED IN - NVM this doesnt seem to matter)
	// FOR TEST PURPOSES - BASHING
	//std::shared_ptr<ShoppingCartPlayer> vehicle2 = std::dynamic_pointer_cast<ShoppingCartPlayer>(instantiateEntity(EntityTypes::SHOPPING_CART_PLAYER, PxTransform(20.0f, 5.0f, 70.0f, PxQuat(PxIdentity)), "vehicle2"));
	//std::shared_ptr<PlayerScript> player2Script = std::static_pointer_cast<PlayerScript>(vehicle2->getComponent(ComponentTypes::PLAYER_SCRIPT));
	//player2Script->_inputID = 2;

	// BOT 1:
	std::shared_ptr<ShoppingCartPlayer> bot1 = std::dynamic_pointer_cast<ShoppingCartPlayer>(instantiateEntity(EntityTypes::SHOPPING_CART_PLAYER, PxTransform(-190.0f, 5.0f, 0.0f, PxQuat(PxIdentity)), "bot1"));
	std::shared_ptr<PlayerScript> bot1Script = std::static_pointer_cast<PlayerScript>(bot1->getComponent(ComponentTypes::PLAYER_SCRIPT));
	bot1Script->_playerType = PlayerScript::PlayerTypes::BOT;
	bot1Script->_inputID = -1;

	// BOT 2:
	std::shared_ptr<ShoppingCartPlayer> bot2 = std::dynamic_pointer_cast<ShoppingCartPlayer>(instantiateEntity(EntityTypes::SHOPPING_CART_PLAYER, PxTransform(-180.0f, 5.0f, 0.0f, PxQuat(PxIdentity)), "bot2"));
	std::shared_ptr<PlayerScript> bot2Script = std::static_pointer_cast<PlayerScript>(bot2->getComponent(ComponentTypes::PLAYER_SCRIPT));
	bot2Script->_playerType = PlayerScript::PlayerTypes::BOT;
	bot2Script->_inputID = -2;

	// BOT 3:

	//std::shared_ptr<ShoppingCartPlayer> bot3 = std::dynamic_pointer_cast<ShoppingCartPlayer>(instantiateEntity(EntityTypes::SHOPPING_CART_PLAYER, PxTransform(-80.0f, 5.0f, -80.0f, PxQuat(PxIdentity)), "bot3"));
	//std::shared_ptr<PlayerScript> bot3Script = std::static_pointer_cast<PlayerScript>(bot3->getComponent(ComponentTypes::PLAYER_SCRIPT));
	//bot3Script->_playerType = PlayerScript::PlayerTypes::BOT;
	//bot3Script->_inputID = -3;

}
*/



void PhysicsManager::updateSeconds(double fixedDeltaTime) {
	// call FIXEDUPDATE() for all behaviour scripts...
	std::vector<std::shared_ptr<Entity>> entitiesCopy = _activeScene->_entities;
	for (std::shared_ptr<Entity> &entity : entitiesCopy) {
		std::shared_ptr<Component> comp = entity->getComponent(ComponentTypes::BEHAVIOUR_SCRIPT);
		if (comp != nullptr) {
			std::shared_ptr<BehaviourScript> script = std::static_pointer_cast<BehaviourScript>(comp);
			script->fixedUpdate(fixedDeltaTime);
		}
	}

	// FURTHER VEHICLE UPDATES...

	// TODO: account for bots/obstacle carts in future...
	
	std::vector<std::shared_ptr<ShoppingCartPlayer>> shoppingCartPlayers = _activeScene->getAllShoppingCartPlayers();

	//Raycasts...
	std::vector<PxVehicleWheels*> vehiclesVector;
	for (std::shared_ptr<ShoppingCartPlayer> &shoppingCartPlayer : shoppingCartPlayers) {
		vehiclesVector.push_back(shoppingCartPlayer->_shoppingCartBase->_vehicle4W);
	}

	PxRaycastQueryResult *raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0); // ONLY 1 buffer set up ID = 0
	const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(gBatchQuery, vehiclesVector.size(), vehiclesVector.data(), raycastResultsSize, raycastResults);

	//Vehicle update...
	const PxVec3 grav = _activeScene->_physxScene->getGravity();
	std::vector<PxWheelQueryResult> wheelQueryResults;
	wheelQueryResults.resize(vehiclesVector.size()*PX_MAX_NB_WHEELS); // have a slot for every possible wheel of each vehicle in order

	std::vector<PxVehicleWheelQueryResult> vehicleQueryResults;
	for (int i = 0; i < vehiclesVector.size(); i++) {
		vehicleQueryResults.push_back({ &wheelQueryResults[i*PX_MAX_NB_WHEELS], vehiclesVector[i]->mWheelsSimData.getNbWheels() });
	}

	PxVehicleUpdates(fixedDeltaTime, grav, *gFrictionPairs, vehiclesVector.size(), vehiclesVector.data(), vehicleQueryResults.data());

	for (int i = 0; i < vehiclesVector.size(); i++) {
		shoppingCartPlayers.at(i)->_shoppingCartBase->setIsAirborne(vehiclesVector.at(i)->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults.at(i)));
	}



	// clear collision vectors...
	gContactCollisions.clear();
	gTriggerCollisions.clear();

	// Scene update...
	_activeScene->_physxScene->simulate(fixedDeltaTime);
	_activeScene->_physxScene->fetchResults(true); // wait for results to come in before moving on to next system

	// RESET HIT FLAG...
	for (std::shared_ptr<ShoppingCartPlayer> &shoppingCartPlayer : shoppingCartPlayers) {
		shoppingCartPlayer->_shoppingCartBase->_wasHitFrameTimer--;
		if (shoppingCartPlayer->_shoppingCartBase->_wasHitFrameTimer < 0) shoppingCartPlayer->_shoppingCartBase->_wasHitFrameTimer = 0;
	}

	// now that fetchResults() has cached all collision events in the 2 vectors, call the proper events
	for (ContactCollision &collision : gContactCollisions) {
		if (collision._collisionType == ContactCollision::ContactCollisionTypes::ENTER) {
			collision._caller->onCollisionEnter(collision._localShape, collision._otherShape, collision._otherEntity, collision._contacts, collision._nbContacts);
		}
		else {
			collision._caller->onCollisionExit(collision._localShape, collision._otherShape, collision._otherEntity, collision._contacts, collision._nbContacts);
		}
	}

	for (TriggerCollision &collision : gTriggerCollisions) {
		if (collision._collisionType == TriggerCollision::TriggerCollisionTypes::ENTER) {
			collision._caller->onTriggerEnter(collision._localShape, collision._otherShape, collision._otherEntity);
		}
		else {
			collision._caller->onTriggerExit(collision._localShape, collision._otherShape, collision._otherEntity);
		}
	}

	// ANTI-FLIP OVER...
	// ~~~~~~NOTE: should this be moved to before simulate() ???????
	for (std::shared_ptr<ShoppingCartPlayer> &cart : shoppingCartPlayers) {
		PxVec3 cartPos = cart->_actor->is<PxRigidDynamic>()->getGlobalPose().p;
		PxQuat cartRot = cart->_actor->is<PxRigidDynamic>()->getGlobalPose().q;

		PxQuat clampedRot(glm::clamp(cartRot.x, -0.3f, 0.3f), cartRot.y, glm::clamp(cartRot.z, -0.3f, 0.3f), cartRot.w);
		clampedRot = clampedRot.getNormalized();

		cart->_actor->is<PxRigidDynamic>()->setGlobalPose(PxTransform(cartPos, clampedRot));
	}
}



void PhysicsManager::cleanup() {
	/*
	gVehicle4W->getRigidDynamicActor()->release();
	gVehicle4W->free();
	gGroundPlane->release();
	gBatchQuery->release();
	gVehicleSceneQueryData->free(gAllocator);
	gFrictionPairs->release();
	PxCloseVehicleSDK();

	gMaterial->release();
	gCooking->release();
	gScene->release();
	gDispatcher->release();
	gPhysics->release();



	#ifdef PVD_ENABLED
	PxPvdTransport* transport = gPvd->getTransport();
	gPvd->release();
	transport->release();
	#endif // PVD_ENABLED


	
	gFoundation->release();
	*/
}


// ~~~~~~~~~~~~TODO: delete all local pointers


std::shared_ptr<Entity> PhysicsManager::instantiateEntity(EntityTypes type, physx::PxTransform transform, const char *name) {

	std::shared_ptr<Entity> entity;

	switch (type) {
	case EntityTypes::SHOPPING_CART_PLAYER:
	{
		// SHAPES / ACTOR...
		VehicleShoppingCart *shoppingCartBase = new VehicleShoppingCart(gPhysics, gCooking);
		shoppingCartBase->_vehicle4W->getRigidDynamicActor()->setGlobalPose(transform);
		shoppingCartBase->_vehicle4W->getRigidDynamicActor()->setName(name);

		// DEFAULT: NON-KINEMATIC DYNAMIC (GRAVITY ENABLED)

/*
// NOTE: I believe its the LOOKAT shape that breaks the AI since the raycasts hit it!
		PxFilterData simDataLA(CollisionFlags::COLLISION_FLAG_LOOKAT_SHAPE, CollisionFlags::COLLISION_FLAG_LOOKAT_SHAPE_AGAINST, 0, 0);
		PxFilterData qryDataLA;
		setupNonDrivableSurface(qryDataLA);
		bool isExclusiveLA = true;
		PxShapeFlags shapeFlagsLA = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eVISUALIZATION;

		PxShape *lookAtShape = createSphereCollider(0.1f, gPhysics->createMaterial(0.0f, 0.0f, 0.0f), simDataLA, qryDataLA, isExclusiveLA, shapeFlagsLA);
		lookAtShape->setLocalPose(PxTransform(0.0f, 0.0f, 3.0f));

		shoppingCartBase->_vehicle4W->getRigidDynamicActor()->attachShape(*lookAtShape);


		PxFilterData simDataCam(CollisionFlags::COLLISION_FLAG_CAMERA_SHAPE, CollisionFlags::COLLISION_FLAG_CAMERA_SHAPE_AGAINST, 0, 0);
		PxFilterData qryDataCam;
		setupNonDrivableSurface(qryDataCam);
		bool isExclusiveCam = true;
		PxShapeFlags shapeFlagsCam = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eVISUALIZATION;

		PxShape *cameraShape = createSphereCollider(0.1f, gPhysics->createMaterial(0.0f, 0.0f, 0.0f), simDataCam, qryDataCam, isExclusiveCam, shapeFlagsCam);
		cameraShape->setLocalPose(PxTransform(0.0f, 15.0f, -25.0f));

		shoppingCartBase->_vehicle4W->getRigidDynamicActor()->attachShape(*cameraShape);

		shoppingCartBase->_lookAtShape = lookAtShape;
		shoppingCartBase->_cameraShape = cameraShape;
*/
		// ENTITY...
		entity = std::make_shared<ShoppingCartPlayer>(shoppingCartBase);
		break;
	}
	case EntityTypes::GROUND:
	{
		std::vector<PxVec3> verts = castVectorOfGLMVec4ToVectorOfPxVec3(_broker->getLoadingManager()->getGeometry(GeometryTypes::GROUND_GEO)->verts);
		std::vector<PxU32> indices = _broker->getLoadingManager()->getGeometry(GeometryTypes::GROUND_GEO)->vIndex;
		PxMaterial *material = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_GROUND, CollisionFlags::COLLISION_FLAG_GROUND_AGAINST, 0, 0);
		PxFilterData qryData;
		setupDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createTriMeshCollider(verts, indices, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidStatic *actor = gPhysics->createRigidStatic(transform);
		actor->setName(name);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Ground>(actor);
		break;
	}
	case EntityTypes::ROOF:
	{
		std::vector<PxVec3> verts = castVectorOfGLMVec4ToVectorOfPxVec3(_broker->getLoadingManager()->getGeometry(GeometryTypes::ROOF_GEO)->verts);
		std::vector<PxU32> indices = _broker->getLoadingManager()->getGeometry(GeometryTypes::ROOF_GEO)->vIndex;
		PxMaterial *material = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_OBSTACLE, CollisionFlags::COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createTriMeshCollider(verts, indices, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidStatic *actor = gPhysics->createRigidStatic(transform);
		actor->setName(name);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Roof>(actor);
		break;
	}
	case EntityTypes::OBSTACLE1:
	{
		std::vector<PxVec3> verts = castVectorOfGLMVec4ToVectorOfPxVec3(_broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE1_GEO)->verts);
		std::vector<PxU32> indices = _broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE1_GEO)->vIndex;
		PxMaterial *material = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_OBSTACLE, CollisionFlags::COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createTriMeshCollider(verts, indices, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidStatic *actor = gPhysics->createRigidStatic(transform);
		actor->setName(name);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Obstacle1>(actor);
		break;
	}
	case EntityTypes::OBSTACLE2:
	{
		std::vector<PxVec3> verts = castVectorOfGLMVec4ToVectorOfPxVec3(_broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE2_GEO)->verts);
		std::vector<PxU32> indices = _broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE2_GEO)->vIndex;
		PxMaterial *material = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_OBSTACLE, CollisionFlags::COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createTriMeshCollider(verts, indices, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidStatic *actor = gPhysics->createRigidStatic(transform);
		actor->setName(name);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Obstacle2>(actor);
		break;
	}
	case EntityTypes::OBSTACLE3:
	{
		std::vector<PxVec3> verts = castVectorOfGLMVec4ToVectorOfPxVec3(_broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE3_GEO)->verts);
		std::vector<PxU32> indices = _broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE3_GEO)->vIndex;
		PxMaterial *material = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_OBSTACLE, CollisionFlags::COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createTriMeshCollider(verts, indices, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidStatic *actor = gPhysics->createRigidStatic(transform);
		actor->setName(name);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Obstacle3>(actor);
		break;
	}
	case EntityTypes::OBSTACLE4:
	{
		std::vector<PxVec3> verts = castVectorOfGLMVec4ToVectorOfPxVec3(_broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE4_GEO)->verts);
		std::vector<PxU32> indices = _broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE4_GEO)->vIndex;
		PxMaterial *material = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_OBSTACLE, CollisionFlags::COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createTriMeshCollider(verts, indices, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidStatic *actor = gPhysics->createRigidStatic(transform);
		actor->setName(name);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Obstacle4>(actor);
		break;
	}
	case EntityTypes::OBSTACLE5:
	{
		std::vector<PxVec3> verts = castVectorOfGLMVec4ToVectorOfPxVec3(_broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE5_GEO)->verts);
		std::vector<PxU32> indices = _broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE5_GEO)->vIndex;
		PxMaterial *material = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_OBSTACLE, CollisionFlags::COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createTriMeshCollider(verts, indices, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidStatic *actor = gPhysics->createRigidStatic(transform);
		actor->setName(name);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Obstacle5>(actor);
		break;
	}
	case EntityTypes::OBSTACLE6:
	{
		std::vector<PxVec3> verts = castVectorOfGLMVec4ToVectorOfPxVec3(_broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE6_GEO)->verts);
		std::vector<PxU32> indices = _broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE6_GEO)->vIndex;
		PxMaterial *material = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_OBSTACLE, CollisionFlags::COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createTriMeshCollider(verts, indices, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidStatic *actor = gPhysics->createRigidStatic(transform);
		actor->setName(name);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Obstacle6>(actor);
		break;
	}
	case EntityTypes::OBSTACLE7:
	{
		std::vector<PxVec3> verts = castVectorOfGLMVec4ToVectorOfPxVec3(_broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE7_GEO)->verts);
		std::vector<PxU32> indices = _broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE7_GEO)->vIndex;
		PxMaterial *material = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_OBSTACLE, CollisionFlags::COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createTriMeshCollider(verts, indices, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidStatic *actor = gPhysics->createRigidStatic(transform);
		actor->setName(name);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Obstacle7>(actor);
		break;
	}
	case EntityTypes::MILK:
	{
		PxReal radius = 2.5f;
		PxMaterial *material = gPhysics->createMaterial(1.0f, 1.0f, 1.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_PICKUP, CollisionFlags::COLLISION_FLAG_PICKUP_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createSphereCollider(radius, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidDynamic *actor = gPhysics->createRigidDynamic(transform);
		actor->setName(name);
		actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Milk>(actor);
		break;
	}
	case EntityTypes::WATER:
	{
		PxReal radius = 2.5f;
		PxMaterial *material = gPhysics->createMaterial(1.0f, 1.0f, 1.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_PICKUP, CollisionFlags::COLLISION_FLAG_PICKUP_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createSphereCollider(radius, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidDynamic *actor = gPhysics->createRigidDynamic(transform);
		actor->setName(name);
		actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Water>(actor);
		break;
	}
	case EntityTypes::COLA:
	{
		PxReal radius = 2.5f;
		PxMaterial *material = gPhysics->createMaterial(1.0f, 1.0f, 1.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_PICKUP, CollisionFlags::COLLISION_FLAG_PICKUP_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createSphereCollider(radius, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidDynamic *actor = gPhysics->createRigidDynamic(transform);
		actor->setName(name);
		actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Cola>(actor);
		break;
	}
	case EntityTypes::APPLE:
	{
		PxReal radius = 2.5f;
		PxMaterial *material = gPhysics->createMaterial(1.0f, 1.0f, 1.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_PICKUP, CollisionFlags::COLLISION_FLAG_PICKUP_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createSphereCollider(radius, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidDynamic *actor = gPhysics->createRigidDynamic(transform);
		actor->setName(name);
		actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Apple>(actor);
		break;
	}
	case EntityTypes::WATERMELON:
	{
		PxReal radius = 2.5f;
		PxMaterial *material = gPhysics->createMaterial(1.0f, 1.0f, 1.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_PICKUP, CollisionFlags::COLLISION_FLAG_PICKUP_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createSphereCollider(radius, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidDynamic *actor = gPhysics->createRigidDynamic(transform);
		actor->setName(name);
		actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Watermelon>(actor);
		break;
	}
	case EntityTypes::BANANA:
	{
		PxReal radius = 2.5f;
		PxMaterial *material = gPhysics->createMaterial(1.0f, 1.0f, 1.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_PICKUP, CollisionFlags::COLLISION_FLAG_PICKUP_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createSphereCollider(radius, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidDynamic *actor = gPhysics->createRigidDynamic(transform);
		actor->setName(name);
		actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Banana>(actor);
		break;
	}
	case EntityTypes::CARROT:
	{
		PxReal radius = 2.5f;
		PxMaterial *material = gPhysics->createMaterial(1.0f, 1.0f, 1.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_PICKUP, CollisionFlags::COLLISION_FLAG_PICKUP_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createSphereCollider(radius, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidDynamic *actor = gPhysics->createRigidDynamic(transform);
		actor->setName(name);
		actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Carrot>(actor);
		break;
	}
	case EntityTypes::EGGPLANT:
	{
		PxReal radius = 2.5f;
		PxMaterial *material = gPhysics->createMaterial(1.0f, 1.0f, 1.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_PICKUP, CollisionFlags::COLLISION_FLAG_PICKUP_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createSphereCollider(radius, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidDynamic *actor = gPhysics->createRigidDynamic(transform);
		actor->setName(name);
		actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Eggplant>(actor);
		break;
	}
	case EntityTypes::BROCCOLI:
	{
		PxReal radius = 2.5f;
		PxMaterial *material = gPhysics->createMaterial(1.0f, 1.0f, 1.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_PICKUP, CollisionFlags::COLLISION_FLAG_PICKUP_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createSphereCollider(radius, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidDynamic *actor = gPhysics->createRigidDynamic(transform);
		actor->setName(name);
		actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Broccoli>(actor);
		break;
	}
	case EntityTypes::MYSTERY_BAG:
	{
		PxReal radius = 2.5f;
		PxMaterial *material = gPhysics->createMaterial(1.0f, 1.0f, 1.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_PICKUP, CollisionFlags::COLLISION_FLAG_PICKUP_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createSphereCollider(radius, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidDynamic *actor = gPhysics->createRigidDynamic(transform);
		actor->setName(name);
		actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<MysteryBag>(actor);
		break;
	}
	case EntityTypes::COOKIE:
	{
		PxReal radius = 2.5f;
		PxMaterial *material = gPhysics->createMaterial(1.0f, 1.0f, 1.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_PICKUP, CollisionFlags::COLLISION_FLAG_PICKUP_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createSphereCollider(radius, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidDynamic *actor = gPhysics->createRigidDynamic(transform);
		actor->setName(name);
		actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<Cookie>(actor);
		break;
	}
	case EntityTypes::SPARE_CHANGE:
	{
		PxReal radius = 2.5f;
		PxMaterial *material = gPhysics->createMaterial(1.0f, 1.0f, 1.0f);
		PxFilterData simData(CollisionFlags::COLLISION_FLAG_PICKUP, CollisionFlags::COLLISION_FLAG_PICKUP_AGAINST, 0, 0);
		PxFilterData qryData;
		setupNonDrivableSurface(qryData);
		bool isExclusive = true;
		PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eVISUALIZATION;

		// SHAPE...
		PxShape *shape = createSphereCollider(radius, material, simData, qryData, isExclusive, shapeFlags);

		// ACTOR...
		PxRigidDynamic *actor = gPhysics->createRigidDynamic(transform);
		actor->setName(name);
		actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

		actor->attachShape(*shape);

		// ENTITY...
		entity = std::make_shared<SpareChange>(actor);
		break;
	}
	default:
		entity = nullptr;
		break;
	}

	if (entity != nullptr) {
		// call ONSPAWN() if entity has a behaviour script component...
		std::shared_ptr<Component> comp = entity->getComponent(ComponentTypes::BEHAVIOUR_SCRIPT);
		if (comp != nullptr) {
			std::shared_ptr<BehaviourScript> script = std::static_pointer_cast<BehaviourScript>(comp);
			script->onSpawn();
		}
		_activeScene->addEntity(entity);
	}

	return entity;
}





PxShape* PhysicsManager::createSphereCollider(PxReal radius, PxMaterial *material, const PxFilterData& simData, const PxFilterData& qryData, bool isExclusive, PxShapeFlags shapeFlags) {
	PxShape *shape = gPhysics->createShape(PxSphereGeometry(radius), *material, isExclusive, shapeFlags);

	shape->setQueryFilterData(qryData);
	shape->setSimulationFilterData(simData);

	return shape;
}

PxShape* PhysicsManager::createBoxCollider(PxReal xSize, PxReal ySize, PxReal zSize, PxMaterial *material, const PxFilterData& simData, const PxFilterData& qryData, bool isExclusive, PxShapeFlags shapeFlags) {
	PxShape *shape = gPhysics->createShape(PxBoxGeometry(xSize / 2, ySize / 2, zSize / 2), *material, isExclusive, shapeFlags);
	
	shape->setQueryFilterData(qryData);
	shape->setSimulationFilterData(simData);
	
	return shape;
}


PxShape* PhysicsManager::createTriMeshCollider(const std::vector<PxVec3>& verts, const std::vector<PxU32>& indices, PxMaterial *material, const PxFilterData& simData, const PxFilterData& qryData, bool isExclusive, PxShapeFlags shapeFlags) {
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = verts.size();
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = verts.data(); 

	meshDesc.triangles.count = indices.size() / 3;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = indices.data();

	PxTriangleMesh *triMesh = nullptr;
	PxDefaultMemoryOutputStream buf;
	if (gCooking->cookTriangleMesh(meshDesc, buf)) {
		PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
		triMesh = gPhysics->createTriangleMesh(id);
	}

	PxShape *shape = gPhysics->createShape(PxTriangleMeshGeometry(triMesh), *material, isExclusive, shapeFlags);

	shape->setQueryFilterData(qryData);
	shape->setSimulationFilterData(simData);

	return shape;
}








bool PhysicsManager::raycast(const PxVec3 &origin, const PxVec3 &unitDir, const PxReal distance, PxRaycastCallback &hitCall) {
	return _activeScene->_physxScene->raycast(origin, unitDir, distance, hitCall);
}









//////////////////////////////////////////////////////////////


physx::PxShape** PhysicsManager::getAllShapes() {
	PxU32 bufSize = gPhysics->getNbShapes();
	PxShape** buf = new PxShape*[bufSize];
	gPhysics->getShapes(buf, bufSize);
	return buf;
}



physx::PxU32 PhysicsManager::getNbShapes() {
	return gPhysics->getNbShapes();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
// MAKE KINEMATIC?
void PhysicsManager::disableShapeInContactTests(physx::PxShape *shape) {
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
}

// MAKE NON_KINEMATIC?
void PhysicsManager::enableShapeInContactTests(physx::PxShape *shape) {
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
}

// MAKE FOREIGN
void PhysicsManager::disableShapeInSceneQueryTests(physx::PxShape *shape) {
	shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
}

// MAKE LOCAL
void PhysicsManager::enableShapeInSceneQueryTests(physx::PxShape *shape) {
	shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
}


void PhysicsManager::setShapeTrigger(physx::PxShape *shape) {
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
}

void PhysicsManager::setShapeSolid(physx::PxShape *shape) {
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
}
*/





// RESEARCH NOTES BELOW ///////////////////////////////////////////////////////////////////////////

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


/*
NOTES: http://gameworksdocs.nvidia.com/PhysX/4.0/documentation/PhysXGuide/Manual/Threading.html

User changes take precedence: for example, a user change to the position of an object while the simulation is running will overwrite the position which resulted from the simulation.
The delayed application of updates does not affect scene queries, which always take into account the latest changes.

*/

/*
geometry: https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/RigidBodyCollision.html

When a geometry object is specified for a shape, the geometry object is copied into the shape. There are some restrictions on which geometries may be specified for a shape, depending on the shape flags and the type of the parent actors.

TriangleMesh, HeightField and Plane geometries are not supported for simulation shapes that are attached to dynamic actors, unless the dynamic actors are configured to be kinematic.
TriangleMesh and HeightField geometries are not supported for trigger shapes.

DEFAULTS: SOLID COLLIDER, QUERIED, DEBUG RENDERED

NOTE: SIMULATION SHAPE (SOLID) AND TRIGGER SHAPE are MUTUALLY exlusive

TriangleMesh, HeightField and Plane geometries are not supported for simulation shapes that are attached to dynamic actors, unless the dynamic actors are configured to be kinematic.
TriangleMesh and HeightField geometries are not supported for trigger shapes.

Detach the shape from the actor as follows:

myActor.detachShape(*shape);


Note If the movement of the shape's actor does not need to be controlled by the simulation at all, i.e., the shape is used for scene queries only and gets moved manually if necessary, then memory can be saved by additionally disabling simulation on the actor (see the API documentation on PxActorFlag::eDISABLE_SIMULATION).

STATIC - Transform doesnt change at all
vs.
DYNAMIC - Transform can change

KINEMATIC - A dynamic actor that can ignore the rules of physics, moves based on what we tell it

WARNING
there is a major caveat for kinematic actors.
they do not trigger callbacks.
you will not get a callback when a "box" hit a kinematic object.
you need to use rays or sweeps to get objects around your actor.


The desired broad-phase algorithm is controlled by the PxBroadPhaseType enum, within the PxSceneDesc structure.


A region of interest is a world-space AABB around a volume of space controlled by the broad-phase. Objects contained inside those regions are properly handled by the broad-phase. Objects falling outside of those regions lose all collision detection. Ideally those regions should cover the whole game space, while limiting the amount of covered empty space.
Regions can overlap, although for maximum efficiency it is recommended to minimize the amount of overlap between regions as much as possible. Note that two regions whose AABBs just touch are not considered overlapping. For example the PxBroadPhaseExt::createRegionsFromWorldBounds helper function creates a number of non-overlapping region bounds by simply subdividing a given world AABB into a regular 2D grid.

The maximum number of regions is currently limited to 256.

Broad-Phase Callback:

A callback for broad-phase-related events can be defined within the PxSceneDesc structure. This PxBroadPhaseCallback object will be called when objects are found out of the specified regions of interest, i.e. "out of bounds". The SDK disables collision detection for those objects. It is re-enabled automatically as soon as the objects re-enter a valid region.

It is up to users to decide what to do with out-of-bounds objects. Typical options are:

delete the objects
let them continue their motion without collisions until they re-enter a valid region
artificially teleport them back to a valid place


*****Collision Filtering:


SampleSubmarineFilterShader() will be called for all pairs of shapes that come near each other -- more precisely: for all pairs of shapes whose axis aligned bounding boxes in world space are found to intersect for the first time. All behavior beyond that is determined by what SampleSubmarineFilterShader() returns.



THUS, I think that PhysX inits all shapes as non-interacting, until the move and then if there AABB intersect, they then get flagged as a potential collision pair

*******Each PxShape and PxParticleBase object in PhysX has a member variable of type PxFilterData. This is 128 bits of user defined data that can be used to store application specific information related to collision filtering. This is the other variable that is passed to SampleSubmarineFilterShader() for each object.
There is also the constant block. This is a chunk of per-scene global information that the application can give to the shader to operate on. You will want to use this to encode rules about what to filter and what not.


EVENTS:
- consult PxSimulationEventCallback::onTrigger/onWake/onSleep/onContact

ONTRIGGER():
ex. at bottom of https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/RigidBodyCollision.html



*/

/*
other notes:

set the scene desc. gravity and bounceThresholeBeclotiy

release geometry after attaching to an actor

The parameter 'true' to createShape() informs the SDK that the shape will not be shared with other actors. You can use shape sharing to reduce the memory costs of your simulation when you have many actors with identical geometry, but shared shapes have a very strong restriction: you cannot update the attributes of a shared shape while it is attached to an actor.

https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/apireference/files/structPxShapeFlag.html


NOTE: I THINK THAY CREATEEXCLUSIVESHAPE just makes a no-name shape and attaches it to an actor to force only 1 actor having that shape


****************Scene.addACtor()

*/


/*
RIGIDBODY DYNAMICS: https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/RigidBodyDynamics.html


The easiest way to calculate mass properties is to use the PxRigidBodyExt::updateMassAndInertia() helper function, which will set all three properties based on the actor's shapes and a uniform density value.

When manually setting the mass/inertia tensor of bodies, PhysX requires positive values for the mass and each principal axis of inertia. However, it is legal to provide 0s in these values. When provided with a 0 mass or inertia value, PhysX interprets this to mean infinite mass or inertia around that principal axis. This can be used to create bodies that resist all linear motion or that resist all or some angular motion. Examples of the effects that could be achieved using this approach are:

Bodies that behave as if they were kinematic.
Bodies whose translation behaves kinematically but whose rotation is dynamic.
Bodies whose translation is dynamic but whose rotation is kinematic.
Bodies which can only rotate around a specific axis.

0 mass == infinite

*/


/*
PhysX types: ACTORS

PxRigidStatic - SOLID & FIXED
PxRigidDynamic - SOLID & FIXED (split into kinematic and non-kinematic)

Thus the RigidBody component can have a constructor param isStatic that you must specify
then depending on flag, you create either the static or dynamic



*/