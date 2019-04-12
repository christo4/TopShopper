#include "sparechange.h"
#include "PxRigidDynamic.h"

using namespace physx;

SpareChange::SpareChange(PxRigidDynamic *actor) 
	: Entity(actor, EntityTypes::SPARE_CHANGE)
{
	_actor->userData = this;

	addComponent(ComponentTypes::PICKUP_SCRIPT);
	std::shared_ptr<PickupScript> pickupScript = std::static_pointer_cast<PickupScript>(getComponent(ComponentTypes::PICKUP_SCRIPT));
	pickupScript->_points = 1;
}


SpareChange::~SpareChange() {

}