#include "loadingmanager.h"
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "rendering/Geometry.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>



Geometry* VehicleChassisGeo = new Geometry();
Geometry* VehicleWheelGeo = new Geometry();
Geometry* GroundGeo = new Geometry();
Geometry* SpareChangeGeo = new Geometry();

Geometry* VehicleChassisGeoNoIndex = new Geometry();
Geometry* VehicleWheelGeoNoIndex = new Geometry();
Geometry* GroundGeoNoIndex = new Geometry();
Geometry* SpareChangeGeoNoIndex = new Geometry();


LoadingManager::LoadingManager(Broker *broker) 
	: _broker(broker)
{

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

	////////////////////

	// CHASSIS GEOMETRY:

	// TODO: change this to chassis.obj later
	loadObject("../TopShopper/resources/Objects/ShoppingCart.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);
	
	
	VehicleChassisGeo->verts = returnVertices;
	VehicleChassisGeo->uvs = returnUV;
	VehicleChassisGeo->normals = returnNormal;
	VehicleChassisGeo->vIndex = vIndex;
	VehicleChassisGeo->uvIndex = uvIndex;
	VehicleChassisGeo->normalIndex = normalIndex;
	


	for (unsigned int i = 0; i < vIndex.size(); i++) {
		VehicleChassisGeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		VehicleChassisGeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		VehicleChassisGeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
	}



	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();


	//////////////////

	// WHEEL GEOMETRY:

	loadObject("../TopShopper/resources/Objects/Wheel.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);


	VehicleWheelGeo->verts = returnVertices;
	VehicleWheelGeo->uvs = returnUV;
	VehicleWheelGeo->normals = returnNormal;
	VehicleWheelGeo->vIndex = vIndex;
	VehicleWheelGeo->uvIndex = uvIndex;
	VehicleWheelGeo->normalIndex = normalIndex;



	for (unsigned int i = 0; i < vIndex.size(); i++) {
		VehicleWheelGeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		VehicleWheelGeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		VehicleWheelGeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
	}



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
	loadObject("../TopShopper/resources/Objects/StoreFloor.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);
	

	
	GroundGeo->verts = returnVertices;
	GroundGeo->uvs = returnUV;
	GroundGeo->normals = returnNormal;
	GroundGeo->vIndex = vIndex;
	GroundGeo->uvIndex = uvIndex;
	GroundGeo->normalIndex = normalIndex;
	

	for (unsigned int i = 0; i < vIndex.size(); i++) {
		GroundGeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		GroundGeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		GroundGeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
	}

	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();


	//////////////////////

	// PICKUP GEOMETRY (TEMP):


	loadObject("../TopShopper/resources/Objects/Collider.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);
	
	
	SpareChangeGeo->verts = returnVertices;
	SpareChangeGeo->uvs = returnUV;
	SpareChangeGeo->normals = returnNormal;
	SpareChangeGeo->vIndex = vIndex;
	SpareChangeGeo->uvIndex = uvIndex;
	SpareChangeGeo->normalIndex = normalIndex;
	


	for (unsigned int i = 0; i < vIndex.size(); i++) {
		SpareChangeGeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		SpareChangeGeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		SpareChangeGeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
	}


	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();


}

void LoadingManager::updateSeconds(double variableDeltaTime) {

}



bool LoadingManager::loadObject(const char* imageName, std::vector<glm::vec4>&returnVertices, std::vector<glm::vec2>&returnUV, std::vector<glm::vec3>&returnNormal, std::vector<unsigned int>&vIndex, std::vector<unsigned int>&uvIndex, std::vector<unsigned int>&normalIndex) {

	FILE *inputFile = fopen(imageName, "r");
	if (inputFile == NULL) {
		return false;
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
			char v1[100];
			char v2[100];
			char v3[100];
			fscanf(inputFile, "%s %s %s\n", v1, v2, v3);
			const char delimiters[] = "/";
			char *running1 = _strdup(v1);
			char *running2 = _strdup(v2);
			char *running3 = _strdup(v3);
			char *token1;
			char *token2;
			char *token3;

			token1 = mystrsep(&running1, delimiters); // token1 = "" or an int
			if (token1[0] != '\0') {
				vIndex.push_back(atoi(token1) - 1);
			}
			token1 = mystrsep(&running1, delimiters); // token1 = "" or an int
			if (token1[0] != '\0') {
				uvIndex.push_back(atoi(token1) - 1);
			}
			token1 = mystrsep(&running1, delimiters); // token1 = "" or an int
			if (token1[0] != '\0') {
				normalIndex.push_back(atoi(token1) - 1);
			}

			token2 = mystrsep(&running2, delimiters); // token2 = "" or an int
			if (token2[0] != '\0') {
				vIndex.push_back(atoi(token2) - 1);
			}
			token2 = mystrsep(&running2, delimiters); // token2 = "" or an int
			if (token2[0] != '\0') {
				uvIndex.push_back(atoi(token2) - 1);
			}
			token2 = mystrsep(&running2, delimiters); // token2 = "" or an int
			if (token2[0] != '\0') {
				normalIndex.push_back(atoi(token2) - 1);
			}

			token3 = mystrsep(&running3, delimiters); // token3 = "" or an int
			if (token3[0] != '\0') {
				vIndex.push_back(atoi(token3) - 1);
			}
			token3 = mystrsep(&running3, delimiters); // token3 = "" or an int
			if (token3[0] != '\0') {
				uvIndex.push_back(atoi(token3) - 1);
			}
			token3 = mystrsep(&running3, delimiters); // token3 = "" or an int
			if (token3[0] != '\0') {
				normalIndex.push_back(atoi(token3) - 1);
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
	case GeometryTypes::VEHICLE_CHASSIS_GEO_NO_INDEX:
		return VehicleChassisGeoNoIndex;
	case GeometryTypes::VEHICLE_WHEEL_GEO_NO_INDEX:
		return VehicleWheelGeoNoIndex;
	case GeometryTypes::GROUND_GEO_NO_INDEX:
		return GroundGeoNoIndex;
	case GeometryTypes::SPARE_CHANGE_GEO_NO_INDEX:
		return SpareChangeGeoNoIndex;
	default:
		return nullptr;
	}
}



// FROM: https://stackoverflow.com/questions/8512958/is-there-a-windows-variant-of-strsep
char* mystrsep(char** stringp, const char* delim)
{
	char* start = *stringp;
	char* p;

	p = (start != NULL) ? strpbrk(start, delim) : NULL;

	if (p == NULL)
	{
		*stringp = NULL;
	}
	else
	{
		*p = '\0';
		*stringp = p + 1;
	}

	return start;
}