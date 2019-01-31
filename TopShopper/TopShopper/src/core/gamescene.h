


#ifndef GAMESCENE_H_
#define GAMESCENE_H_

#include <vector>
#include <memory>

class Entity;
class PxScene;



// DEFINITION:
// EACH SCENE WRAPS AROUND A PHYSX SCENE
class GameScene {
	public:
		GameScene(std::shared_ptr<PxScene> physxScene);
		virtual ~GameScene();

		std::shared_ptr<PxScene> _physxScene = nullptr;

		void addEntity(std::shared_ptr<Entity> entity);
		// ~~~~~~~~also have remove entity - by name? or maybe by pointer
	private:
		std::vector<std::shared_ptr<Entity>> _entities;
};





#endif // SCENE_H_
