#include "broker.h"
#include <iostream>
#include <Windows.h>
#include <ctime>

int main() {

	srand(time(NULL)); // set the seed for all calls to rand(), seed will be different on every run of program 

	// initial loading... (slow)
	Broker *broker = Broker::getInstance();
	broker->initAll();
	GLFWwindow *window = broker->getRenderingManager()->getWindow();

	// !!!NOTE: all time variables are in SECONDS!!! 
	// SIMILAR TO https://gafferongames.com/post/fix_your_timestep/
	double simTime = 0.0; // total accumulation of simulated physics timesteps
	const double fixedDeltaTime = 1.0 / 60.0;
	double prevTime = glfwGetTime(); // needs to be set here after initialization, gets time in SECONDS!!!
	double accumulator = 0.0;

	// call main loop
	while (!glfwWindowShouldClose(window)) {
		double currentTime = glfwGetTime(); // gets time in SECONDS!!!
		double variableDeltaTime = currentTime - prevTime;
		prevTime = currentTime;

		if (broker->_scene == GAME) {
			accumulator += variableDeltaTime;
		}

		broker->updateAllSeconds(simTime, fixedDeltaTime, variableDeltaTime, accumulator);

	}

	// if main loop ends, call cleanup
	return 0;
}