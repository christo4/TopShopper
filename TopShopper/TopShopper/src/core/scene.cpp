
#include "scene.h"
#include "objects/entity.h"



Scene::Scene(std::shared_ptr<PxScene> physxScene)
	: _physxScene(physxScene)
{

}

Scene::~Scene() {

}


void Scene::addEntity(std::shared_ptr<Entity> entity) {
	_entities.push_back(entity);
	// ~~~~~~~~TODO: also need to add the entity's actor(s) into _physScene with .addActor
}
