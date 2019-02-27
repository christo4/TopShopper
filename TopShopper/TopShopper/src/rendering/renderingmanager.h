#ifndef RENDERINGMANAGER_H_
#define RENDERINGMANAGER_H_

//**Must include glad and GLFW in this order or it breaks**
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Geometry.h"

class Broker; 
struct GLFWwindow;

class RenderingManager {
public:
	RenderingManager(Broker *broker);
	virtual ~RenderingManager();


	void init();
	void updateSeconds(double variableDeltaTime);
	void cleanup();

	//Renders each object
	void RenderScene(std::vector<Geometry>& objects);

	//Create vao and vbos for objects
	static void assignBuffers(Geometry& geometry);
	static void setBufferData(Geometry& geometry);
	static void deleteBufferData(Geometry& geometry);

	//Ensures that vao and vbos are set up properly
	bool CheckGLErrors();

	//Pointer to the current shader program being used to render
	GLuint shaderProgram;

	GLFWwindow* getWindow();

	void QueryGLVersion();

	glm::mat4 Camera(float theta, float radius, float phi);

private:

	Broker *_broker = nullptr;

	GLFWwindow *_window = nullptr;

	std::vector<Geometry> _objects;

	void openWindow();
};

#endif // RENDERINGMANAGER_H_
