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
	_scene = MAIN_MENU;
	_cursorPositionStart = 3;
	_cursorPositionSetup = 2;
	_cursorPositionPause = 1;
}



void Broker::updateAllSeconds(double& simTime, const double& fixedDeltaTime, double& variableDeltaTime, double& accumulator) {
	
	_loadingManager->updateSeconds(variableDeltaTime); // useless right now, but if we want dynamic loading it could go first
	_inputManager->updateSeconds(variableDeltaTime); // NOTE: this needs to be done before physics updates
	
	manageScene(accumulator);
	//std::cout << std::to_string(_scene) << std::endl;

	if (_scene == GAME) {
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


void Broker::manageScene(double& accumlator) {
	//ADD DELAY
	if (delayX != 0) {
		delayX -= 1;
		return;
	}

	player1 = _inputManager->getGamePad(1);
	bool playerControlled = true;
	if (player1 == nullptr) playerControlled = false;
	KeyboardAndMouse *kam = getInputManager()->getKeyboardAndMouse();	
	switch (_scene) {
	case (MAIN_MENU):

		if (kam->wKey || (playerControlled && player1->leftStickY > 0.5)) {
			if (_cursorPositionStart == 3) {
				_cursorPositionStart = 0;
			}
			else {
				_cursorPositionStart += 1; 
			}
			delayX = 40;
		}
		else if (kam->sKey || (playerControlled && player1->leftStickY < -0.5)) {
			if (_cursorPositionStart == 0) {
				_cursorPositionStart = 3;
			}
			else {
				_cursorPositionStart -= 1;
			}
			delayX = 40;
		}
		if (kam->spaceKey || (playerControlled && player1->aButton)) {
			switch (_cursorPositionStart) {
			case 0:
				std::exit(0);
			case 1:
				_scene = CREDITS;
				break;
			case 2:
				_scene = CONTROLS;
				break;
			case 3:
				_scene = SETUP;
				_cursorPositionStart = 3;
				break;
			}
			delayX = 40;
		}
		break;
	case(CREDITS):
		if (kam->spaceKey || (playerControlled && player1->bButton)) {
			_scene = MAIN_MENU;
			delayX = 40;
		}
		break;
	case(CONTROLS):
		if (kam->spaceKey || (playerControlled && player1->bButton)) {
			_scene = MAIN_MENU;
			delayX = 40;
		}
		break;
	case(SETUP):
		if (kam->wKey || (playerControlled && player1->leftStickY > 0.02)) {
			if (_cursorPositionSetup == 2) {
				_cursorPositionSetup = 0;
			}
			else {
				_cursorPositionSetup += 1;
			}
			delayX = 40;
		}
		else if (kam->sKey || (playerControlled && player1->leftStickY < -0.02)) {
			if (_cursorPositionSetup == 0) {
				_cursorPositionSetup = 2;
			}
			else {
				_cursorPositionSetup -= 1;
			}
			delayX = 40;
		}
		if ((kam->spaceKey || (playerControlled && player1->aButton)) && _cursorPositionSetup == 0) {
			_scene = LOADING;
			_cursorPositionSetup = 2;
			delayX = 40;
		}
		if ((kam->dKey || (playerControlled && player1->bButton))) {
			_scene = MAIN_MENU;
			_cursorPositionSetup = 2;
			delayX = 40;
		}
		break;
	case(LOADING):
		if (kam->spaceKey || (playerControlled && player1->aButton)) {
			_scene = GAME;
			accumlator = 0;
			delayX = 20;
		}
		break;
	case(PAUSED):
		if (kam->wKey || (playerControlled && player1->leftStickY > 0.02)) {
			if (_cursorPositionPause == 1) {
				_cursorPositionPause = 0;
			}
			else {
				_cursorPositionPause += 1;
			}
			delayX = 40;
		}
		else if (kam->sKey || (playerControlled && player1->leftStickY < -0.02)) {
			if (_cursorPositionPause == 0) {
				_cursorPositionPause = 1;
			}
			else {
				_cursorPositionPause -= 1;
			}
			delayX = 40;
		}
		if (kam->spaceKey || (playerControlled && player1->aButton)) {
			if (_cursorPositionPause == 1) {
				_scene = GAME;
				accumlator = 0;
				_cursorPositionPause = 1;
			}
			else {
				_scene = MAIN_MENU;
				_cursorPositionPause = 1;
				//Have to reset physics stuff somehow
			}
			delayX = 40;
		}
		break;
	case (GAME):
		// if the pause button/key was pressed in last frame...
		// right now, only player 1 (keyboard or gamepad1 can pause the game)
		//Gamepad *pad1 = broker->getInputManager()->getGamePad(1);
		if (kam->pKey || (playerControlled && player1->startButton)) {
			_scene = PAUSED;
		}
		delayX = 20;
		break;
	case (END_SCREEN):
		if (kam->spaceKey || (playerControlled && player1->aButton)) {
			_scene = MAIN_MENU;
		}
		delayX = 40;
		break;
	}
}








