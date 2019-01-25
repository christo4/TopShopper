


#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include <memory>

class Entity;

class Scene {
	public:
		Scene();
		virtual ~Scene();
	private:
		std::vector<std::shared_ptr<Entity>> _entities;
};






#endif // SCENE_H_
