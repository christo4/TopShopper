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
void PickupScript::onCollisionEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) {}
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
		Broker::getInstance()->getAudioManager()->playSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::TEST_SOUND));
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
	}
}

void PlayerScript::onCollisionEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) {}
void PlayerScript::onCollisionExit(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) {}
void PlayerScript::onTriggerEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity) {}
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
