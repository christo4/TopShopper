


#ifndef RENDERINGMANAGER_H_
#define RENDERINGMANAGER_H_

#include <GLFW/glfw3.h>

class Broker;

class RenderingManager {
public:
	RenderingManager(Broker* broker);
	virtual ~RenderingManager();

	void init();
	void updateMilliseconds(double deltaTime);
	void cleanup();

	GLFWwindow* getWindow();
private:
	Broker* _broker = nullptr;

	GLFWwindow* _window = nullptr;

	void openWindow();
};

#endif // RENDERINGMANAGER_H_
