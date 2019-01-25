


#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include <memory>


class NonPhysicalEntity;
class StaticPhysicalEntity;
class DynamicPhysicalEntity;

// DEFINITION:
// EACH SCENE IS A SUPERSET OF A PHYSX SCENE
// A SCENE CONTAINS PHYSICAL ENTITIES (ALSO CONTAINED IN PHYSX SCENE) AND NONPHYSICAL ENTITIES (E.G. UI ELEMENTS)
class Scene {
	public:
		Scene();
		virtual ~Scene();
	private:
		std::vector<std::shared_ptr<NonPhysicalEntity>> _nonPhysicalEntities;
		std::vector<std::shared_ptr<StaticPhysicalEntity>> _staticPhysicalEntities;
		std::vector<std::shared_ptr<DynamicPhysicalEntity>> _dynamicPhysicalEntities;
};





#endif // SCENE_H_
