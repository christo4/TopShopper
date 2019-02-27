#ifndef LOADINGMANAGER_H_
#define LOADINGMANAGER_H_


#include <vector>
#include <glm/glm.hpp>


class Geometry;


enum GeometryTypes {
	VEHICLE_CHASSIS_GEO,
	VEHICLE_WHEEL_GEO,
	GROUND_GEO,
	SPARE_CHANGE_GEO,
	VEHICLE_CHASSIS_GEO_NO_INDEX,
	VEHICLE_WHEEL_GEO_NO_INDEX,
	GROUND_GEO_NO_INDEX,
	SPARE_CHANGE_GEO_NO_INDEX
};




class Broker;

class LoadingManager {
public:
	LoadingManager(Broker *broker);
	virtual ~LoadingManager();
	void init();
	void updateSeconds(double variableDeltaTime);
	bool loadObject(const char* imageName, std::vector<glm::vec4>&returnVertices, std::vector<glm::vec2>&returnUV, std::vector<glm::vec3>&returnNormal, std::vector<unsigned int>&vIndex, std::vector<unsigned int>&uvIndex, std::vector<unsigned int>&normalIndex );
	
	Geometry* getGeometry(GeometryTypes type);

private:
	Broker *_broker = nullptr;
};



char* mystrsep(char** stringp, const char* delim);







#endif // LOADINGMANAGER_H_
