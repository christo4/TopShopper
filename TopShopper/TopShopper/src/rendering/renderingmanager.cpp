


#include "renderingmanager.h"
#include <iostream>



RenderingManager::RenderingManager(Broker* broker) 
	: _broker(broker)
{

}

RenderingManager::~RenderingManager() {

}

GLFWwindow* RenderingManager::getWindow() {
	return _window;
}



void RenderingManager::init() {
	openWindow();
}


void RenderingManager::updateMilliseconds(double deltaTime) {
	// render stuff...
	glClear(GL_COLOR_BUFFER_BIT);

	// swap front and back buffers
	glfwSwapBuffers(_window);

	// poll for events (probably move into inputManager later on)
	glfwPollEvents();

	// any other stuff...
}



void RenderingManager::cleanup() {
	glfwTerminate();
}



void RenderingManager::openWindow() {
	// init the library
	if (glfwInit() == 0) {
		exit(EXIT_FAILURE);
	}

	//_window = glfwCreateWindow(1024, 576, "Top Shopper", glfwGetPrimaryMonitor(), NULL); // full screen window (currently unresponsive and can't close)
	_window = glfwCreateWindow(1024, 768, "Top Shopper", NULL, NULL);
	if (!_window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(_window);

}



