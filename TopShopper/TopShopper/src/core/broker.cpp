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



void Broker::updateAllMilliseconds(double& simTime, const double& fixedDeltaTime, double& variableDeltaTime, double& accumulator) {
	
	_loadingManager->updateMilliseconds(variableDeltaTime); // useless right now, but if we want dynamic loading it could go first
	_inputManager->updateMilliseconds(variableDeltaTime); // NOTE: this needs to be done before physics updates
	
	while (accumulator >= fixedDeltaTime) {
		_physicsManager->updateMilliseconds(fixedDeltaTime);
		accumulator -= fixedDeltaTime;
		simTime += fixedDeltaTime;
	}
	
	_aiManager->updateMilliseconds(variableDeltaTime);
	_renderingManager->updateMilliseconds(variableDeltaTime);
	_audioManager->updateMilliseconds(variableDeltaTime);
	
}









