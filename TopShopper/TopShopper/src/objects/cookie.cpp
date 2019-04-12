#include "cookie.h"
#include "PxRigidDynamic.h"

using namespace physx;

Cookie::Cookie(PxRigidDynamic *actor)
	: Entity(actor, EntityTypes::COOKIE)
{
	_actor->userData = this;

	addComponent(ComponentTypes::PICKUP_SCRIPT);
	std::shared_ptr<PickupScript> pickupScript = std::static_pointer_cast<PickupScript>(getComponent(ComponentTypes::PICKUP_SCRIPT));
	pickupScript->_points = 75;
}


Cookie::~Cookie() {

}