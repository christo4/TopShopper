


#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include "scene.h"

class SceneManager {
public:
	SceneManager();
	virtual ~SceneManager();
	//void init();
	//void updateMilliseconds(double deltaTime);
	// ~~~~~have a changeScene(int index) method that switches current scene to proper one
	// ~~~~~have a bunch of other methods that the broker can call to extract similar data from all the entities...
	// e.g. getAllMeshes() would return all mesh components attached to any of the entities, which could then be passed to the renderingManager by the broker
private:
	std::shared_ptr<Scene> _activeScene = nullptr;
	std::vector<std::shared_ptr<Scene>> _scenes;
};









#endif // SCENEMANAGER_H_

