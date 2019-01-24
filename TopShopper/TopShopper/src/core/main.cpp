


#include "broker.h"

int main() {
	// ~~~~~load assets (init necessary data structures)???
	
	double prevTime = 0.0;

	Broker::getInstance()->initAll(); // init all subsystems

	// call main loop
	while (true) { // ~~~~~will be changed to have a proper end condition later - while user hasnt closed window)~~~~~
		// 1. get deltaTime (time last frame took to execute)
		// 2. get device input
		// 3. simulate (update game state)
		// 4. render (output)
		// 5. sound (output)

		// new way:
		// 1. get deltaTime
		// 2. call Broker::getInstance()->updateAllMilliseconds(deltaTime);


		// ~~~~~NOTE: I think that the first deltaTime could screw up some stuff
		//double currentTime = glfwGetTime(); // get new
		double currentTime = 0.03;
		double deltaTime = currentTime - prevTime; // diff
		prevTime = currentTime; // update

		Broker::getInstance()->updateAllMilliseconds(deltaTime);

	}

	// if main loop ends, call cleanup
	return 0;
}