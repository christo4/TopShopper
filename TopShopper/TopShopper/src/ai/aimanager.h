#ifndef AIMANAGER_H_
#define AIMANAGER_H_

#include <array>
#include <foundation/PxTransform.h>
#include <vector>
#include <map>
#include <set>
#include <tuple>
#include "utility/utility.h"

class Broker;
class Entity;
class SpareChange;
class Cookie;
class MysteryBag;



class AIManager {
public:
	AIManager(Broker *broker);
	virtual ~AIManager();
	void init();
	void updateSeconds(double variableDeltaTime);

	void loadScene1();
	void cleanupScene1();

	std::string getMatchTimePrettyFormat();

private:
	Broker *_broker = nullptr;

	// update these vectors every frame...
	void updateLocations();
	// ITEM LOCATIONS (either physically in world or on a player...)
	
	// ONLY IN WORLD...
	std::vector<ItemLocation> _cookieLocations;
	std::vector<ItemLocation> _mysteryBagLocations;
	std::vector<ItemLocation> _spareChangeLocations;

	// BOTH POSSIBLE...
	std::vector<ItemLocation> _milkLocations;
	std::vector<ItemLocation> _waterLocations;
	std::vector<ItemLocation> _colaLocations;
	std::vector<ItemLocation> _appleLocations;
	std::vector<ItemLocation> _watermelonLocations;
	std::vector<ItemLocation> _bananaLocations;
	std::vector<ItemLocation> _carrotLocations;
	std::vector<ItemLocation> _eggplantLocations;
	std::vector<ItemLocation> _broccoliLocations;

	static const int MAX_NB_INSTANCES_OF_EACH_GROCERY_ITEM = 1; // WARNING: if I increase this in the future, I need to increase NB_SPAWN_POINTS for each item...


	bool _cookieCanSpawn = true;
	std::shared_ptr<Cookie> _startingCookie = nullptr; // ONLY SPAWNS ONCE AT START OF GAME
	physx::PxTransform _startingCookieSpawnPoint = physx::PxTransform(0.0f, 19.0f, 0.0f, physx::PxQuat(physx::PxIdentity));


	bool _mysteryBagCanSpawn = false;
	std::shared_ptr<MysteryBag> _mysteryBag = nullptr; // ONLY BEGINS SPAWNING AFTER STARTING COOKIE GETS PICKED UP! 
	physx::PxTransform _mysteryBagSpawnPoint = physx::PxTransform(0.0f, 19.0f, 0.0f, physx::PxQuat(physx::PxIdentity));
	double _mysteryBagSpawnTimer = 30.0; // this will vary from spawn to spawn in range 30-60 secs


	static const int NB_SPARE_CHANGE_SPAWN_POINTS = 32;
	const double SPARE_CHANGE_RESPAWN_TIME = 10.0; // seconds
	std::array<std::shared_ptr<SpareChange>, NB_SPARE_CHANGE_SPAWN_POINTS> spareChangeInstances;
	std::array<physx::PxTransform, NB_SPARE_CHANGE_SPAWN_POINTS> spareChangeSpawnPoints;
	std::array<double, NB_SPARE_CHANGE_SPAWN_POINTS> spareChangeSpawnTimers; // spawn timers in seconds



	static const int NB_DRINK_SPAWN_POINTS = 6;
	std::array<std::shared_ptr<Entity>, NB_DRINK_SPAWN_POINTS> drinkInstances;
	std::array<physx::PxTransform, NB_DRINK_SPAWN_POINTS> drinkSpawnPoints;

	static const int NB_FRUIT_SPAWN_POINTS = 6;
	std::array<std::shared_ptr<Entity>, NB_FRUIT_SPAWN_POINTS> fruitInstances;
	std::array<physx::PxTransform, NB_FRUIT_SPAWN_POINTS> fruitSpawnPoints;

	static const int NB_VEGGIE_SPAWN_POINTS = 6;
	std::array<std::shared_ptr<Entity>, NB_VEGGIE_SPAWN_POINTS> veggieInstances;
	std::array<physx::PxTransform, NB_VEGGIE_SPAWN_POINTS> veggieSpawnPoints;


	int getNextDrinkSpawnIndex();
	int getNextFruitSpawnIndex();
	int getNextVeggieSpawnIndex();

	void setNewAITargets();

	double _matchTimer = 300; // 5min (300s) match
};




















#endif // AIMANAGER_H_

