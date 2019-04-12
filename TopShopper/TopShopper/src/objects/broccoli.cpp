#include "broccoli.h"
#include "PxRigidDynamic.h"

using namespace physx;

Broccoli::Broccoli(PxRigidDynamic *actor)
	: Entity(actor, EntityTypes::BROCCOLI)
{
	_actor->userData = this;

	addComponent(ComponentTypes::PICKUP_SCRIPT);
	std::shared_ptr<PickupScript> pickupScript = std::static_pointer_cast<PickupScript>(getComponent(ComponentTypes::PICKUP_SCRIPT));
	pickupScript->_points = 10;
}


Broccoli::~Broccoli() {

}