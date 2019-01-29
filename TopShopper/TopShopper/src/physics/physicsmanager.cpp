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


//#define PVD_ENABLED // ~~~~~NOTE: comment this out if you dont have PVD installed or on release



#include <ctype.h>

#include "physicsmanager.h"
#include <iostream>

#include "PxPhysicsAPI.h"

#include "vehicle/PxVehicleUtil.h"
#include "vehicle/snippetvehiclecommon/SnippetVehicleSceneQuery.h"
#include "vehicle/snippetvehiclecommon/SnippetVehicleFilterShader.h"
#include "vehicle/snippetvehiclecommon/SnippetVehicleTireFriction.h"
#include "vehicle/snippetvehiclecommon/SnippetVehicleCreate.h"

//#include "../snippetcommon/SnippetPrint.h"

#ifdef PVD_ENABLED
#include "vehicle/snippetcommon/SnippetPVD.h"
#endif // PVD_ENABLED

//#include "../snippetutils/SnippetUtils.h"


using namespace physx;
using namespace snippetvehicle;


PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation*			gFoundation = NULL;
PxPhysics*				gPhysics = NULL;

PxDefaultCpuDispatcher*	gDispatcher = NULL;
PxScene*				gScene = NULL;

PxCooking*				gCooking = NULL;

PxMaterial*				gMaterial = NULL;

#ifdef PVD_ENABLED
PxPvd*                  gPvd = NULL;
#endif // PVD_ENABLED

VehicleSceneQueryData*	gVehicleSceneQueryData = NULL;
PxBatchQuery*			gBatchQuery = NULL;

PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

PxRigidStatic*			gGroundPlane = NULL;
PxVehicleDrive4W*		gVehicle4W = NULL;

bool					gIsVehicleInAir = true;


PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
	0.0f,		0.75f,
	5.0f,		0.75f,
	30.0f,		0.125f,
	120.0f,		0.1f,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};
PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);

PxVehicleKeySmoothingData gKeySmoothingData =
{
	{
		6.0f,	//rise rate eANALOG_INPUT_ACCEL
		6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f,	//fall rate eANALOG_INPUT_ACCEL
		10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

PxVehiclePadSmoothingData gPadSmoothingData =
{
	{
		6.0f,	//rise rate eANALOG_INPUT_ACCEL
		6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f,	//fall rate eANALOG_INPUT_ACCEL
		10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

PxVehicleDrive4WRawInputData gVehicleInputData;

enum DriveMode
{
	eDRIVE_MODE_ACCEL_FORWARDS = 0,
	eDRIVE_MODE_ACCEL_REVERSE,
	eDRIVE_MODE_HARD_TURN_LEFT,
	eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
	eDRIVE_MODE_HARD_TURN_RIGHT,
	eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_NONE
};

DriveMode gDriveModeOrder[] =
{
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_ACCEL_FORWARDS,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_ACCEL_REVERSE,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_HARD_TURN_LEFT,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_HARD_TURN_RIGHT,
	eDRIVE_MODE_ACCEL_FORWARDS,
	eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
	eDRIVE_MODE_ACCEL_FORWARDS,
	eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
	eDRIVE_MODE_NONE
};

PxF32					gVehicleModeLifetime = 4.0f;
PxF32					gVehicleModeTimer = 0.0f;
PxU32					gVehicleOrderProgress = 0;
bool					gVehicleOrderComplete = false;
bool					gMimicKeyInputs = false;

VehicleDesc initVehicleDesc()
{
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const PxF32 chassisMass = 1500.0f;
	const PxVec3 chassisDims(2.5f, 2.0f, 5.0f);
	const PxVec3 chassisMOI
	((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass / 12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y*0.5f + 0.65f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 20.0f;
	const PxF32 wheelRadius = 0.5f;
	const PxF32 wheelWidth = 0.4f;
	const PxF32 wheelMOI = 0.5f*wheelMass*wheelRadius*wheelRadius;
	const PxU32 nbWheels = 6;

	VehicleDesc vehicleDesc;

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = gMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelRadius = wheelRadius;
	vehicleDesc.wheelWidth = wheelWidth;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = gMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

	return vehicleDesc;
}

void startAccelerateForwardsMode()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalAccel(true);
	}
	else
	{
		gVehicleInputData.setAnalogAccel(1.0f);
	}
}

void startAccelerateReverseMode()
{
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);

	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalAccel(true);
	}
	else
	{
		gVehicleInputData.setAnalogAccel(1.0f);
	}
}

void startBrakeMode()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalBrake(true);
	}
	else
	{
		gVehicleInputData.setAnalogBrake(1.0f);
	}
}

void startTurnHardLeftMode()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalAccel(true);
		gVehicleInputData.setDigitalSteerLeft(true);
	}
	else
	{
		gVehicleInputData.setAnalogAccel(true);
		gVehicleInputData.setAnalogSteer(-1.0f);
	}
}

void startTurnHardRightMode()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalAccel(true);
		gVehicleInputData.setDigitalSteerRight(true);
	}
	else
	{
		gVehicleInputData.setAnalogAccel(1.0f);
		gVehicleInputData.setAnalogSteer(1.0f);
	}
}

void startHandbrakeTurnLeftMode()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalSteerLeft(true);
		gVehicleInputData.setDigitalHandbrake(true);
	}
	else
	{
		gVehicleInputData.setAnalogSteer(-1.0f);
		gVehicleInputData.setAnalogHandbrake(1.0f);
	}
}

void startHandbrakeTurnRightMode()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalSteerRight(true);
		gVehicleInputData.setDigitalHandbrake(true);
	}
	else
	{
		gVehicleInputData.setAnalogSteer(1.0f);
		gVehicleInputData.setAnalogHandbrake(1.0f);
	}
}


void releaseAllControls()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalAccel(false);
		gVehicleInputData.setDigitalSteerLeft(false);
		gVehicleInputData.setDigitalSteerRight(false);
		gVehicleInputData.setDigitalBrake(false);
		gVehicleInputData.setDigitalHandbrake(false);
	}
	else
	{
		gVehicleInputData.setAnalogAccel(0.0f);
		gVehicleInputData.setAnalogSteer(0.0f);
		gVehicleInputData.setAnalogBrake(0.0f);
		gVehicleInputData.setAnalogHandbrake(0.0f);
	}
}

/*
void initPhysics()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	gPvd = PxCreatePvd(*gFoundation);
	//PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	//gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	PxU32 numWorkers = 1;
	gDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = VehicleFilterShader;

	gScene = gPhysics->createScene(sceneDesc);
	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));

	/////////////////////////////////////////////

	PxInitVehicleSDK(*gPhysics);
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	//Create the batched scene queries for the suspension raycasts.
	gVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, gAllocator);
	gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);

	//Create the friction table for each combination of tire and surface type.
	gFrictionPairs = createFrictionPairs(gMaterial);

	//Create a plane to drive on.
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics);
	gScene->addActor(*gGroundPlane);

	//Create a vehicle that will drive on the plane.
	VehicleDesc vehicleDesc = initVehicleDesc();
	gVehicle4W = createVehicle4W(vehicleDesc, gPhysics, gCooking);
	PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y*0.5f + vehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
	gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	gScene->addActor(*gVehicle4W->getRigidDynamicActor());

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	gVehicle4W->setToRestState();
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	gVehicle4W->mDriveDynData.setUseAutoGears(true);

	gVehicleModeTimer = 0.0f;
	gVehicleOrderProgress = 0;
	startBrakeMode();
}
*/

void incrementDrivingMode(const PxF32 timestep)
{
	gVehicleModeTimer += timestep;
	if (gVehicleModeTimer > gVehicleModeLifetime)
	{
		//If the mode just completed was eDRIVE_MODE_ACCEL_REVERSE then switch back to forward gears.
		if (eDRIVE_MODE_ACCEL_REVERSE == gDriveModeOrder[gVehicleOrderProgress])
		{
			gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
		}

		//Increment to next driving mode.
		gVehicleModeTimer = 0.0f;
		gVehicleOrderProgress++;
		releaseAllControls();

		//If we are at the end of the list of driving modes then start again.
		if (eDRIVE_MODE_NONE == gDriveModeOrder[gVehicleOrderProgress])
		{
			gVehicleOrderProgress = 0;
			gVehicleOrderComplete = true;
		}

		//Start driving in the selected mode.
		DriveMode eDriveMode = gDriveModeOrder[gVehicleOrderProgress];
		switch (eDriveMode)
		{
		case eDRIVE_MODE_ACCEL_FORWARDS:
			startAccelerateForwardsMode();
			break;
		case eDRIVE_MODE_ACCEL_REVERSE:
			startAccelerateReverseMode();
			break;
		case eDRIVE_MODE_HARD_TURN_LEFT:
			startTurnHardLeftMode();
			break;
		case eDRIVE_MODE_HANDBRAKE_TURN_LEFT:
			startHandbrakeTurnLeftMode();
			break;
		case eDRIVE_MODE_HARD_TURN_RIGHT:
			startTurnHardRightMode();
			break;
		case eDRIVE_MODE_HANDBRAKE_TURN_RIGHT:
			startHandbrakeTurnRightMode();
			break;
		case eDRIVE_MODE_BRAKE:
			startBrakeMode();
			break;
		case eDRIVE_MODE_NONE:
			break;
		};

		//If the mode about to start is eDRIVE_MODE_ACCEL_REVERSE then switch to reverse gears.
		if (eDRIVE_MODE_ACCEL_REVERSE == gDriveModeOrder[gVehicleOrderProgress])
		{
			gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
		}
	}
}

/*
void stepPhysics()
{
	const PxF32 timestep = 1.0f / 60.0f;

	//Cycle through the driving modes to demonstrate how to accelerate/reverse/brake/turn etc.
	incrementDrivingMode(timestep);

	//Update the control inputs for the vehicle.
	if (gMimicKeyInputs)
	{
		PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, timestep, gIsVehicleInAir, *gVehicle4W);
	}
	else
	{
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, timestep, gIsVehicleInAir, *gVehicle4W);
	}

	//Raycasts.
	PxVehicleWheels* vehicles[1] = { gVehicle4W };
	PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

	//Vehicle update.
	const PxVec3 grav = gScene->getGravity();
	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
	PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, gVehicle4W->mWheelsSimData.getNbWheels()} };
	PxVehicleUpdates(timestep, grav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);

	//Work out if the vehicle is in the air.
	gIsVehicleInAir = gVehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);

	//Scene update.
	gScene->simulate(timestep);
	gScene->fetchResults(true);
}
*/

/*
void cleanupPhysics()
{
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
	//PxPvdTransport* transport = gPvd->getTransport();
	//gPvd->release();
	//transport->release();
	gFoundation->release();

	printf("SnippetVehicle4W done.\n");
}
*/

/*
void keyPress(unsigned char key, const PxTransform& camera)
{
	PX_UNUSED(camera);
	PX_UNUSED(key);
}
*/

/*
int snippetMain(int, const char*const*)
{
#ifdef RENDER_SNIPPET
	extern void renderLoop();
	renderLoop();
#else
	initPhysics();
	while (!gVehicleOrderComplete)
	{
		stepPhysics();
	}
	cleanupPhysics();
#endif

	return 0;
}
*/











///////////////////////////////////////////////



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

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	// SETUP VEHICLE SDK
	PxInitVehicleSDK(*gPhysics);
	PxVehicleSetBasisVectors(PxVec3(0.0f, 1.0f, 0.0f), PxVec3(0.0f, 0.0f, 1.0f)); // up = y, forward = z
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);





	// SCENE STUFF THAT WILL BE MOVED OUT LATER TO THEIR OWN METHODS...
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	PxU32 numWorkers = 1; // 1 thread off the main thread (~~~~~~~~MAYBE CHANGE TO 0, to run on main thread?)
	gDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = VehicleFilterShader;

	gScene = gPhysics->createScene(sceneDesc);


	#ifdef PVD_ENABLED
	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	#endif // PVD_ENABLED


	



	//Create the batched scene queries for the suspension raycasts.
	gVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, gAllocator);
	gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);

	//Create the friction table for each combination of tire and surface type.
	gFrictionPairs = createFrictionPairs(gMaterial);

	//Create a plane to drive on.
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics);
	gScene->addActor(*gGroundPlane);

	//Create a vehicle that will drive on the plane.
	VehicleDesc vehicleDesc = initVehicleDesc();
	gVehicle4W = createVehicle4W(vehicleDesc, gPhysics, gCooking);
	PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y*0.5f + vehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
	gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	gScene->addActor(*gVehicle4W->getRigidDynamicActor());

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	gVehicle4W->setToRestState();
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	gVehicle4W->mDriveDynData.setUseAutoGears(true);

	gVehicleModeTimer = 0.0f;
	gVehicleOrderProgress = 0;
	startBrakeMode();

}


// ASIDE: if we want a fixed timestep like Unity then pass in say 1/30 or 1/60 but then we need to change UpdateAll() to only call physcismanager.init() when the deltaTimes accumulate enough
void PhysicsManager::updateMilliseconds(double deltaTime) {

	//Cycle through the driving modes to demonstrate how to accelerate/reverse/brake/turn etc.
	incrementDrivingMode(deltaTime);

	//Update the control inputs for the vehicle.
	if (gMimicKeyInputs)
	{
		PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, deltaTime, gIsVehicleInAir, *gVehicle4W);
	}
	else
	{
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, deltaTime, gIsVehicleInAir, *gVehicle4W);
	}

	//Raycasts.
	PxVehicleWheels* vehicles[1] = { gVehicle4W };
	PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

	//Vehicle update.
	const PxVec3 grav = gScene->getGravity();
	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
	PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, gVehicle4W->mWheelsSimData.getNbWheels()} };
	PxVehicleUpdates(deltaTime, grav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);

	//Work out if the vehicle is in the air.
	gIsVehicleInAir = gVehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);

	//Scene update.
	gScene->simulate(deltaTime);
	gScene->fetchResults(true); // wait for results to come in before moving on to next system

}



void PhysicsManager::cleanup() {
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
}



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