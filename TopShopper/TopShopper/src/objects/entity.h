


#ifndef ENTITY_H_
#define ENTITY_H_

#include <string>
#include <array>

#include "component.h"

// ANALOG OF UNITY'S GAMEOBJECT
// This class will not be derivable (marked final) and thus instances will be distinct based on the components they possess
// But, this class will have all the fields that every entity should logically have (e.g. a name, tag, list of attached components, ability to add/remove/retrieve components, etc.)
// ~~~~~~~~In the current model, an entity can have 0 or 1 of each component type (maybe change this to allow more?, but this current way leads to easier management/lookup)
class Entity final {
public:
	Entity();
	virtual ~Entity();

	void addComponent(ComponentTypes componentType); // only adds a component with default values to entity. These values can be overwritten afterwords through getComponent and public field changing or getters/setters
	void removeComponent(ComponentTypes componentType);
	std::shared_ptr<Component> getComponent(ComponentTypes componentType);

private:
	bool _isActive; // if false, all systems ignore this entity (and it is disabled from simulation)
	std::string _name; // e.g. (PlayerCart, AICart1, AICart2, etc.)
	std::string _tag; // e.g. (player, enemies, pickups, etc.)

	// have a vector of components (or maybe fixed size where each index coresponds to null or the component of a certain type, but in the latter case, that forces only 0/1 of each component attached to an entity)
	// e.g. _component.at(0) returns the attached Transform component
	// ~~~~~~~~~NOTE: each Entity must always have a Transform component
	std::array<std::shared_ptr<Component>, ComponentTypes::NUMBER_OF_COMPONENT_TYPES> _components;

	// ~~~~~probably also needs to have a vector of children Entities to search recursively)
};






#endif // ENTITY_H_
