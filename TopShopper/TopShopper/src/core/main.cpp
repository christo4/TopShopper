#include "broker.h"
#include <iostream>
#include <Windows.h>

int main() {
	// initial loading... (slow)
	Broker *broker = Broker::getInstance();
	broker->initAll();
	GLFWwindow *window = broker->getRenderingManager()->getWindow();

	// SIMILAR TO https://gafferongames.com/post/fix_your_timestep/
	double simTime = 0.0; // total accumulation of simulated physics timesteps
	const double fixedDeltaTime = 1.0 / 60.0;
	double prevTime = glfwGetTime(); // needs to be set here after initialization
	double accumulator = 0.0;

	// call main loop
	while (!glfwWindowShouldClose(window)) {

		double currentTime = glfwGetTime();
		double variableDeltaTime = currentTime - prevTime;
		prevTime = currentTime;

		accumulator += variableDeltaTime;

		broker->updateAllMilliseconds(simTime, fixedDeltaTime, variableDeltaTime, accumulator);

	}

	// if main loop ends, call cleanup
	return 0;
}