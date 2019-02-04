
#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include <memory>

class Entity;
class ShoppingCartPlayer;
//#include "objects/shoppingcartplayer.h"

namespace physx {
	class PxScene;
};



// DEFINITION:
// EACH SCENE WRAPS AROUND A PHYSX SCENE
class Scene {
	public:
		Scene(physx::PxScene *physxScene);
		virtual ~Scene();

		physx::PxScene *_physxScene = nullptr;

		void addEntity(std::shared_ptr<Entity> entity);
		// ~~~~~~~~also have remove entity - by name? or maybe by pointer

		std::vector<std::shared_ptr<ShoppingCartPlayer>> getAllShoppingCartPlayers();


		std::vector<std::shared_ptr<Entity>> _entities;

	private:
};





#endif // SCENE_H_
