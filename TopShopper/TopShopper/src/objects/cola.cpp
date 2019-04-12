#include "cola.h"
#include "PxRigidDynamic.h"

using namespace physx;

Cola::Cola(PxRigidDynamic *actor)
	: Entity(actor, EntityTypes::COLA)
{
	_actor->userData = this;

	addComponent(ComponentTypes::PICKUP_SCRIPT);
	std::shared_ptr<PickupScript> pickupScript = std::static_pointer_cast<PickupScript>(getComponent(ComponentTypes::PICKUP_SCRIPT));
	pickupScript->_points = 10;
}


Cola::~Cola() {

}