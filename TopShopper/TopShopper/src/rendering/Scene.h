/*
 * Scene.h
 *	Class for storing objects in a scene
 *  Created on: Sep 10, 2018
 *      Author: cb-ha
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <vector>

#include "Geometry.h"

//Forward declaration of classes
//(note this is necessary because these are pointers and it allows the #include to appear in the .cpp file)
class RenderingEngine;

class Scene {
public:
	Scene(RenderingEngine* renderer, GLFWwindow* window);
	virtual ~Scene();

	//Send geometry to the renderer
	void displayScene();
	bool objectLoader(const char* imageName, std::vector<glm::vec3>&returnVertex, std::vector<glm::vec2>&returnUV, std::vector<glm::vec3>&returnNormal);
	void sphereMaker(float newTheta, float radius, float phi, std::vector<glm::vec3> vertex, std::vector<glm::vec2> uv,	std::vector<glm::vec3> normal);
	float theta;
	float radius;
	float phi;
	glm::mat4 projection(float newTheta, float newRadius, float newPhi);

	/*
	void setVertex(glm::vec3 vertex);
	void setUv(glm::vec2 uv);
	void setNormal(glm::vec3 normal);

	vector<glm::vec3> getVertex();
	vector<glm::vec2> getUv();
	vector<glm::vec3> getNormal();

	*/




private:
	RenderingEngine* renderer;
	GLFWwindow* window;
	int width;
	int height;

	//list of objects in the scene
	std::vector<Geometry> objects;
};

#endif /* SCENE_H_ */
