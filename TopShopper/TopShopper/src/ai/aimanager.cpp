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
#include "utility/utility.h"

using namespace physx;


AIManager::AIManager(Broker *broker) 
	: _broker(broker)
{
	spareChangeSpawnPoints.at(0) = PxTransform(80.0f, 2.0f, 0.0f);
	spareChangeSpawnPoints.at(1) = PxTransform(90.0f, 2.0f, 0.0f);
	spareChangeSpawnPoints.at(2) = PxTransform(100.0f, 2.0f, 0.0f);
	spareChangeSpawnPoints.at(3) = PxTransform(110.0f, 2.0f, 0.0f);
	spareChangeSpawnPoints.at(4) = PxTransform(120.0f, 2.0f, 0.0f);
	spareChangeSpawnPoints.at(5) = PxTransform(130.0f, 2.0f, 0.0f);
	spareChangeSpawnPoints.at(6) = PxTransform(140.0f, 2.0f, 0.0f);
	spareChangeSpawnPoints.at(7) = PxTransform(110.0f, 2.0f, 40.0f);
	spareChangeSpawnPoints.at(8) = PxTransform(110.0f, 2.0f, -40.0f);
	spareChangeSpawnPoints.at(9) = PxTransform(120.0f, 2.0f, 30.0f);
	spareChangeSpawnPoints.at(10) = PxTransform(120.0f, 2.0f, -30.0f);
	spareChangeSpawnPoints.at(11) = PxTransform(130.0f, 2.0f, 20.0f);
	spareChangeSpawnPoints.at(12) = PxTransform(130.0f, 2.0f, -20.0f);
	spareChangeSpawnPoints.at(13) = PxTransform(140.0f, 2.0f, 10.0f);
	spareChangeSpawnPoints.at(14) = PxTransform(140.0f, 2.0f, -10.0f);
	spareChangeSpawnPoints.at(15) = PxTransform(150.0f, 2.0f, 0.0f);

	spareChangeSpawnPoints.at(16) = PxTransform(-70.0f, 2.0f, -80.0f);
	spareChangeSpawnPoints.at(17) = PxTransform(-70.0f, 2.0f, -70.0f);
	spareChangeSpawnPoints.at(18) = PxTransform(-70.0f, 2.0f, -60.0f);
	spareChangeSpawnPoints.at(19) = PxTransform(-70.0f, 2.0f, -50.0f);
	spareChangeSpawnPoints.at(20) = PxTransform(-70.0f, 2.0f, -40.0f);
	spareChangeSpawnPoints.at(21) = PxTransform(-70.0f, 2.0f, -30.0f);
	spareChangeSpawnPoints.at(22) = PxTransform(-70.0f, 2.0f, -20.0f);
	spareChangeSpawnPoints.at(23) = PxTransform(-70.0f, 2.0f, -10.0f);
	spareChangeSpawnPoints.at(24) = PxTransform(-70.0f, 2.0f, 0.0f);
	spareChangeSpawnPoints.at(25) = PxTransform(-70.0f, 2.0f, 10.0f);
	spareChangeSpawnPoints.at(26) = PxTransform(-70.0f, 2.0f, 20.0f);
	spareChangeSpawnPoints.at(27) = PxTransform(-70.0f, 2.0f, 30.0f);
	spareChangeSpawnPoints.at(28) = PxTransform(-70.0f, 2.0f, 40.0f);
	spareChangeSpawnPoints.at(29) = PxTransform(-70.0f, 2.0f, 50.0f);
	spareChangeSpawnPoints.at(30) = PxTransform(-70.0f, 2.0f, 60.0f);
	spareChangeSpawnPoints.at(31) = PxTransform(-70.0f, 2.0f, 70.0f);

	// SAFE-GUARD...
	for (int i = 0; i < NB_SPARE_CHANGE_SPAWN_POINTS; i++) {
		spareChangeInstances.at(i) = nullptr;
		spareChangeSpawnTimers.at(i) = -1.0;
	}



	drinkSpawnPoints.at(0) = PxTransform(-59.0f, 2.0f, -23.0f);
	drinkSpawnPoints.at(1) = PxTransform(-86.0f, 2.0f, -23.0f);
	drinkSpawnPoints.at(2) = PxTransform(-156.0f, 2.0f, -23.0f);
	drinkSpawnPoints.at(3) = PxTransform(-117.0f, 2.0f, -95.0f);
	drinkSpawnPoints.at(4) = PxTransform(-50.0f, 2.0f, -74.0f);
	drinkSpawnPoints.at(5) = PxTransform(-81.0f, 2.0f, -135.0f);
	drinkSpawnPoints.at(6) = PxTransform(-29.0f, 2.0f, -110.0f);
	drinkSpawnPoints.at(7) = PxTransform(26.0f, 2.0f, -158.0f);
	drinkSpawnPoints.at(8) = PxTransform(6.0f, 2.0f, -84.0f);

	fruitSpawnPoints.at(0) = PxTransform(-59.0f, 2.0f, 23.0f);
	fruitSpawnPoints.at(1) = PxTransform(-86.0f, 2.0f, 23.0f);
	fruitSpawnPoints.at(2) = PxTransform(-156.0f, 2.0f, 23.0f);
	fruitSpawnPoints.at(3) = PxTransform(-117.0f, 2.0f, 95.0f);
	fruitSpawnPoints.at(4) = PxTransform(-50.0f, 2.0f, 74.0f);
	fruitSpawnPoints.at(5) = PxTransform(-81.0f, 2.0f, 135.0f);
	fruitSpawnPoints.at(6) = PxTransform(-29.0f, 2.0f, 110.0f);
	fruitSpawnPoints.at(7) = PxTransform(26.0f, 2.0f, 158.0f);
	fruitSpawnPoints.at(8) = PxTransform(6.0f, 2.0f, 84.0f);

	veggieSpawnPoints.at(0) = PxTransform(33.0f, 2.0f, 50.0f);
	veggieSpawnPoints.at(1) = PxTransform(33.0f, 2.0f, -50.0f);
	veggieSpawnPoints.at(2) = PxTransform(77.0f, 2.0f, 145.0f);
	veggieSpawnPoints.at(3) = PxTransform(77.0f, 2.0f, -145.0f);
	veggieSpawnPoints.at(4) = PxTransform(133.0f, 2.0f, 66.0f);
	veggieSpawnPoints.at(5) = PxTransform(133.0f, 2.0f, -66.0f);
	veggieSpawnPoints.at(6) = PxTransform(103.0f, 2.0f, 42.0f);
	veggieSpawnPoints.at(7) = PxTransform(103.0f, 2.0f, -42.0f);
	veggieSpawnPoints.at(8) = PxTransform(63.0f, 2.0f, 0.0f);


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
			removeDeletedTarget(spareChangeSpawnPoints.at(i).p);
			spareChangeInstances.at(i) = nullptr;
			spareChangeSpawnTimers.at(i) = SPARE_CHANGE_RESPAWN_TIME;
		}
	}

	// check if any open spawn points are ready to spawn a new spare change instance...
	for (int i = 0; i < NB_SPARE_CHANGE_SPAWN_POINTS; i++) {
		if (spareChangeInstances.at(i) == nullptr) {
			spareChangeSpawnTimers.at(i) -= variableDeltaTime;
			if (spareChangeSpawnTimers.at(i) <= 0.0) {
				//std::cout << "AIMANAGER.CPP | NEW SPARE CHANGE SPAWN" << std::endl;
				spareChangeInstances.at(i) = std::dynamic_pointer_cast<SpareChange>(_broker->getPhysicsManager()->instantiateEntity(EntityTypes::SPARE_CHANGE, spareChangeSpawnPoints.at(i), "SpareChangeSP"+i));
			}
		}
	}


	// GROCERY ITEM SPAWNING...

	// check if any spawn-point grocery items have been picked up in this frame (destroyed)...
	if (milkSpawnIndex != -1 && drinkInstances.at(milkSpawnIndex)->getDestroyFlag()) {
		removeDeletedTarget(drinkSpawnPoints.at(milkSpawnIndex).p);
		drinkInstances.at(milkSpawnIndex) = nullptr;
		milkSpawnIndex = -1;
		milkSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}
	if (waterSpawnIndex != -1 && drinkInstances.at(waterSpawnIndex)->getDestroyFlag()) {
		removeDeletedTarget(drinkSpawnPoints.at(waterSpawnIndex).p);
		drinkInstances.at(waterSpawnIndex) = nullptr;
		waterSpawnIndex = -1;
		waterSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}
	if (colaSpawnIndex != -1 && drinkInstances.at(colaSpawnIndex)->getDestroyFlag()) {
		removeDeletedTarget(drinkSpawnPoints.at(colaSpawnIndex).p);
		drinkInstances.at(colaSpawnIndex) = nullptr;
		colaSpawnIndex = -1;
		colaSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}

	if (appleSpawnIndex != -1 && fruitInstances.at(appleSpawnIndex)->getDestroyFlag()) {
		removeDeletedTarget(fruitSpawnPoints.at(appleSpawnIndex).p);
		fruitInstances.at(appleSpawnIndex) = nullptr;
		appleSpawnIndex = -1;
		appleSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}
	if (watermelonSpawnIndex != -1 && fruitInstances.at(watermelonSpawnIndex)->getDestroyFlag()) {
		removeDeletedTarget(fruitSpawnPoints.at(watermelonSpawnIndex).p);
		fruitInstances.at(watermelonSpawnIndex) = nullptr;
		watermelonSpawnIndex = -1;
		watermelonSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}
	if (bananaSpawnIndex != -1 && fruitInstances.at(bananaSpawnIndex)->getDestroyFlag()) {
		removeDeletedTarget(fruitSpawnPoints.at(bananaSpawnIndex).p);
		fruitInstances.at(bananaSpawnIndex) = nullptr;
		bananaSpawnIndex = -1;
		bananaSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}

	if (carrotSpawnIndex != -1 && veggieInstances.at(carrotSpawnIndex)->getDestroyFlag()) {
		removeDeletedTarget(veggieSpawnPoints.at(carrotSpawnIndex).p);
		veggieInstances.at(carrotSpawnIndex) = nullptr;
		carrotSpawnIndex = -1;
		carrotSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}
	if (eggplantSpawnIndex != -1 && veggieInstances.at(eggplantSpawnIndex)->getDestroyFlag()) {
		removeDeletedTarget(veggieSpawnPoints.at(eggplantSpawnIndex).p);
		veggieInstances.at(eggplantSpawnIndex) = nullptr;
		eggplantSpawnIndex = -1;
		eggplantSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}
	if (broccoliSpawnIndex != -1 && veggieInstances.at(broccoliSpawnIndex)->getDestroyFlag()) {
		removeDeletedTarget(veggieSpawnPoints.at(broccoliSpawnIndex).p);
		veggieInstances.at(broccoliSpawnIndex) = nullptr;
		broccoliSpawnIndex = -1;
		broccoliSpawnTimer = GROCERY_ITEM_RESPAWN_TIME;
	}


	// check each missing grocery item if they are ready to be spawned in...
	if (milkSpawnIndex == -1) {
		milkSpawnTimer -= variableDeltaTime;
		if (milkSpawnTimer <= 0.0) {
			milkSpawnIndex = getNextDrinkSpawnIndex();
			//std::cout << "AIMANAGER.CPP | NEW MILK SPAWN" << std::endl;
			drinkInstances.at(milkSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::MILK, drinkSpawnPoints.at(milkSpawnIndex), "MilkSP");
		}
	}
	if (waterSpawnIndex == -1) {
		waterSpawnTimer -= variableDeltaTime;
		if (waterSpawnTimer <= 0.0) {
			waterSpawnIndex = getNextDrinkSpawnIndex();
			//std::cout << "AIMANAGER.CPP | NEW WATER SPAWN" << std::endl;
			drinkInstances.at(waterSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::WATER, drinkSpawnPoints.at(waterSpawnIndex), "WaterSP");
		}
	}
	if (colaSpawnIndex == -1) {
		colaSpawnTimer -= variableDeltaTime;
		if (colaSpawnTimer <= 0.0) {
			colaSpawnIndex = getNextDrinkSpawnIndex();
			//std::cout << "AIMANAGER.CPP | NEW COLA SPAWN" << std::endl;
			drinkInstances.at(colaSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::COLA, drinkSpawnPoints.at(colaSpawnIndex), "ColaSP");
		}
	}

	if (appleSpawnIndex == -1) {
		appleSpawnTimer -= variableDeltaTime;
		if (appleSpawnTimer <= 0.0) {
			appleSpawnIndex = getNextFruitSpawnIndex();
			//std::cout << "AIMANAGER.CPP | NEW APPLE SPAWN" << std::endl;
			fruitInstances.at(appleSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::APPLE, fruitSpawnPoints.at(appleSpawnIndex), "AppleSP");
		}
	}
	if (watermelonSpawnIndex == -1) {
		watermelonSpawnTimer -= variableDeltaTime;
		if (watermelonSpawnTimer <= 0.0) {
			watermelonSpawnIndex = getNextFruitSpawnIndex();
			//std::cout << "AIMANAGER.CPP | NEW WATERMELON SPAWN" << std::endl;
			fruitInstances.at(watermelonSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::WATERMELON, fruitSpawnPoints.at(watermelonSpawnIndex), "WatermelonSP");
		}
	}
	if (bananaSpawnIndex == -1) {
		bananaSpawnTimer -= variableDeltaTime;
		if (bananaSpawnTimer <= 0.0) {
			bananaSpawnIndex = getNextFruitSpawnIndex();
			//std::cout << "AIMANAGER.CPP | NEW BANANA SPAWN" << std::endl;
			fruitInstances.at(bananaSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::BANANA, fruitSpawnPoints.at(bananaSpawnIndex), "BananaSP");
		}
	}

	if (carrotSpawnIndex == -1) {
		carrotSpawnTimer -= variableDeltaTime;
		if (carrotSpawnTimer <= 0.0) {
			carrotSpawnIndex = getNextVeggieSpawnIndex();
			//std::cout << "AIMANAGER.CPP | NEW CARROT SPAWN" << std::endl;
			veggieInstances.at(carrotSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::CARROT, veggieSpawnPoints.at(carrotSpawnIndex), "CarrotSP");
		}
	}
	if (eggplantSpawnIndex == -1) {
		eggplantSpawnTimer -= variableDeltaTime;
		if (eggplantSpawnTimer <= 0.0) {
			eggplantSpawnIndex = getNextVeggieSpawnIndex();
			//std::cout << "AIMANAGER.CPP | NEW EGGPLANT SPAWN" << std::endl;
			veggieInstances.at(eggplantSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::EGGPLANT, veggieSpawnPoints.at(eggplantSpawnIndex), "EggplantSP");
		}
	}
	if (broccoliSpawnIndex == -1) {
		broccoliSpawnTimer -= variableDeltaTime;
		if (broccoliSpawnTimer <= 0.0) {
			broccoliSpawnIndex = getNextVeggieSpawnIndex();
			//std::cout << "AIMANAGER.CPP | NEW BROCCOLI SPAWN" << std::endl;
			veggieInstances.at(broccoliSpawnIndex) = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::BROCCOLI, veggieSpawnPoints.at(broccoliSpawnIndex), "BroccoliSP");
		}
	}


	setNewAITargets();



	// update match timer...
	_matchTimer -= variableDeltaTime;
	if (_matchTimer <= 0.0) {
		_matchTimer = 0.0; // clamp at 0 so rendering doesnt screw up
		_broker->_isPaused = true; // FOR NOW, we pause the game when match is over
		//std::cout << "MATCH END" << std::endl;
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





void AIManager::setNewAITargets() {
	const std::vector<std::shared_ptr<ShoppingCartPlayer>> &players = Broker::getInstance()->getPhysicsManager()->getActiveScene()->getAllShoppingCartPlayers();
	for (std::shared_ptr<ShoppingCartPlayer> player : players) {
		std::shared_ptr<PlayerScript> playerScript = std::static_pointer_cast<PlayerScript>(player->getComponent(ComponentTypes::PLAYER_SCRIPT));
		if (playerScript->_playerType == PlayerScript::BOT) {
			if (playerScript->_targets.size() == 0) {
				// find a new final target (pickup on your list for now)
				PxVec3 playerPos = player->_actor->is<PxRigidDynamic>()->getGlobalPose().p;

				PxVec3 closestTarget;
				bool targetFound = false;
				float smallestSeparation = FLT_MAX;
				for (int i = 0; i < 3; i++) {
					if (!playerScript->_shoppingList_Flags.at(i)) { // for each pickup on list that bot doesnt have yet...
						PxVec3 candTarget;

						switch (playerScript->_shoppingList_Types.at(i)) {
						case EntityTypes::MILK:
							if (milkSpawnIndex != -1) candTarget = drinkSpawnPoints.at(milkSpawnIndex).p;
							else continue;
							break;
						case EntityTypes::WATER:
							if (waterSpawnIndex != -1) candTarget = drinkSpawnPoints.at(waterSpawnIndex).p;
							else continue;
							break;
						case EntityTypes::COLA:
							if (colaSpawnIndex != -1) candTarget = drinkSpawnPoints.at(colaSpawnIndex).p;
							else continue;
							break;
						case EntityTypes::APPLE:
							if (appleSpawnIndex != -1) candTarget = fruitSpawnPoints.at(appleSpawnIndex).p;
							else continue;
							break;
						case EntityTypes::WATERMELON:
							if (watermelonSpawnIndex != -1) candTarget = fruitSpawnPoints.at(watermelonSpawnIndex).p;
							else continue;
							break;
						case EntityTypes::BANANA:
							if (bananaSpawnIndex != -1) candTarget = fruitSpawnPoints.at(bananaSpawnIndex).p;
							else continue;
							break;
						case EntityTypes::CARROT:
							if (carrotSpawnIndex != -1) candTarget = veggieSpawnPoints.at(carrotSpawnIndex).p;
							else continue;
							break;
						case EntityTypes::EGGPLANT:
							if (eggplantSpawnIndex != -1) candTarget = veggieSpawnPoints.at(eggplantSpawnIndex).p;
							else continue;
							break;
						case EntityTypes::BROCCOLI:
							if (broccoliSpawnIndex != -1) candTarget = veggieSpawnPoints.at(broccoliSpawnIndex).p;
							else continue;
							break;
						}

						targetFound = true;

						float separation = (candTarget - playerPos).magnitude();
						if (separation < smallestSeparation) {
							smallestSeparation = separation;
							closestTarget = candTarget;
						}

					}
				}

				if (targetFound) {
					playerScript->_targets.push_back(closestTarget);
					//std::cout << closestTarget.x << " " << closestTarget.y << " " << closestTarget.z << std::endl;
				}

			}
		}
	}
}






void AIManager::removeDeletedTarget(physx::PxVec3 deletedTarget) {
	const std::vector<std::shared_ptr<ShoppingCartPlayer>> &players = Broker::getInstance()->getPhysicsManager()->getActiveScene()->getAllShoppingCartPlayers();
	for (std::shared_ptr<ShoppingCartPlayer> player : players) {
		std::shared_ptr<PlayerScript> playerScript = std::static_pointer_cast<PlayerScript>(player->getComponent(ComponentTypes::PLAYER_SCRIPT));
		if (playerScript->_playerType == PlayerScript::BOT) {
			for (int i = 0; i < playerScript->_targets.size(); i++) {
				if (isApproxEqual(playerScript->_targets.at(i), deletedTarget)) {
					//std::cout << "target removed" << std::endl;
					playerScript->_targets.erase(playerScript->_targets.begin() + i);
					break;
				}
			}
		}
	}
}





std::string AIManager::getMatchTimePrettyFormat() {
	int timeCeiling = (int) ceil(_matchTimer);
	int minutes = timeCeiling / 60;
	int seconds = timeCeiling % 60;

	std::string prettyTime = "";
	if (minutes < 10) prettyTime += "0";
	prettyTime += std::to_string(minutes);
	prettyTime += ":";
	if (seconds < 10) prettyTime += "0";
	prettyTime += std::to_string(seconds);

	return prettyTime;
}
