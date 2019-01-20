

#include "renderingmanager.h"

#include <GLFW/glfw3.h>
#include <iostream>

RenderingManager::RenderingManager() {
	openWindow();
}

RenderingManager::~RenderingManager() {

}

void RenderingManager::openWindow() {
	if (glfwInit() == 0) {
		exit(EXIT_FAILURE);
	}

	GLFWwindow* window = glfwCreateWindow(1024, 768, "Test window", NULL, NULL);
	glfwMakeContextCurrent(window);

	if (!window) {
		std::cout << "window not created successfully" << std::endl;
	}

	while (true) {
	}
}