#include "broker.h"
#include <iostream>

int main() {
	//load assets here
	double prevTime = 0.0;
	Broker* broker = Broker::getInstance();
	broker->initAll();
	GLFWwindow* window = broker->get_RenderingManager_Window();



	// call main loop
	while (!glfwWindowShouldClose(window)) {

		
		int gamePadPresent = glfwJoystickPresent(GLFW_JOYSTICK_1);
		//std::cout << "Gamepad Status: " << gamePadPresent << std::endl;

		int numAxes;
		int numButtons;
		const float * axesArray = NULL;
		const unsigned char * buttonArray = NULL;



		if (gamePadPresent) {

			axesArray = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &numAxes);
			std::cout << std::endl;
			std::cout << std::endl;

			
			std::cout << "Left Stick X: " << axesArray[0] << std::endl;
			std::cout << "Left Stick Y: " << axesArray[1] << std::endl;
			std::cout << "Right Stick X: " << axesArray[2] << std::endl;
			std::cout << "Right Stick Y: " << axesArray[3] << std::endl;
			std::cout << "Left Trigger: " << axesArray[4] << std::endl;
			std::cout << "Right Trigger: " << axesArray[5] << std::endl;

			


			buttonArray = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &numButtons);

			//button array: A B X Y LB RB LEFTSTICK RIGHTSTICK UP RIGHT LEFT DOWN

			/*
			for (int i = 0; i < numButtons; i++) {

				if (buttonArray[i] == GLFW_PRESS) {
					std::cout << i << " " << "press" << std::endl;
				}
			}

			*/
		}


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