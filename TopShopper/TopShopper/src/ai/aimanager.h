#ifndef AIMANAGER_H_
#define AIMANAGER_H_

#include <array>
#include <foundation/PxTransform.h>


class Broker;
class Entity;
class SpareChange;

class AIManager {
public:
	AIManager(Broker *broker);
	virtual ~AIManager();
	void init();
	void updateSeconds(double variableDeltaTime);
private:
	Broker *_broker = nullptr;

	static const int NB_SPARE_CHANGE_SPAWN_POINTS = 27;
	const double SPARE_CHANGE_RESPAWN_TIME = 10.0; // seconds
	std::array<std::shared_ptr<SpareChange>, NB_SPARE_CHANGE_SPAWN_POINTS> spareChangeInstances;
	std::array<physx::PxTransform, NB_SPARE_CHANGE_SPAWN_POINTS> spareChangeSpawnPoints;
	std::array<double, NB_SPARE_CHANGE_SPAWN_POINTS> spareChangeSpawnTimers; // spawn timers in seconds



	static const int NB_DRINK_SPAWN_POINTS = 9;
	std::array<std::shared_ptr<Entity>, NB_DRINK_SPAWN_POINTS> drinkInstances;
	std::array<physx::PxTransform, NB_DRINK_SPAWN_POINTS> drinkSpawnPoints;

	static const int NB_FRUIT_SPAWN_POINTS = 9;
	std::array<std::shared_ptr<Entity>, NB_FRUIT_SPAWN_POINTS> fruitInstances;
	std::array<physx::PxTransform, NB_FRUIT_SPAWN_POINTS> fruitSpawnPoints;

	static const int NB_VEGGIE_SPAWN_POINTS = 9;
	std::array<std::shared_ptr<Entity>, NB_VEGGIE_SPAWN_POINTS> veggieInstances;
	std::array<physx::PxTransform, NB_VEGGIE_SPAWN_POINTS> veggieSpawnPoints;

	const double GROCERY_ITEM_RESPAWN_TIME = 5.0; // seconds

	// NOTE: an index of -1 means that the item is not currently spawned in
	double milkSpawnTimer = -1.0;
	int milkSpawnIndex = -1;
	double waterSpawnTimer = -1.0;
	int waterSpawnIndex = -1;
	double colaSpawnTimer = -1.0;
	int colaSpawnIndex = -1;

	double appleSpawnTimer = -1.0;
	int appleSpawnIndex = -1;
	double watermelonSpawnTimer = -1.0;
	int watermelonSpawnIndex = -1;
	double bananaSpawnTimer = -1.0;
	int bananaSpawnIndex = -1;

	double carrotSpawnTimer = -1.0;
	int carrotSpawnIndex = -1;
	double eggplantSpawnTimer = -1.0;
	int eggplantSpawnIndex = -1;
	double broccoliSpawnTimer = -1.0;
	int broccoliSpawnIndex = -1;



	int getNextDrinkSpawnIndex();
	int getNextFruitSpawnIndex();
	int getNextVeggieSpawnIndex();

};




















#endif // AIMANAGER_H_

