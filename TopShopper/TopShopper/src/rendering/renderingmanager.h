


#ifndef RENDERINGMANAGER_H_
#define RENDERINGMANAGER_H_

#include <GLFW/glfw3.h>

class Broker; 
struct GLFWwindow;
class RenderingEngine;
class Scene;

class RenderingManager {
public:
	RenderingManager(Broker* broker);
	virtual ~RenderingManager();

	void init();
	void updateMilliseconds(double deltaTime);
	void cleanup();

	GLFWwindow* getWindow();

	void QueryGLVersion();

	Scene* getScene() { return scene; }

private:
	Broker* _broker = nullptr;

	GLFWwindow* _window = nullptr;

	RenderingEngine* renderingEngine;
	Scene* scene;

	void openWindow();
};

#endif // RENDERINGMANAGER_H_
