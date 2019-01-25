

#include "scenemanager.h"

#include "broker.h"

using namespace physx;


SceneManager::SceneManager(Broker* broker)
	: _broker(broker)
{

}

SceneManager::~SceneManager() {

}



void SceneManager::init() {
	buildSampleScene();
}



// ~~~~~~~~~~NOTE: I think ill change this to do have the physicsmanager do most of this work (e.g. creating a physx scene and then returning it from there)
std::shared_ptr<Scene> SceneManager::buildSampleScene() {
	std::shared_ptr<Scene> scene = std::make_shared<Scene>();

	PxPhysics* factory = _broker->get_PhysicsManager_Factory();

	PxSceneDesc sceneDesc(factory->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	PxDefaultCpuDispatcher* dispatcher = PxDefaultCpuDispatcherCreate(2); // ~~~~~~~~~~~~2 = num of threads, DELETE THIS LATER OR MOVE INTO PHYSICS MANAGER
	sceneDesc.cpuDispatcher = dispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	physx::PxScene* physxScene = factory->createScene(sceneDesc);

	PxMaterial* material = factory->createMaterial(0.5f, 0.5f, 0.6f);

	// ~~~~~~~~~~~NOTE: this method is just to test physx, later i'll change this to actually use the physicsmanager to create a StaticPhysicsEntity w/ a plane shape and then pass it through the broker

	PxRigidStatic* groundPlane = PxCreatePlane(*factory, PxPlane(0, 1, 0, 0), *material);
	physxScene->addActor(*groundPlane);

	return scene;

}
