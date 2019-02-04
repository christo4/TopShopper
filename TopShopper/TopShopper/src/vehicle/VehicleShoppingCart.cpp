
#include "VehicleShoppingCart.h"


//#include <ctype.h>
//#include <iostream>
//#include "vehicle/PxVehicleUtil.h"
//#include "snippetvehiclecommon/SnippetVehicleSceneQuery.h"
#include "snippetvehiclecommon/SnippetVehicleFilterShader.h"
//#include "snippetvehiclecommon/SnippetVehicleTireFriction.h"



using namespace physx;
using namespace snippetvehicle;





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




VehicleDesc initVehicleDesc(PxPhysics *physics)
{
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const PxF32 chassisMass = 1500.0f; // ORIGINAL
	//const PxF32 chassisMass = 100.0f; // WARNING: DONT SET IT TO A LOW VALUE WITHOUT CHANGING OTHER STUFF, cause it causes the cart to flip over and over
	const PxVec3 chassisDims(2.5f, 2.0f, 5.0f); // ORIGINAL
	//const PxVec3 chassisDims(25.0f, 2.0f, 5.0f); // REALLY WIDE (X is widescreen axis)
	//const PxVec3 chassisDims(2.5f, 20.0f, 5.0f); // REALLY TALL (Y is up)
	//const PxVec3 chassisDims(2.5f, 2.0f, 50.0f); // REALLY LONG (Z is towards you)
	//const PxVec3 chassisDims(3.0f, 3.5f, 5.0f); // MORE SHOPPING CART SIZED


	const PxVec3 chassisMOI
	((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass / 12.0f, // EASIER TO ROTATE AROUND ITS Y-AXIS
		(chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y*0.5f + 0.65f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 20.0f;
	const PxF32 wheelRadius = 0.5f;
	const PxF32 wheelWidth = 0.4f;
	const PxF32 wheelMOI = 0.5f*wheelMass*wheelRadius*wheelRadius;
	//const PxU32 nbWheels = 6; // ORIGINAL
	const PxU32 nbWheels = 4;

	VehicleDesc vehicleDesc;

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = physics->createMaterial(0.5f, 0.5f, 0.6f);
	//word0 = collide type, word1 = collide against types, word2 = PxPairFlags
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelRadius = wheelRadius;
	vehicleDesc.wheelWidth = wheelWidth;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = physics->createMaterial(0.5f, 0.5f, 0.6f);
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

	return vehicleDesc;
}













// CLASS STUFF...


VehicleShoppingCart::VehicleShoppingCart(PxPhysics *physics, PxCooking *cooking) {
	_vehicleDesc = initVehicleDesc(physics);
	_vehicle4W = createVehicle4W(_vehicleDesc, physics, cooking);
	_vehicle4W->getRigidDynamicActor()->setGlobalPose(PxTransform(0.0f, 0.0f, 0.0f, PxQuat(PxIdentity))); // all entities default to world origin
	_vehicle4W->setToRestState();
	_vehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	_vehicle4W->mDriveDynData.setUseAutoGears(false); // NOTE: for now only using eREVERSE and eFIRST
}

VehicleShoppingCart::~VehicleShoppingCart() {

}









// ~~~~~~TODO: write these to take into account reverse trigger and figure out what to do in key-combos... 
void VehicleShoppingCart::processRawInputDataKeyboard(const bool accelKeyPressed, const bool reverseKeyPressed, const bool handbrakeKeyPressed, const bool steerLeftKeyPressed, const bool steerRightKeyPressed, const bool turboKeyPressed) {

	// CASES:
	// UP/DOWN pressed = accel = false (no gear change needed)
	// UP pressed = accel = true (FIRST)
	// DOWN pressed = accel = true (REVERSE)
	// neither pressed = accel = false (no gear change needed)

	bool isAccelerating;

	// TURBO KEY OVERRIDES ACCELKEY/REVERSEKEY (It does not override handbrake however)
	if (turboKeyPressed) {
		_vehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFOURTH);
		isAccelerating = true;
	}
	else if (accelKeyPressed && reverseKeyPressed) {
		isAccelerating = false;
	}
	else if (accelKeyPressed) {
		_vehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
		isAccelerating = true;
	}
	else if (reverseKeyPressed) {
		_vehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
		isAccelerating = true;
	}
	else {
		isAccelerating = false;
	}
	
	
	_rawInputData.setDigitalAccel(isAccelerating);
	_rawInputData.setDigitalBrake(false);
	_rawInputData.setDigitalHandbrake(handbrakeKeyPressed);
	_rawInputData.setDigitalSteerLeft(steerLeftKeyPressed);
	_rawInputData.setDigitalSteerRight(steerRightKeyPressed);
}

void VehicleShoppingCart::processRawInputDataController(const PxReal accel, const PxReal reverse, const PxReal handbrake, const PxReal steer, const bool turboButtonPressed) {
	
	/*
	ROCKET LEAGUE DRIVING MODEL...

	ACCEL = R2/RT
	REVERSE = L2/LT
	HANDBRAKE = square/X
	TURBO = circle/B
	STEER = LSTICK
	CAMERA = RSTICK (renderer will handle this input???)


	ALTERNATIVE...
	Other way I thought was if you set accel = max and brake = min and still change egars same way
	maybe for turbo it puts you in a higher gear? and sets you're accel = 1.0f and disables 

	//TODO: could have a threshold change for minimizing twitches
	//TODO: maybe I should change it so that if the current gear is the same as the new one, then dont call a change to it since I dont know if it resets anything
	*/

	PxReal netAccel;

	// TURBO BUTTON OVERRIDES TRIGGERS (It does not override handbrake however)
	if (turboButtonPressed) {
		_vehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFOURTH); // TODO: play around with which gear to use
		netAccel = 1.0f; 
	}
	else if (accel >= reverse) {
		_vehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
		netAccel = accel - reverse;
	}
	else {
		_vehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
		netAccel = reverse - accel;
	}
	
	_rawInputData.setAnalogAccel(netAccel);
	_rawInputData.setAnalogBrake(0.0f);
	_rawInputData.setAnalogHandbrake(handbrake);
	_rawInputData.setAnalogSteer(steer);
}

void VehicleShoppingCart::clearRawInputDataKeyboard() {
	_rawInputData.setDigitalAccel(false);
	_rawInputData.setDigitalBrake(false);
	_rawInputData.setDigitalHandbrake(false);
	_rawInputData.setDigitalSteerLeft(false);
	_rawInputData.setDigitalSteerRight(false);
}

void VehicleShoppingCart::clearRawInputDataController() {
	_rawInputData.setAnalogAccel(0.0f);
	_rawInputData.setAnalogBrake(0.0f);
	_rawInputData.setAnalogHandbrake(0.0f);
	_rawInputData.setAnalogSteer(0.0f);
}






void VehicleShoppingCart::updatePhysics(double deltaTime) {

	// Update the control inputs for the vehicle...

	// if key controlled...
	//PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable, _rawInputData, deltaTime, getIsAirborne(), *_vehicle4W);
	// if controller controlled...
	PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, _rawInputData, deltaTime, getIsAirborne(), *_vehicle4W);
}
