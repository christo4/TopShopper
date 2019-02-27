#include "component.h"
#include "entity.h"
#include "shoppingcartplayer.h"
#include "vehicle/VehicleShoppingCart.h"
#include "core/broker.h"
#include <iostream>
#include <cstdlib>


using namespace physx;

////////////////////////////
Component::Component(Entity *entity, ComponentTypes tag) : _entity(entity), _tag(tag) {}


////////////////////////////
BehaviourScript::BehaviourScript(Entity *entity, ComponentTypes tag) : Component(entity, tag) {}


////////////////////////////
PickupScript::PickupScript(Entity *entity) : BehaviourScript(entity, ComponentTypes::PICKUP_SCRIPT) {}

void PickupScript::onSpawn() {}
void PickupScript::fixedUpdate(double fixedDeltaTime) {}

void PickupScript::onCollisionEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) {
	// NOTE: this will only get called for pickups instantiated by a bash collision which will then land on the ground somewhere.
	if (otherEntity->getTag() == EntityTypes::GROUND) {
		// make pickup back into a trigger...
		localShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		localShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

		// disable gravity
		_entity->_actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

		// clear all forces and set velocity to 0...
		PxRigidDynamic *dyn = _entity->_actor->is<PxRigidDynamic>();
		dyn->clearForce(PxForceMode::eACCELERATION);
		dyn->clearForce(PxForceMode::eVELOCITY_CHANGE);

		// ~~~~NOTE: these might conflict with future spinning pickups
		dyn->clearTorque(PxForceMode::eACCELERATION);
		dyn->clearTorque(PxForceMode::eVELOCITY_CHANGE);

		dyn->setLinearVelocity(PxVec3(0.0f));
		// ~~~~NOTE: this might conflict with future spinning pickups
		dyn->setAngularVelocity(PxVec3(0.0f));

	}

}

void PickupScript::onCollisionExit(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) {}

void PickupScript::onTriggerEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity) {
	if (otherEntity->getTag() == EntityTypes::SHOPPING_CART_PLAYER) {
		ShoppingCartPlayer *player = static_cast<ShoppingCartPlayer*>(otherEntity);
		std::shared_ptr<PlayerScript> playerScript = std::static_pointer_cast<PlayerScript>(player->getComponent(ComponentTypes::PLAYER_SCRIPT));
		playerScript->addPoints(_points); // increase player points by this pickup's value
		playerScript->pickedUpItem(_entity->getTag()); // tell player that this type of pickup was picked up
		std::cout << "COMPONENT.CPP | PICKUP WAS PICKED UP!" << std::endl;
		std::cout << "PLAYER POINTS = " << playerScript->_points << std::endl;
		_entity->destroy(); // destroy this pickup
	}
}

void PickupScript::onTriggerExit(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity) {}
void PickupScript::update(double variableDeltaTime) {}
void PickupScript::lateUpdate(double variableDeltaTime) {}
void PickupScript::onDestroy() {}


////////////////////////////
PlayerScript::PlayerScript(Entity *entity) : BehaviourScript(entity, ComponentTypes::PLAYER_SCRIPT) {}

void PlayerScript::onSpawn() {
	generateNewShoppingList();
}

void PlayerScript::fixedUpdate(double fixedDeltaTime) {
	ShoppingCartPlayer *player = dynamic_cast<ShoppingCartPlayer*>(_entity);
	if (player != nullptr) {
		Gamepad *pad = Broker::getInstance()->getInputManager()->getGamePad(_inputID);
		if (pad != nullptr) {
			PxReal accel = glm::clamp(((pad->rightTrigger + 1) / 2), 0.0f, 1.0f);
			PxReal reverse = glm::clamp(((pad->leftTrigger + 1) / 2), 0.0f, 1.0f);
			PxReal handbrake = pad->xButton ? 1.0f : 0.0f;
			PxReal steer = glm::clamp(pad->leftStickX *-1, -1.0f, 1.0f); // must be negated otherwise steering is backwards
			bool turboButtonPressed = pad->bButton; // this function doesnt work as intended yet...

			player->_shoppingCartBase->processRawInputDataController(accel, reverse, handbrake, steer, turboButtonPressed);
		}
		else {
			if (_inputID == 1) {
				KeyboardAndMouse *kam = Broker::getInstance()->getInputManager()->getKeyboardAndMouse();
				bool accelKeyPressed = kam->wKey;
				bool reverseKeyPressed = kam->sKey;
				bool handbrakeKeyPressed = kam->leftShiftKey;
				bool steerLeftKeyPressed = kam->dKey; // NOTE: the steer keys have to be reversed here
				bool steerRightKeyPressed = kam->aKey;
				bool turboKeyPressed = kam->spaceKey;

				player->_shoppingCartBase->processRawInputDataKeyboard(accelKeyPressed, reverseKeyPressed, handbrakeKeyPressed, steerLeftKeyPressed, steerRightKeyPressed, turboKeyPressed);
			}
		}

		player->_shoppingCartBase->smoothAndFeedInputs(fixedDeltaTime);

		if (player->_shoppingCartBase->IsBashProtected()) {
			player->_shoppingCartBase->tickBashProtectionTimer(fixedDeltaTime);
		}
	}
}

void PlayerScript::onCollisionEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) {}
void PlayerScript::onCollisionExit(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) {}

void PlayerScript::onTriggerEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity) {
	
	// BASH EVENT CONDITIONS:
	// 1. This Cart is turboing
	// 2. other cart is not in hot potato mode or bash-recovery mode
	
	if (otherEntity->getTag() == EntityTypes::SHOPPING_CART_PLAYER) { // if we hit another cart with our bash trigger volume at the front of the chassis...
		ShoppingCartPlayer *otherCart = static_cast<ShoppingCartPlayer*>(otherEntity);
		ShoppingCartPlayer *localCart = static_cast<ShoppingCartPlayer*>(_entity);

		if (localCart->_shoppingCartBase->IsTurboing() && !otherCart->_shoppingCartBase->IsBashProtected()) {
			std::shared_ptr<PlayerScript> otherScript = std::static_pointer_cast<PlayerScript>(otherCart->getComponent(ComponentTypes::PLAYER_SCRIPT));
			otherScript->bashed();
		}
		
	}


}

void PlayerScript::onTriggerExit(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity) {}
void PlayerScript::update(double variableDeltaTime) {}
void PlayerScript::lateUpdate(double variableDeltaTime) {}
void PlayerScript::onDestroy() {}

void PlayerScript::addPoints(int gain) { _points += gain; }
void PlayerScript::subPoints(int loss) {
	_points -= loss;
	if (_points < 0) _points = 0; // clamp
}

void PlayerScript::generateNewShoppingList() {
	_shoppingList_Flags.at(0) = false;
	_shoppingList_Flags.at(1) = false;
	_shoppingList_Flags.at(2) = false;

	int rng = rand() % 3;
	if (rng == 0) _shoppingList_Types.at(0) = EntityTypes::MILK;
	else if (rng == 1) _shoppingList_Types.at(0) = EntityTypes::WATER;
	else _shoppingList_Types.at(0) = EntityTypes::COLA;

	rng = rand() % 3;
	if (rng == 0) _shoppingList_Types.at(1) = EntityTypes::APPLE;
	else if (rng == 1) _shoppingList_Types.at(1) = EntityTypes::WATERMELON;
	else _shoppingList_Types.at(1) = EntityTypes::BANANA;

	rng = rand() % 3;
	if (rng == 0) _shoppingList_Types.at(2) = EntityTypes::CARROT;
	else if (rng == 1) _shoppingList_Types.at(2) = EntityTypes::EGGPLANT;
	else _shoppingList_Types.at(2) = EntityTypes::BROCCOLI;


	std::cout << "COMPONENT.CPP | NEW SHOPPING LIST!" << std::endl;
	std::cout << _shoppingList_Types.at(0) << "|" << _shoppingList_Types.at(1) << "|" << _shoppingList_Types.at(2) << std::endl;

}


void PlayerScript::pickedUpItem(EntityTypes pickupType) {
	for (int i = 0; i < 3; i++) { // loop through shopping list...
		if (_shoppingList_Types.at(i) == pickupType) { // if we just picked up item on our list...
			_shoppingList_Flags.at(i) = true; // flag that it has been picked up
			
			// now check if all 3 items on list have been collected...
			for (bool flag : _shoppingList_Flags) {
				if (!flag) return;
			}
			// get here if all 3 are true...
			addPoints(SHOPPING_LIST_COMPLETED_POINTS); // add bonus points for completing your list
			generateNewShoppingList();

			break;
		}
	}
}


void PlayerScript::bashed() {
	ShoppingCartPlayer *bashedCart = static_cast<ShoppingCartPlayer*>(_entity);
	bashedCart->_shoppingCartBase->setBashProtected();

	std::vector<EntityTypes> lostItems; // can have size 0,1 or 2
	for (int i = 0; i < 3; i++) {
		if (_shoppingList_Flags.at(i)) {
			lostItems.push_back(_shoppingList_Types.at(i));
			_shoppingList_Flags.at(i) = false;
		}
	}
	
	PxRigidDynamic *bashedCartDyn = bashedCart->_actor->is<PxRigidDynamic>();
	PxTransform bashedCartTransform = bashedCartDyn->getGlobalPose();
	PxVec3 bashedCartPos = bashedCartTransform.p;

	PxVec3 nextSpawnPos = bashedCartPos + PxVec3(0.0f, 5.0f, 0.0f);

	std::vector<PxVec3> forces = { PxVec3(0.0f, 40.0f, 20.0f), PxVec3(0.0f, 40.0f, -20.0f), PxVec3(20.0f, 40.0f, 0.0f), PxVec3(-20.0f, 40.0f, 0.0f) }; // NOTE: fast HACK for now
	int forceIndex = 0;

	for (EntityTypes item : lostItems) {
		// spawn new item of this type above cart, apply an impulse to each in a different direction
		std::shared_ptr<Entity> spawnedItem = Broker::getInstance()->getPhysicsManager()->instantiateEntity(item, PxTransform(nextSpawnPos), "LostItem");

		// now enable gravity on item
		// make item solid until it hits the ground somewhere (onCollisionEnter - which will then set item to trigger and disable gravity) 
		// apply an impulse in xz-plane at a different (maybe random) rotation (0, 90, 180, 270) - can keep cycling through this

		PxRigidDynamic *spawnedItemDyn = spawnedItem->_actor->is<PxRigidDynamic>();
		PxShape* shapeBuffer[1];
		spawnedItemDyn->getShapes(shapeBuffer, 1);
		PxShape* spawnedItemShape = shapeBuffer[0];

		// make it solid...
		spawnedItemShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
		spawnedItemShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		
		// enable gravity
		spawnedItemDyn->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);

		// apply a force vector in a different direction...
		spawnedItemDyn->addForce(forces.at(forceIndex), PxForceMode::eIMPULSE);
		forceIndex++;
		if (forceIndex >= forces.size()) forceIndex = 0;

		nextSpawnPos += PxVec3(0.0f, 3.0f, 0.0f);
	}


	std::cout << "BASH EVENT" << std::endl;
}
