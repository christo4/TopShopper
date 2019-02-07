#ifndef ENTITY_H_
#define ENTITY_H_

#include <array>
#include "component.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>


namespace physx {
	class PxActor;
};


// DEFINITION:
// NOTE: this base class should not be instantiated, and the derived prefab entity classes should only be constructed through the physics manager's instantiateEntity() method.
// ENTITY = An object placed anywhere in game world (simulated (non-kinematic dynamic, kinematic dynamic, static) and non-simulated (e.g. UI))
// Each entity has
//		- PxActor* _actor (either PxRigidStatic* or PxRigidDynamic*)
//		- EntityTypes _tag (enum matching name of entity prefab subclass)
//
// PHYSX CLASS HIERARCHY...
// PxActor important data/methods... getType() - eRigidStatic / eRigidDynamic
//									 getScene()
//									 get/setName()
//									 get/setActorFlag(s)() - eVisualization, eDisable_Gravity, eSend_Sleep_Notifies, eDisable_Simulation
//									 (void*) userdata - This pointer will get set to 'this' pointer to link an entity with its actor in a 1-1 relationship
//
// PxRigidActor important data/methods... get/setGlobalPose() - this refers to a PxTransform (has a vec3 pos and quat rotation)
//										  create/attach/detachShape()
//										  get/setNBShapes() - PhysX shapes can be attached to actors to define collider volumes, query volumes, etc.
//										  get/getNBConstraints() - constraint shaders!
//
// PxRigidStatic important data/methods... getConcreteTypeName() - returns string of most derived type
// 
// PxRigidBody important data/methods... bunch of physics force/velocity/mass/etc. methods
//
// PxRigidStatic important data/methods... get/setKinematicTarget() - use this to move a kinematic actor
//										   damping stuff
//										   get/setMaxAngularVelocity()
//										   isSleeping()
//										   get/setWakeCounter() - minimum amount of time before body can be put to sleep (won't be put to sleep if touching another awake body or has too much energy)
//										   putToSleep()
//										   getConcreteTypeName() - returns string of most derived type
//
// ADDITIONAL TIPS:
// 1. actor dynamic downcasting example:
//		if (PxRigidDynamic *dynActor = _actor->is<PxRigidDynamic>()) {...}
//		if (PxRigidStatic *statActor = _actor->is<PxRigidStatic>()) {...}
//
// OTHER NOTES:
// EACH ENTITY WILL BE POSITIONED BASED ON PXTRANSFORM OF THEIR ACTOR
// EACH ENTITY CAN HAVE ATTACHED DIFFERENT COMPONENTS TO CONFIGURE THE DATA STORED BY THE ENTITY
// IF A COMPONENT NEEDS A POSITION, IT WILL USE THIS ENTITY'S TRANSFORM BY DEFAULT, BUT AN OFFSET COULD BE DEFINED (E.G. CAMERA COMPONENT HAS A RELATIVE POSITION TO ITS PARENT ENTITY)





enum EntityTypes {
	ENVIRONMENT_GEOMETRY=0, // COMPONENTS = MESH ~~~~~~~CAN BE SPLIT UP INTO WALLS/SHELVES,RAMPS,ETC.
	GROUND, // COMPONENTS = MESH + NAVMESH?
	PICKUP, // COMPONENTS = MESH + AUDIO SOURCE? -- THIS INCLUDES GROCERIES, SPARE CHANGE, AND PAPER BAG
	PUDDLE, // COMPONENTS = MESH + AUDIO SOURCE?
	SHOPPING_CART_BOT, // COMPONENTS = MESH + NAVMESHAGENT + AUDIO SOURCE
	SHOPPING_CART_OBSTACLE, // COMPONENTS = MESH + NAVMESHAGENT? + AUDIO SOURCE?
	SHOPPING_CART_PLAYER, // COMPONENTS = MESH + INPUTCONTROLLER + AUDIO SOURCE + AUDIO LISTENER? + CAMERA
	UI, // COMPONENTS = MESH?/UISPRITE
	NUMBER_OF_ENTITY_TYPES
};



class Entity {
	public:
		Entity(physx::PxActor *actor, EntityTypes tag);
		virtual ~Entity();

		physx::PxActor *_actor = nullptr;

		void addComponent(ComponentTypes componentType); // only adds a component with default values to entity. These values can be overwritten afterwords through getComponent and public field changing or getters/setters
		void removeComponent(ComponentTypes componentType);
		std::shared_ptr<Component> getComponent(ComponentTypes componentType);

		virtual void updatePhysics(double deltaTime)=0;

		EntityTypes getTag() { return _tag; }

	private:
		EntityTypes _tag;

		std::array<std::shared_ptr<Component>, ComponentTypes::NUMBER_OF_COMPONENT_TYPES> _components;

};



#endif // ENTITY_H_	