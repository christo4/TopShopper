


#ifndef ENTITY_H_
#define ENTITY_H_

#include <array>
#include "component.h"
#include <string>



// DEFINITION:
// ENTITIES ARE ALL OBJECTS PLACED IN THE GAME WORLD (E.G. TYPICAL PHYSICAL ACTORS, LIGHT SOURCES, UI ELEMENTS, ETC.)
// EACH ENTITY WILL BE POSITIONED BASED ON PXTRANSFORM OF THEIR ACTOR(S)
// EACH ENTITY CAN HAVE ATTACHED DIFFERENT COMPONENTS TO CONFIGURE THE DATA STORED BY THE ENTITY
// IF A COMPONENT NEEDS A POSITION, IT WILL USE THIS ENTITY'S TRANSFORM BY DEFAULT, BUT AN OFFSET COULD BE DEFINED (E.G. CAMERA COMPONENT HAS A RELATIVE POSITION TO ITS PARENT ENTITY)
// ***THIS CLASS SHOULN NOT BE INSTANTIATED ON ITS OWN AND WILL BE SUBCLASSES INTO PREFABS (E.G. SHOPPINGCART, BANANA, HOT POTATO, ETC.)

class Entity {
	public:
		Entity();
		virtual ~Entity();

		void addComponent(ComponentTypes componentType); // only adds a component with default values to entity. These values can be overwritten afterwords through getComponent and public field changing or getters/setters
		void removeComponent(ComponentTypes componentType);
		std::shared_ptr<Component> getComponent(ComponentTypes componentType);
	private:
		std::string _name;
		std::array<std::shared_ptr<Component>, ComponentTypes::NUMBER_OF_COMPONENT_TYPES> _components;
};



#endif // ENTITY_H_
