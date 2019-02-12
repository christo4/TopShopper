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
	std::vector<unsigned int>returnIndex;

	// TODO: change this to chassis.obj later
	loadObject("../TopShopper/resources/Objects/Earth.obj", returnVertices, returnUV, returnNormal, returnIndex);

	VehicleChassisGeo->verts = returnVertices;
	VehicleChassisGeo->uvs = returnUV;
	VehicleChassisGeo->normals = returnNormal;
	VehicleChassisGeo->indices = returnIndex;

	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	returnIndex.clear();

	//////////////////

	// GROUND GEOMETRY:

	// TODO: change this to ground.obj later
	// NOTE: an .obj file requires normals! (even if we dont use them)
	loadObject("../TopShopper/resources/Objects/groundTest.obj", returnVertices, returnUV, returnNormal, returnIndex);

	GroundGeo->verts = returnVertices;
	GroundGeo->uvs = returnUV;
	GroundGeo->normals = returnNormal;
	GroundGeo->indices = returnIndex;

	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	returnIndex.clear();

	///////////////////


	loadObject("../TopShopper/resources/Objects/sphere.obj", returnVertices, returnUV, returnNormal, returnIndex);

	SpareChangeGeo->verts = returnVertices;
	SpareChangeGeo->uvs = returnUV;
	SpareChangeGeo->normals = returnNormal;
	SpareChangeGeo->indices = returnIndex;

	returnVertices.clear();
	returnUV.clear();
	returnNormal.clear();
	returnIndex.clear();


}

void LoadingManager::updateMilliseconds(double deltaTime) {

}



// TODO: change the objparser (AGAIN :( ) since this one is incredibly inneficient 
void LoadingManager::loadObject(const char* imageName, std::vector<glm::vec4>&returnVertices, std::vector<glm::vec2>&returnUV, std::vector<glm::vec3>&returnNormal, std::vector<unsigned int>&returnIndex) {
	
	objl::Loader loader;
	bool loaded = loader.LoadFile(imageName);
	if (!loaded) {
		std::exit(EXIT_FAILURE);
	}

	// NOTE: loader.LoadedVertices will have size = number of faces (f-lines)
	// thus, there will be duplicated pos/uv/norm data and the indices will just be 0, 1, 2, ..., indices.size()-1
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
	

	// NOTE: this parser is stupid and should be replaced, but for now this vector is 0-indexed which is what we want
	returnIndex = loader.LoadedIndices;
	
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