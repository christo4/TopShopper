


#ifndef ENTITY_H_
#define ENTITY_H_

#include <PxPhysicsAPI.h> // ~~~~~NOTE: change this to be more specific later
#include <array>
#include "component.h"


// DEFINITION:
// ENTITIES ARE ALL OBJECTS PLACED IN THE GAME WORLD (E.G. PHYSICAL ACTORS, LIGHT SOURCES, UI ELEMENTS, ETC.)
// EACH ENTITY WILL BE POSITIONED BASED ON A BUILT-IN PXTRANSFORM
// EACH ENTITY CAN HAVE ATTACHED DIFFERENT COMPONENTS TO CONFIGURE THE DATA STORED BY THE ENTITY
// IF A COMPONENT NEEDS A POSITION, IT WILL USE THIS ENTITY'S TRANSFORM BY DEFAULT, BUT AN OFFSET COULD BE DEFINED

// NOTE: ONLY 3 SUBCLASSES ARE MEANT TO BE INSTANTIATED:
// NONPHYSICALENTITY
// STATICPHYSICALENTITY
// DYNAMICPHYSICALENTITY
class Entity {
	public:
		Entity();
		virtual ~Entity();

		void addComponent(ComponentTypes componentType); // only adds a component with default values to entity. These values can be overwritten afterwords through getComponent and public field changing or getters/setters
		void removeComponent(ComponentTypes componentType);
		std::shared_ptr<Component> getComponent(ComponentTypes componentType);
	private:
		std::array<std::shared_ptr<Component>, ComponentTypes::NUMBER_OF_COMPONENT_TYPES> _components;
};



#endif // ENTITY_H_
