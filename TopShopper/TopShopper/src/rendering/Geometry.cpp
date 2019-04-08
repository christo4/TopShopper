/*
 * Geometry.cpp
 *
 *  Created on: Sep 10, 2018
 *      Author: John Hall
 */

#include "Geometry.h"

Geometry::Geometry() : vao(0), vertexBuffer(0), normalBuffer(0), uvBuffer(0), colorBuffer(0), gradientShader(false), cullBackFace(false), isTransparent(false){
	//vectors are initially empty
	//Pointers are initially null
	//Call RenderingEngine::assignBuffers and RenderingEngine::setBufferData to fully initialize the geometry
	//Overload the constructor for more functionality or create subclasses for specific objects
}

Geometry::~Geometry() {

}