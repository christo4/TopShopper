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
	_nbPlayers = 4;
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
	_nbOfDevices = _inputManager->_numGamepads > 0 ? _inputManager->_numGamepads : 1;
}



void Broker::updateAllSeconds(double& simTime, const double& fixedDeltaTime, double& variableDeltaTime, double& accumulator) {
	
	_loadingManager->updateSeconds(variableDeltaTime); // useless right now, but if we want dynamic loading it could go first
	_inputManager->updateSeconds(variableDeltaTime); // NOTE: this needs to be done before physics updates
	
	manageScene(accumulator, variableDeltaTime);
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
	

	if (_scene == GAME || _scene == PAUSED || _scene == END_SCREEN) {
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

}


void Broker::manageScene(double& accumulator, double vartime) {
	//ADD DELAY
	if (delayX > 0.0) {
		delayX -= vartime;
		return;
	}

	player1 = _inputManager->getGamePad(1);
	bool playerControlled = true;
	if (player1 == nullptr) playerControlled = false;
	KeyboardAndMouse *kam = getInputManager()->getKeyboardAndMouse();	
	switch (_scene) {
	case (MAIN_MENU):
		if (kam->wKey || (playerControlled && player1->leftStickY > 0.5f)) {
			if (_cursorPositionStart == 3) {
				_cursorPositionStart = 0;
			}
			else {
				_cursorPositionStart += 1; 
			}
			delayX = 0.2;
		}
		else if (kam->sKey || (playerControlled && player1->leftStickY < -0.5f)) {
			if (_cursorPositionStart == 0) {
				_cursorPositionStart = 3;
			}
			else {
				_cursorPositionStart -= 1;
			}
			delayX = 0.2;
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
			delayX = 0.0;
		}
		break;
	case(CREDITS):
		if (kam->spaceKeyJustPressed || (playerControlled && player1->bButtonJustPressed)) {
			_scene = MAIN_MENU;
			delayX = 0.0;
		}
		break;
	case(CONTROLS):
		if (kam->spaceKeyJustPressed || (playerControlled && player1->bButtonJustPressed)) {
			_scene = MAIN_MENU;
			delayX = 0.0;
		}
		break;
	case(SETUP):
		if (kam->wKey || (playerControlled && player1->leftStickY > 0.5f)) {
			if (_cursorPositionSetup == 2) {
				_cursorPositionSetup = 0;
			}
			else {
				_cursorPositionSetup += 1;
			}
			delayX = 0.2;
		}
		else if (kam->sKey || (playerControlled && player1->leftStickY < -0.5f)) {
			if (_cursorPositionSetup == 0) {
				_cursorPositionSetup = 2;
			}
			else {
				_cursorPositionSetup -= 1;
			}
			delayX = 0.2;
		}
		if (kam->dKey || (playerControlled && player1->leftStickX > 0.5f)) {
			if (_cursorPositionSetup == 1) {
				if (_nbPlayers == _nbOfDevices) {
					_nbPlayers = 1;
				}
				else {
					_nbPlayers += 1;
				}
			}
			delayX = 0.2;
		}
		if (kam->aKey || (playerControlled && player1->leftStickX < -0.5f)) {
			if (_cursorPositionSetup == 1) {
				if (_nbPlayers == 1) {
					_nbPlayers = _nbOfDevices;
				}
				else {
					_nbPlayers -= 1;
				}
			}
			delayX = 0.2;
		}

		if ((kam->enterKeyJustPressed || (playerControlled && player1->aButtonJustPressed)) && _cursorPositionSetup == 0) {
			_scene = LOADING;
			_cursorPositionSetup = 2;
			delayX = 0.0;

			// NOTE: this is where we need to reset the PhysX and AI (should probably display the loading screen image first before calling reset functions)
			// NOTE: keep it in this order...
			_physicsManager->loadScene1(_nbPlayers);
			_aiManager->loadScene1();
			_renderingManager->loadScene1();
		}
		if ((kam->spaceKeyJustPressed || (playerControlled && player1->bButtonJustPressed))) {
			_scene = MAIN_MENU;
			_cursorPositionSetup = 2;
			delayX = 0.0;
		}
		break;
	case(LOADING):
		if (kam->enterKeyJustPressed || (playerControlled && player1->aButtonJustPressed)) {
			_audioManager->changeBGM(BGMTypes::GAME_SCENE);
			_scene = GAME;
			accumulator = 0.0; // NOTE: maybe move this above?
			delayX = 0.0;
			
		}
		break;
	case(PAUSED):
		if (kam->wKey || (playerControlled && player1->leftStickY > 0.5f)) {
			if (_cursorPositionPause == 1) {
				_cursorPositionPause = 0;
			}
			else {
				_cursorPositionPause += 1;
			}
			delayX = 0.2;
		}
		else if (kam->sKey || (playerControlled && player1->leftStickY < -0.5f)) {
			if (_cursorPositionPause == 0) {
				_cursorPositionPause = 1;
			}
			else {
				_cursorPositionPause -= 1;
			}
			delayX = 0.2;
		}
		if (kam->enterKeyJustPressed || (playerControlled && player1->aButtonJustPressed)) {
			if (_cursorPositionPause == 1) {
				_scene = GAME;
				_cursorPositionPause = 1;
				_audioManager->resumeAllSFX();
			}
			else {
				_audioManager->resetAudio();
				_audioManager->changeBGM(BGMTypes::MENU_SCENE);
				_scene = MAIN_MENU;
				_cursorPositionPause = 1;

				// NOTE: keep in this order...
				_physicsManager->cleanupScene1();
				_aiManager->cleanupScene1();
			}
			delayX = 0.0;
		}
		if (kam->pKeyJustPressed || (playerControlled && player1->startButtonJustPressed)) {
			_scene = GAME;
			delayX = 0.0;
			_cursorPositionPause = 1;
			_audioManager->resumeAllSFX();
		}
		break;
	case (GAME):  
		if (kam->pKeyJustPressed || (playerControlled && player1->startButtonJustPressed)) {
			_scene = PAUSED;
			delayX = 0.0;
			_audioManager->pauseAllSFX();
		}
		break;
	case (END_SCREEN):
		if (kam->enterKeyJustPressed || (playerControlled && player1->aButtonJustPressed)) {
			_audioManager->changeBGM(BGMTypes::MENU_SCENE);
			_scene = MAIN_MENU;
			delayX = 0.0;

			// NOTE: keep in this order...
			_physicsManager->cleanupScene1();
			_aiManager->cleanupScene1();
			_audioManager->resetAudio();
		}
		break;
	}
}








