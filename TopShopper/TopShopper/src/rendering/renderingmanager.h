


#ifndef RENDERINGMANAGER_H_
#define RENDERINGMANAGER_H_

#include "Geometry.h"

class Broker; 
struct GLFWwindow;
class RenderingEngine;
//class Scene;

class RenderingManager {
public:
	RenderingManager(Broker* broker);
	virtual ~RenderingManager();

	void init();
	void updateMilliseconds(double deltaTime);
	void cleanup();

	GLFWwindow* getWindow();

	void QueryGLVersion();

	glm::mat4 Camera(float theta, float radius, float phi);


private:

	Broker* _broker = nullptr;

	GLFWwindow* _window = nullptr;

	RenderingEngine* renderingEngine;

	std::vector<Geometry> _objects;

	

	void openWindow();
};

#endif // RENDERINGMANAGER_H_
