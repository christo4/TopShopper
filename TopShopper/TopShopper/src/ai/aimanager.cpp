#include "aimanager.h"
#include "core/broker.h"
#include "PxPhysicsAPI.h"
#include "objects/sparechange.h"


using namespace physx;


AIManager::AIManager(Broker *broker) 
	: _broker(broker)
{
	spareChangeSpawnPoints.at(0) = PxTransform(-60.0f, 2.0f, -60.0f);
	spareChangeSpawnPoints.at(1) = PxTransform(-70.0f, 2.0f, -50.0f);
	spareChangeSpawnPoints.at(2) = PxTransform(-80.0f, 2.0f, -40.0f);
	spareChangeSpawnPoints.at(3) = PxTransform(-70.0f, 2.0f, -30.0f);
	spareChangeSpawnPoints.at(4) = PxTransform(-60.0f, 2.0f, -20.0f);
	spareChangeSpawnPoints.at(5) = PxTransform(-50.0f, 2.0f, -10.0f);
	spareChangeSpawnPoints.at(6) = PxTransform(-40.0f, 2.0f, 0.0f);
	spareChangeSpawnPoints.at(7) = PxTransform(-50.0f, 2.0f, 10.0f);
	spareChangeSpawnPoints.at(8) = PxTransform(-60.0f, 2.0f, 20.0f);
	spareChangeSpawnPoints.at(9) = PxTransform(-70.0f, 2.0f, 30.0f);
	spareChangeSpawnPoints.at(10) = PxTransform(-80.0f, 2.0f, 40.0f);
	spareChangeSpawnPoints.at(11) = PxTransform(-70.0f, 2.0f, 50.0f);
	spareChangeSpawnPoints.at(12) = PxTransform(-60.0f, 2.0f, 60.0f);

	spareChangeSpawnPoints.at(13) = PxTransform(60.0f, 2.0f, -60.0f);
	spareChangeSpawnPoints.at(14) = PxTransform(70.0f, 2.0f, -50.0f);
	spareChangeSpawnPoints.at(15) = PxTransform(80.0f, 2.0f, -40.0f);
	spareChangeSpawnPoints.at(16) = PxTransform(70.0f, 2.0f, -30.0f);
	spareChangeSpawnPoints.at(17) = PxTransform(60.0f, 2.0f, -20.0f);
	spareChangeSpawnPoints.at(18) = PxTransform(50.0f, 2.0f, -10.0f);
	spareChangeSpawnPoints.at(19) = PxTransform(40.0f, 2.0f, 0.0f);
	spareChangeSpawnPoints.at(20) = PxTransform(50.0f, 2.0f, 10.0f);
	spareChangeSpawnPoints.at(21) = PxTransform(60.0f, 2.0f, 20.0f);
	spareChangeSpawnPoints.at(22) = PxTransform(70.0f, 2.0f, 30.0f);
	spareChangeSpawnPoints.at(23) = PxTransform(80.0f, 2.0f, 40.0f);
	spareChangeSpawnPoints.at(24) = PxTransform(70.0f, 2.0f, 50.0f);
	spareChangeSpawnPoints.at(25) = PxTransform(60.0f, 2.0f, 60.0f);

	spareChangeSpawnPoints.at(26) = PxTransform(0.0f, 2.0f, 60.0f);

	// SAFE-GUARD...
	for (int i = 0; i < NB_SPARE_CHANGE_SPAWN_POINTS; i++) {
		spareChangeInstances.at(i) = nullptr;
		spareChangeSpawnTimers.at(i) = -1.0;
	}
}

AIManager::~AIManager() {

}

void AIManager::init() {

}

void AIManager::updateSeconds(double variableDeltaTime) {
	// call UPDATE() for all behaviour scripts...
	for (std::shared_ptr<Entity> &entity : _broker->getPhysicsManager()->getActiveScene()->_entities) {
		std::shared_ptr<Component> comp = entity->getComponent(ComponentTypes::BEHAVIOUR_SCRIPT);
		if (comp != nullptr) {
			std::shared_ptr<BehaviourScript> script = std::static_pointer_cast<BehaviourScript>(comp);
			script->update(variableDeltaTime);
		}
	}

	// check if any spawn-point spare change have been picked up in this frame (destroyed)...
	for (int i = 0; i < NB_SPARE_CHANGE_SPAWN_POINTS; i++) {
		if (spareChangeInstances.at(i) != nullptr && spareChangeInstances.at(i)->getDestroyFlag()) {
			spareChangeInstances.at(i) = nullptr;
			spareChangeSpawnTimers.at(i) = SPARE_CHANGE_RESPAWN_TIME;
		}
	}

	// check if any open spawn points are ready to spawn a new spare change instance...
	for (int i = 0; i < NB_SPARE_CHANGE_SPAWN_POINTS; i++) {
		if (spareChangeInstances.at(i) == nullptr) {
			spareChangeSpawnTimers.at(i) -= variableDeltaTime;
			if (spareChangeSpawnTimers.at(i) <= 0.0) {
				std::cout << "AIMANAGER.CPP | NEW SPARE CHANGE SPAWN" << std::endl;
				spareChangeInstances.at(i) = std::dynamic_pointer_cast<SpareChange>(_broker->getPhysicsManager()->instantiateEntity(EntityTypes::SPARE_CHANGE, spareChangeSpawnPoints.at(i), "SpareChangeSP"+i));
			}
		}
	}
}