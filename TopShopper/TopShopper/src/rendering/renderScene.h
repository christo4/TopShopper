/*
 * Scene.h
 *	Class for storing objects in a scene
 *  Created on: Sep 10, 2018
 *      Author: cb-ha
 */

#ifndef RENDERSCENE_H_
#define RENDERSCENE_H_

#include <vector>


//Forward declaration of classes
//(note this is necessary because these are pointers and it allows the #include to appear in the .cpp file)
class renderingEngine;

struct Geometry {
	GLuint vao;
	GLuint vertexBuffer;
	GLuint normalBuffer;
	GLuint uvBuffer;

	GLuint drawMode;
};	

class renderScene {
public:
	renderScene(renderingEngine* renderer, GLFWwindow* window);
	virtual ~renderScene();

	//Send geometry to the renderer
	void displayScene();
	void sphereMaker(float newTheta, float radius, float phi, std::vector<glm::vec3> vertex, std::vector<glm::vec2> uv,	std::vector<glm::vec3> normal);
	float theta;
	float radius;
	float phi;
	glm::mat4 projection(float newTheta, float newRadius, float newPhi);


private:
	renderingEngine* renderer;
	GLFWwindow* window;
	int width;
	int height;

	//list of objects in the scene
	std::vector<Geometry> objects;
};

#endif /* SCENE_H_ */
