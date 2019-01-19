


#include "entity.h"

#include "core/transform.h"


Entity::Entity() {

}

Entity::~Entity() {

}



// RULES:
// if an entity already has a component of type A, then it can't add a new component of type A, unless it first removes it
// if an existing component type tries to get added, nothing will happen
void Entity::addComponent(ComponentTypes componentType) {
	// input safety check...
	if (componentType == ComponentTypes::NUMBER_OF_COMPONENT_TYPES) { // this is a nonsense call
		return;
	}

	int index = componentType; // enum -> int

	// block adding an existing component type
	if (_components.at(index) != nullptr) {
		return;
	}

	// otherwise, just create a default version of the component type and store in the array
	switch (componentType) {
		case ComponentTypes::TRANSFORM:
			_components.at(index) = std::make_shared<Transform>();
			break;
	}

}

void Entity::removeComponent(ComponentTypes componentType) {
	// input safety check...
	if (componentType == ComponentTypes::NUMBER_OF_COMPONENT_TYPES) { // this is a nonsense call
		return;
	}

	int index = componentType; // enum -> int

	// check if this component type is attached to the entity...
	if (_components.at(index) == nullptr) {
		return; // nothing to remove
	}

	// get here if type is attached, so we simply remove it (cleanup and reset that element in array to null)
	_components.at(index) = nullptr;
}

std::shared_ptr<Component> Entity::getComponent(ComponentTypes componentType) {
	// input safety check...
	if (componentType == ComponentTypes::NUMBER_OF_COMPONENT_TYPES) { // this is a nonsense call
		return nullptr;
	}

	int index = componentType; // enum -> int
	return _components.at(index); // either a shared_ptr (if attached) or nullptr (if not attached)
}