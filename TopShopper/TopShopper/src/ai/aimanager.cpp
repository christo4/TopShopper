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
#include "objects/shoppingcartplayer.h"
#include "vehicle/VehicleShoppingCart.h"

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



	drinkSpawnPoints.at(0) = PxTransform(-48.0f, 2.0f, -94.0f);
	drinkSpawnPoints.at(1) = PxTransform(-132.0f, 2.0f, -94.0f);
	drinkSpawnPoints.at(2) = PxTransform(-13.0f, 2.0f, -161.0f);
	drinkSpawnPoints.at(3) = PxTransform(-109.0f, 2.0f, -200.0f);
	drinkSpawnPoints.at(4) = PxTransform(-220.0f, 2.0f, -82.0f);
	drinkSpawnPoints.at(5) = PxTransform(43.0f, 2.0f, -237.0f);

	fruitSpawnPoints.at(0) = PxTransform(-45.0f, 2.0f, 94.0f);
	fruitSpawnPoints.at(1) = PxTransform(-86.0f, 2.0f, 148.0f);
	fruitSpawnPoints.at(2) = PxTransform(-155.0f, 2.0f, 123.0f);
	fruitSpawnPoints.at(3) = PxTransform(-220.0f, 2.0f, 57.0f);
	fruitSpawnPoints.at(4) = PxTransform(-35.0f, 2.0f, 194.0f);
	fruitSpawnPoints.at(5) = PxTransform(55.0f, 2.0f, 217.0f);

	veggieSpawnPoints.at(0) = PxTransform(91.0f, 2.0f, 0.0f);
	veggieSpawnPoints.at(1) = PxTransform(161.0f, 2.0f, 40.0f);
	veggieSpawnPoints.at(2) = PxTransform(161.0f, 2.0f, -40.0f);
	veggieSpawnPoints.at(3) = PxTransform(238.0f, 2.0f, 0.0f);
	veggieSpawnPoints.at(4) = PxTransform(158.0f, 2.0f, -161.0f);
	veggieSpawnPoints.at(5) = PxTransform(158.0f, 2.0f, 161.0f);

	
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
	std::vector<std::shared_ptr<Entity>> entitiesCopy = _broker->getPhysicsManager()->getActiveScene()->_entities;
	for (std::shared_ptr<Entity> &entity : entitiesCopy) {
		std::shared_ptr<Component> comp = entity->getComponent(ComponentTypes::BEHAVIOUR_SCRIPT);
		if (comp != nullptr) {
			std::shared_ptr<BehaviourScript> script = std::static_pointer_cast<BehaviourScript>(comp);
			script->update(variableDeltaTime);
		}
	}


	// FIND THE LOCATION OF EACH ITEM (COOKIE, MYSTERYBAG, SPARECHANGE, 9 LIST ITEMS) - EITHER IN WORLD OR ON A PLAYER...
	updateLocations();

	// HANDLE NEW SPAWNING...

	if (_cookieCanSpawn && _startingCookie == nullptr) {
		std::shared_ptr<Cookie> cookie = std::dynamic_pointer_cast<Cookie>(_broker->getPhysicsManager()->instantiateEntity(EntityTypes::COOKIE, _startingCookieSpawnPoint, "startingCookie"));
		_startingCookie = cookie;
		_cookieLocations.push_back(ItemLocation(_startingCookieSpawnPoint.p, true));

	}

	if (_mysteryBagCanSpawn && _mysteryBag == nullptr) {
		_mysteryBagSpawnTimer -= variableDeltaTime;
		if (_mysteryBagSpawnTimer <= 0.0) {
			std::shared_ptr<MysteryBag> mysteryBag = std::dynamic_pointer_cast<MysteryBag>(_broker->getPhysicsManager()->instantiateEntity(EntityTypes::MYSTERY_BAG, _mysteryBagSpawnPoint, "mysteryBag"));
			_mysteryBag = mysteryBag;
			_mysteryBagLocations.push_back(ItemLocation(_mysteryBagSpawnPoint.p, true));
		}
	}


	// check if any open spawn points are ready to spawn a new spare change instance...
	for (int i = 0; i < NB_SPARE_CHANGE_SPAWN_POINTS; i++) {
		if (spareChangeInstances.at(i) == nullptr) {
			spareChangeSpawnTimers.at(i) -= variableDeltaTime;
			if (spareChangeSpawnTimers.at(i) <= 0.0) {
				std::shared_ptr<SpareChange> spareChange = std::dynamic_pointer_cast<SpareChange>(_broker->getPhysicsManager()->instantiateEntity(EntityTypes::SPARE_CHANGE, spareChangeSpawnPoints.at(i), "SpareChangeSP" + i));
				spareChangeInstances.at(i) = spareChange;
				_spareChangeLocations.push_back(ItemLocation(spareChangeSpawnPoints.at(i).p, true));
			}
		}
	}


	// NOTE: I'm not worrying about the entity names since duplicates dont matter in our game
	while (_milkLocations.size() < MAX_NB_INSTANCES_OF_EACH_GROCERY_ITEM) {
		// spawn
		int spawnIndex = getNextDrinkSpawnIndex();
		if (-1 == spawnIndex) break; // fail the spawning for this frame

		std::shared_ptr<Entity> milk = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::MILK, drinkSpawnPoints.at(spawnIndex), "MilkSP"); 
		drinkInstances.at(spawnIndex) = milk;
		_milkLocations.push_back(ItemLocation(drinkSpawnPoints.at(spawnIndex).p, true));

	}
	while (_waterLocations.size() < MAX_NB_INSTANCES_OF_EACH_GROCERY_ITEM) {
		// spawn
		int spawnIndex = getNextDrinkSpawnIndex();
		if (-1 == spawnIndex) break; // fail the spawning for this frame
		
		std::shared_ptr<Entity> water = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::WATER, drinkSpawnPoints.at(spawnIndex), "WaterSP");
		drinkInstances.at(spawnIndex) = water;
		_waterLocations.push_back(ItemLocation(drinkSpawnPoints.at(spawnIndex).p, true));
	}
	while (_colaLocations.size() < MAX_NB_INSTANCES_OF_EACH_GROCERY_ITEM) {
		// spawn
		int spawnIndex = getNextDrinkSpawnIndex();
		if (-1 == spawnIndex) break; // fail the spawning for this frame

		std::shared_ptr<Entity> cola = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::COLA, drinkSpawnPoints.at(spawnIndex), "ColaSP");
		drinkInstances.at(spawnIndex) = cola;
		_colaLocations.push_back(ItemLocation(drinkSpawnPoints.at(spawnIndex).p, true));
	}
	while (_appleLocations.size() < MAX_NB_INSTANCES_OF_EACH_GROCERY_ITEM) {
		// spawn
		int spawnIndex = getNextFruitSpawnIndex();
		if (-1 == spawnIndex) break; // fail the spawning for this frame

		std::shared_ptr<Entity> apple = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::APPLE, fruitSpawnPoints.at(spawnIndex), "AppleSP");
		fruitInstances.at(spawnIndex) = apple;
		_appleLocations.push_back(ItemLocation(fruitSpawnPoints.at(spawnIndex).p, true));
	}
	while (_watermelonLocations.size() < MAX_NB_INSTANCES_OF_EACH_GROCERY_ITEM) {
		// spawn
		int spawnIndex = getNextFruitSpawnIndex();
		if (-1 == spawnIndex) break; // fail the spawning for this frame

		std::shared_ptr<Entity> watermelon = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::WATERMELON, fruitSpawnPoints.at(spawnIndex), "WatermelonSP");
		fruitInstances.at(spawnIndex) = watermelon;
		_watermelonLocations.push_back(ItemLocation(fruitSpawnPoints.at(spawnIndex).p, true));
	}
	while (_bananaLocations.size() < MAX_NB_INSTANCES_OF_EACH_GROCERY_ITEM) {
		// spawn
		int spawnIndex = getNextFruitSpawnIndex();
		if (-1 == spawnIndex) break; // fail the spawning for this frame

		std::shared_ptr<Entity> banana = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::BANANA, fruitSpawnPoints.at(spawnIndex), "BananaSP");
		fruitInstances.at(spawnIndex) = banana;
		_bananaLocations.push_back(ItemLocation(fruitSpawnPoints.at(spawnIndex).p, true));
	}
	while (_carrotLocations.size() < MAX_NB_INSTANCES_OF_EACH_GROCERY_ITEM) {
		// spawn
		int spawnIndex = getNextVeggieSpawnIndex();
		if (-1 == spawnIndex) break; // fail the spawning for this frame

		std::shared_ptr<Entity> carrot = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::CARROT, veggieSpawnPoints.at(spawnIndex), "CarrotSP");
		veggieInstances.at(spawnIndex) = carrot;
		_carrotLocations.push_back(ItemLocation(veggieSpawnPoints.at(spawnIndex).p, true));
	}
	while (_eggplantLocations.size() < MAX_NB_INSTANCES_OF_EACH_GROCERY_ITEM) {
		// spawn
		int spawnIndex = getNextVeggieSpawnIndex();
		if (-1 == spawnIndex) break; // fail the spawning for this frame

		std::shared_ptr<Entity> eggplant = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::EGGPLANT, veggieSpawnPoints.at(spawnIndex), "EggplantSP");
		veggieInstances.at(spawnIndex) = eggplant;
		_eggplantLocations.push_back(ItemLocation(veggieSpawnPoints.at(spawnIndex).p, true));
	}
	while (_broccoliLocations.size() < MAX_NB_INSTANCES_OF_EACH_GROCERY_ITEM) {
		// spawn
		int spawnIndex = getNextVeggieSpawnIndex();
		if (-1 == spawnIndex) break; // fail the spawning for this frame

		std::shared_ptr<Entity> broccoli = _broker->getPhysicsManager()->instantiateEntity(EntityTypes::BROCCOLI, veggieSpawnPoints.at(spawnIndex), "BroccoliSP");
		veggieInstances.at(spawnIndex) = broccoli;
		_broccoliLocations.push_back(ItemLocation(veggieSpawnPoints.at(spawnIndex).p, true));
	}


	// CLEANUP SPAWN POINT INFO OF DESTROYED ITEMS (FOR NEXT FRAME)...

	if (_startingCookie != nullptr && _startingCookie->getDestroyFlag()) {
		_startingCookie = nullptr;
		_cookieCanSpawn = false;
		_mysteryBagCanSpawn = true; // mystery bag can now spawn after this 1st cookie has been picked up
	}


	if (_mysteryBag != nullptr && _mysteryBag->getDestroyFlag()) {
		_mysteryBag = nullptr;
		_mysteryBagSpawnTimer = ((rand() % 31) + 30); // 30-60 second range
	}


	for (int i = 0; i < NB_SPARE_CHANGE_SPAWN_POINTS; i++) {
		if (spareChangeInstances.at(i) != nullptr && spareChangeInstances.at(i)->getDestroyFlag()) {
			spareChangeInstances.at(i) = nullptr;
			spareChangeSpawnTimers.at(i) = SPARE_CHANGE_RESPAWN_TIME;
		}
	}


	for (std::shared_ptr<Entity> &e : drinkInstances) {
		if (nullptr != e && e->getDestroyFlag()) {
			e = nullptr;
		}
	}

	for (std::shared_ptr<Entity> &e : fruitInstances) {
		if (nullptr != e && e->getDestroyFlag()) {
			e = nullptr;
		}
	}

	for (std::shared_ptr<Entity> &e : veggieInstances) {
		if (nullptr != e && e->getDestroyFlag()) {
			e = nullptr;
		}
	}


	// SET A NEW TARGET LOCATION FOR EACH AI BOT TO TRAVEL TO...
	setNewAITargets();


	// update match timer...
	_matchTimer -= variableDeltaTime;
	if (_matchTimer <= 0.0) {
		_matchTimer = 0.0; // clamp at 0 so rendering doesnt screw up
		_broker->_isEnd = true; // FOR NOW, we pause the game when match is over
	}
}


void AIManager::updateLocations() {
	_cookieLocations.clear();
	_mysteryBagLocations.clear();
	_spareChangeLocations.clear();
	_milkLocations.clear();
	_waterLocations.clear();
	_colaLocations.clear();
	_appleLocations.clear();
	_watermelonLocations.clear();
	_bananaLocations.clear();
	_carrotLocations.clear();
	_eggplantLocations.clear();
	_broccoliLocations.clear();

	for (const std::shared_ptr<Entity> &entity : _broker->getPhysicsManager()->getActiveScene()->_entities) {
		
		if (entity->getDestroyFlag()) continue; // ignore entities that were flagged for destroy at the end og this scene
		
		switch (entity->getTag()) {
			case EntityTypes::SHOPPING_CART_PLAYER:
			{
				std::shared_ptr<PlayerScript> playerScript = std::static_pointer_cast<PlayerScript>(entity->getComponent(ComponentTypes::PLAYER_SCRIPT));
				for (int i = 0; i < 3; i++) {
					if (playerScript->_shoppingList_Flags.at(i)) {
						switch (playerScript->_shoppingList_Types.at(i)) {
							case EntityTypes::MILK:
								_milkLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, false));
								break;
							case EntityTypes::WATER:
								_waterLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, false));
								break;
							case EntityTypes::COLA:
								_colaLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, false));
								break;
							case EntityTypes::APPLE:
								_appleLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, false));
								break;
							case EntityTypes::WATERMELON:
								_watermelonLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, false));
								break;
							case EntityTypes::BANANA:
								_bananaLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, false));
								break;
							case EntityTypes::CARROT:
								_carrotLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, false));
								break;
							case EntityTypes::EGGPLANT:
								_eggplantLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, false));
								break;
							case EntityTypes::BROCCOLI:
								_broccoliLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, false));
								break;
						}
					}
				}

				break;
			}
			case EntityTypes::COOKIE:
				_cookieLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, true));
				break;
			case EntityTypes::MYSTERY_BAG:
				_mysteryBagLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, true));
				break;
			case EntityTypes::SPARE_CHANGE:
				_spareChangeLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, true));
				break;
			case EntityTypes::MILK:
				_milkLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, true));
				break;
			case EntityTypes::WATER:
				_waterLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, true));
				break;
			case EntityTypes::COLA:
				_colaLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, true));
				break;
			case EntityTypes::APPLE:
				_appleLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, true));
				break;
			case EntityTypes::WATERMELON:
				_watermelonLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, true));
				break;
			case EntityTypes::BANANA:
				_bananaLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, true));
				break;
			case EntityTypes::CARROT:
				_carrotLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, true));
				break;
			case EntityTypes::EGGPLANT:
				_eggplantLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, true));
				break;
			case EntityTypes::BROCCOLI:
				_broccoliLocations.push_back(ItemLocation(entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p, true));
				break;
		}
	}
}


// return -1 on failure
int AIManager::getNextDrinkSpawnIndex() {
	std::vector<int> openIndices;
	for (int i = 0; i < NB_DRINK_SPAWN_POINTS; i++) {
		if (drinkInstances.at(i) == nullptr) openIndices.push_back(i);
	}
	if (0 == openIndices.size()) return -1; // no open spots

	int rng = rand() % openIndices.size();
	return openIndices.at(rng);
}

// return -1 on failure
int AIManager::getNextFruitSpawnIndex() {
	std::vector<int> openIndices;
	for (int i = 0; i < NB_FRUIT_SPAWN_POINTS; i++) {
		if (fruitInstances.at(i) == nullptr) openIndices.push_back(i);
	}
	if (0 == openIndices.size()) return -1; // no open spots

	int rng = rand() % openIndices.size(); // ~~~~~~~~~BUG: my new changes can cause this size to be 0!!!! - get divide by zero exception! - this is because i'm setting thw wrong pointer to nullptr...
	return openIndices.at(rng);
}

// return -1 on failure
int AIManager::getNextVeggieSpawnIndex() {
	std::vector<int> openIndices;
	for (int i = 0; i < NB_VEGGIE_SPAWN_POINTS; i++) {
		if (veggieInstances.at(i) == nullptr) openIndices.push_back(i);
	}
	if (0 == openIndices.size()) return -1; // no open spots

	int rng = rand() % openIndices.size();
	return openIndices.at(rng);
}


void AIManager::setNewAITargets() {
	const std::vector<std::shared_ptr<ShoppingCartPlayer>> &players = _broker->getPhysicsManager()->getActiveScene()->getAllShoppingCartPlayers();
	for (std::shared_ptr<ShoppingCartPlayer> player : players) {
		std::shared_ptr<PlayerScript> playerScript = std::static_pointer_cast<PlayerScript>(player->getComponent(ComponentTypes::PLAYER_SCRIPT));
		if (playerScript->_playerType == PlayerScript::BOT) {

			// change up the target every frame, so need to clear it first...
			playerScript->_targets.clear();


			// 0. IF AN AI HAS THE HOT POTATO THEY WILL SIMPLY TRY TO FIND THE NEAREST (NON BASH_PROTECTED) PLAYER TO BASH AND PASS ON THE HOT POTATO...
			if (playerScript->_hasHotPotato) {

				PxVec3 playerPos = player->_actor->is<PxRigidDynamic>()->getGlobalPose().p;

				std::shared_ptr<ShoppingCartPlayer> closestCart = nullptr;
				PxVec3 closestCartPos;
				float smallestSeparation = FLT_MAX;

				for (std::shared_ptr<ShoppingCartPlayer> otherPlayer : players) {
					if (player == otherPlayer) continue; // ignore comparison with self...
					if (otherPlayer->_shoppingCartBase->IsBashProtected()) continue; // ignore comparison with bash protected carts...

					PxVec3 otherPlayerPos = otherPlayer->_actor->is<PxRigidDynamic>()->getGlobalPose().p;

					float separation = (otherPlayerPos - playerPos).magnitude();
					if (separation < smallestSeparation) {
						smallestSeparation = separation;
						closestCart = otherPlayer;
						closestCartPos = otherPlayerPos;
					}
				}

				if (closestCart != nullptr) { // target found...
					playerScript->_targets.push_back(ItemLocation(closestCartPos, false));
				}
			}
			else {
				// 1. IF STARTING COOKIE ON FIELD, DROP WHAT YOU'RE DOING AND SEEK IT OUT...

				if (_cookieLocations.size() > 0) {
					playerScript->_targets.push_back(_cookieLocations.at(0));
					continue;
				}

				// 2. IF MYSTERY BAG ON FIELD, DROP WHAT YOU'RE DOING AND SEEK IT OUT...

				if (_mysteryBagLocations.size() > 0) {
					playerScript->_targets.push_back(_mysteryBagLocations.at(0));
					continue;
				}

				// 3. SEEK OUT LIST ITEMS THAT YOU ARE MISSING...
				// PRIORITY:
				// A) CLOSEST WORLD ITEM
				// B) CLOSEST ITEM ON A PLAYER

				// target candidates will only contain list items that the player needs...
				std::vector<ItemLocation> targetCandidates;

				// loop over player's list...
				for (int i = 0; i < 3; i++) {
					if (!playerScript->_shoppingList_Flags.at(i)) { // see which items the player needs to complete their list... NOTE: this will also inherently prevent an AI from trying to infinitely seek out an APPLE (distance 0 from them) while they already have an APPLE
						switch (playerScript->_shoppingList_Types.at(i)) {
						case EntityTypes::MILK:
							for (ItemLocation loc : _milkLocations) {
								targetCandidates.push_back(loc);
							}
							break;
						case EntityTypes::WATER:
							for (ItemLocation loc : _waterLocations) {
								targetCandidates.push_back(loc);
							}
							break;
						case EntityTypes::COLA:
							for (ItemLocation loc : _colaLocations) {
								targetCandidates.push_back(loc);
							}
							break;
						case EntityTypes::APPLE:
							for (ItemLocation loc : _appleLocations) {
								targetCandidates.push_back(loc);
							}
							break;
						case EntityTypes::WATERMELON:
							for (ItemLocation loc : _watermelonLocations) {
								targetCandidates.push_back(loc);
							}
							break;
						case EntityTypes::BANANA:
							for (ItemLocation loc : _bananaLocations) {
								targetCandidates.push_back(loc);
							}
							break;
						case EntityTypes::CARROT:
							for (ItemLocation loc : _carrotLocations) {
								targetCandidates.push_back(loc);
							}
							break;
						case EntityTypes::EGGPLANT:
							for (ItemLocation loc : _eggplantLocations) {
								targetCandidates.push_back(loc);
							}
							break;
						case EntityTypes::BROCCOLI:
							for (ItemLocation loc : _broccoliLocations) {
								targetCandidates.push_back(loc);
							}
							break;
						}
					}
				}


				PxVec3 playerPos = player->_actor->is<PxRigidDynamic>()->getGlobalPose().p;
				ItemLocation closestTarget;
				bool targetFound = false;
				float smallestSeparation = FLT_MAX;

				for (ItemLocation loc : targetCandidates) {
					if (!targetFound) {
						targetFound = true;
						float separation = (loc._pos - playerPos).magnitude();
						smallestSeparation = separation;
						closestTarget = loc;
					}
					else {
						if (closestTarget._inWorld == loc._inWorld) {
							// tiebreak on closest...
							float separation = (loc._pos - playerPos).magnitude();
							if (separation < smallestSeparation) {
								smallestSeparation = separation;
								closestTarget = loc;
							}
						}
						else if (!closestTarget._inWorld && loc._inWorld) {
							float separation = (loc._pos - playerPos).magnitude();
							smallestSeparation = separation;
							closestTarget = loc;
						}
					}
				}

				if (targetFound) {
					playerScript->_targets.push_back(closestTarget);
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
