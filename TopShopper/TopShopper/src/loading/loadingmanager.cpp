#include "loadingmanager.h"
#include <iostream>
//#define _CRT_SECURE_NO_WARNINGS // this shouldnt be necessary since I already defined it in properties>c++>preprocessor

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "rendering/Geometry.h"
#include "OBJ_Loader.h"


using namespace objl;

Geometry* VehicleChassisGeo = new Geometry();
Geometry* VehicleWheelGeo = new Geometry();
Geometry* GroundGeo = new Geometry();
Geometry* SpareChangeGeo = new Geometry();


LoadingManager::LoadingManager(Broker* broker) {
}

LoadingManager::~LoadingManager() {

}

void LoadingManager::init() {
	std::vector<glm::vec4>returnVertices;
	std::vector<glm::vec2>returnUV;
	std::vector<glm::vec3>returnNormal;
	std::vector<unsigned int>vIndex;
	std::vector<unsigned int>uvIndex;
	std::vector<unsigned int>normalIndex;

	// TODO: change this to chassis.obj later
	loadObject("../TopShopper/resources/Objects/sphere.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);

	VehicleChassisGeo->verts = returnVertices;
	VehicleChassisGeo->uvs = returnUV;
	VehicleChassisGeo->normals = returnNormal;
	VehicleChassisGeo->vIndex = vIndex;
	VehicleChassisGeo->uvIndex = uvIndex;
	VehicleChassisGeo->normalIndex = normalIndex;

	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();

	//////////////////

	// GROUND GEOMETRY:

	// TODO: change this to ground.obj later
	// NOTE: an .obj file requires normals! (even if we dont use them)
	loadObject("../TopShopper/resources/Objects/sphere.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);

	GroundGeo->verts = returnVertices;
	GroundGeo->uvs = returnUV;
	GroundGeo->normals = returnNormal;
	GroundGeo->vIndex = vIndex;
	GroundGeo->uvIndex = uvIndex;
	GroundGeo->normalIndex = normalIndex;

	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();

	///////////////////


	loadObject("../TopShopper/resources/Objects/sphere.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);

	SpareChangeGeo->verts = returnVertices;
	SpareChangeGeo->uvs = returnUV;
	SpareChangeGeo->normals = returnNormal;
	SpareChangeGeo->vIndex = vIndex;
	SpareChangeGeo->uvIndex = uvIndex;
	SpareChangeGeo->normalIndex = normalIndex;

	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();


}

void LoadingManager::updateMilliseconds(double deltaTime) {

}



// TODO: change the objparser (AGAIN :( ) since this one is incredibly inneficient 
bool LoadingManager::loadObject(const char* imageName, std::vector<glm::vec4>&returnVertices, std::vector<glm::vec2>&returnUV, std::vector<glm::vec3>&returnNormal, std::vector<unsigned int>&vIndex, std::vector<unsigned int>&uvIndex, std::vector<unsigned int>&normalIndex) {
	//std::vector<unsigned int> vIndex;
	//std::vector<unsigned int> uvIndex;
	//std::vector<unsigned int> normalIndex;
	std::vector<glm::vec3> inputVertex;
	std::vector<glm::vec2> inputUV;
	std::vector<glm::vec3> inputNormal;

	FILE * inputFile = fopen(imageName, "r");
	if (inputFile == NULL) {
		return false;
		std::exit(0);
	}
	while (true) {

		char firstWordInLine[365];

		int check = fscanf(inputFile, "%s", firstWordInLine);
		if (check == EOF) break;

		if (strcmp(firstWordInLine, "v") == 0) {
			glm::vec3 v;
			fscanf(inputFile, "%f %f %f\n", &v.x, &v.y, &v.z);
			returnVertices.push_back(glm::vec4(v, 1.0f));
		}
		else if (strcmp(firstWordInLine, "vt") == 0) {
			glm::vec3 uv;
			fscanf(inputFile, "%f %f\n", &uv.x, &uv.y);
			returnUV.push_back(uv);
		}
		else if (strcmp(firstWordInLine, "vn") == 0) {
			glm::vec3 n;
			fscanf(inputFile, "%f %f %f\n", &n.x, &n.y, &n.z);
			returnNormal.push_back(n);
		}
		else if (strcmp(firstWordInLine, "f") == 0) {
			std::string v1, v2, v3;
			unsigned int vert1[3], vert2[3], vert3[3];
			int matches = fscanf(inputFile, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vert1[0], &vert2[0], &vert3[0], &vert1[1], &vert2[1], &vert3[1], &vert1[2], &vert2[2], &vert3[2]);
			if (matches == 9) {
				vIndex.push_back(vert1[0]-1);
				vIndex.push_back(vert1[1]-1);
				vIndex.push_back(vert1[2]-1);

				uvIndex.push_back(vert2[0]-1);
				uvIndex.push_back(vert2[1]-1);
				uvIndex.push_back(vert2[2]-1);

				normalIndex.push_back(vert3[0]-1);
				normalIndex.push_back(vert3[1]-1);
				normalIndex.push_back(vert3[2]-1);
			}
			else {
				//unsigned int vert1[3], vert2[3], vert3[3];
				matches = fscanf(inputFile, "%d//%d %d//%d %d//%d\n", &vert1[0], &vert3[0], &vert1[1], &vert3[1], &vert1[2], &vert3[2]);
				vIndex.push_back(vert1[0]-1);
				vIndex.push_back(vert1[1]-1);
				vIndex.push_back(vert1[2]-1);

				normalIndex.push_back(vert3[0]-1);
				normalIndex.push_back(vert3[1]-1);
				normalIndex.push_back(vert3[2]-1);
			}
			
		}
	}

	return true;

	
}


Geometry* LoadingManager::getGeometry(GeometryTypes type) {
	switch (type) {
	case GeometryTypes::VEHICLE_CHASSIS_GEO:
		return VehicleChassisGeo;
	case GeometryTypes::VEHICLE_WHEEL_GEO:
		return VehicleWheelGeo;
	case GeometryTypes::GROUND_GEO:
		return GroundGeo;
	case GeometryTypes::SPARE_CHANGE_GEO:
		return SpareChangeGeo;
	default:
		return nullptr;
	}
}