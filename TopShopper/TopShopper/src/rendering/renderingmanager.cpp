


#include "renderingmanager.h"
#include "../core/broker.h"
#include "../loading/loadingmanager.h"
#include <iostream>
#include <string>


//**Must include glad and GLFW in this order or it breaks**
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "RenderingEngine.h"


RenderingManager::RenderingManager(Broker* broker): _broker(broker){
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
	renderingEngine = new RenderingEngine();
	glm::mat4 mvp = Camera(60.0f, 90.0f, 0.0f);
	renderingEngine->setCamera(mvp);
}

glm::mat4 RenderingManager::Camera(float theta, float radius, float phi) {
	int height;
	int width;
	glfwGetWindowSize(_window, &width, &height);
	//float fov = 60.0f;

	glm::mat4 Projection = glm::perspective(glm::radians(theta), (float)width / (float)height, 0.1f, 200.0f);


	float x, y, z;

	theta *= 3.14f / 180.f;
	x = float(radius * sin(theta) * sin(phi));
	y = float(radius * cos(theta));
	z = float(radius * cos(phi) * sin(theta));

	glm::mat4 View = glm::lookAt(
		glm::vec3(x, y, z), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // up vector
	);

	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 mvp = Projection * View * Model;
	return mvp;
}


void RenderingManager::updateMilliseconds(double deltaTime) {

	Geometry  meme = *(_broker->get_LoadingManager_Geometry(VEHICLE_CHASSIS_GEO));


	meme.drawMode = GL_LINE_STRIP;
	//meme.drawMode = GL_TRIANGLES;

	renderingEngine->assignBuffers(meme);
	renderingEngine->setBufferData(meme);
	

	_objects.push_back(meme);


	renderingEngine->RenderScene(_objects);
	glfwSwapBuffers(_window);
}



void RenderingManager::cleanup() {
	glfwTerminate();
}



void RenderingManager::openWindow() {

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
	_window = glfwCreateWindow(width, height, "Top Shopper", 0, 0);
	if (!_window) {
		std::cout << "Program failed to create GLFW window, TERMINATING" << std::endl;
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(_window);
	glfwSetWindowUserPointer(_window, this);

	if (!gladLoadGL()) {
		std::cout << "GLAD init failed" << std::endl;
		return;
	}

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

