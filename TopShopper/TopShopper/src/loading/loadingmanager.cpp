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
Geometry* BananaGeo = new Geometry();
Geometry* MilkGeo = new Geometry();
Geometry* WaterGeo = new Geometry();
Geometry* ColaGeo = new Geometry();
Geometry* AppleGeo = new Geometry();
Geometry* WatermelonGeo = new Geometry();
Geometry* CarrotGeo = new Geometry();
Geometry* EggplantGeo = new Geometry();
Geometry* BroccoliGeo = new Geometry();
Geometry* Obstacle1Geo = new Geometry();
Geometry* Obstacle2Geo = new Geometry();
Geometry* CookieGeo = new Geometry();
Geometry* MysteryBagGeo = new Geometry();
Geometry* HotPotatoGeo = new Geometry();

Geometry* VehicleChassisGeoNoIndex = new Geometry();
Geometry* VehicleWheelGeoNoIndex = new Geometry();
Geometry* GroundGeoNoIndex = new Geometry();
Geometry* SpareChangeGeoNoIndex = new Geometry();
Geometry* BananaGeoNoIndex = new Geometry();
Geometry* MilkGeoNoIndex = new Geometry();
Geometry* WaterGeoNoIndex = new Geometry();
Geometry* ColaGeoNoIndex = new Geometry();
Geometry* AppleGeoNoIndex = new Geometry();
Geometry* WatermelonGeoNoIndex = new Geometry();
Geometry* CarrotGeoNoIndex = new Geometry();
Geometry* EggplantGeoNoIndex = new Geometry();
Geometry* BroccoliGeoNoIndex = new Geometry();
Geometry* Obstacle1GeoNoIndex = new Geometry();
Geometry* Obstacle2GeoNoIndex = new Geometry();
Geometry* CookieGeoNoIndex = new Geometry();
Geometry* MysteryBagGeoNoIndex = new Geometry();
Geometry* HotPotatoGeoNoIndex = new Geometry();

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

	// spare change...
	loadObject("../TopShopper/resources/Objects/Change.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);
	
	
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

	//Banana
	loadObject("../TopShopper/resources/Objects/Banana.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);


	BananaGeo->verts = returnVertices;
	BananaGeo->uvs = returnUV;
	BananaGeo->normals = returnNormal;
	BananaGeo->vIndex = vIndex;
	BananaGeo->uvIndex = uvIndex;
	BananaGeo->normalIndex = normalIndex;


	for (unsigned int i = 0; i < vIndex.size(); i++) {
		BananaGeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		BananaGeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		BananaGeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
	}


	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();

	

	//milk
	loadObject("../TopShopper/resources/Objects/Milk.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);


	MilkGeo->verts = returnVertices;
	MilkGeo->uvs = returnUV;
	MilkGeo->normals = returnNormal;
	MilkGeo->vIndex = vIndex;
	MilkGeo->uvIndex = uvIndex;
	MilkGeo->normalIndex = normalIndex;


	for (unsigned int i = 0; i < vIndex.size(); i++) {
		MilkGeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		MilkGeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		MilkGeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
	}


	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();


	//water
	loadObject("../TopShopper/resources/Objects/Water.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);


	WaterGeo->verts = returnVertices;
	WaterGeo->uvs = returnUV;
	WaterGeo->normals = returnNormal;
	WaterGeo->vIndex = vIndex;
	WaterGeo->uvIndex = uvIndex;
	WaterGeo->normalIndex = normalIndex;


	for (unsigned int i = 0; i < vIndex.size(); i++) {
		WaterGeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		WaterGeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		WaterGeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
	}


	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();


	//cola
	loadObject("../TopShopper/resources/Objects/Cola.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);


	ColaGeo->verts = returnVertices;
	ColaGeo->uvs = returnUV;
	ColaGeo->normals = returnNormal;
	ColaGeo->vIndex = vIndex;
	ColaGeo->uvIndex = uvIndex;
	ColaGeo->normalIndex = normalIndex;


	for (unsigned int i = 0; i < vIndex.size(); i++) {
		ColaGeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		ColaGeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		ColaGeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
	}


	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();

	//Apple
	loadObject("../TopShopper/resources/Objects/Apple.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);


	AppleGeo->verts = returnVertices;
	AppleGeo->uvs = returnUV;
	AppleGeo->normals = returnNormal;
	AppleGeo->vIndex = vIndex;
	AppleGeo->uvIndex = uvIndex;
	AppleGeo->normalIndex = normalIndex;


	for (unsigned int i = 0; i < vIndex.size(); i++) {
		AppleGeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		AppleGeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		AppleGeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
	}


	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();

	//Watermelon
	loadObject("../TopShopper/resources/Objects/Watermelon.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);


	WatermelonGeo->verts = returnVertices;
	WatermelonGeo->uvs = returnUV;
	WatermelonGeo->normals = returnNormal;
	WatermelonGeo->vIndex = vIndex;
	WatermelonGeo->uvIndex = uvIndex;
	WatermelonGeo->normalIndex = normalIndex;


	for (unsigned int i = 0; i < vIndex.size(); i++) {
		WatermelonGeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		WatermelonGeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		WatermelonGeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
	}


	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();


	//Carrot
	loadObject("../TopShopper/resources/Objects/Carrot.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);


	CarrotGeo->verts = returnVertices;
	CarrotGeo->uvs = returnUV;
	CarrotGeo->normals = returnNormal;
	CarrotGeo->vIndex = vIndex;
	CarrotGeo->uvIndex = uvIndex;
	CarrotGeo->normalIndex = normalIndex;


	for (unsigned int i = 0; i < vIndex.size(); i++) {
		CarrotGeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		CarrotGeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		CarrotGeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
	}


	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();

	//Eggplant
	loadObject("../TopShopper/resources/Objects/Eggplant.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);


	EggplantGeo->verts = returnVertices;
	EggplantGeo->uvs = returnUV;
	EggplantGeo->normals = returnNormal;
	EggplantGeo->vIndex = vIndex;
	EggplantGeo->uvIndex = uvIndex;
	EggplantGeo->normalIndex = normalIndex;


	for (unsigned int i = 0; i < vIndex.size(); i++) {
		EggplantGeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		EggplantGeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		EggplantGeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
	}


	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();


	//Broccoli
	loadObject("../TopShopper/resources/Objects/Broccoli.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);


	BroccoliGeo->verts = returnVertices;
	BroccoliGeo->uvs = returnUV;
	BroccoliGeo->normals = returnNormal;
	BroccoliGeo->vIndex = vIndex;
	BroccoliGeo->uvIndex = uvIndex;
	BroccoliGeo->normalIndex = normalIndex;


	for (unsigned int i = 0; i < vIndex.size(); i++) {
		BroccoliGeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		BroccoliGeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		BroccoliGeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
	}


	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();


	//MysteryBag
	loadObject("../TopShopper/resources/Objects/Bag.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);


	MysteryBagGeo->verts = returnVertices;
	MysteryBagGeo->uvs = returnUV;
	MysteryBagGeo->normals = returnNormal;
	MysteryBagGeo->vIndex = vIndex;
	MysteryBagGeo->uvIndex = uvIndex;
	MysteryBagGeo->normalIndex = normalIndex;


	for (unsigned int i = 0; i < vIndex.size(); i++) {
		MysteryBagGeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		MysteryBagGeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		MysteryBagGeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
	}


	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();


	//Cookie
	loadObject("../TopShopper/resources/Objects/Cookie.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);


	CookieGeo->verts = returnVertices;
	CookieGeo->uvs = returnUV;
	CookieGeo->normals = returnNormal;
	CookieGeo->vIndex = vIndex;
	CookieGeo->uvIndex = uvIndex;
	CookieGeo->normalIndex = normalIndex;


	for (unsigned int i = 0; i < vIndex.size(); i++) {
		CookieGeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		CookieGeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		CookieGeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
	}


	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();


	//HOTPOTATO
	loadObject("../TopShopper/resources/Objects/Potato.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);


	HotPotatoGeo->verts = returnVertices;
	HotPotatoGeo->uvs = returnUV;
	HotPotatoGeo->normals = returnNormal;
	HotPotatoGeo->vIndex = vIndex;
	HotPotatoGeo->uvIndex = uvIndex;
	HotPotatoGeo->normalIndex = normalIndex;


	for (unsigned int i = 0; i < vIndex.size(); i++) {
		HotPotatoGeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		HotPotatoGeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		HotPotatoGeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
	}


	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();


	//obstacle1
	loadObject("../TopShopper/resources/Objects/Wall.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);


	Obstacle1Geo->verts = returnVertices;
	Obstacle1Geo->uvs = returnUV;
	Obstacle1Geo->normals = returnNormal;
	Obstacle1Geo->vIndex = vIndex;
	Obstacle1Geo->uvIndex = uvIndex;
	Obstacle1Geo->normalIndex = normalIndex;


	for (unsigned int i = 0; i < vIndex.size(); i++) {
		Obstacle1GeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		Obstacle1GeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		Obstacle1GeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
	}


	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	vIndex.clear();
	uvIndex.clear();
	normalIndex.clear();


	//obstacle2
	loadObject("../TopShopper/resources/Objects/Wall2.obj", returnVertices, returnUV, returnNormal, vIndex, uvIndex, normalIndex);


	Obstacle2Geo->verts = returnVertices;
	Obstacle2Geo->uvs = returnUV;
	Obstacle2Geo->normals = returnNormal;
	Obstacle2Geo->vIndex = vIndex;
	Obstacle2Geo->uvIndex = uvIndex;
	Obstacle2Geo->normalIndex = normalIndex;


	for (unsigned int i = 0; i < vIndex.size(); i++) {
		Obstacle2GeoNoIndex->verts.push_back(returnVertices[vIndex[i]]);
	}

	for (unsigned int i = 0; i < uvIndex.size(); i++) {
		Obstacle2GeoNoIndex->uvs.push_back(returnUV[uvIndex[i]]);
	}

	for (unsigned int i = 0; i < normalIndex.size(); i++) {
		Obstacle2GeoNoIndex->normals.push_back(returnNormal[normalIndex[i]]);
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
	case GeometryTypes::BANANA_GEO:
		return BananaGeo;
	case GeometryTypes::MILK_GEO:
		return MilkGeo;
	case GeometryTypes::WATER_GEO:
		return WaterGeo;
	case GeometryTypes::COLA_GEO:
		return ColaGeo;
	case GeometryTypes::APPLE_GEO:
		return AppleGeo;
	case GeometryTypes::WATERMELON_GEO:
		return WatermelonGeo;
	case GeometryTypes::CARROT_GEO:
		return CarrotGeo;
	case GeometryTypes::EGGPLANT_GEO:
		return EggplantGeo;
	case GeometryTypes::BROCCOLI_GEO:
		return BroccoliGeo;
	case GeometryTypes::MYSTERY_BAG_GEO:
		return MysteryBagGeo;
	case GeometryTypes::COOKIE_GEO:
		return CookieGeo;
	case GeometryTypes::HOT_POTATO_GEO:
		return HotPotatoGeo;
	case GeometryTypes::OBSTACLE1_GEO:
		return Obstacle1Geo;
	case GeometryTypes::OBSTACLE2_GEO:
		return Obstacle2Geo;
	case GeometryTypes::VEHICLE_CHASSIS_GEO_NO_INDEX:
		return VehicleChassisGeoNoIndex;
	case GeometryTypes::VEHICLE_WHEEL_GEO_NO_INDEX:
		return VehicleWheelGeoNoIndex;
	case GeometryTypes::GROUND_GEO_NO_INDEX:
		return GroundGeoNoIndex;
	case GeometryTypes::SPARE_CHANGE_GEO_NO_INDEX:
		return SpareChangeGeoNoIndex;
	case GeometryTypes::BANANA_GEO_NO_INDEX:
		return BananaGeoNoIndex;
	case GeometryTypes::MILK_GEO_NO_INDEX:
		return MilkGeoNoIndex;
	case GeometryTypes::WATER_GEO_NO_INDEX:
		return WaterGeoNoIndex;
	case GeometryTypes::COLA_GEO_NO_INDEX:
		return ColaGeoNoIndex;
	case GeometryTypes::APPLE_GEO_NO_INDEX:
		return AppleGeoNoIndex;
	case GeometryTypes::WATERMELON_GEO_NO_INDEX:
		return WatermelonGeoNoIndex;
	case GeometryTypes::CARROT_GEO_NO_INDEX:
		return CarrotGeoNoIndex;
	case GeometryTypes::EGGPLANT_GEO_NO_INDEX:
		return EggplantGeoNoIndex;
	case GeometryTypes::BROCCOLI_GEO_NO_INDEX:
		return BroccoliGeoNoIndex;
	case GeometryTypes::MYSTERY_BAG_GEO_NO_INDEX:
		return MysteryBagGeoNoIndex;
	case GeometryTypes::COOKIE_GEO_NO_INDEX:
		return CookieGeoNoIndex;
	case GeometryTypes::HOT_POTATO_GEO_NO_INDEX:
		return HotPotatoGeoNoIndex;
	case GeometryTypes::OBSTACLE1_GEO_NO_INDEX:
		return Obstacle1GeoNoIndex;
	case GeometryTypes::OBSTACLE2_GEO_NO_INDEX:
		return Obstacle2GeoNoIndex;
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