#include "aimanager.h"
#include "core/broker.h"
#include "PxPhysicsAPI.h"
#include "objects/entity.h"
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
#include <cstdlib>

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

	spareChangeSpawnPoints.at(26) = PxTransform(0.0f, 2.0f, 80.0f);

	// SAFE-GUARD...
	for (int i = 0; i < NB_SPARE_CHANGE_SPAWN_POINTS; i++) {
		spareChangeInstances.at(i) = nullptr;
		spareChangeSpawnTimers.at(i) = -1.0;
	}



	drinkSpawnPoints.at(0) = PxTransform(0.0f, 2.0f, 75.0f);
	drinkSpawnPoints.at(1) = PxTransform(0.0f, 2.0f, 70.0f);
	drinkSpawnPoints.at(2) = PxTransform(0.0f, 2.0f, 65.0f);
	drinkSpawnPoints.at(3) = PxTransform(0.0f, 2.0f, 60.0f);
	drinkSpawnPoints.at(4) = PxTransform(0.0f, 2.0f, 55.0f);
	drinkSpawnPoints.at(5) = PxTransform(0.0f, 2.0f, 50.0f);
	drinkSpawnPoints.at(6) = PxTransform(0.0f, 2.0f, 45.0f);
	drinkSpawnPoints.at(7) = PxTransform(0.0f, 2.0f, 40.0f);
	drinkSpawnPoints.at(8) = PxTransform(0.0f, 2.0f, 35.0f);

	fruitSpawnPoints.at(0) = PxTransform(0.0f, 2.0f, -75.0f);
	fruitSpawnPoints.at(1) = PxTransform(0.0f, 2.0f, -70.0f);
	fruitSpawnPoints.at(2) = PxTransform(0.0f, 2.0f, -65.0f);
	fruitSpawnPoints.at(3) = PxTransform(0.0f, 2.0f, -60.0f);
	fruitSpawnPoints.at(4) = PxTransform(0.0f, 2.0f, -55.0f);
	fruitSpawnPoints.at(5) = PxTransform(0.0f, 2.0f, -50.0f);
	fruitSpawnPoints.at(6) = PxTransform(0.0f, 2.0f, -45.0f);
	fruitSpawnPoints.at(7) = PxTransform(0.0f, 2.0f, -40.0f);
	fruitSpawnPoints.at(8) = PxTransform(0.0f, 2.0f, -35.0f);

	veggieSpawnPoints.at(0) = PxTransform(50.0f, 2.0f, 50.0f);
	veggieSpawnPoints.at(1) = PxTransform(45.0f, 2.0f, 45.0f);
	veggieSpawnPoints.at(2) = PxTransform(40.0f, 2.0f, 40.0f);
	veggieSpawnPoints.at(3) = PxTransform(35.0f, 2.0f, 35.0f);
	veggieSpawnPoints.at(4) = PxTransform(30.0f, 2.0f, 30.0f);
	veggieSpawnPoints.at(5) = PxTransform(35.0f, 2.0f, 30.0f);
	veggieSpawnPoints.at(6) = PxTransform(30.0f, 2.0f, 35.0f);
	veggieSpawnPoints.at(7) = PxTransform(40.0f, 2.0f, 35.0f);
	veggieSpawnPoints.at(8) = PxTransform(35.0f, 2.0f, 40.0f);


	for (int i = 0; i < NB_DRINK_SPAWN_POINTS; i++) {
		drinkInstances.at(i) = nullptr;
	}

	for (int i = 0; i < NB_FRUIT_SPAWN_POINTS; i++) {
		fruitInstances.at(i) = nullptr;
	}

	for (int i = 0; i < NB_VEGGIE_SPAWN_POINTS; i++) {
		veggieInstances.at(i) = nullptr;
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

	// SPARE CHANGE SPAWNING...
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


	// GROCERY ITEM SPAWNING...

	// check if any spawn-point grocery items have been picked up in this frame (destroyed)...
	if (milkSpawnIndex != -1 && drinkInstances.at(milkSpawnIndex)->getDestroyFlag()) {
		drinkInstances.at(milkSpawnIndex) = nullptr;
		milkSpawnIndex = -1;
		milkSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}
	if (waterSpawnIndex != -1 && drinkInstances.at(waterSpawnIndex)->getDestroyFlag()) {
		drinkInstances.at(waterSpawnIndex) = nullptr;
		waterSpawnIndex = -1;
		waterSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}
	if (colaSpawnIndex != -1 && drinkInstances.at(colaSpawnIndex)->getDestroyFlag()) {
		drinkInstances.at(colaSpawnIndex) = nullptr;
		colaSpawnIndex = -1;
		colaSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}

	if (appleSpawnIndex != -1 && fruitInstances.at(appleSpawnIndex)->getDestroyFlag()) {
		fruitInstances.at(appleSpawnIndex) = nullptr;
		appleSpawnIndex = -1;
		appleSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}
	if (watermelonSpawnIndex != -1 && fruitInstances.at(watermelonSpawnIndex)->getDestroyFlag()) {
		fruitInstances.at(watermelonSpawnIndex) = nullptr;
		watermelonSpawnIndex = -1;
		watermelonSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}
	if (bananaSpawnIndex != -1 && fruitInstances.at(bananaSpawnIndex)->getDestroyFlag()) {
		fruitInstances.at(bananaSpawnIndex) = nullptr;
		bananaSpawnIndex = -1;
		bananaSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}

	if (carrotSpawnIndex != -1 && veggieInstances.at(carrotSpawnIndex)->getDestroyFlag()) {
		veggieInstances.at(carrotSpawnIndex) = nullptr;
		carrotSpawnIndex = -1;
		carrotSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}
	if (eggplantSpawnIndex != -1 && veggieInstances.at(eggplantSpawnIndex)->getDestroyFlag()) {
		veggieInstances.at(eggplantSpawnIndex) = nullptr;
		eggplantSpawnIndex = -1;
		eggplantSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}
	if (broccoliSpawnIndex != -1 && veggieInstances.at(broccoliSpawnIndex)->getDestroyFlag()) {
		veggieInstances.at(broccoliSpawnIndex) = nullptr;
		broccoliSpawnIndex = -1;
		broccoliSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}


	// check each missing grocery item if they are ready to be spawned in...
	if (milkSpawnIndex == -1) {
		milkSpawnTimer -= variableDeltaTime;
		if (milkSpawnTimer <= 0.0) {
			milkSpawnIndex = getNextDrinkSpawnIndex();
			std::cout << "AIMANAGER.CPP | NEW MILK SPAWN" << std::endl;
			drinkInstances.at(milkSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::MILK, drinkSpawnPoints.at(milkSpawnIndex), "MilkSP");
		}
	}
	if (waterSpawnIndex == -1) {
		waterSpawnTimer -= variableDeltaTime;
		if (waterSpawnTimer <= 0.0) {
			waterSpawnIndex = getNextDrinkSpawnIndex();
			std::cout << "AIMANAGER.CPP | NEW WATER SPAWN" << std::endl;
			drinkInstances.at(waterSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::WATER, drinkSpawnPoints.at(waterSpawnIndex), "WaterSP");
		}
	}
	if (colaSpawnIndex == -1) {
		colaSpawnTimer -= variableDeltaTime;
		if (colaSpawnTimer <= 0.0) {
			colaSpawnIndex = getNextDrinkSpawnIndex();
			std::cout << "AIMANAGER.CPP | NEW COLA SPAWN" << std::endl;
			drinkInstances.at(colaSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::COLA, drinkSpawnPoints.at(colaSpawnIndex), "ColaSP");
		}
	}

	if (appleSpawnIndex == -1) {
		appleSpawnTimer -= variableDeltaTime;
		if (appleSpawnTimer <= 0.0) {
			appleSpawnIndex = getNextFruitSpawnIndex();
			std::cout << "AIMANAGER.CPP | NEW APPLE SPAWN" << std::endl;
			fruitInstances.at(appleSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::APPLE, fruitSpawnPoints.at(appleSpawnIndex), "AppleSP");
		}
	}
	if (watermelonSpawnIndex == -1) {
		watermelonSpawnTimer -= variableDeltaTime;
		if (watermelonSpawnTimer <= 0.0) {
			watermelonSpawnIndex = getNextFruitSpawnIndex();
			std::cout << "AIMANAGER.CPP | NEW WATERMELON SPAWN" << std::endl;
			fruitInstances.at(watermelonSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::WATERMELON, fruitSpawnPoints.at(watermelonSpawnIndex), "WatermelonSP");
		}
	}
	if (bananaSpawnIndex == -1) {
		bananaSpawnTimer -= variableDeltaTime;
		if (bananaSpawnTimer <= 0.0) {
			bananaSpawnIndex = getNextFruitSpawnIndex();
			std::cout << "AIMANAGER.CPP | NEW BANANA SPAWN" << std::endl;
			fruitInstances.at(bananaSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::BANANA, fruitSpawnPoints.at(bananaSpawnIndex), "BananaSP");
		}
	}

	if (carrotSpawnIndex == -1) {
		carrotSpawnTimer -= variableDeltaTime;
		if (carrotSpawnTimer <= 0.0) {
			carrotSpawnIndex = getNextVeggieSpawnIndex();
			std::cout << "AIMANAGER.CPP | NEW CARROT SPAWN" << std::endl;
			veggieInstances.at(carrotSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::CARROT, veggieSpawnPoints.at(carrotSpawnIndex), "CarrotSP");
		}
	}
	if (eggplantSpawnIndex == -1) {
		eggplantSpawnTimer -= variableDeltaTime;
		if (eggplantSpawnTimer <= 0.0) {
			eggplantSpawnIndex = getNextVeggieSpawnIndex();
			std::cout << "AIMANAGER.CPP | NEW EGGPLANT SPAWN" << std::endl;
			veggieInstances.at(eggplantSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::EGGPLANT, veggieSpawnPoints.at(eggplantSpawnIndex), "EggplantSP");
		}
	}
	if (broccoliSpawnIndex == -1) {
		broccoliSpawnTimer -= variableDeltaTime;
		if (broccoliSpawnTimer <= 0.0) {
			broccoliSpawnIndex = getNextVeggieSpawnIndex();
			std::cout << "AIMANAGER.CPP | NEW BROCCOLI SPAWN" << std::endl;
			veggieInstances.at(broccoliSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::BROCCOLI, veggieSpawnPoints.at(broccoliSpawnIndex), "BroccoliSP");
		}
	}

}



int AIManager::getNextDrinkSpawnIndex() {
	std::vector<int> openIndices;
	for (int i = 0; i < NB_DRINK_SPAWN_POINTS; i++) {
		if (drinkInstances.at(i) == nullptr) openIndices.push_back(i);
	}
	int rng = rand() % openIndices.size();
	return openIndices.at(rng);
}

int AIManager::getNextFruitSpawnIndex() {
	std::vector<int> openIndices;
	for (int i = 0; i < NB_FRUIT_SPAWN_POINTS; i++) {
		if (fruitInstances.at(i) == nullptr) openIndices.push_back(i);
	}
	int rng = rand() % openIndices.size();
	return openIndices.at(rng);
}

int AIManager::getNextVeggieSpawnIndex() {
	std::vector<int> openIndices;
	for (int i = 0; i < NB_VEGGIE_SPAWN_POINTS; i++) {
		if (veggieInstances.at(i) == nullptr) openIndices.push_back(i);
	}
	int rng = rand() % openIndices.size();
	return openIndices.at(rng);
}