
#include "gamescene.h"
#include "objects/entity.h"

#include "objects/shoppingcartplayer.h"


GameScene::GameScene(std::shared_ptr<PxScene> physxScene)
using namespace physx;


Scene::Scene(PxScene *physxScene)
	: _physxScene(physxScene)
{

}

GameScene::~GameScene() {

}


void GameScene::addEntity(std::shared_ptr<Entity> entity) {
	_entities.push_back(entity);
	// ~~~~~~~~TODO: also need to add the entity's actor(s) into _physScene with .addActor
}



// ~~~~~~NOTE: about dynamic casting, you must have a complete type (e.g. bug was caused by forward declaring ShoppingCartPlayer in scene.h, but forgetting to include shoppingcartplayer.h in this file...)
// ~~~~~~NOTE: could change this in future to be general (pass in tag enum and return proper vector) or instead cache a vector of every entity type
std::vector<std::shared_ptr<ShoppingCartPlayer>> Scene::getAllShoppingCartPlayers() {
	std::vector<std::shared_ptr<ShoppingCartPlayer>> shoppingCartPlayers; // empty vector
	for (std::shared_ptr<Entity> &entity : _entities) {
		if (entity->getTag() == EntityTypes::SHOPPING_CART_PLAYER) {
			// downcast...
			// BUILD errors with line below...
			std::shared_ptr<ShoppingCartPlayer> shoppingCartPlayer = std::dynamic_pointer_cast<ShoppingCartPlayer>(entity);
			shoppingCartPlayers.push_back(shoppingCartPlayer);
		}
	}

	return shoppingCartPlayers;
}
