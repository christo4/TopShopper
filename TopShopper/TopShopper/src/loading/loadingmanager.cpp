


#include "loadingmanager.h"
#include <iostream>
#define _CRT_SECURE_NO_WARNINGS

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

LoadingManager::LoadingManager(Broker* broker) {

}

LoadingManager::~LoadingManager() {

}

void LoadingManager::init() {

}

void LoadingManager::updateMilliseconds(double deltaTime) {

}
//Added ObjLoader that takes the image name and vectors
//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
bool LoadingManager::loadObject(const char* imageName, std::vector<glm::vec3>&returnVertex, std::vector<glm::vec2>&returnUV, std::vector<glm::vec3>&returnNormal) {
	std::vector<unsigned int> vIndex;
	std::vector<unsigned int> uvIndex;
	std::vector<unsigned int> normalIndex;
	std::vector<glm::vec3> inputVertex;
	std::vector<glm::vec2> inputUV;
	std::vector<glm::vec3> inputNormal;

	FILE * inputFile;
	errno_t error = fopen_s(&inputFile, imageName, "r");


	if (inputFile == NULL) {
		return false;
		std::exit(0);
	}
	while (true) {

		char firstWordInLine[365];

		int check = fscanf_s(inputFile, "%s", firstWordInLine);
		if (check == EOF) break;

		if (strcmp(firstWordInLine, "v") == 0) {
			glm::vec3 v;
			fscanf_s(inputFile, "%f %f %f\n", &v.x, &v.y, &v.z);
			inputVertex.push_back(v);
		}
		else if (strcmp(firstWordInLine, "vt") == 0) {
			glm::vec3 uv;
			fscanf_s(inputFile, "%f %f\n", &uv.x, &uv.y);
			inputUV.push_back(uv);
		}
		else if (strcmp(firstWordInLine, "vn") == 0) {
			glm::vec3 n;
			fscanf_s(inputFile, "%f %f %f\n", &n.x, &n.y, &n.z);
			inputNormal.push_back(n);
		}
		else if (strcmp(firstWordInLine, "f") == 0) {
			std::string v1, v2, v3;
			unsigned int vert1[3], vert2[3], vert3[3];
			int matches = fscanf_s(inputFile, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vert1[0], &vert2[0], &vert3[0], &vert1[1], &vert2[1], &vert3[1], &vert1[2], &vert2[2], &vert3[2]);

			if (matches != 9) {
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
	for (int i = 0; i < vIndex.size(); i++) {
		unsigned int currentIndex = vIndex[i];
		glm::vec3 vert = inputVertex[currentIndex - 1];
		returnVertex.push_back(vert);
	}

	for (int i = 0; i < uvIndex.size(); i++) {
		unsigned int currentIndex = uvIndex[i];
		glm::vec2 vert = inputUV[currentIndex - 1];
		returnUV.push_back(vert);
	}

	for (int i = 0; i < normalIndex.size(); i++) {
		unsigned int currentIndex = normalIndex[i];
		glm::vec3 vert = inputNormal[currentIndex - 1];
		returnNormal.push_back(vert);
	}

	return true;
}