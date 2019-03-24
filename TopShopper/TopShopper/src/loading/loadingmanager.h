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
	BANANA_GEO,
	MILK_GEO,
	WATER_GEO,
	COLA_GEO,
	APPLE_GEO,
	WATERMELON_GEO,
	CARROT_GEO,
	EGGPLANT_GEO,
	BROCCOLI_GEO,
	OBSTACLE1_GEO,
	OBSTACLE2_GEO,
	COOKIE_GEO,

	VEHICLE_CHASSIS_GEO_NO_INDEX,
	VEHICLE_WHEEL_GEO_NO_INDEX,
	GROUND_GEO_NO_INDEX,
	SPARE_CHANGE_GEO_NO_INDEX,
	BANANA_GEO_NO_INDEX,
	MILK_GEO_NO_INDEX,
	WATER_GEO_NO_INDEX,
	COLA_GEO_NO_INDEX,
	APPLE_GEO_NO_INDEX,
	WATERMELON_GEO_NO_INDEX,
	CARROT_GEO_NO_INDEX,
	EGGPLANT_GEO_NO_INDEX,
	BROCCOLI_GEO_NO_INDEX,
	OBSTACLE1_GEO_NO_INDEX,
	OBSTACLE2_GEO_NO_INDEX,
	COOKIE_GEO_NO_INDEX,
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
