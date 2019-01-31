/*
 * Program.cpp
 *
 *  Created on: Sep 10, 2018
 *      Author: John Hall
 */

#include "Program.h"

#include <iostream>
#include <string>

//**Must include glad and GLFW in this order or it breaks**
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "RenderingEngine.h"
#include "Scene.h"

Program::Program() {
	setupWindow();
}

Program::~Program() {
	//Must be cleaned up in the destructor because these are allocated to the heap
	delete renderingEngine;
	delete scene;
}

void Program::start() {
	renderingEngine = new RenderingEngine();
	scene = new Scene(renderingEngine, window);

	bool check = scene->objectLoader("sphere.obj", vertex, uv, normal);
	if (check == false){
		std::exit(0);
	}

	scene->sphereMaker((scene->theta), scene->radius, scene->phi, vertex, uv, normal);

	//Main render loop
	while(!glfwWindowShouldClose(window)) {
		scene->displayScene();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

}

void Program::setupWindow() {
	//Initialize the GLFW windowing system
	if (!glfwInit()) {
		std::cout << "ERROR: GLFW failed to initialize, TERMINATING" << std::endl;
		return;
	}

	//Set the custom error callback function
	//Errors will be printed to the console
	glfwSetErrorCallback(ErrorCallback);

	//Attempt to create a window with an OpenGL 4.1 core profile context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	int width = 512;
	int height = 512;
	window = glfwCreateWindow(width, height, "CPSC 453 OpenGL Boilerplate", 0, 0);
	if (!window) {
		std::cout << "Program failed to create GLFW window, TERMINATING" << std::endl;
		glfwTerminate();
		return;
	}

	//Set the custom function that tracks key presses
	glfwSetKeyCallback(window, KeyCallback);

	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	glfwSetCursorPosCallback(window, cursor_position_callback);

	//Bring the new window to the foreground (not strictly necessary but convenient)
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);


	//Intialize GLAD (finds appropriate OpenGL configuration for your system)
	if (!gladLoadGL()) {
		std::cout << "GLAD init failed" << std::endl;
		return;
	}

	//Query and print out information about our OpenGL environment
	QueryGLVersion();
}



void Program::QueryGLVersion() {
	// query opengl version and renderer information
	std::string version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	std::string glslver = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	std::string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

	std::cout << "OpenGL [ " << version << " ] "
		<< "with GLSL [ " << glslver << " ] "
		<< "on renderer [ " << renderer << " ]" << std::endl;
}

void ErrorCallback(int error, const char* description) {
	std::cout << "GLFW ERROR " << error << ":" << std::endl;
	std::cout << description << std::endl;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//Key codes are often prefixed with GLFW_KEY_ and can be found on the GLFW website
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
		std::exit(0);
	}
	Program* p = (Program*)glfwGetWindowUserPointer(window);
	Scene* s = p->getScene();
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		if(s->radius < 100){
			s->radius += 2;
			s->sphereMaker((s->theta), s->radius, s->phi, p->vertex, p->uv, p->normal);
		}
		
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		if(s->radius > 0){
			s->radius -= 2;
			s->sphereMaker((s->theta), s->radius, s->phi, p->vertex, p->uv, p->normal);
		}
	}
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
	
	Program* p = (Program*)glfwGetWindowUserPointer(window);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		p->mouseIsPressed = true;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
		p->mouseIsPressed = false;
	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
	Program* p = (Program*)glfwGetWindowUserPointer(window);
	Scene* s = p->getScene();
	
	if (p->mouseIsPressed == true){
		double diffX = xpos - p->oldX;
		if (diffX > 0){
			if (s->phi < 20){
				s->phi += 0.05;
				s->sphereMaker((s->theta), s->radius, s->phi, p->vertex, p->uv, p->normal);
			}
		}

		if (diffX < 0){
			if (s->phi > 0){
				s->phi -= 0.05;
				s->sphereMaker((s->theta), s->radius, s->phi, p->vertex, p->uv, p->normal);
			}
		}
		
		double diffY = ypos - p->oldY;
		if (diffY > 0){
			if (s->theta < 180){
				s->theta += 1;
				s->sphereMaker((s->theta), s->radius, s->phi, p->vertex, p->uv, p->normal);
			}
			
		}
		if (diffY < 0){
			if (s->theta > 1){
				s->theta -= 1;
				s->sphereMaker((s->theta), s->radius, s->phi, p->vertex, p->uv, p->normal);
			}
		}

		p->oldX = xpos;
		p->oldY = ypos; 
	}
	if (p->mouseIsPressed == false){
		p->oldX = xpos;
		p->oldY = ypos;
	}
	
}