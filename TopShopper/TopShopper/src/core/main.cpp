#include "broker.h"
#include <iostream>
#include <Windows.h>

int main() {
	//load assets here
	double prevTime = 0.0;
	Broker* broker = Broker::getInstance();
	broker->initAll();
	GLFWwindow* window = broker->get_RenderingManager_Window();
	std::vector<glm::vec3> returnVertex;
	std::vector<glm::vec2> returnUV; 
	std::vector<glm::vec3> returnNormal;
	broker->loadObj("../TopShopper/Objects/rectangle.obj", returnVertex, returnUV, returnNormal);


	// call main loop
	while (!glfwWindowShouldClose(window)) {

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