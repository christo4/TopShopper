

#include "physicsmanager.h"

using namespace physx;

PhysicsManager::PhysicsManager() {

}

PhysicsManager::~PhysicsManager() {

}


void PhysicsManager::init() {
	test = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAlloc, defaultError);
	if (!test) {
		std::cout << "PxCreateFoundation failed!" << "\n";
		exit(1);
	}

}


void PhysicsManager::updateMilliseconds(double deltaTime) {
}

void PhysicsManager::createObject() {
	bool recordMemoryAllocations = true;

	//Not sure how to implement this
	//Its for the visual debugger
	//PxPvd* mPvd = PxCreatePvd(*test);
	//PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	//mPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);


	//PxPhysics* mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *test,
		//PxTolerancesScale(), recordMemoryAllocations, mPvd);
	//if (!mPhysics) {
	//	exit(1); //("PxCreatePhysics failed!");
	//}



	//PxCooking* mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *test, PxCookingParams(scale));
	//if (!mCooking) ("PxCreateCooking failed!");

	//if (!PxInitExtensions(*mPhysics, mPvd))		fatalError("PxInitExtensions failed!");
}