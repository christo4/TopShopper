#include "mysterybag.h"
#include "PxRigidDynamic.h"

using namespace physx;

MysteryBag::MysteryBag(PxRigidDynamic *actor)
	: Entity(actor, EntityTypes::MYSTERY_BAG)
{
	_actor->userData = this;

	addComponent(ComponentTypes::MYSTERY_BAG_SCRIPT);
	std::shared_ptr<MysteryBagScript> mysteryBagScript = std::static_pointer_cast<MysteryBagScript>(getComponent(ComponentTypes::MYSTERY_BAG_SCRIPT));
}


MysteryBag::~MysteryBag() {

}