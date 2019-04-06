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
	_audioManager->updateSeconds(variableDeltaTime); // NOTE: probably need to guard this to either only play in GAME scene or stop audio once left GAME scene??
	

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


void Broker::manageScene(double& accumulator) {
	//ADD DELAY
	if (delayX != 0) { // NOTE: this should probably be changed to use time, to make it framerate independant... (also a delay of 200frames in game scene is longer than a delay of 200 in main menu, due to physics/ai updates) 
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
			delayX = 200;
		}
		else if (kam->sKey || (playerControlled && player1->leftStickY < -0.5)) {
			if (_cursorPositionStart == 0) {
				_cursorPositionStart = 3;
			}
			else {
				_cursorPositionStart -= 1;
			}
			delayX = 200;
		}
		if (kam->enterKeyJustPressed || (playerControlled && player1->aButtonJustPressed)) {
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
			delayX = 0;
		}
		break;
	case(CREDITS):
		if (kam->spaceKeyJustPressed || (playerControlled && player1->bButtonJustPressed)) {
			_scene = MAIN_MENU;
			delayX = 0;
		}
		break;
	case(CONTROLS):
		if (kam->spaceKeyJustPressed || (playerControlled && player1->bButtonJustPressed)) {
			_scene = MAIN_MENU;
			delayX = 0;
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
			delayX = 200;
		}
		else if (kam->sKey || (playerControlled && player1->leftStickY < -0.02)) {
			if (_cursorPositionSetup == 0) {
				_cursorPositionSetup = 2;
			}
			else {
				_cursorPositionSetup -= 1;
			}
			delayX = 200;
		}

		if ((kam->enterKeyJustPressed || (playerControlled && player1->aButtonJustPressed)) && _cursorPositionSetup == 0) {
			// NOTE: this is where we need to reset the PhysX and AI (should probably display the loading screen image first before calling reset functions)
			_scene = LOADING;
			_cursorPositionSetup = 2;
			delayX = 0;
		}
		if ((kam->spaceKeyJustPressed || (playerControlled && player1->bButtonJustPressed))) {
			_scene = MAIN_MENU;
			_cursorPositionSetup = 2;
			delayX = 0;
		}
		break;
	case(LOADING):
		if (kam->enterKeyJustPressed || (playerControlled && player1->aButtonJustPressed)) {
			_scene = GAME;
			accumulator = 0.0; // NOTE: maybe move this above?
			delayX = 0;
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
			delayX = 100;
		}
		else if (kam->sKey || (playerControlled && player1->leftStickY < -0.02)) {
			if (_cursorPositionPause == 0) {
				_cursorPositionPause = 1;
			}
			else {
				_cursorPositionPause -= 1;
			}
			delayX = 100;
		}
		if (kam->enterKeyJustPressed || (playerControlled && player1->aButtonJustPressed)) {
			if (_cursorPositionPause == 1) {
				_scene = GAME;
				//accumulator = 0.0; // I don't think the accumulator needs to be reset here since it isnt incrementing while game is paused and we would lose out on some left over time from frame before pause
				_cursorPositionPause = 1;
			}
			else {
				_scene = MAIN_MENU;
				_cursorPositionPause = 1;
				//Have to reset physics stuff somehow NOTE: physics won't be reset here, only during loading scene!
			}
			delayX = 0;
		}
		break;
	case (GAME):
		if (kam->pKeyJustPressed || (playerControlled && player1->startButtonJustPressed)) {
			_scene = PAUSED;
		}
		delayX = 0;
		break;
	case (END_SCREEN):
		if (kam->enterKeyJustPressed || (playerControlled && player1->aButtonJustPressed)) {
			_scene = MAIN_MENU;
		}
		delayX = 0;
		break;
	}
}








