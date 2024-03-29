#ifndef COMPONENT_H_
#define COMPONENT_H_

#include <array>
#include <vector>
#include <glm/glm.hpp>
#include "utility/utility.h"


class Entity;
enum EntityTypes;
namespace physx {
	class PxShape;
	struct PxContactPairPoint;
	typedef uint32_t PxU32;
};


// ORDERED ALPHABETICALLY FROM 0 (corresponding to index in entity::_components)
// NOTE: I'm enforcing that an entity can have at most 1 BehaviourScript component and it will be last index
enum ComponentTypes {
	BEHAVIOUR_SCRIPT,
	NUMBER_OF_COMPONENT_TYPES,

	// CHILD SCRIPT NAMES DECLARED BELOW HERE SO THAT THEY DONT SCREW UP THE INDEXING...
	MYSTERY_BAG_SCRIPT,
	PICKUP_SCRIPT,
	PLAYER_SCRIPT
};


// SHOULD NOT BE INSTANTIATED DIRECTLY
struct Component {
	Component(Entity *entity, ComponentTypes tag);

	Entity *_entity = nullptr;
	ComponentTypes _tag;
};


// SCRIPTS ...

// SHOULD NOT BE INSTANTIATED DIRECTLY
struct BehaviourScript : Component {
	BehaviourScript(Entity *entity, ComponentTypes tag);

	// EXECUTION ORDER OF THESE EVENT CALLBACKS...
	virtual void onSpawn()=0; // should be called only ONCE immediately after instantiation
	virtual void fixedUpdate(double fixedDeltaTime)=0; // should be called EVERY PHYSICS UPDATE before fetchresults()
	
	// NOTE: the order of these 5 is unknown, but they all happen between fixedUpdate() and update(), during fetchResults()
	virtual void onCollisionEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts)=0;
	virtual void onCollisionExit(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts)=0;
	virtual void onTriggerEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity)=0;
	virtual void onTriggerExit(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity)=0;
	
	virtual void update(double variableDeltaTime)=0; // should be called ONCE PER FRAME (inside AIManager's update)
	virtual void lateUpdate(double variableDeltaTime)=0; // should be called ONCE PER FRAME (inside RenderingManager's update)
	virtual void onDestroy()=0; // should be called ONCE immediately before frame ends (after all other updates)
};


struct MysteryBagScript : BehaviourScript {
	MysteryBagScript(Entity *entity);

	void onSpawn() override;
	void fixedUpdate(double fixedDeltaTime) override;

	void onCollisionEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) override;
	void onCollisionExit(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) override;
	void onTriggerEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity) override;
	void onTriggerExit(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity) override;

	void update(double variableDeltaTime) override;
	void lateUpdate(double variableDeltaTime) override;
	void onDestroy() override;

	// SPECIFICS...

	const int _cookiePoints = 75; // ~~~~NOTE: always make sure this matches Cookie.cpp _points value!
	const int _cookiePercent = 35;
	// NOTE: Hot Potato percent = 100 - cookiePercent, so make sure cookie Percent is in range [0,100]

	const double _hotPotatoDuration = 15.0; // 15 seconds...
};


struct PickupScript : BehaviourScript {
	PickupScript(Entity *entity);

	void onSpawn() override;
	void fixedUpdate(double fixedDeltaTime) override;

	void onCollisionEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) override;
	void onCollisionExit(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) override;
	void onTriggerEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity) override;
	void onTriggerExit(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity) override;

	void update(double variableDeltaTime) override;
	void lateUpdate(double variableDeltaTime) override;
	void onDestroy() override;

	// SPECIFICS...

	int _points = 0; // value of this pickup
};


struct PlayerScript : BehaviourScript {

	enum PlayerTypes {
		HUMAN,
		BOT
	};

	PlayerScript(Entity *entity);

	void onSpawn() override;
	void fixedUpdate(double fixedDeltaTime) override;

	void onCollisionEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) override;
	void onCollisionExit(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) override;
	void onTriggerEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity) override;
	void onTriggerExit(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity) override;

	void update(double variableDeltaTime) override;
	void lateUpdate(double variableDeltaTime) override;
	void onDestroy() override;

	// SPECIFICS...

	PlayerTypes _playerType;

	// EACH ID WILL BE UNIQUE TO A CART... (set in physicsmanager)
	// NEW DEFINITION: AI1.id = -1, AI2.id = -2, etc., HUMAN1.id = 1, HUMAN2.id =2, etc. DEFAULT ID = 0 (UNUSED)
	int _inputID = 0;

	int _points = 0; // amount of points this player has
	// NOTE: maybe init this array with a special NONE enum value?
	std::array<EntityTypes, 3> _shoppingList_Types; // e.g. MILK, APPLE, CARROT
	std::array<bool, 3> _shoppingList_Flags; // e.g. MILK=false, APPLE=true, CARROT=true

	void addPoints(int gain);
	void subPoints(int loss);

	void generateNewShoppingList();

	void pickedUpItem(EntityTypes pickupType);

	static const int SHOPPING_LIST_COMPLETED_POINTS = 100;

	void bashed();
	void coinExplosion();



	// AI STUFF...
	std::vector<ItemLocation> _targets; // starts empty
	void navigate();



	bool _hasHotPotato = false;
	double _hotPotatoTimer = -1.0;
	void giveHotPotato(double remainingDuration);
	void tickHotPotatoTimer(double fixedDeltaTime);
	void explodeHotPotato();
	
	int getNBBoosts();
};





#endif // COMPONENT_H_