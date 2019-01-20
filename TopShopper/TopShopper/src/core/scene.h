


#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include <memory>

class Entity;

class Scene {
	public:
		Scene();
		virtual ~Scene();

		// have a funch that is able to switch between the scenes with different entities and an init function
		// ~~~~~have a bunch of other methods that the broker can call to extract similar data from all the entities...
		// e.g. getAllMeshes() would return all mesh components attached to any of the entities, which could then be passed to the renderingManager by the broker
	private:
		std::vector<std::shared_ptr<Entity>> _entities;
};






#endif // SCENE_H_
