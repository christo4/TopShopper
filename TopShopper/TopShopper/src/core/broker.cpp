#include "broker.h"
#include <iostream>

// init statics:
Broker* Broker::_instance = nullptr; // singleton instance starts out null

// implement rest: 
Broker* Broker::getInstance() {
	if (_instance == nullptr) {
		_instance = new Broker();
	}
	return _instance;
}

Broker::Broker() {
	_aiManager = new AIManager(this);
	_audioManager = new AudioManager(this);
	_inputManager = new InputManager(this);
	_loadingManager = new LoadingManager(this);
	_physicsManager = new PhysicsManager(this);
	_renderingManager = new RenderingManager(this);
	
}



void Broker::initAll() {
	
	_loadingManager->init(); // loads in all assets
	_renderingManager->init(); // inits GLFW
	_inputManager->init(); // sets up gamepads w/ IDS
	_physicsManager->init(); // inits PhysX/Vehicle SDKs + starting scene
	_audioManager->init(); // inits SDL
	_aiManager->init(); // could init menus

}



void Broker::updateAllSeconds(double& simTime, const double& fixedDeltaTime, double& variableDeltaTime, double& accumulator) {
	
	_loadingManager->updateSeconds(variableDeltaTime); // useless right now, but if we want dynamic loading it could go first
	_inputManager->updateSeconds(variableDeltaTime); // NOTE: this needs to be done before physics updates
	
	if (!_isPaused) {
		while (accumulator >= fixedDeltaTime) {
			_physicsManager->updateSeconds(fixedDeltaTime);
			accumulator -= fixedDeltaTime;
			simTime += fixedDeltaTime;
			
		}
		
		_aiManager->updateSeconds(variableDeltaTime);
		
	}
	

	_renderingManager->updateSeconds(variableDeltaTime);
	_audioManager->updateSeconds(variableDeltaTime);
	

	// CLEANUP ENTITIES FLAGGED TO BE DESTROYED...
	std::vector<std::shared_ptr<Entity>> destroyedEntities;
	for (std::shared_ptr<Entity> &entity : _physicsManager->getActiveScene()->_entities) {
		if (entity->getDestroyFlag()) {
			destroyedEntities.push_back(entity);
		}
	}

	for (std::shared_ptr<Entity> &entity : destroyedEntities) {
		std::shared_ptr<Component> comp = entity->getComponent(ComponentTypes::BEHAVIOUR_SCRIPT);
		if (comp != nullptr) {
			std::shared_ptr<BehaviourScript> script = std::static_pointer_cast<BehaviourScript>(comp);
			script->onDestroy();
		}
		_physicsManager->getActiveScene()->removeEntity(entity);
	}

}









