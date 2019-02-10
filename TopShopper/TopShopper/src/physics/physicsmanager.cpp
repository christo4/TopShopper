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



#define PVD_ENABLED // ~~~~~NOTE: comment this out if you dont have PVD installed or on release

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
std::vector<PxTransform> gSpawnPoints = { PxTransform(40.0f, 51.0f, 40.0f, PxQuat(PxIdentity)), PxTransform(-40.0f, 51.0f, 40.0f, PxQuat(PxIdentity)), PxTransform(40.0f, 51.0f, -40.0f, PxQuat(PxIdentity)), PxTransform(-40.0f, 51.0f, -40.0f, PxQuat(PxIdentity)) };
int gSpawnID = 0;


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

// TODO: change this in the future to be a customfiltershader

PxFilterFlags VehicleFilterShader
(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);

	if ((0 == (filterData0.word0 & filterData1.word1)) && (0 == (filterData1.word0 & filterData0.word1)))
		return PxFilterFlag::eSUPPRESS;

	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	pairFlags |= PxPairFlags(PxU16(filterData0.word2 | filterData1.word2));

	return PxFilterFlags();
}


//the basic usage of the filter shader, and it will ensure that SampleSubmarine::onContact() is called for all interesting pairs.
// Callback for every collision pair??? (i guess it applies to each shape?)
// will be called for all pairs of shapes that come near each other -- more precisely: for all pairs of shapes whose axis aligned bounding boxes in world space are found to intersect for the first time. All behavior beyond that is determined by what SampleSubmarineFilterShader() returns.
PxFilterFlags CustomFilterShader
(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);

	if ((0 == (filterData0.word0 & filterData1.word1)) && (0 == (filterData1.word0 & filterData0.word1)))
		return PxFilterFlag::eSUPPRESS;

	// if either (or both) object is a trigger...
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1)) {
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	// if both are solid (or not-simulated???)...
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	//pairFlags |= PxPairFlags(PxU16(filterData0.word2 | filterData1.word2));


	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice-versa (symmetric relationship)
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return PxFilterFlag::eDEFAULT;

/*
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);

	// NOTE: this is just making sure we defined a symmetric relationship for collisions...
	if ((0 == (filterData0.word0 & filterData1.word1)) && (0 == (filterData1.word0 & filterData0.word1)))
		return PxFilterFlag::eSUPPRESS;

	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	pairFlags |= PxPairFlags(PxU16(filterData0.word2 | filterData1.word2));

	return PxFilterFlags();
	*/
}


/*
PxFilterFlags CustomFilterShader
(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{

	// ~~~~~NOTE: I think that PX_UNUSED means that these params arent used anywhere
	PX_UNUSED(attributes0);
	PX_UNUSED(filterData0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);

	// this was passed by reference
	// I think these are the only flags we currently need
	pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT | PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return PxFilterFlag::eDEFAULT;

// use eCALLBACK and eSURPORESS/KILL


	if ((0 == (filterData0.word0 & filterData1.word1)) && (0 == (filterData1.word0 & filterData0.word1)))
		return PxFilterFlag::eSUPPRESS;

	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	pairFlags |= PxPairFlags(PxU16(filterData0.word2 | filterData1.word2));


	//return PxFilterFlags();
}
*/





/////////////////////////////////////////////////////////////////////////////
// PHYSICSMANAGER STUFF...



PhysicsManager::PhysicsManager(Broker* broker)
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
	simScale.speed = 9.81f; // 9.81 m/s by default (speed reached after falling for 1 second under 9.81m/s^2 gravity)

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



	switchToScene1();

}



void PhysicsManager::switchToScene1() {
	// TODO: cleanup scene we just transitioned from...

	// init vehicle stuff, create entities / add their actors into PxScene, position them at starting transforms, etc.....
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

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


	// NOTE: PXU32 is a typedef of UINT32
	// ~~~~NOTE: below its calling the constructor to pass in word0, word1, word2, and word3 making up 128 bits
	// the words can store any data that we want to pass to the filter shader
	// The sample uses word0 = out collision flag
	// word1 = flag combo of all flags that we can collide with


	// GROUND:

	std::shared_ptr<Ground> ground = std::dynamic_pointer_cast<Ground>(instantiateEntity(EntityTypes::GROUND, PxTransform(0.0f, 0.0f, 0.0f, PxQuat(PxIdentity)), "ground"));
	physxScene->addActor(*(ground->_actor));


	// IT NOW WORKS!!!
	// WHAT IVE LEARNED!!!
	// THE SAMPLE OBJ FILE I WAS USING HAD the faces in wrong order, I reversed the order and that worked, but 1 triangle was pass-throughable
	// to fix this, I tried decrementing the indices by 1 (now its 0-indexed rather than 1-indexed like in the file) and that seems to have fixed it.
	// Thus if you reverse the order of the indices (within their triplets) it flips the plane upside down and NO COLLISION OCCURS WITH THE "INSIDE" OF A TRIANGLE 
	

	// VEHICLE 1:

	//Create a vehicle that will drive on the plane.
	
	std::shared_ptr<ShoppingCartPlayer> vehicle1 = std::dynamic_pointer_cast<ShoppingCartPlayer>(instantiateEntity(EntityTypes::SHOPPING_CART_PLAYER, PxTransform(0.0f, 60.0f, 0.0f, PxQuat(PxIdentity)), "vehicle1"));
	vehicle1->setInputID(1);
	physxScene->addActor(*(vehicle1->_actor));



	// SPARE CHANGE 1:

	//FOR TESTING COLLISION BEFORE WE AUTO SPAWN THEM (NOTE: THEIR NAMES WILL BE THE SAME BUT THAT DOESNT MATTER)

	std::shared_ptr<SpareChange> spareChange1 = std::dynamic_pointer_cast<SpareChange>(instantiateEntity(EntityTypes::SPARE_CHANGE, PxTransform(30.0f, 51.0f, 30.0f, PxQuat(PxIdentity)), "spareChange1"));
	physxScene->addActor(*(spareChange1->_actor));

	_activeScene = std::make_shared<GameScene>(physxScene);
	_activeScene->addEntity(ground);
	_activeScene->addEntity(vehicle1);
	_activeScene->addEntity(spareChange1);

}



// ASIDE: if we want a fixed timestep like Unity then pass in say 1/30 or 1/60 but then we need to change UpdateAll() to only call physcismanager.init() when the deltaTimes accumulate enough
void PhysicsManager::updateMilliseconds(double deltaTime) {

	// call updatePhysics() on each entity in scene...
	for (std::shared_ptr<Entity> &entity : _activeScene->_entities) {
		entity->updatePhysics(deltaTime);
	}


	// FURTHER VEHICLE UPDATES...

	// ~~~~~~~TODO: change NBVehicles to account for bots and dumbcarts in future
	
	std::vector<std::shared_ptr<ShoppingCartPlayer>> shoppingCartPlayers = _activeScene->getAllShoppingCartPlayers();

	//Raycasts...
	
	std::vector<PxVehicleWheels*> vehiclesVector;
	for (std::shared_ptr<ShoppingCartPlayer> &shoppingCartPlayer : shoppingCartPlayers) {
		vehiclesVector.push_back(shoppingCartPlayer->_shoppingCartBase->_vehicle4W);
	}

	PxRaycastQueryResult *raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(gBatchQuery, vehiclesVector.size(), vehiclesVector.data(), raycastResultsSize, raycastResults);


	//Vehicle update...
	const PxVec3 grav = _activeScene->_physxScene->getGravity();
	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS]; // ~~~~~~~~~~~~~~maybe this should be 4???
	//PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, gVehicle4W->mWheelsSimData.getNbWheels()} };
	// ~~~~~~~~~~~~~HACK FOR NOW... change to NBVEHICLES tomorrow...
	PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, 4} };
	PxVehicleUpdates(deltaTime, grav, *gFrictionPairs, shoppingCartPlayers.size(), vehiclesVector.data(), vehicleQueryResults);

	//Work out if the vehicle is in the air.
	//gIsVehicleInAir = gVehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);
	

	// ~~~~~~~~~~~~~HACK FOR NOW... change to for int i = 0 and then queryresults[i] tomorrow...
	// Update isAirborne flags for each vehicle...
	for (std::shared_ptr<ShoppingCartPlayer> &shoppingCartPlayer : shoppingCartPlayers) {
		shoppingCartPlayer->_shoppingCartBase->setIsAirborne(shoppingCartPlayer->_shoppingCartBase->_vehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]));
	}


	// Scene update...
	_activeScene->_physxScene->simulate(deltaTime);
	_activeScene->_physxScene->fetchResults(true); // wait for results to come in before moving on to next system


	// remove actors that were flagged for destroy
	for (int i = 0; i < _activeScene->_entities.size(); i++) {
		if (_activeScene->_entities.at(i)->getDestroyFlag()) {
			_activeScene->_physxScene->removeActor(*(_activeScene->_entities.at(i)->_actor));
			_activeScene->_entities.erase(_activeScene->_entities.begin() + i);
		}
	}

	// TEMPORARY:
	// spawn a new spare change if last one was destroyed

	std::vector<std::shared_ptr<SpareChange>> spareChangeVec = _activeScene->getAllSpareChange();
	if (spareChangeVec.size() == 0) {
		gSpawnID++;
		if (gSpawnID >= gSpawnPoints.size()) gSpawnID = 0;
		std::shared_ptr<SpareChange> spareChangeNEW = std::dynamic_pointer_cast<SpareChange>(instantiateEntity(EntityTypes::SPARE_CHANGE, gSpawnPoints.at(gSpawnID), "spareChangeNEW"));
		_activeScene->_physxScene->addActor(*(spareChangeNEW->_actor));
		_activeScene->addEntity(spareChangeNEW);
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


// TODO: delete all local pointers
// HAVE TO CHANGE THIS!!! - change std::string to const char* (since cStr returns empty..)
std::shared_ptr<Entity> PhysicsManager::instantiateEntity(EntityTypes type, physx::PxTransform transform, const char *name) {

	switch (type) {
	case EntityTypes::GROUND:
	{
		std::vector<PxVec3> groundVerts = castVectorOfGLMVec4ToVectorOfPxVec3((_broker->get_LoadingManager_Geometry(GeometryTypes::GROUND_GEO))->verts);
		std::vector<PxU32> groundIndices = (_broker->get_LoadingManager_Geometry(GeometryTypes::GROUND_GEO))->indices;
		//std::reverse(std::begin(groundIndices), std::end(groundIndices)); // NO NEED TO REVERSE IN THIS CONFIGURATION< BUT MIGHT NEED THIS IN FUTURE (OR I COULD USE THE PXMESHFLAG)
		PxFilterData groundSimFilterData(CollisionFlags::COLLISION_FLAG_GROUND, CollisionFlags::COLLISION_FLAG_GROUND_AGAINST, 0, 0);
		PxMaterial *groundMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
		PxRigidStatic *groundActor = createDrivableTerrain(groundVerts, groundIndices, groundSimFilterData, groundMaterial, gPhysics, gCooking);
		std::shared_ptr<Ground> ground = std::make_shared<Ground>(groundActor);
		ground->_actor->setName(name);
		(ground->_actor->is<PxRigidStatic>())->setGlobalPose(transform);
		return ground;
	}
	case EntityTypes::SHOPPING_CART_PLAYER:
	{
		VehicleShoppingCart *shoppingCartBase = new VehicleShoppingCart(gPhysics, gCooking);
		std::shared_ptr<ShoppingCartPlayer> shoppingCartPlayer = std::make_shared<ShoppingCartPlayer>(shoppingCartBase, 0); // NOTE: an invalid input ID of 0 is placed here as a default since it must be set later
		shoppingCartPlayer->_actor->setName(name);
		(shoppingCartPlayer->_actor->is<PxRigidDynamic>())->setGlobalPose(transform);
		return shoppingCartPlayer;
	}
	case EntityTypes::SPARE_CHANGE:
	{
		PxReal radius = 1.0f;
		PxSphereGeometry col(radius);
		PxFilterData simFilterData(CollisionFlags::COLLISION_FLAG_PICKUP, CollisionFlags::COLLISION_FLAG_PICKUP_AGAINST, 0, 0);
		PxMaterial *mat = gPhysics->createMaterial(1.0f, 1.0f, 1.0f);
		PxShape *shape = gPhysics->createShape(col, *mat, true, PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eVISUALIZATION);
		PxFilterData qryFilterData;
		setupNonDrivableSurface(qryFilterData);
		shape->setQueryFilterData(qryFilterData);
		shape->setSimulationFilterData(simFilterData);
		PxRigidDynamic *actor = gPhysics->createRigidDynamic(PxTransform(0.0f, 0.0f, 0.0f, PxQuat(PxIdentity)));
		actor->attachShape(*shape);
		actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		std::shared_ptr<SpareChange> spareChange = std::make_shared<SpareChange>(actor);
		spareChange->_actor->setName(name);
		(spareChange->_actor->is<PxRigidDynamic>())->setGlobalPose(transform);
		return spareChange;
	}
	default:
		return nullptr;
	}
}


// NOTE: if you dont attach the shape to an actor it wont render (DUH!)

// NOTE: a solid collider will be pass-throughable by default without filtering...
















// ~~~~~~~~~~~~~~NOTE: use PxActorFlag::Enum eDISABLE_SIMULATION / GRAVITY 
// USE	pxrigiddynamic::setKinematicTarget to move kinematic actors





/*
CALLBACK INFO
- callback are called from fetchresults() rather than on the simulation thread

- OnTrigger
- OnContact

- NOTE: for these 2 events to be received, must set a flag in the filter shader callback for ALL PAIRS of interacting objects for which events are required

- use PxActor.GetType and then be able to downcast to subclass Static or DYnamic

*/



void CustomSimulationEventCallback::onAdvance(const physx::PxRigidBody *const *bodyBuffer, const physx::PxTransform *poseBuffer, const physx::PxU32 count) {}
void CustomSimulationEventCallback::onConstraintBreak(physx::PxConstraintInfo *constraints, physx::PxU32 count) {}

void CustomSimulationEventCallback::onContact(const physx::PxContactPairHeader &pairHeader, const physx::PxContactPair *pairs, physx::PxU32 nbPairs) {
	//std::cout << pairHeader.actors[0]->getName() << std::endl;
	//std::cout << pairHeader.actors[1]->getName() << std::endl;
}


void CustomSimulationEventCallback::onSleep(physx::PxActor **actors, physx::PxU32 count) {}



// ????
// I think this gets called ONCE for all pairs that a collision was reported for during this frame...
//void CustomSimulationEventCallback::onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs) {
	//pairHeader.
//}

//https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/RigidBodyCollision.html
//The code above iterates through all pairs of overlapping shapes that involve a trigger shape.If it is found that the treasure has been touched by the submarine then the flag gTreasureFound is set true.

// NOTE: MY GUESS IS THAT THIS CALLBACK GETS CALLED TWICE PER PAIR???
// OR MAYBE the triggershape is the shape with a trigger collider and othershape is the solid shape (or 2nd trigger shape?)
// ~~~~~~~~POSSIBLE BUG ... it seems like this gets called both for ontriggereneter and ontriggerleave (possible fix would be to change defaulttrigger flags to remove notfiy_touch_lost
void CustomSimulationEventCallback::onTrigger(PxTriggerPair *pairs, PxU32 count) {

	for (PxU32 i = 0; i < count; i++) {

		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
			PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		Entity *otherEntity = static_cast<Entity*>((pairs[i].otherActor)->userData);
		Entity *triggerEntity = static_cast<Entity*>((pairs[i].triggerActor)->userData);

		if (otherEntity->getTag() == EntityTypes::SHOPPING_CART_PLAYER && triggerEntity->getTag() == EntityTypes::SPARE_CHANGE) {
			ShoppingCartPlayer *shoppingCartPlayer = static_cast<ShoppingCartPlayer*>((pairs[i].otherActor)->userData);
			SpareChange *spareChange = static_cast<SpareChange*>((pairs[i].triggerActor)->userData);

			spareChange->destroy();

			// call shoppingCartPlayer->incrementPoints(spareChange->pts))
			// also destroy pickup
			// call GameScene::removeEntity(spareChange) which removes it from _entities and then releases all physics stuff (maybe just releast _actor which might auto release the shapes??)
			// ~~~~~~~~NOTE: should GameScene::addEntity also add it to the physx scene?? - maybe not in case we want some entities that arent in physx scene. Then again each entity has a transform and could just be set to non simulated
			//spareChange->_actor->getScene()->removeActor(*(spareChange->_actor));
			//PxScene *sceneTEST = spareChange->_actor->getScene();
			//sceneTEST->removeActor(*(spareChange->_actor)); NOTE: API WRITES CANT OCCUR IN A CALLBACK FUCNTION, so I can't call any function that will affect ask here???
			// NEW PLAN: call Entity::flagToDestroy() on spare change, which will set Entity::toDestroy() which will then the PhysicsManager will call its ::cleanupDestroyedEntities()
		}

		// CAN add more pairs of interest here...

		//if (dynamic_cast<ShoppingCartPlayer*>((&pairs[i].otherActor->userData)

		//if ((&pairs[i].otherShape->getActor() == mSubmarineActor) &&
			//(&pairs[i].triggerShape->getActor() == gTreasureActor))
		//{
			//gTreasureFound = true;
		//}
	}
}


void CustomSimulationEventCallback::onWake(physx::PxActor **actors, physx::PxU32 count) {}



/*
class CustomSimulationEventCallback : public PxSimulationEventCallback {
	public:
		CustomSimulationEventCallback() {}
		virtual ~CustomSimulationEventCallback() {}
		void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) override {}
		void onWake(PxActor **actors, PxU32 count) override {}
		void onSleep(PxActor **actors, PxU32 count) override {}


		// SAMPLE SUBMARINE EXAMPLE IS AMAZING!!!!!!!!!!!!!!!!!
		// ~~~~~~~~NOTE: pairs is a pointer to an array of PxContactPair
		// size of this array is nbPairs
		// an array name is a constant pointer to the 1st element of the array
		void onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs) override {

			for (PxU32 i = 0; i < nbPairs; i++) { // loop through each contact pair
				const PxContactPair& pair = pairs[i]; // pure alias of pair

				// we only care about NOTIFY_TOUCH_FOUND events, but you could have else if blocks for each event to check for
				if (pair.events & PxPairFlag::eNOTIFY_TOUCH_FOUND) { // check if NOTIFY_TOUCH_FOUND bit was set in last frame...
					// HERE DO ALL THE PAIR EVENT CHECKING...
					// E.G. A SHOPPING CART PLAYER COLLIDES WITH ANOTHER PLAYER, IF SO CHECK IF EITHER IS BOOSTING AND SEE WHICH ONE(S) HIT HEAD ON


				}
			}


			// can get actors / pairs
			// can get actor name as a way of identifying what happened
			// OR MAYBE SETUP USERDATA to point to an instance of Entity which has a tag enum field
			// then based on this enum flag, it can be downcasted when necessary to get subclass info (if needed)
		} // CARE ABOUT THIS
		
		// ~~~~~~~~~NOTE: in c++ false ==0 and true = !false (thats why we can use bitwise flags, so 1000000 ~= 0001000 ~= true, but 0000000 == false 
		// but NOTE: if (2 == true) will be false
		// TIP: could have an event represented as a bitflag and then to check if no events occured, we just check if the flag combo == 0 like if (flags)
		
		// QUESTION: I WONDER IF YOU COULD HAVE ENTITY SUBCLASS PXSIMEVENTCALLBACK AND THEN EACH HAS THEIR OWN ONCONTACT/ONTRIGGER, BUT THEN IT IS MORE DISJOINT AND UGLY


		// THIS vvvvvvvvvvvvvvvvvvvv
		// ~~~~~~~~~~~~~~~~~~~~~TODO: figure out configuring the userData of each Actor to be set to the specific entity subclass so that once a tag is identified, we can reinterpret_cast and call a reaction method

		//WARNING: By default collisions between kinematic rigid bodies and kinematic and static rigid bodies will not get reported.To enable these reports raise the PxSceneFlag::eENABLE_KINEMATIC_PAIRS or ::eENABLE_KINEMATIC_STATIC_PAIRS flag respectively by calling PxScene::setFlag().




		void onTrigger(PxTriggerPair *pairs, PxU32 count) override {} // CARE ABOUT THIS



		void onAdvance(const PxRigidBody *const *bodyBuffer, const PxTransform *poseBuffer, const PxU32 count) override {}
	private:

};
*/

/*

later on for a single scene, do CustomSimEC instance; and then call scene.setSImulationEventCallback() or scenedesc.simulationEVentCallback



*/






// ENTITY BUILDER METHODS...


// RETURNS AN INSTANCE OF SHOPPINGCARTPLAYER WITH DEFAULT (PREFAB) VALUES THAT SHOULD BE OVERWRITTEN IN THE CHANGESCENE METHODS
//std::shared_ptr<ShoppingCartPlayer> PhysicsManager::instantiateShoppingCartPlayer() {
	//VehicleShoppingCart *shoppingCartBase = new VehicleShoppingCart(gPhysics, gCooking);
	//std::shared_ptr<ShoppingCartPlayer> shoppingCartPlayer = std::make_shared<ShoppingCartPlayer>("default_name", shoppingCartBase, 0); // NOTE: an invalid input ID of 0 is placed here as a default since it must be set later
	//return shoppingCartPlayer;
//}




















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





// RESEARCH NOTES BELOW

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