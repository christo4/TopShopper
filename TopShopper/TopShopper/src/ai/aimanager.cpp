#include "aimanager.h"
#include "PxPhysicsAPI.h"
#include "objects/sparechange.h"
#include "core/broker.h"


using namespace physx;

// TEMPORARY: for Milestone 2 only
//std::vector<PxTransform> gSpawnPoints = { PxTransform(40.0f, 51.0f, 40.0f, PxQuat(PxIdentity)), PxTransform(-40.0f, 51.0f, 40.0f, PxQuat(PxIdentity)), PxTransform(40.0f, 51.0f, -40.0f, PxQuat(PxIdentity)), PxTransform(-40.0f, 51.0f, -40.0f, PxQuat(PxIdentity)) };
//int gSpawnID = 0;

AIManager::AIManager(Broker* broker) 
	: _broker(broker)
{

}

AIManager::~AIManager() {

}

void AIManager::init() {

}

void AIManager::updateMilliseconds(double deltaTime) {

}