


#include "broker.h"

int main() {
	// ~~~~~load assets (init necessary data structures)???
	
	// fields...
	double prevTime = 0.0;
	Broker* broker = Broker::getInstance();

	// work...
	broker->initAll(); // init all subsystems

	GLFWwindow* window = broker->get_RenderingManager_Window();

	// call main loop
	while (!glfwWindowShouldClose(window)) { // ~~~~~~~~~ also probably add || QUIT menu option was pressed.
		// 1. get deltaTime (time last frame took to execute)
		// 2. get device input
		// 3. simulate (update game state)
		// 4. render (output)
		// 5. sound (output)

		// new way:
		// 1. get deltaTime
		// 2. call Broker::getInstance()->updateAllMilliseconds(deltaTime);


		// ~~~~~NOTE: I think that the first deltaTime could screw up some stuff
		double currentTime = glfwGetTime(); // get new
		double deltaTime = currentTime - prevTime; // diff
		prevTime = currentTime; // update

		broker->updateAllMilliseconds(deltaTime);

	}

	// if main loop ends, call cleanup
	return 0;
}