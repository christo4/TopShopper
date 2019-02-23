#include "broccoli.h"
#include "PxRigidDynamic.h"

using namespace physx;

Broccoli::Broccoli(PxRigidDynamic *actor)
	: Entity(actor, EntityTypes::BROCCOLI)
{
	_actor->userData = this;

	//addComponent(ComponentTypes::AUDIO_SOURCE);
	//addComponent(ComponentTypes::MESH);

	addComponent(ComponentTypes::PICKUP_SCRIPT);
	std::shared_ptr<PickupScript> pickupScript = std::static_pointer_cast<PickupScript>(getComponent(ComponentTypes::PICKUP_SCRIPT));
	pickupScript->_points = 10;
	pickupScript->_rotationSpeed = 180.0; // degrees/s ??? -- TODO: figure this out
}


Broccoli::~Broccoli() {

}