
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
	_loadingManager = new LoadingManager(this);
	_physicsManager = new PhysicsManager(this);
	_renderingManager = new RenderingManager(this);
	_inputManager = new InputManager(this);
	
}



void Broker::initAll() {
	// ~~~~~TODO: figure out proper order to init each system
	
	_loadingManager->init();
	_physicsManager->init();
	_aiManager->init();
	_renderingManager->init();
	_inputManager->init();
	// etc. ........;
}



void Broker::updateAllMilliseconds(double deltaTime) {
	// ~~~~~TODO: figure out proper order to update each system
	// ex:
	// 1. _inputManager.updateAllMilliseconds(deltaTime);
	// 2. _physicsManager.updateAllMilliseconds(deltaTime);
	// 3. _renderingManager.updateAllMilliseconds(deltaTime);
	// 4. _audioManager.updateAllMilliseconds(deltaTime);
	// etc.

	_inputManager->updateMilliseconds(deltaTime); // NOTE: this needs to be done before physics updates
	_physicsManager->updateMilliseconds(deltaTime);
	_aiManager->updateMilliseconds(deltaTime); // TODO: figure out what order to put this in
	_renderingManager->updateMilliseconds(deltaTime);

	// ~~~~~~~~~WARNING: this should be commented out when no controller is connected...

	/*
	Gamepad * meme = _inputManager->getGamePad(1);

	
	std::cout << "LeftX: " <<meme->leftStickX << std::endl;
	std::cout << "LeftY: " <<meme->leftStickY << std::endl;
	std::cout << "RightX: "<<meme->rightStickX << std::endl;
	std::cout << "RigthY: "<<meme->rightStickY << std::endl;
	std::cout << "LeftTrig: " <<meme->leftTrigger << std::endl;
	std::cout << "RightTrig: " <<meme->rightTrigger << std::endl;
	std::cout << "A: " <<meme->aButton << std::endl;
	std::cout << "B: " <<meme->bButton << std::endl;
	std::cout << "X: " <<meme->xButton << std::endl;
	std::cout << "Y: " <<meme->yButton << std::endl;
	std::cout << "LeftBump: " <<meme->leftBump << std::endl;
	std::cout << "RightBump: " <<meme->rightBump << std::endl;
	std::cout << "Back: " <<meme->backButton << std::endl;
	std::cout <<"Start: " <<meme->startButton << std::endl;
	std::cout << "LeftStick: " <<meme->leftStick << std::endl;
	std::cout << "RightStick: " <<meme->rightStick << std::endl;
	std::cout << "Up: " <<meme->upButton << std::endl;
	std::cout << "Right: " <<meme->rightButton << std::endl;
	std::cout << "Down: " <<meme->downButton << std::endl;
	std::cout << "Left: " <<meme->leftButton << std::endl;
	system("cls");
	*/
	
}



GLFWwindow* Broker::get_RenderingManager_Window() {
	return _renderingManager->getWindow();
}




std::vector<std::shared_ptr<ShoppingCartPlayer>> Broker::get_PhysicsManager_ActiveScene_AllShoppingCartPlayers() {
	return _physicsManager->getActiveScene()->getAllShoppingCartPlayers();
}

std::vector<std::shared_ptr<SpareChange>> Broker::get_PhysicsManager_ActiveScene_AllSpareChange() {
	return _physicsManager->getActiveScene()->getAllSpareChange();
}



Geometry* Broker::get_LoadingManager_Geometry(GeometryTypes type) {
	return _loadingManager->getGeometry(type);
}



physx::PxShape** Broker::get_PhysicsManager_AllShapes() {
	return _physicsManager->getAllShapes();
}


physx::PxU32 Broker::get_PhysicsManager_NbShapes() {
	return _physicsManager->getNbShapes();
}
