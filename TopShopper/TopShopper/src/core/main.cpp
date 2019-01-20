
#include "rendering/renderingmanager.h"
class RenderingManager;

int main() {
	// load assets (init necessary data structures)

	// ~~~~~~~~~~~maybe have Broker::getInstance()->initAll(); that would init all subsytems based on data in components attached to each entity
	
	// call main loop
	while (true) { // ~~~~~will be changed to have a proper end condition later)~~~~~
		// 1. get deltaTime (time last frame took to execute)
		// 2. get device input
		// 3. simulate (update game state)
		// 4. render (output)
		// 5. sound (output)

		// new way:
		// 1. get deltaTime
		// 2. call Broker::getInstance()->updateAllMilliseconds(deltaTime);

		 RenderingManager* renderingmanager = new RenderingManager();

	}

	// if main loop ends, call cleanup
	return 0;
}