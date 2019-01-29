


#ifndef LOADINGMANAGER_H_
#define LOADINGMANAGER_H_

#include <vector>

class Broker;

class LoadingManager {
public:
	LoadingManager(Broker* broker);
	virtual ~LoadingManager();
	void init();
	void updateMilliseconds(double deltaTime);
	bool loadObject(const char* imageName, std::vector<glm::vec3>&returnVertex, std::vector<glm::vec2>&returnUV, std::vector<glm::vec3>&returnNormal);
private:
	Broker* _broker = nullptr;
};









#endif // LOADINGMANAGER_H_
