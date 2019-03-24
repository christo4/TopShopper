#ifndef COMPONENT_H_
#define COMPONENT_H_

#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <foundation/PxVec3.h>



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
	// AUDIO...
	//AUDIO_LISTENER=0,
	//AUDIO_SOURCE,

	// RENDERING...
	//CAMERA,
	//MESH,

	// AI...
	//NAV_MESH_AGENT,


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

//struct AudioListener : Component {
	// simple, just flags we want audio to be relative to this point
//};

//struct AudioSource : Component {
	// audioclip(s), isMute ? , playOnAwake ? , Loop ? , Priority, volume)
//};

//struct Camera : Component {
	// FOV, clipping planes(near and far), etc.
//};

//struct Mesh : Component {
	//std::vector<glm::vec4> verts;
	//std::vector<glm::vec3> normals;
	//std::vector<glm::vec2> uvs;
	//std::vector<unsigned int> indices;
	// TODO: material properties 
//};

//struct NavMeshAgent : Component {
	//movement properties, target, path finding weights, etc.
//};



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
	double _rotationSpeed = 0.0;

};


struct PlayerScript : BehaviourScript {

	enum PlayerTypes {
		HUMAN,
		BOT
		//OBSTACLE
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

	int _inputID = -1; // 1, 2, 3, or 4 (unique)
	int _points = 0; // amount of points this player has
	// NOTE: maybe init this array with a special NONE enum value?
	std::array<EntityTypes, 3> _shoppingList_Types; // e.g. MILK, APPLE, CARROT
	std::array<bool, 3> _shoppingList_Flags; // e.g. MILK=false, APPLE=true, CARROT=true

	void addPoints(int gain);
	void subPoints(int loss);

	void generateNewShoppingList();

	void pickedUpItem(EntityTypes pickupType);

	static const int SHOPPING_LIST_COMPLETED_POINTS = 50;

	void bashed();
	void coinExplosion();



	// AI STUFF...
	std::vector<physx::PxVec3> _targets; // starts empty

	//std::vector<physx::PxVec3> _targets = { physx::PxVec3(0.0f, 2.0f, -80.0f), physx::PxVec3(80.0f, 2.0f, 0.0f), physx::PxVec3(0.0f, 2.0f, 80.0f), physx::PxVec3(-80.0f, 2.0f, 0.0f) }; // hardcoded test point
	//std::vector<physx::PxVec3> _targets = { physx::PxVec3(0.0f, 2.0f, 160.0f) }; // hardcoded test point


	void navigate();




	bool _hasHotPotato = false;
	double _hotPotatoTimer = -1.0;
	void giveHotPotato(double remainingDuration);
	void tickHotPotatoTimer(double fixedDeltaTime);
	void explodeHotPotato();


	int getNBBoosts();
};





#endif // COMPONENT_H_
