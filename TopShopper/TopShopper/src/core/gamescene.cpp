#include "gamescene.h"
#include "PxScene.h"
#include "objects/shoppingcartplayer.h"
#include "objects/sparechange.h"
#include <iostream>
#include "PxRigidActor.h"
#include "vehicle/VehicleShoppingCart.h"

using namespace physx;


GameScene::GameScene(PxScene *physxScene) 
	: _physxScene(physxScene)
{

}

GameScene::~GameScene() {

}


void GameScene::addEntity(std::shared_ptr<Entity> entity) {
	_entities.push_back(entity);
	_physxScene->addActor(*(entity->_actor));
}


void GameScene::removeEntity(std::shared_ptr<Entity> entity) {
	for (int i = 0; i < _entities.size(); i++) {
		if (_entities.at(i) == entity) { // if both pointers point to same address...
			_physxScene->removeActor(*(entity->_actor));
			entity->_actor->is<PxRigidActor>()->release();
			entity->_actor = nullptr;
			if (entity->getTag() == EntityTypes::SHOPPING_CART_PLAYER) {
				std::dynamic_pointer_cast<ShoppingCartPlayer>(entity)->_shoppingCartBase->_vehicle4W->free();
			}
			_entities.erase(_entities.begin() + i);
			break;
		}
	}
}








// ~~~~~~NOTE: about dynamic casting, you must have a complete type (e.g. bug was caused by forward declaring ShoppingCartPlayer in scene.h, but forgetting to include shoppingcartplayer.h in this file...)
// ~~~~~~NOTE: could change this in future to be general (pass in tag enum and return proper vector) or instead cache a vector of every entity type
std::vector<std::shared_ptr<ShoppingCartPlayer>> GameScene::getAllShoppingCartPlayers() {
	std::vector<std::shared_ptr<ShoppingCartPlayer>> shoppingCartPlayers; // empty vector
	for (std::shared_ptr<Entity> &entity : _entities) {
		if (entity->getTag() == EntityTypes::SHOPPING_CART_PLAYER) {
			// downcast...
			std::shared_ptr<ShoppingCartPlayer> shoppingCartPlayer = std::dynamic_pointer_cast<ShoppingCartPlayer>(entity);
			shoppingCartPlayers.push_back(shoppingCartPlayer);
		}
	}

	return shoppingCartPlayers;
}



std::vector<std::shared_ptr<SpareChange>> GameScene::getAllSpareChange() {
	std::vector<std::shared_ptr<SpareChange>> spareChangeVec; // empty vector
	for (std::shared_ptr<Entity> &entity : _entities) {
		if (entity->getTag() == EntityTypes::SPARE_CHANGE) {
			// downcast...
			std::shared_ptr<SpareChange> spareChange = std::dynamic_pointer_cast<SpareChange>(entity);
			spareChangeVec.push_back(spareChange);
		}
	}

	return spareChangeVec;
}
