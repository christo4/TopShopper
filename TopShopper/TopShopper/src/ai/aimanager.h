#ifndef AIMANAGER_H_
#define AIMANAGER_H_

#include <array>
#include <foundation/PxTransform.h>
#include <vector>
#include <map>
#include <set>
#include <tuple>

class Broker;
class Entity;
class SpareChange;



/*
struct NavMesh {
	std::vector<physx::PxVec3> _nodes; // node 0, 1, 2...
	std::map<int, std::set<int>> _connections; // e.g. key (node 0) is connected to value (all nodes 1, 5, 7...)
	std::vector<std::tuple<int, int, int>> _triangles; // e.g. triangle 0 is made of nodes 1, 5, 10

	// http://blackpawn.com/texts/pointinpoly/default.html
	bool sameSide(physx::PxVec3 p1, physx::PxVec3 p2, physx::PxVec3 a, physx::PxVec3 b) {
		// project triangles in 2d-space (xz-plane)
		p1.y = 0;
		p2.y = 0;
		a.y = 0;
		b.y = 0;

		physx::PxVec3 cp1 = (b - a).cross(p1 - a);
		physx::PxVec3 cp2 = (b - a).cross(p2 - a);
		return (cp1.dot(cp2) >= 0);
	}

	bool pointInTriangle(physx::PxVec3 p, physx::PxVec3 a, physx::PxVec3 b, physx::PxVec3 c) {
		return (sameSide(p, a, b, c) && sameSide(p, b, a, c) && sameSide(p, c, a, b));
	}

	int getTriangleIndexContainingPoint(physx::PxVec3 p) {
		// loop through all triangles until point is found in one of them, return the 1st one (in case point is on an edge) or return -1 if outside all tris
		for (int i = 0; i < _triangles.size(); i++) {
			physx::PxVec3 a = _nodes.at(std::get<0>(_triangles.at(i)));
			physx::PxVec3 b = _nodes.at(std::get<1>(_triangles.at(i)));
			physx::PxVec3 c = _nodes.at(std::get<2>(_triangles.at(i)));
			if (pointInTriangle(p, a, b, c)) return i;
		}
		return -1;
	}


	// A* 
	// these 2 costs do not take into account obstructions in path
	// G-cost = cost to travel from start node to current node
	// H-cost = cost to travel from current node to end node
	// F-cost = G-cost + H-cost
	// keep track of parent node ID (= parent node that 'found' our current node) - I guess this is the node we just came from?
	// null set = node hasnt been seen or visited
	// closed set = node has been visited
	// open set = node has been seen but not visited yet


	// start node goes into closed set
	// current = start
	// surrounding nodes go into open set
	// calculate G/h>F costs for all open nodes...


	// NOTE: for later dynamic obstacle avoidance... I can raycast around (or maybe dynamically change nodes to be blocked if another cart is on top of them)
	// to make a divergence from the path, I will just push a new position to the front of the _targets

};
*/


class AIManager {
public:
	AIManager(Broker *broker);
	virtual ~AIManager();
	void init();
	void updateSeconds(double variableDeltaTime);

	void removeDeletedTarget(physx::PxVec3 deletedTarget);

	std::string getMatchTimePrettyFormat();

private:
	Broker *_broker = nullptr;

	static const int NB_SPARE_CHANGE_SPAWN_POINTS = 32;
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



	void setNewAITargets();


	double _matchTimer = 300; // 5min (300s) match


};




















#endif // AIMANAGER_H_

