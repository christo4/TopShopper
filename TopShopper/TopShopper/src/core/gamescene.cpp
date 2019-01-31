
#include "gamescene.h"
#include "objects/entity.h"



GameScene::GameScene(std::shared_ptr<PxScene> physxScene)
	: _physxScene(physxScene)
{

}

GameScene::~GameScene() {

}


void GameScene::addEntity(std::shared_ptr<Entity> entity) {
	_entities.push_back(entity);
	// ~~~~~~~~TODO: also need to add the entity's actor(s) into _physScene with .addActor
}
