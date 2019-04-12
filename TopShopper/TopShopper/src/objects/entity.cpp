#include "entity.h"

using namespace physx;



Entity::Entity(PxActor *actor, EntityTypes tag) 
	: _actor(actor), _tag(tag)
{

}

Entity::~Entity() {

}


// RULES:
// if an entity already has a component of type A, then it can't add a new component of type A, unless it first removes it
// if an existing component type tries to get added, nothing will happen
// just adds in a component with default values
void Entity::addComponent(ComponentTypes componentType) {
	// input safety check...
	if (componentType == ComponentTypes::NUMBER_OF_COMPONENT_TYPES) return; // NONSENSE!
	if (componentType == ComponentTypes::BEHAVIOUR_SCRIPT) return; // NONSENSE!

	int index = (componentType == ComponentTypes::MYSTERY_BAG_SCRIPT || componentType == ComponentTypes::PICKUP_SCRIPT || componentType == ComponentTypes::PLAYER_SCRIPT) ? ComponentTypes::BEHAVIOUR_SCRIPT : componentType;

	// block adding an existing component type
	if (_components.at(index) != nullptr) {
		return;
	}

	// otherwise, just create a default version of the component type and store in the array
	switch (componentType) {
		case ComponentTypes::MYSTERY_BAG_SCRIPT:
			_components.at(index) = std::make_shared<MysteryBagScript>(this);
			break;
		case ComponentTypes::PICKUP_SCRIPT:
			_components.at(index) = std::make_shared<PickupScript>(this);
			break;
		case ComponentTypes::PLAYER_SCRIPT:
			_components.at(index) = std::make_shared<PlayerScript>(this);
			break;
	}

}


std::shared_ptr<Component> Entity::getComponent(ComponentTypes componentType) {
	// input safety check...
	if (componentType == ComponentTypes::NUMBER_OF_COMPONENT_TYPES) return nullptr; // NONSENSE!

	int index = (componentType == ComponentTypes::MYSTERY_BAG_SCRIPT || componentType == ComponentTypes::PICKUP_SCRIPT || componentType == ComponentTypes::PLAYER_SCRIPT) ? ComponentTypes::BEHAVIOUR_SCRIPT : componentType;

	if (componentType == ComponentTypes::MYSTERY_BAG_SCRIPT || componentType == ComponentTypes::PICKUP_SCRIPT || componentType == ComponentTypes::PLAYER_SCRIPT) {
		return (_components.at(index) != nullptr && _components.at(index)->_tag == componentType ? _components.at(index) : nullptr);
	}
	else {
		return _components.at(index); // either a shared_ptr (if attached) or nullptr (if not attached)
	}

}


void Entity::destroy() {
	_destroyFlag = true;
}
