


#include "renderingmanager.h"
#include "../core/broker.h"
#include "../loading/loadingmanager.h"
#include <iostream>
#include <string>


//**Must include glad and GLFW in this order or it breaks**
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "RenderingEngine.h"


RenderingManager::RenderingManager(Broker* broker) : _broker(broker){
	openWindow();
	init();
}

RenderingManager::~RenderingManager() {
	delete renderingEngine;
}

GLFWwindow* RenderingManager::getWindow() {
	return _window;
}



void RenderingManager::init() {
	//openWindow();
	renderingEngine = new RenderingEngine();
}


void RenderingManager::updateMilliseconds(double deltaTime) {
	// render stuff...
	
	Geometry  meme = *(_broker->get_LoadingManager_Geometry(VEHICLE_CHASSIS_GEO));


	meme.drawMode = GL_LINE_STRIP;

	renderingEngine->assignBuffers(meme);
	renderingEngine->setBufferData(meme);


	_objects.push_back(meme);


	renderingEngine->RenderScene(_objects);
	
	
	// swap front and back buffers
	glfwSwapBuffers(_window);

	// any other stuff...
}



void RenderingManager::cleanup() {
	glfwTerminate();
}



void RenderingManager::openWindow() {
	/*
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
	*/


	//Initialize the GLFW windowing system
	if (!glfwInit()) {
		std::cout << "ERROR: GLFW failed to initialize, TERMINATING" << std::endl;
		return;
	}

	//Attempt to create a window with an OpenGL 4.1 core profile context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	int width = 1024;
	int height = 512;
	_window = glfwCreateWindow(width, height, "TopShopper", 0, 0);
	if (!_window) {
		std::cout << "Program failed to create GLFW window, TERMINATING" << std::endl;
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(_window);
	glfwSetWindowUserPointer(_window, this);


	//Intialize GLAD (finds appropriate OpenGL configuration for your system)
	if (!gladLoadGL()) {
		std::cout << "GLAD init failed" << std::endl;
		return;
	}

	//Query and print out information about our OpenGL environment
	QueryGLVersion();
}

void RenderingManager::QueryGLVersion() {
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

