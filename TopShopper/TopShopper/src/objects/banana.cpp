#include "banana.h"
#include "PxRigidDynamic.h"

using namespace physx;

Banana::Banana(PxRigidDynamic *actor)
	: Entity(actor, EntityTypes::BANANA)
{
	_actor->userData = this;

	addComponent(ComponentTypes::PICKUP_SCRIPT);
	std::shared_ptr<PickupScript> pickupScript = std::static_pointer_cast<PickupScript>(getComponent(ComponentTypes::PICKUP_SCRIPT));
	pickupScript->_points = 10;
}


Banana::~Banana() {

}