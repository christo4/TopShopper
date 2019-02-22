#ifndef AIMANAGER_H_
#define AIMANAGER_H_

#include <array>
#include <foundation/PxTransform.h>


class Broker;
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


};




















#endif // AIMANAGER_H_

