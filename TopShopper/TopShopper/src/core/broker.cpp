
#include "broker.h"

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

}



void Broker::initAll() {
	// ~~~~~TODO: figure out proper order to init each system
	
	_physicsManager.init();
	_renderingManager.init();
	// etc. ........
}



void Broker::updateAllMilliseconds(double deltaTime) {
	// ~~~~~TODO: figure out proper order to update each system
	// ex:
	// 1. _inputManager.updateAllMilliseconds(deltaTime);
	// 2. _physicsManager.updateAllMilliseconds(deltaTime);
	// 3. _renderingManager.updateAllMilliseconds(deltaTime);
	// 4. _audioManager.updateAllMilliseconds(deltaTime);
	// etc.

	_physicsManager.updateMilliseconds(deltaTime);
	_renderingManager.updateMilliseconds(deltaTime);

	// etc. .....
}



GLFWwindow* Broker::get_RenderingManager_Window() {
	return _renderingManager.getWindow();
}