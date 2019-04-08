/*
 * Geometry.h
 *	Class for storing geometry
 *  Created on: Sep 10, 2018
 *      Author: John Hall
 */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

 //**Must include glad and GLFW in this order or it breaks**
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "texture.h"

class Geometry {
public:
	Geometry();
	virtual ~Geometry();


	glm::mat4 model;
	glm::vec3 color;
	MyTexture texture;
	bool gradientShader;
	bool cullBackFace;
	bool isTransparent;

	//Data structures for storing vertices, normals colors and uvs
	std::vector<glm::vec4> verts;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec2> uvs;

	//std::vector<unsigned int> indices; // NOTE: these indices will start from 0, rather than 1 as in a .obj file (since both OpenGL and PhysX require 0-indexing)
	std::vector<unsigned int>vIndex;
	std::vector<unsigned int>uvIndex;
	std::vector<unsigned int>normalIndex;


	//Pointers to the vao and vbos associated with the geometry
	GLuint vao;
	GLuint vertexBuffer;
	GLuint normalBuffer;
	GLuint uvBuffer;
	GLuint colorBuffer;

	//Draw mode for how OpenGL interprets primitives
	GLuint drawMode;
	//MyTexture texture;


};

#endif /* GEOMETRY_H_ */
