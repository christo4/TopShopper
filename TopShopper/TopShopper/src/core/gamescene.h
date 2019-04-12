#ifndef GAMESCENE_H_
#define GAMESCENE_H_

#include <vector>
#include <memory>

class Entity;
class ShoppingCartPlayer;
class SpareChange;

namespace physx {
	class PxScene;
};



// DEFINITION:
// EACH GAMESCENE WRAPS AROUND A PHYSX SCENE
class GameScene {
	public:
		GameScene(physx::PxScene *physxScene);
		virtual ~GameScene();

		physx::PxScene *_physxScene = nullptr;

		void addEntity(std::shared_ptr<Entity> entity);
		void removeEntity(std::shared_ptr<Entity> entity);

		std::vector<std::shared_ptr<ShoppingCartPlayer>> getAllShoppingCartPlayers();

		std::vector<std::shared_ptr<SpareChange>> getAllSpareChange();


		std::vector<std::shared_ptr<Entity>> _entities;

	private:
};





#endif // GAMESCENE_H_
