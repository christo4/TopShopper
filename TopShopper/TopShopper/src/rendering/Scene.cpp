/*
 * Scene.cpp
 *
 *  Created on: Sep 10, 2018
 *  Author: John Hall
 */

#include "Scene.h"

#include <iostream>

#include "RenderingEngine.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//**Must include glad and GLFW in this order or it breaks**
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "texture.h"

Scene::Scene(RenderingEngine* renderer, GLFWwindow* window) : renderer(renderer), window(window), theta(60.0f), radius(90.0f), phi(0.0f){

	
	MyTexture texture;
	InitializeTexture(&texture, "sunmap.jpg", GL_TEXTURE_2D);

	//Load texture uniform
	//Shaders need to be active to load uniforms
	glUseProgram(renderer->shaderProgram);
	//Set which texture unit the texture is bound to
	glActiveTexture(GL_TEXTURE0);
	//Bind the texture to GL_TEXTURE0
	glBindTexture(GL_TEXTURE_2D, texture.textureID);
	//Get identifier for uniform
	GLuint uniformLocation = glGetUniformLocation(renderer->shaderProgram, "imageTexture");
	//Load texture unit number into uniform
	glUniform1i(uniformLocation, 0);
	



	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if(renderer->CheckGLErrors()) {
		std::cout << "Texture creation failed" << std::endl;
	}

	glfwGetWindowSize(window, &width, &height);

}

Scene::~Scene() {

}

void Scene::displayScene() {
	renderer->RenderScene(objects);
}


void Scene::sphereMaker(float newTheta, float newRadius, float newPhi, std::vector<glm::vec3> vertex, std::vector<glm::vec2> uv, std::vector<glm::vec3> normal){
	objects.clear();

	// all images from http://planetpixelemporium.com/earth.html
	Geometry sun;
	Geometry earth;
	Geometry moon;
	Geometry mars;

	double sunRadius = double(695508);
	double earthRadius = double(6371);
	double moonRadius = double(1737);
	double marsRadius = double(3390);

	double sunScale = log10(sunRadius)*3;
	double earthScale = log10(earthRadius)*3;
	double moonScale = log10(moonRadius)*3;
	double marsScale = log10(marsRadius)*3;


	glm::mat4 mvp = projection(newTheta, newRadius, newPhi);

	glm::mat4 sunWorld = glm::mat4(1.0f);

	glm::mat4 earthWorld = glm::scale(sunWorld, glm::vec3(earthScale, earthScale, earthScale));
	
	glm::mat4 moonWorld = glm::scale(sunWorld, glm::vec3(moonScale, moonScale, moonScale));

	glm::mat4 marsWorld = glm::scale(sunWorld, glm::vec3(marsScale, marsScale, marsScale));

	marsWorld = glm::translate(marsWorld, glm::vec3(4, 0, 0));

	earthWorld = glm::translate(earthWorld, glm::vec3(6, 0, 0));

	moonWorld = glm::translate(earthWorld, glm::vec3(2, 0, 0));

	sunWorld = glm::scale(sunWorld, glm::vec3(sunScale, sunScale, sunScale));

	
	
	sun.drawMode = GL_TRIANGLES;
	// three vertex positions and assocated colours of a triangle
	for(glm::vec3 each : vertex){
		glm::vec4 meme = mvp * sunWorld * glm::vec4(each, 1.0);
		sun.verts.push_back(meme);
	}

	for(glm::vec2 each : uv){
		sun.uvs.push_back(each);
	}

	//Construct vao and vbos for the triangle
	RenderingEngine::assignBuffers(sun);

	//Send the triangle data to the GPU
	//Must be done every time the triangle is modified in any way, ex. verts, colors, normals, uvs, etc.
	RenderingEngine::setBufferData(sun);

	//Add the triangle to the scene objects
	objects.push_back(sun);

	
	earth.drawMode = GL_TRIANGLES;
	// three vertex positions and assocated colours of a triangle
	for(glm::vec3 each : vertex){
		glm::vec4 meme = mvp * earthWorld * glm::vec4(each, 1.0);
		earth.verts.push_back(meme);
	}

	for(glm::vec2 each : uv){
		earth.uvs.push_back(each);
	}

	//Construct vao and vbos for the triangle
	RenderingEngine::assignBuffers(earth);

	//Send the triangle data to the GPU
	//Must be done every time the triangle is modified in any way, ex. verts, colors, normals, uvs, etc.
	RenderingEngine::setBufferData(earth);

	//Add the triangle to the scene objects
	objects.push_back(earth);


	moon.drawMode = GL_TRIANGLES;
	// three vertex positions and assocated colours of a triangle
	for(glm::vec3 each : vertex){
		glm::vec4 meme = mvp * moonWorld * glm::vec4(each, 1.0);
		moon.verts.push_back(meme);
	}

	for(glm::vec2 each : uv){
		moon.uvs.push_back(each);
	}

	//Construct vao and vbos for the triangle
	RenderingEngine::assignBuffers(moon);

	//Send the triangle data to the GPU
	//Must be done every time the triangle is modified in any way, ex. verts, colors, normals, uvs, etc.
	RenderingEngine::setBufferData(moon);

	//Add the triangle to the scene objects
	objects.push_back(moon);
	

	
	mars.drawMode = GL_TRIANGLES;
	// three vertex positions and assocated colours of a triangle
	for(glm::vec3 each : vertex){
		glm::vec4 meme = mvp * marsWorld * glm::vec4(each, 1.0);
		mars.verts.push_back(meme);
	}

	for(glm::vec2 each : uv){
		mars.uvs.push_back(each);
	}

	//Construct vao and vbos for the triangle
	RenderingEngine::assignBuffers(mars);

	//Send the triangle data to the GPU
	//Must be done every time the triangle is modified in any way, ex. verts, colors, normals, uvs, etc.
	RenderingEngine::setBufferData(mars);

	//Add the triangle to the scene objects
	objects.push_back(mars);
	
	

}

glm::mat4 Scene::projection(float newTheta, float newRadius, float newPhi){
	glm::mat4 Projection = glm::perspective(glm::radians(newRadius), (float)width / (float)height, 0.1f, 100.0f);

	float x, y, z;
	
	newTheta *= 3.14f / 180.f;
	x = float(newRadius * sin(newTheta) * sin(newPhi));
	y = float(newRadius * cos(newTheta));
	z = float(newRadius * cos(newPhi) * sin(newTheta));

	//std::cout << x << " x,"  << y << " y, " << z << " z" << "\n";

	glm::mat4 View = glm::lookAt(
		glm::vec3(x, y, z), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 mvp = Projection * View * Model;
	return mvp;
}


//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
bool Scene::objectLoader(const char* imageName, std::vector<glm::vec3>&returnVertex, std::vector<glm::vec2>&returnUV, std::vector<glm::vec3>&returnNormal) {
	std::vector<unsigned int> vIndex;
	std::vector<unsigned int> uvIndex;
	std::vector<unsigned int> normalIndex;
	std::vector<glm::vec3> inputVertex;
	std::vector<glm::vec2> inputUV;
	std::vector<glm::vec3> inputNormal;

	FILE * inputFile = fopen(imageName, "r");
	if(inputFile == NULL){
		return false;
		std::exit(0);
	}
	while (true){

		char firstWordInLine[365];

		int check = fscanf(inputFile, "%s", firstWordInLine);
		if (check == EOF) break;

		if ( strcmp( firstWordInLine, "v") == 0 ){
			glm::vec3 v;
			fscanf(inputFile, "%f %f %f\n", &v.x, &v.y, &v.z);
			inputVertex.push_back(v);
		}
		else if ( strcmp( firstWordInLine, "vt") == 0 ){
			glm::vec3 uv;
			fscanf(inputFile, "%f %f\n", &uv.x, &uv.y);
			inputUV.push_back(uv);
		}
		else if ( strcmp( firstWordInLine, "vn") == 0 ){
			glm::vec3 n;
			fscanf(inputFile, "%f %f %f\n", &n.x, &n.y, &n.z);
			inputNormal.push_back(n);
		}
		else if ( strcmp( firstWordInLine, "f") == 0 ){
			std::string v1, v2, v3;
			unsigned int vert1[3], vert2[3], vert3[3];
			int matches = fscanf(inputFile, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vert1[0], &vert2[0], &vert3[0], &vert1[1], &vert2[1], &vert3[1], &vert1[2], &vert2[2], &vert3[2] );
			
			if (matches != 9){
				return false;
				std::exit(0);
			}

			vIndex.push_back(vert1[0]);
			vIndex.push_back(vert1[1]);
			vIndex.push_back(vert1[2]);

			uvIndex.push_back(vert2[0]);
			uvIndex.push_back(vert2[1]);
			uvIndex.push_back(vert2[2]);

			normalIndex.push_back(vert3[0]);
			normalIndex.push_back(vert3[1]);
			normalIndex.push_back(vert3[2]);
		}
	}
	for (int i = 0; i < vIndex.size(); i++){
		unsigned int currentIndex = vIndex[i];
		glm::vec3 vert = inputVertex[currentIndex - 1];
		returnVertex.push_back(vert);
	}

	for (int i = 0; i < uvIndex.size(); i++){
		unsigned int currentIndex = uvIndex[i];
		glm::vec2 vert = inputUV[currentIndex - 1];
		returnUV.push_back(vert);
	}

	for (int i = 0; i < normalIndex.size(); i++){
		unsigned int currentIndex = normalIndex[i];
		glm::vec3 vert = inputNormal[currentIndex - 1];
		returnNormal.push_back(vert);
	}

	return true;
}

