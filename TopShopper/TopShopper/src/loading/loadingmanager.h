


#ifndef LOADINGMANAGER_H_
#define LOADINGMANAGER_H_


#include <vector>
#include <glm/glm.hpp>






class Broker;

class LoadingManager {
public:
	LoadingManager(Broker* broker);
	virtual ~LoadingManager();
	void init();
	void updateMilliseconds(double deltaTime);
	void loadObject(const char* imageName, std::vector<glm::vec4>&returnVertices, std::vector<glm::vec2>&returnUV, std::vector<glm::vec3>&returnNormal, std::vector<unsigned int>&returnIndex);
	
private:
	Broker* _broker = nullptr;
};









#endif // LOADINGMANAGER_H_
