#include "component.h"
#include "entity.h"
#include "shoppingcartplayer.h"
#include "vehicle/VehicleShoppingCart.h"
#include "core/broker.h"
#include <iostream>
#include <cstdlib>



using namespace physx;
bool turboState = false;

////////////////////////////
Component::Component(Entity *entity, ComponentTypes tag) : _entity(entity), _tag(tag) {}


////////////////////////////
BehaviourScript::BehaviourScript(Entity *entity, ComponentTypes tag) : Component(entity, tag) {}


////////////////////////////
MysteryBagScript::MysteryBagScript(Entity *entity) : BehaviourScript(entity, ComponentTypes::MYSTERY_BAG_SCRIPT) {}

void MysteryBagScript::onSpawn() {
	_entity->_actor->is<PxRigidDynamic>()->setAngularDamping(0.0f);
	_entity->_actor->is<PxRigidDynamic>()->setAngularVelocity(PxVec3(1, 5, 1));
}

void MysteryBagScript::fixedUpdate(double fixedDeltaTime) {}
void MysteryBagScript::onCollisionEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) {}
void MysteryBagScript::onCollisionExit(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) {}

void MysteryBagScript::onTriggerEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity) {
	if (_entity->getDestroyFlag()) return; // PREVENTS MULTIPLE CARTS PICKING THIS UP IN SAME FRAME
	
	if (otherEntity->getTag() == EntityTypes::SHOPPING_CART_PLAYER) {
		ShoppingCartPlayer *player = static_cast<ShoppingCartPlayer*>(otherEntity);
		std::shared_ptr<PlayerScript> playerScript = std::static_pointer_cast<PlayerScript>(player->getComponent(ComponentTypes::PLAYER_SCRIPT));

		int rng = rand() % 100; // 0-99
		if (rng < _cookiePercent) { // IT WAS A COOKIE!...
			std::cout << "MYSTERY BAG REVEALED... A COOKIE!" << std::endl;
			//TODO: display some UI notification (cookie sprite)
			playerScript->addPoints(_cookiePoints); // increase player points by this pickup's value
		}
		else { // IT WAS A HOT POTATO!...
			std::cout << "MYSTERY BAG REVEALED... A HOT POTATO!" << std::endl;
			//TODO: display some UI notification (hot potato sprite) - this may be unnecessary since hot potato sprite will show up next to player list?
			playerScript->giveHotPotato(_hotPotatoDuration);
			// start the sound here
			Broker::getInstance()->getAudioManager()->playSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::TICKING_SOUND));
		}

		_entity->destroy(); // destroy this mystery bag
		Broker::getInstance()->getAudioManager()->playSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::PICKITEM_SOUND));
	}
}

void MysteryBagScript::onTriggerExit(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity) {}
void MysteryBagScript::update(double variableDeltaTime) {}
void MysteryBagScript::lateUpdate(double variableDeltaTime) {}
void MysteryBagScript::onDestroy() {}


////////////////////////////
PickupScript::PickupScript(Entity *entity) : BehaviourScript(entity, ComponentTypes::PICKUP_SCRIPT) {}

void PickupScript::onSpawn() {
	_entity->_actor->is<PxRigidDynamic>()->setAngularDamping(0.0f);
	_entity->_actor->is<PxRigidDynamic>()->setAngularVelocity(PxVec3(1, 5, 1));
}

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
		//dyn->setAngularVelocity(PxVec3(0.0f));

		// reset rotation info to be same as on spawn...
		dyn->setAngularDamping(0.0f);
		dyn->setAngularVelocity(PxVec3(1, 5, 1));

	}

}

void PickupScript::onCollisionExit(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) {}

void PickupScript::onTriggerEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity) {
	if (_entity->getDestroyFlag()) return; // PREVENTS MULTIPLE CARTS PICKING THIS UP IN SAME FRAME
	
	if (otherEntity->getTag() == EntityTypes::SHOPPING_CART_PLAYER) {
		ShoppingCartPlayer *player = static_cast<ShoppingCartPlayer*>(otherEntity);
		std::shared_ptr<PlayerScript> playerScript = std::static_pointer_cast<PlayerScript>(player->getComponent(ComponentTypes::PLAYER_SCRIPT));
		playerScript->addPoints(_points); // increase player points by this pickup's value
		playerScript->pickedUpItem(_entity->getTag()); // tell player that this type of pickup was picked up
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
		if (_playerType == PlayerTypes::HUMAN) {
			Gamepad *pad = Broker::getInstance()->getInputManager()->getGamePad(_inputID);
			if (pad != nullptr) {
				PxReal accel = glm::clamp(((pad->rightTrigger + 1) / 2), 0.0f, 1.0f);
				PxReal reverse = glm::clamp(((pad->leftTrigger + 1) / 2), 0.0f, 1.0f);
				PxReal handbrake = pad->xButton ? 1.0f : 0.0f;
				PxReal steer = glm::clamp(pad->leftStickX *-1, -1.0f, 1.0f); // must be negated otherwise steering is backwards
				bool turboButtonPressed = (_hasHotPotato || pad->bButton);
				bool turboTank = PlayerScript::getNBBoosts() > 0;
				if (turboButtonPressed && !(turboState) && turboTank) {
					Broker::getInstance()->getAudioManager()->playSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::TURBO_SOUND));
					turboState = true;
				}
				else if (!turboButtonPressed && turboState) {
					turboState = false;
				}
				PxRigidDynamic *playerDyn = player->_actor->is<PxRigidDynamic>();
				PxVec3 velocity = playerDyn->getLinearVelocity();
				float speed = velocity.magnitude();

				//std::cout << speed << std::endl;
				Broker::getInstance()->getAudioManager()->playSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::ROLL_SOUND_PLAYER1));
				Broker::getInstance()->getAudioManager()->changeVolumeSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::ROLL_SOUND_PLAYER1), Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::ROLL_SOUND_PLAYER1)->volume*speed / 60);
				
				
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
					bool turboKeyPressed = (_hasHotPotato || kam->spaceKey);
					bool turboTank = PlayerScript::getNBBoosts() > 0;
					if (turboKeyPressed && !(turboState) && turboTank) {
						Broker::getInstance()->getAudioManager()->playSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::TURBO_SOUND));
						turboState = true;
					}
					else if (!turboKeyPressed && turboState) {
						turboState = false;
					}
					PxRigidDynamic *playerDyn = player->_actor->is<PxRigidDynamic>();
					PxVec3 velocity = playerDyn->getLinearVelocity();
					float speed = velocity.magnitude();

					//std::cout << speed << std::endl;
					Broker::getInstance()->getAudioManager()->playSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::ROLL_SOUND_PLAYER1));
					Broker::getInstance()->getAudioManager()->changeVolumeSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::ROLL_SOUND_PLAYER1), Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::ROLL_SOUND_PLAYER1)->volume*speed / 60);
					player->_shoppingCartBase->processRawInputDataKeyboard(accelKeyPressed, reverseKeyPressed, handbrakeKeyPressed, steerLeftKeyPressed, steerRightKeyPressed, turboKeyPressed);
				}
			}

			// HUMAN ONLY...
			// CONSUME FUEL OR RECHARGE FUEL...
			// NO NEED TO CONSUME/RECHARGE TURBO WHEN YOU HAVE THE HOT POTATO...
			if (!_hasHotPotato) {
				if (player->_shoppingCartBase->IsTurboing()) {
					player->_shoppingCartBase->consumeTurbo(fixedDeltaTime);
				}
				else {
					player->_shoppingCartBase->rechargeTurbo(fixedDeltaTime);
				}
			}

		}
		else if (_playerType == PlayerTypes::BOT) {
			std::vector<std::shared_ptr<ShoppingCartPlayer>> carts = Broker::getInstance()->getPhysicsManager()->getActiveScene()->getAllShoppingCartPlayers();
			std::shared_ptr<ShoppingCartPlayer> player1 = nullptr;
			for (const std::shared_ptr<ShoppingCartPlayer> &cart : carts) {
				std::shared_ptr<PlayerScript> cartScript = std::static_pointer_cast<PlayerScript>(cart->getComponent(ComponentTypes::PLAYER_SCRIPT));
				if (cartScript->_playerType == PlayerTypes::HUMAN && cartScript->_inputID == 1) {
					player1 = cart;
					break;
				}
			}

			physx::PxVec3 playerPos = player1->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().p;
			
			physx::PxVec3 myPos = (dynamic_cast<ShoppingCartPlayer*>(_entity))->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().p;
			physx::PxVec3 myVelocity = (dynamic_cast<ShoppingCartPlayer*>(_entity))->_actor->is<physx::PxRigidDynamic>()->getLinearVelocity();

			float speed = myVelocity.magnitude();
			float distanceBetween = (myPos - playerPos).magnitude() + 20;
			if (distanceBetween >= 255)
				distanceBetween = 255;

			physx::PxQuat playerRot = player1->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().q;
			physx::PxVec3 forward(0.0f, 0.0f, 1.0f); // on spawn the forward vector of a cart is pointing in the +z direction
			forward = playerRot.rotate(forward); // now forward has been rotated to match the cart's rotation
			// this forward vector is the player's direction
			// if you only need the vector in the xz-plane you can do
			//physx::PxVec3 forwardNoYNormalized = forward;
			physx::PxVec3 forwardNoYNormalized = physx::PxVec3(forward.x, 0.0f, forward.z).getNormalized();
			//physx::PxVec3 myPosNormalized = myPos;
			physx::PxVec3 myPosNormalized = physx::PxVec3(myPos.x, 0.0f, myPos.z).getNormalized();
			
			float angle = acos(((forwardNoYNormalized.x*myPosNormalized.x) + (forwardNoYNormalized.z*myPosNormalized.z)));
			angle = (angle / 3.1415926) * 180;
			PxVec3 crossprod = forwardNoYNormalized.cross(myPosNormalized);
			bool isCCW = crossprod.y <= 0.0f;
			if (!isCCW) angle = 360 - angle;
			//std::cout << "Angle: " << angle << std::endl;
			if (_inputID == -1) {
				Broker::getInstance()->getAudioManager()->changeVolumeSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::ROLL_SOUND_AI1), Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::ROLL_SOUND_AI1)->volume*(speed / 60));
				Broker::getInstance()->getAudioManager()->changeDistanceSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::ROLL_SOUND_AI1), (distanceBetween), angle);

				Broker::getInstance()->getAudioManager()->playSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::ROLL_SOUND_AI1));
			}
			else if (_inputID = -2) {
				//printf("play bot 2 distance: %f\n", distanceBetween);
				Broker::getInstance()->getAudioManager()->changeVolumeSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::ROLL_SOUND_AI2), Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::ROLL_SOUND_AI2)->volume*(speed / 60));
				Broker::getInstance()->getAudioManager()->changeDistanceSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::ROLL_SOUND_AI2), (distanceBetween), angle);

				Broker::getInstance()->getAudioManager()->playSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::ROLL_SOUND_AI2));
			}
			

			navigate();
		}
		

		player->_shoppingCartBase->smoothAndFeedInputs(fixedDeltaTime);

		if (player->_shoppingCartBase->IsBashProtected()) {
			player->_shoppingCartBase->tickBashProtectionTimer(fixedDeltaTime);
		}

		if (_hasHotPotato) {
			tickHotPotatoTimer(fixedDeltaTime);
		}
	}
}

void PlayerScript::onCollisionEnter(physx::PxShape *localShape, physx::PxShape *otherShape, Entity *otherEntity, physx::PxContactPairPoint *contacts, physx::PxU32 nbContacts) {
	std::vector<std::shared_ptr<ShoppingCartPlayer>> carts = Broker::getInstance()->getPhysicsManager()->getActiveScene()->getAllShoppingCartPlayers();
	std::shared_ptr<ShoppingCartPlayer> player1 = nullptr;
	for (const std::shared_ptr<ShoppingCartPlayer> &cart : carts) {
		std::shared_ptr<PlayerScript> cartScript = std::static_pointer_cast<PlayerScript>(cart->getComponent(ComponentTypes::PLAYER_SCRIPT));
		if (cartScript->_playerType == PlayerTypes::HUMAN && cartScript->_inputID == 1) {
			player1 = cart;
			break;
		}
	}

	physx::PxVec3 playerPos = player1->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().p;

	physx::PxVec3 myPos = (dynamic_cast<ShoppingCartPlayer*>(_entity))->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().p;
	physx::PxVec3 myVelocity = (dynamic_cast<ShoppingCartPlayer*>(_entity))->_actor->is<physx::PxRigidDynamic>()->getLinearVelocity();

	float speed = myVelocity.magnitude();
	float distanceBetween = (myPos - playerPos).magnitude() + 20;
	if (distanceBetween >= 255)
		distanceBetween = 255;

	physx::PxQuat playerRot = player1->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().q;
	physx::PxVec3 forward(0.0f, 0.0f, 1.0f); // on spawn the forward vector of a cart is pointing in the +z direction
	forward = playerRot.rotate(forward); // now forward has been rotated to match the cart's rotation
	// this forward vector is the player's direction
	// if you only need the vector in the xz-plane you can do
	//physx::PxVec3 forwardNoYNormalized = forward;
	physx::PxVec3 forwardNoYNormalized = physx::PxVec3(forward.x, 0.0f, forward.z).getNormalized();
	//physx::PxVec3 myPosNormalized = myPos;
	physx::PxVec3 myPosNormalized = physx::PxVec3(myPos.x, 0.0f, myPos.z).getNormalized();
	float angle = acos(((forwardNoYNormalized.x*myPosNormalized.x) + (forwardNoYNormalized.z*myPosNormalized.z)));
	angle = (angle / 3.1415926) * 180;
	PxVec3 crossprod = forwardNoYNormalized.cross(myPosNormalized);
	bool isCCW = crossprod.y <= 0.0f;
	if (!isCCW) angle = 360 - angle;
	//std::cout << "Angle: " << angle << std::endl;
	//Broker::getInstance()->getAudioManager()->changeVolumeSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::ROLL_SOUND_AI1), Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::ROLL_SOUND_AI1)->volume*(speed / 60)*(distanceBetween / 255));
	

	//Broker::getInstance()->getAudioManager()->playSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::ROLL_SOUND_AI1));
	
	if (otherEntity->getTag() == EntityTypes::OBSTACLE1 || otherEntity->getTag() == EntityTypes::OBSTACLE2 || otherEntity->getTag() == EntityTypes::OBSTACLE3 || otherEntity->getTag() == EntityTypes::OBSTACLE4 || otherEntity->getTag() == EntityTypes::OBSTACLE5 || otherEntity->getTag() == EntityTypes::OBSTACLE6 || otherEntity->getTag() == EntityTypes::OBSTACLE7 || otherEntity->getTag() == EntityTypes::SHOPPING_CART_PLAYER || otherEntity->getTag() == EntityTypes::ROOF) {
		Broker::getInstance()->getAudioManager()->changeDistanceSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::HITWALL_SOUND), distanceBetween, angle);
		Broker::getInstance()->getAudioManager()->playSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::HITWALL_SOUND));
		
		PxVec3 impulseDir = contacts->normal;
		// now, make sure the collision normal points towards the cart body...
		PxVec3 contactPosToCartCenter = (_entity->_actor->is<PxRigidDynamic>()->getGlobalPose().p - contacts->position).getNormalized();
		if (impulseDir.dot(contactPosToCartCenter) < 0) impulseDir *= -1; // switch normal direction to point in same hemispehere as cart body
		
		float impulseMag;
		if (otherEntity->getTag() == EntityTypes::SHOPPING_CART_PLAYER) {
			impulseMag = 10000.0f;
		}
		else {
			impulseMag = 25000.0f;
		}
		_entity->_actor->is<PxRigidDynamic>()->addForce(impulseMag * impulseDir, PxForceMode::eIMPULSE);
		ShoppingCartPlayer *localCart = static_cast<ShoppingCartPlayer*>(_entity);
		localCart->_shoppingCartBase->_wasHitFrameTimer = 10; // magic number for now
	}
}
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
			if (_hasHotPotato) {
				otherScript->giveHotPotato(_hotPotatoTimer);
				_hasHotPotato = false;
				_hotPotatoTimer = -1.0;
			}
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


	//std::cout << "COMPONENT.CPP | NEW SHOPPING LIST!" << std::endl;
	//std::cout << _shoppingList_Types.at(0) << "|" << _shoppingList_Types.at(1) << "|" << _shoppingList_Types.at(2) << std::endl;

}


void PlayerScript::pickedUpItem(EntityTypes pickupType) {
	// add test audio
	std::vector<std::shared_ptr<ShoppingCartPlayer>> carts = Broker::getInstance()->getPhysicsManager()->getActiveScene()->getAllShoppingCartPlayers();
	std::shared_ptr<ShoppingCartPlayer> player1 = nullptr;
	for (const std::shared_ptr<ShoppingCartPlayer> &cart : carts) {
		std::shared_ptr<PlayerScript> cartScript = std::static_pointer_cast<PlayerScript>(cart->getComponent(ComponentTypes::PLAYER_SCRIPT));
		if (cartScript->_playerType == PlayerTypes::HUMAN && cartScript->_inputID == 1) {
			player1 = cart;
			break;
		}
	}

	physx::PxVec3 playerPos = player1->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().p;

	physx::PxVec3 myPos = (dynamic_cast<ShoppingCartPlayer*>(_entity))->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().p;
	physx::PxVec3 myVelocity = (dynamic_cast<ShoppingCartPlayer*>(_entity))->_actor->is<physx::PxRigidDynamic>()->getLinearVelocity();

	float speed = myVelocity.magnitude();
	float distanceBetween = (myPos - playerPos).magnitude() + 20;
	if (distanceBetween >= 255)
		distanceBetween = 255;

	physx::PxQuat playerRot = player1->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().q;
	physx::PxVec3 forward(0.0f, 0.0f, 1.0f); // on spawn the forward vector of a cart is pointing in the +z direction
	forward = playerRot.rotate(forward); // now forward has been rotated to match the cart's rotation
	// this forward vector is the player's direction
	// if you only need the vector in the xz-plane you can do
	//physx::PxVec3 forwardNoYNormalized = forward;
	physx::PxVec3 forwardNoYNormalized = physx::PxVec3(forward.x, 0.0f, forward.z).getNormalized();
	//physx::PxVec3 myPosNormalized = myPos;
	physx::PxVec3 myPosNormalized = physx::PxVec3(myPos.x, 0.0f, myPos.z).getNormalized();
	float angle = acos(((forwardNoYNormalized.x*myPosNormalized.x) + (forwardNoYNormalized.z*myPosNormalized.z)));
	angle = (angle / 3.1415926) * 180;

	PxVec3 crossprod = forwardNoYNormalized.cross(myPosNormalized);
	bool isCCW = crossprod.y <= 0.0f;
	if (!isCCW) angle = 360 - angle;
	if(!(_playerType == PlayerTypes::HUMAN))
		Broker::getInstance()->getAudioManager()->changeDistanceSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::PICKITEM_SOUND), distanceBetween, angle);
	Broker::getInstance()->getAudioManager()->playSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::PICKITEM_SOUND));
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
//	Broker::getInstance()->getAudioManager()->playSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::HITWALL_SOUND));

	std::vector<std::shared_ptr<ShoppingCartPlayer>> carts = Broker::getInstance()->getPhysicsManager()->getActiveScene()->getAllShoppingCartPlayers();
	std::shared_ptr<ShoppingCartPlayer> player1 = nullptr;
	for (const std::shared_ptr<ShoppingCartPlayer> &cart : carts) {
		std::shared_ptr<PlayerScript> cartScript = std::static_pointer_cast<PlayerScript>(cart->getComponent(ComponentTypes::PLAYER_SCRIPT));
		if (cartScript->_playerType == PlayerTypes::HUMAN && cartScript->_inputID == 1) {
			player1 = cart;
			break;
		}
	}

	physx::PxVec3 playerPos = player1->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().p;

	physx::PxVec3 myPos = (dynamic_cast<ShoppingCartPlayer*>(_entity))->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().p;
	physx::PxVec3 myVelocity = (dynamic_cast<ShoppingCartPlayer*>(_entity))->_actor->is<physx::PxRigidDynamic>()->getLinearVelocity();

	float speed = myVelocity.magnitude();
	float distanceBetween = (myPos - playerPos).magnitude() + 20;
	if (distanceBetween >= 255)
		distanceBetween = 255;

	physx::PxQuat playerRot = player1->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().q;
	physx::PxVec3 forward(0.0f, 0.0f, 1.0f); // on spawn the forward vector of a cart is pointing in the +z direction
	forward = playerRot.rotate(forward); // now forward has been rotated to match the cart's rotation
	// this forward vector is the player's direction
	// if you only need the vector in the xz-plane you can do
	//physx::PxVec3 forwardNoYNormalized = forward;
	physx::PxVec3 forwardNoYNormalized = physx::PxVec3(forward.x, 0.0f, forward.z).getNormalized();
	//physx::PxVec3 myPosNormalized = myPos;
	physx::PxVec3 myPosNormalized = physx::PxVec3(myPos.x, 0.0f, myPos.z).getNormalized();
	float angle = acos(((forwardNoYNormalized.x*myPosNormalized.x) + (forwardNoYNormalized.z*myPosNormalized.z)));
	angle = (angle / 3.1415926) * 180;

	PxVec3 crossprod = forwardNoYNormalized.cross(myPosNormalized);
	bool isCCW = crossprod.y <= 0.0f;
	if (!isCCW) angle = 360 - angle;
	if (!(_playerType == PlayerTypes::HUMAN))
		Broker::getInstance()->getAudioManager()->changeDistanceSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::DROPITEM_SOUND), distanceBetween, angle);

	Broker::getInstance()->getAudioManager()->playSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::DROPITEM_SOUND));

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


	//std::cout << "BASH EVENT" << std::endl;
}



// BUG: I THINK I UNDERSTAND THE BUG NOW!!!, This function gets called as a result of fixedupdate(),
// since I instantiate spare change inside here, but I am also inside a loop over all entities, the vector gets changed so the iterator is screwed up!
// FIX: MAKE SURE THAT ENTITIES LIST IS DEEP COPIED BEFORE ITERATING OVER DEEP COPY OR MAKE SURE THAT NO INSTANTIATE CALLS/ REMOVE FROM SCENE CALLS ARE MADE INSIDE A LOOP OVER ENTITIES
// IT WORKED!
void PlayerScript::coinExplosion() {
	ShoppingCartPlayer *cart = static_cast<ShoppingCartPlayer*>(_entity);
	//Broker::getInstance()->getAudioManager()->playSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::HITWALL_SOUND));

	std::vector<EntityTypes> lostItems;
	for (int i = 0; i < 12; i++) {
		lostItems.push_back(EntityTypes::SPARE_CHANGE);
	}

	PxRigidDynamic *cartDyn = cart->_actor->is<PxRigidDynamic>();
	PxTransform cartTransform = cartDyn->getGlobalPose();
	PxVec3 cartPos = cartTransform.p;

	PxVec3 nextSpawnPos = cartPos + PxVec3(0.0f, 11.0f, 0.0f); // NOTE: I'm setting the y-value on the assumption that bashed() will spawn 2 pickups

	std::vector<PxVec3> forces = { PxVec3(0.0f, 40.0f, 20.0f), PxVec3(0.0f, 40.0f, -20.0f), PxVec3(20.0f, 40.0f, 0.0f), PxVec3(-20.0f, 40.0f, 0.0f), PxVec3(0.0f, 40.0f, 30.0f), PxVec3(0.0f, 40.0f, -30.0f), PxVec3(30.0f, 40.0f, 0.0f), PxVec3(-30.0f, 40.0f, 0.0f), PxVec3(0.0f, 40.0f, 40.0f), PxVec3(0.0f, 40.0f, -40.0f), PxVec3(40.0f, 40.0f, 0.0f), PxVec3(-40.0f, 40.0f, 0.0f) };
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

}



// NOTE: the bots should be raycasting every single frame to prevent slowing down and getting stuck
void PlayerScript::navigate() {

	ShoppingCartPlayer *player = dynamic_cast<ShoppingCartPlayer*>(_entity);
	PxTransform transform = player->_actor->is<PxRigidDynamic>()->getGlobalPose();
	PxVec3 pos = transform.p;
	PxQuat rot = transform.q;

	// find angle (in xz-plane between forward direction (can get forward vector, remove the y component then normalize it and compare it to a normalized vector from current pos to target pos)

	PxVec3 forward(0.0f, 0.0f, 1.0f);
	forward = rot.rotate(forward);
	PxVec3 forwardNoY(forward.x, 0.0f, forward.z);
	PxVec3 forwardNormalized = forwardNoY.getNormalized();

	PxVec3 diffNoY;
	PxVec3 diffNormalized;

	bool targetOnHill = false;
	bool targetOnWall = false;
	bool forcedTurbo = false;

	// NOTE: this depends on the map (hill/walls) being symmetrically round (centered at 0,0,0)
	const PxVec3 mapCenterPos = PxVec3(0.0f, 0.0f, 0.0f);
	const float hillTopRadius = 33.0f; // rounding down to be safe
	const float hillBaseRadius = 70.0f; // rounding up to be safe
	const float wallStartRadius = 250.0f; // rounding down to be safe
	if (_targets.size() > 0) {
		PxVec3 targetPos = _targets.at(0)._pos;
		PxVec3 diff = targetPos - pos;
		diffNoY = PxVec3(diff.x, 0.0f, diff.z);
		diffNormalized = diff.getNormalized();

		PxVec3 targetPosNoY = PxVec3(targetPos.x, 0.0f, targetPos.z);
		float targetDistance = (targetPosNoY - mapCenterPos).magnitude();
		if (targetDistance <= hillBaseRadius) targetOnHill = true;
		else if (targetDistance >= wallStartRadius) targetOnWall = true;

		PxVec3 posNoY = PxVec3(pos.x, 0.0f, pos.z);
		float posDistance = (posNoY - mapCenterPos).magnitude();
		
		// force turbo to go up slopes (outer wall and hill slope)
		if ((targetOnHill && posDistance > hillTopRadius && posDistance <= hillBaseRadius) || (targetOnWall && posDistance >= wallStartRadius)) forcedTurbo = true;

		// ~~~~~~~~~~NOTE: should I only force turbo for cookie until on hill top?

		// force turbo if goind after starting cookie...
		if (ItemLocation::TargetTypes::COOKIE == _targets.at(0)._targetType) forcedTurbo = true;

		// NOTE: I could also force turbo to get to mystery bag, but since AI are omnipotent, this might make it nearly impossible for humans to get to it, unless they are really close...
	}

	
	// 5 raycasts (FarLeft, MidLeft, Center, MidRight, FarRight)

	// 1. find the forward vector...

	// already have it above...

	// normalize the forward vector (this will be the unit dir for the middle 3 raycasts


	PxVec3 farLeftOrigin = rot.rotate(PxVec3(1.75f, 0.0f, 2.7f)) + pos;
	PxVec3 midLeftOrigin = rot.rotate(PxVec3(1.75f, 0.0f, 2.7f)) + pos;
	PxVec3 centerOrigin = rot.rotate(PxVec3(0.0f, 0.0f, 2.7f)) + pos;
	PxVec3 midRightOrigin = rot.rotate(PxVec3(-1.75f, 0.0f, 2.7f)) + pos;
	PxVec3 farRightOrigin = rot.rotate(PxVec3(-1.75f, 0.0f, 2.7f)) + pos;

	PxVec3 farLeftUnitDir = (rot.rotate(PxVec3(1.0, 0.0, 1.0))).getNormalized();
	PxVec3 midLeftUnitDir = forward.getNormalized();
	PxVec3 centerUnitDir = forward.getNormalized();
	PxVec3 midRightUnitDir = forward.getNormalized();
	PxVec3 farRightUnitDir = (rot.rotate(PxVec3(-1.0, 0.0, 1.0))).getNormalized();

	PxReal farLeftDistance = 20.0f;
	PxReal midLeftDistance = 20.0f;
	PxReal centerDistance = 20.0f;
	PxReal midRightDistance = 20.0f;
	PxReal farRightDistance = 20.0f;

	PxHitBuffer<PxRaycastHit> farLeftHit;
	PxHitBuffer<PxRaycastHit> midLeftHit;
	PxHitBuffer<PxRaycastHit> centerHit;
	PxHitBuffer<PxRaycastHit> midRightHit;
	PxHitBuffer<PxRaycastHit> farRightHit;

	bool farLeftStatus = Broker::getInstance()->getPhysicsManager()->raycast(farLeftOrigin, farLeftUnitDir, farLeftDistance, farLeftHit);
	bool midLeftStatus = Broker::getInstance()->getPhysicsManager()->raycast(midLeftOrigin, midLeftUnitDir, midLeftDistance, midLeftHit);
	bool centerStatus = Broker::getInstance()->getPhysicsManager()->raycast(centerOrigin, centerUnitDir, centerDistance, centerHit);
	bool midRightStatus = Broker::getInstance()->getPhysicsManager()->raycast(midRightOrigin, midRightUnitDir, midRightDistance, midRightHit);
	bool farRightStatus = Broker::getInstance()->getPhysicsManager()->raycast(farRightOrigin, farRightUnitDir, farRightDistance, farRightHit);


	bool redirected = false;
	if (farLeftStatus || midLeftStatus || centerStatus || midRightStatus || farRightStatus) {

		int turnDir = 0;

		if (farLeftStatus) {
			if (farLeftHit.hasBlock) {
				Entity *entityHit = static_cast<Entity*>(farLeftHit.block.actor->userData);
				if (entityHit->getTag() == EntityTypes::GROUND) {
					// supress raycast if target on hill and hit point on hill OR target on wall and hit point on wall
					PxVec3 hitPosNoY = PxVec3(farLeftHit.block.position.x, 0.0f, farLeftHit.block.position.z);
					float hitDistance = (hitPosNoY - mapCenterPos).magnitude();
					if (!((targetOnHill && hitDistance <= hillBaseRadius) || (targetOnWall && hitDistance >= wallStartRadius))) {
						if (fabs(farLeftHit.block.normal.y - 1.0f) >= 0.0001f) turnDir += 1; // BUGFIX FOR NOW: ignore raycasts that hit the ground plane (normal.y = 1)
						redirected = true;
					}
				}
				else if (entityHit->getTag() == EntityTypes::SHOPPING_CART_PLAYER) {
					// supress raycasts with another cart that has the same target as you...
					// also supress raycasts if you have hot potato and hit cart is not bash protected...
					ShoppingCartPlayer *hitPlayer = dynamic_cast<ShoppingCartPlayer*>(entityHit);
					std::shared_ptr<PlayerScript> hitPlayerScript = std::static_pointer_cast<PlayerScript>(hitPlayer->getComponent(ComponentTypes::PLAYER_SCRIPT));

					if (_hasHotPotato) {
						if (hitPlayer->_shoppingCartBase->IsBashProtected()) {
							turnDir += 1;
							redirected = true;
						}
					}
					else {
						if (_targets.size() > 0 && hitPlayerScript->_targets.size() > 0) {
							if (!isApproxEqual(_targets.at(0)._pos, hitPlayerScript->_targets.at(0)._pos)) {
								turnDir += 1;
								redirected = true;
							}
						}
						else { // NOTE: this case will always apply if the hitPlayer is a human cart (since targets size == 0)
							turnDir += 1;
							redirected = true;
						}
					}
				}
				else if (entityHit->getTag() == EntityTypes::OBSTACLE1 || entityHit->getTag() == EntityTypes::OBSTACLE2 || entityHit->getTag() == EntityTypes::OBSTACLE3 || entityHit->getTag() == EntityTypes::OBSTACLE4 || entityHit->getTag() == EntityTypes::OBSTACLE5 || entityHit->getTag() == EntityTypes::OBSTACLE6 || entityHit->getTag() == EntityTypes::OBSTACLE7) {
					turnDir += 1;
					redirected = true;
				}
			}
		}
		if (midLeftStatus) {
			if (midLeftHit.hasBlock) {
				Entity *entityHit = static_cast<Entity*>(midLeftHit.block.actor->userData);
				if (entityHit->getTag() == EntityTypes::GROUND) {
					// supress raycast if target on hill and hit point on hill OR target on wall and hit point on wall
					PxVec3 hitPosNoY = PxVec3(midLeftHit.block.position.x, 0.0f, midLeftHit.block.position.z);
					float hitDistance = (hitPosNoY - mapCenterPos).magnitude();
					if (!((targetOnHill && hitDistance <= hillBaseRadius) || (targetOnWall && hitDistance >= wallStartRadius))) {
						if (fabs(midLeftHit.block.normal.y - 1.0f) >= 0.0001f) turnDir += 2; // BUGFIX FOR NOW: ignore raycasts that hit the ground plane (normal.y = 1)
						redirected = true;
					}
				}
				else if (entityHit->getTag() == EntityTypes::SHOPPING_CART_PLAYER) {
					// supress raycasts with another cart that has the same target as you...
					// also supress raycasts if you have hot potato and hit cart is not bash protected...
					ShoppingCartPlayer *hitPlayer = dynamic_cast<ShoppingCartPlayer*>(entityHit);
					std::shared_ptr<PlayerScript> hitPlayerScript = std::static_pointer_cast<PlayerScript>(hitPlayer->getComponent(ComponentTypes::PLAYER_SCRIPT));

					if (_hasHotPotato) {
						if (hitPlayer->_shoppingCartBase->IsBashProtected()) {
							turnDir += 2;
							redirected = true;
						}
					}
					else {
						if (_targets.size() > 0 && hitPlayerScript->_targets.size() > 0) {
							if (!isApproxEqual(_targets.at(0)._pos, hitPlayerScript->_targets.at(0)._pos)) {
								turnDir += 2;
								redirected = true;
							}
						}
						else { // NOTE: this case will always apply if the hitPlayer is a human cart (since targets size == 0)
							turnDir += 2;
							redirected = true;
						}
					}
				}
				else if (entityHit->getTag() == EntityTypes::OBSTACLE1 || entityHit->getTag() == EntityTypes::OBSTACLE2 || entityHit->getTag() == EntityTypes::OBSTACLE3 || entityHit->getTag() == EntityTypes::OBSTACLE4 || entityHit->getTag() == EntityTypes::OBSTACLE5 || entityHit->getTag() == EntityTypes::OBSTACLE6 || entityHit->getTag() == EntityTypes::OBSTACLE7) {
					turnDir += 2;
					redirected = true;
				}
			}
		}
		if (midRightStatus) {
			if (midRightHit.hasBlock) {
				Entity *entityHit = static_cast<Entity*>(midRightHit.block.actor->userData);
				if (entityHit->getTag() == EntityTypes::GROUND) {
					// supress raycast if target on hill and hit point on hill OR target on wall and hit point on wall
					PxVec3 hitPosNoY = PxVec3(midRightHit.block.position.x, 0.0f, midRightHit.block.position.z);
					float hitDistance = (hitPosNoY - mapCenterPos).magnitude();
					if (!((targetOnHill && hitDistance <= hillBaseRadius) || (targetOnWall && hitDistance >= wallStartRadius))) {
						if (fabs(midRightHit.block.normal.y - 1.0f) >= 0.0001f) turnDir -= 2; // BUGFIX FOR NOW: ignore raycasts that hit the ground plane (normal.y = 1)
						redirected = true;
					}
				}
				else if (entityHit->getTag() == EntityTypes::SHOPPING_CART_PLAYER) {
					// supress raycasts with another cart that has the same target as you...
					// also supress raycasts if you have hot potato and hit cart is not bash protected...
					ShoppingCartPlayer *hitPlayer = dynamic_cast<ShoppingCartPlayer*>(entityHit);
					std::shared_ptr<PlayerScript> hitPlayerScript = std::static_pointer_cast<PlayerScript>(hitPlayer->getComponent(ComponentTypes::PLAYER_SCRIPT));

					if (_hasHotPotato) {
						if (hitPlayer->_shoppingCartBase->IsBashProtected()) {
							turnDir -= 2;
							redirected = true;
						}
					}
					else {
						if (_targets.size() > 0 && hitPlayerScript->_targets.size() > 0) {
							if (!isApproxEqual(_targets.at(0)._pos, hitPlayerScript->_targets.at(0)._pos)) {
								turnDir -= 2;
								redirected = true;
							}
						}
						else { // NOTE: this case will always apply if the hitPlayer is a human cart (since targets size == 0)
							turnDir -= 2;
							redirected = true;
						}
					}
				}
				else if (entityHit->getTag() == EntityTypes::OBSTACLE1 || entityHit->getTag() == EntityTypes::OBSTACLE2 || entityHit->getTag() == EntityTypes::OBSTACLE3 || entityHit->getTag() == EntityTypes::OBSTACLE4 || entityHit->getTag() == EntityTypes::OBSTACLE5 || entityHit->getTag() == EntityTypes::OBSTACLE6 || entityHit->getTag() == EntityTypes::OBSTACLE7) {
					turnDir -= 2;
					redirected = true;
				}
			}
		}
		if (farRightStatus) {
			if (farRightHit.hasBlock) {
				Entity *entityHit = static_cast<Entity*>(farRightHit.block.actor->userData);
				if (entityHit->getTag() == EntityTypes::GROUND) {
					// supress raycast if target on hill and hit point on hill OR target on wall and hit point on wall
					PxVec3 hitPosNoY = PxVec3(farRightHit.block.position.x, 0.0f, farRightHit.block.position.z);
					float hitDistance = (hitPosNoY - mapCenterPos).magnitude();
					if (!((targetOnHill && hitDistance <= hillBaseRadius) || (targetOnWall && hitDistance >= wallStartRadius))) {
						if (fabs(farRightHit.block.normal.y - 1.0f) >= 0.0001f) turnDir -= 1; // BUGFIX FOR NOW: ignore raycasts that hit the ground plane (normal.y = 1)
						redirected = true;
					}
				}
				else if (entityHit->getTag() == EntityTypes::SHOPPING_CART_PLAYER) {
					// supress raycasts with another cart that has the same target as you...
					// also supress raycasts if you have hot potato and hit cart is not bash protected...
					ShoppingCartPlayer *hitPlayer = dynamic_cast<ShoppingCartPlayer*>(entityHit);
					std::shared_ptr<PlayerScript> hitPlayerScript = std::static_pointer_cast<PlayerScript>(hitPlayer->getComponent(ComponentTypes::PLAYER_SCRIPT));

					if (_hasHotPotato) {
						if (hitPlayer->_shoppingCartBase->IsBashProtected()) {
							turnDir -= 1;
							redirected = true;
						}
					}
					else {
						if (_targets.size() > 0 && hitPlayerScript->_targets.size() > 0) {
							if (!isApproxEqual(_targets.at(0)._pos, hitPlayerScript->_targets.at(0)._pos)) {
								turnDir -= 1;
								redirected = true;
							}
						}
						else { // NOTE: this case will always apply if the hitPlayer is a human cart (since targets size == 0)
							turnDir -= 1;
							redirected = true;
						}
					}
				}
				else if (entityHit->getTag() == EntityTypes::OBSTACLE1 || entityHit->getTag() == EntityTypes::OBSTACLE2 || entityHit->getTag() == EntityTypes::OBSTACLE3 || entityHit->getTag() == EntityTypes::OBSTACLE4 || entityHit->getTag() == EntityTypes::OBSTACLE5 || entityHit->getTag() == EntityTypes::OBSTACLE6 || entityHit->getTag() == EntityTypes::OBSTACLE7) {
					turnDir -= 1;
					redirected = true;
				}
			}
		}



		if (turnDir == 0) {
			if (centerStatus) {
				if (centerHit.hasBlock) {
					Entity *entityHit = static_cast<Entity*>(centerHit.block.actor->userData);
					if (entityHit->getTag() == EntityTypes::GROUND) {
						// supress raycast if target on hill and hit point on hill OR target on wall and hit point on wall
						PxVec3 hitPosNoY = PxVec3(centerHit.block.position.x, 0.0f, centerHit.block.position.z);
						float hitDistance = (hitPosNoY - mapCenterPos).magnitude();
						if (!((targetOnHill && hitDistance <= hillBaseRadius) || (targetOnWall && hitDistance >= wallStartRadius))) {
							if (fabs(centerHit.block.normal.y - 1.0f) >= 0.0001f) turnDir = 3; // BUGFIX FOR NOW: ignore raycasts that hit the ground plane (normal.y = 1)
							redirected = true;
						}
					}
					else if (entityHit->getTag() == EntityTypes::SHOPPING_CART_PLAYER) {
						// supress raycasts with another cart that has the same target as you...
						// also supress raycasts if you have hot potato and hit cart is not bash protected...
						ShoppingCartPlayer *hitPlayer = dynamic_cast<ShoppingCartPlayer*>(entityHit);
						std::shared_ptr<PlayerScript> hitPlayerScript = std::static_pointer_cast<PlayerScript>(hitPlayer->getComponent(ComponentTypes::PLAYER_SCRIPT));

						if (_hasHotPotato) {
							if (hitPlayer->_shoppingCartBase->IsBashProtected()) {
								turnDir = 3;
								redirected = true;
							}
						}
						else {
							if (_targets.size() > 0 && hitPlayerScript->_targets.size() > 0) {
								if (!isApproxEqual(_targets.at(0)._pos, hitPlayerScript->_targets.at(0)._pos)) {
									turnDir = 3;
									redirected = true;
								}
							}
							else { // NOTE: this case will always apply if the hitPlayer is a human cart (since targets size == 0)
								turnDir = 3;
								redirected = true;
							}
						}
					}
					else if (entityHit->getTag() == EntityTypes::OBSTACLE1 || entityHit->getTag() == EntityTypes::OBSTACLE2 || entityHit->getTag() == EntityTypes::OBSTACLE3 || entityHit->getTag() == EntityTypes::OBSTACLE4 || entityHit->getTag() == EntityTypes::OBSTACLE5 || entityHit->getTag() == EntityTypes::OBSTACLE6 || entityHit->getTag() == EntityTypes::OBSTACLE7) {
						turnDir = 3;
						redirected = true;
					}
				}
			}
		}

		if (redirected) {
			PxReal accel = 1.0f;
			PxReal reverse = 0.0f;
			PxReal handbrake = 0.0f;
			PxReal steer;

			switch (turnDir) {
			case -3:
				steer = 1.0f;
				break;
			case -2:
				steer = 0.65f;
				break;
			case -1:
				steer = 0.35f;
				break;
			case 0:
				steer = 0.0f;
				break;
			case 1:
				steer = -0.35f;
				break;
			case 2:
				steer = -0.65f;
				break;
			case 3:
				steer = -1.0f;
				break;
			}

			bool turboButtonPressed = (_hasHotPotato || forcedTurbo);

			player->_shoppingCartBase->processRawInputDataController(accel, reverse, handbrake, steer, turboButtonPressed);
		}

	}
	if (!redirected) {
		if (_targets.size() == 0) { // if bot doesnt have a current target for some reason...
			//std::cout << "BOT WITHOUT A JOB!" << std::endl;
			//player->_shoppingCartBase->processRawInputDataController(0.0f, 0.0f, 1.0f, 0.0f, false); // put bot into a braking mode
			return;
		}

		

		// otherwise, we have a target to get to...

		PxVec3 crossprod = forwardNormalized.cross(diffNormalized);

		bool isCCW = crossprod.y >= 0;

		// now set steer propertional to angle (abs value set based on dot product angle and then multiply by -1/1 based on isCCW)

		// dot product will be between -1 and 1, so I want to shrink this range to be -1->1 and 1->0 

		float dotProd = forwardNormalized.dot(diffNormalized);
		float steerAbs = ((dotProd * -1) + 1) / 2;


		//PxReal accel = diffNoYMag > 1.0f ? 1.0f : diffNoYMag;
		PxReal accel = 0.9f;
		PxReal reverse = 0.0f;
		PxReal handbrake = 0.0f;
		//PxReal steer = isCCW ? steerAbs : steerAbs * -1;
		//PxReal steer = isCCW ? 1.0f : -1.0f;

		PxReal steer;

		if (steerAbs < 0.01) steer = 0.0;
		else steer = isCCW ? 1.0f : -1.0f;


		bool turboButtonPressed = (_hasHotPotato || forcedTurbo);
		

		player->_shoppingCartBase->processRawInputDataController(accel, reverse, handbrake, steer, turboButtonPressed);
	}


	// now check if we have arrived at (close) to our destination...
/*
	if (_targets.size() > 0) {
		float diffNoYMag = diffNoY.magnitude();
		if (diffNoYMag < 5.0f) {
			_targets.erase(_targets.begin() + 0); // erase first target (element 0)
		}
	}
*/	
}


void PlayerScript::giveHotPotato(double remainingDuration) {
	_hasHotPotato = true;
	_hotPotatoTimer = remainingDuration;
	// give max turbo to hot potato recipient...
	ShoppingCartPlayer *player = dynamic_cast<ShoppingCartPlayer*>(_entity);
	player->_shoppingCartBase->_turboFuel = 4.0f;
	player->_shoppingCartBase->_nbBoosts = 4;
}

void PlayerScript::tickHotPotatoTimer(double fixedDeltaTime) {
	std::vector<std::shared_ptr<ShoppingCartPlayer>> carts = Broker::getInstance()->getPhysicsManager()->getActiveScene()->getAllShoppingCartPlayers();
	std::shared_ptr<ShoppingCartPlayer> player1 = nullptr;
	for (const std::shared_ptr<ShoppingCartPlayer> &cart : carts) {
		std::shared_ptr<PlayerScript> cartScript = std::static_pointer_cast<PlayerScript>(cart->getComponent(ComponentTypes::PLAYER_SCRIPT));
		if (cartScript->_playerType == PlayerTypes::HUMAN && cartScript->_inputID == 1) {
			player1 = cart;
			break;
		}
	}

	physx::PxVec3 playerPos = player1->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().p;

	physx::PxVec3 myPos = (dynamic_cast<ShoppingCartPlayer*>(_entity))->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().p;
	physx::PxVec3 myVelocity = (dynamic_cast<ShoppingCartPlayer*>(_entity))->_actor->is<physx::PxRigidDynamic>()->getLinearVelocity();

	float speed = myVelocity.magnitude();
	float distanceBetween = (myPos - playerPos).magnitude() + 20;
	if (distanceBetween >= 255)
		distanceBetween = 255;

	physx::PxQuat playerRot = player1->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().q;
	physx::PxVec3 forward(0.0f, 0.0f, 1.0f); // on spawn the forward vector of a cart is pointing in the +z direction
	forward = playerRot.rotate(forward); // now forward has been rotated to match the cart's rotation
	// this forward vector is the player's direction
	// if you only need the vector in the xz-plane you can do
	//physx::PxVec3 forwardNoYNormalized = forward;
	physx::PxVec3 forwardNoYNormalized = physx::PxVec3(forward.x, 0.0f, forward.z).getNormalized();
	//physx::PxVec3 myPosNormalized = myPos;
	physx::PxVec3 myPosNormalized = physx::PxVec3(myPos.x, 0.0f, myPos.z).getNormalized();
	float angle = acos(((forwardNoYNormalized.x*myPosNormalized.x) + (forwardNoYNormalized.z*myPosNormalized.z)));
	angle = (angle / 3.1415926) * 180;

	PxVec3 crossprod = forwardNoYNormalized.cross(myPosNormalized);
	bool isCCW = crossprod.y <= 0.0f;
	if (!isCCW) angle = 360 - angle;
	if (!(_playerType == PlayerTypes::HUMAN))
		Broker::getInstance()->getAudioManager()->changeDistanceSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::TICKING_SOUND), distanceBetween, angle);

	_hotPotatoTimer -= fixedDeltaTime;
	if (_hotPotatoTimer <= 0.0) {
		explodeHotPotato();
	}
}

void PlayerScript::explodeHotPotato() {
	_hasHotPotato = false;
	_hotPotatoTimer = -1.0;
	const int hotPotatoLostPoints = 75;
	subPoints(75);
	bashed();
	//TODO: UI indicator that you exploded/points were lost???

	std::vector<std::shared_ptr<ShoppingCartPlayer>> carts = Broker::getInstance()->getPhysicsManager()->getActiveScene()->getAllShoppingCartPlayers();
	std::shared_ptr<ShoppingCartPlayer> player1 = nullptr;
	for (const std::shared_ptr<ShoppingCartPlayer> &cart : carts) {
		std::shared_ptr<PlayerScript> cartScript = std::static_pointer_cast<PlayerScript>(cart->getComponent(ComponentTypes::PLAYER_SCRIPT));
		if (cartScript->_playerType == PlayerTypes::HUMAN && cartScript->_inputID == 1) {
			player1 = cart;
			break;
		}
	}

	physx::PxVec3 playerPos = player1->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().p;

	physx::PxVec3 myPos = (dynamic_cast<ShoppingCartPlayer*>(_entity))->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().p;
	physx::PxVec3 myVelocity = (dynamic_cast<ShoppingCartPlayer*>(_entity))->_actor->is<physx::PxRigidDynamic>()->getLinearVelocity();

	float speed = myVelocity.magnitude();
	float distanceBetween = (myPos - playerPos).magnitude() + 20;
	if (distanceBetween >= 255)
		distanceBetween = 255;

	physx::PxQuat playerRot = player1->_actor->is<physx::PxRigidDynamic>()->getGlobalPose().q;
	physx::PxVec3 forward(0.0f, 0.0f, 1.0f); // on spawn the forward vector of a cart is pointing in the +z direction
	forward = playerRot.rotate(forward); // now forward has been rotated to match the cart's rotation
	// this forward vector is the player's direction
	// if you only need the vector in the xz-plane you can do
	//physx::PxVec3 forwardNoYNormalized = forward;
	physx::PxVec3 forwardNoYNormalized = physx::PxVec3(forward.x, 0.0f, forward.z).getNormalized();
	//physx::PxVec3 myPosNormalized = myPos;
	physx::PxVec3 myPosNormalized = physx::PxVec3(myPos.x, 0.0f, myPos.z).getNormalized();
	float angle = acos(((forwardNoYNormalized.x*myPosNormalized.x) + (forwardNoYNormalized.z*myPosNormalized.z)));
	angle = (angle / 3.1415926) * 180;

	PxVec3 crossprod = forwardNoYNormalized.cross(myPosNormalized);
	bool isCCW = crossprod.y <= 0.0f;
	if (!isCCW) angle = 360 - angle;
	if (!(_playerType == PlayerTypes::HUMAN))
		Broker::getInstance()->getAudioManager()->changeDistanceSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::EXPLOSION_SOUND), distanceBetween, angle);

	Broker::getInstance()->getAudioManager()->playSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::EXPLOSION_SOUND));
	Broker::getInstance()->getAudioManager()->haltSFX(Broker::getInstance()->getAudioManager()->getSoundEffect(SoundEffectTypes::TICKING_SOUND));

	coinExplosion();
}


int PlayerScript::getNBBoosts() {
	ShoppingCartPlayer *player = dynamic_cast<ShoppingCartPlayer*>(_entity);
	return player->_shoppingCartBase->_nbBoosts;
}





