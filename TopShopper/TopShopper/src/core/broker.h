


#ifndef BROKER_H_
#define BROKER_H_

#include "ai/aimanager.h"
#include "audio/audiomanager.h"
#include "input/inputmanager.h"
#include "loading/loadingmanager.h"
#include "physics/physicsmanager.h"
#include "rendering/renderingmanager.h"


// SINGLETON...
class Broker {
public:
	static Broker* getInstance();

	void updateAllMilliseconds(float deltaTime); // this update function will call each subsystem's update function in an appropriate order.
	// have a bunch of functions that connect subsystems (e.g. physics system needs to know (read-only) audio info of an entity (expressed through sum of its components), and has to ask the broker to get that info from audioManager)
private:
	static Broker* _instance;
	Broker();

	// ~~~~~properly init these later after implementing them and figuring out their constructors
	//AIManager _aiManager;
	//AudioManager _audioManager;
	// input manager
	// loading manager
	//PhysicsManager _physicsManager;
	//RenderingManager _renderingManager;

};










#endif // BROKER_H_
