


#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include <memory>

class Entity;
class PxScene;



// DEFINITION:
// EACH SCENE WRAPS AROUND A PHYSX SCENE
class Scene {
	public:
		Scene(std::shared_ptr<PxScene> physxScene);
		virtual ~Scene();

		std::shared_ptr<PxScene> _physxScene = nullptr;

		void addEntity(std::shared_ptr<Entity> entity);
		// ~~~~~~~~also have remove entity - by name? or maybe by pointer
	private:
		std::vector<std::shared_ptr<Entity>> _entities;
};





#endif // SCENE_H_
