#ifndef BROKER_H_
#define BROKER_H_

#include "ai/aimanager.h"
#include "audio/audiomanager.h"
#include "input/inputmanager.h"
#include "loading/loadingmanager.h"
#include "physics/physicsmanager.h"
#include "rendering/renderingmanager.h"



enum Scenes {
	MAIN_MENU,
	CREDITS,
	CONTROLS,
	SETUP,
	LOADING,
	GAME,
	PAUSED,
	END_SCREEN
};

// SINGLETON...
class Broker {
public:
	static Broker* getInstance();

	void initAll(); // calls each subsystem's init function
	void updateAllSeconds(double& simTime, const double& fixedDeltaTime, double& variableDeltaTime, double& accumulator); // this update function will call each subsystem's update function in an appropriate order.
	void manageScene(double& accumulator, double vartime);

	AIManager* getAIManager() { return _aiManager; }
	AudioManager* getAudioManager() { return _audioManager; }
	InputManager* getInputManager() { return _inputManager; }
	LoadingManager* getLoadingManager() { return _loadingManager; }
	PhysicsManager* getPhysicsManager() { return _physicsManager; }
	RenderingManager* getRenderingManager() { return _renderingManager; }

	Scenes _scene;
	unsigned int _cursorPositionStart;
	unsigned int _cursorPositionSetup;
	unsigned int _cursorPositionPause;
	unsigned int delayY;
	double delayX;

private:
	static Broker* _instance;
	Broker();

	AIManager *_aiManager = nullptr;
	AudioManager *_audioManager = nullptr;
	InputManager * _inputManager = nullptr;
	LoadingManager * _loadingManager = nullptr;
	PhysicsManager *_physicsManager = nullptr;
	RenderingManager *_renderingManager = nullptr;
	Gamepad* player1 = nullptr;
};










#endif // BROKER_H_
