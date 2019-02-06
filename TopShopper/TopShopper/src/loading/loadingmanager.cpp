


#include "loadingmanager.h"
#include <iostream>
#define _CRT_SECURE_NO_WARNINGS

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "rendering/Geometry.h"
#include "OBJ_Loader.h"


using namespace objl;



Geometry gVehicleChassisGeo;
Geometry gVehicleWheelGeo;
Geometry gGroundGeo;



LoadingManager::LoadingManager(Broker* broker) {
}

LoadingManager::~LoadingManager() {

}

void LoadingManager::init() {
	std::vector<glm::vec4>returnVertices;
	std::vector<glm::vec2>returnUV;
	std::vector<glm::vec3>returnNormal;
	std::vector<unsigned int>returnIndex;

	loadObject("../TopShopper/resources/Objects/rect.obj", returnVertices, returnUV, returnNormal, returnIndex);

	gVehicleChassisGeo.verts = returnVertices;
	gVehicleChassisGeo.uvs = returnUV;
	gVehicleChassisGeo.normals = returnNormal;
	gVehicleChassisGeo.indices = returnIndex;

	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	returnIndex.clear();

	// THEN DO NEXT GEO BELOW...
}

void LoadingManager::updateMilliseconds(double deltaTime) {

}



void LoadingManager::loadObject(const char* imageName, std::vector<glm::vec4>&returnVertices, std::vector<glm::vec2>&returnUV, std::vector<glm::vec3>&returnNormal, std::vector<unsigned int>&returnIndex) {
	
	objl::Loader loader;
	bool loaded = loader.LoadFile(imageName);
	if (!loaded) {
		std::exit(1);
	}
	
	for (Vertex v : loader.LoadedVertices) {
		glm::vec4 pos;
		glm::vec2 uv;
		glm::vec3 norm;
		pos.x = v.Position.X;
		pos.y = v.Position.Y;
		pos.z = v.Position.Z;
		pos.w = 1.0;

		uv.x = v.TextureCoordinate.X;
		uv.y = v.TextureCoordinate.Y;

		norm.x = v.Normal.X;
		norm.y = v.Normal.Y;
		norm.z = v.Normal.Z;
		returnVertices.push_back(pos);
		returnUV.push_back(uv);
		returnNormal.push_back(norm);
	}
	
	returnIndex = loader.LoadedIndices;
	
}
