


#ifndef RENDERINGMANAGER_H_
#define RENDERINGMANAGER_H_

#include <GLFW/glfw3.h>

class RenderingManager {
public:
	RenderingManager();
	virtual ~RenderingManager();

	void init();
	void updateMilliseconds(double deltaTime);
	void cleanup();

	GLFWwindow* getWindow();
private:
	GLFWwindow* _window = nullptr;

	void openWindow();
};











#endif // RENDERINGMANAGER_H_
