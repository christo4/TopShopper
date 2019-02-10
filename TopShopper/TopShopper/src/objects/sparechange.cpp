#include "sparechange.h"
#include "PxRigidDynamic.h"

using namespace physx;

SpareChange::SpareChange(PxRigidDynamic *actor) 
	: Entity(actor, EntityTypes::SPARE_CHANGE)
{
	_actor->userData = this;

	addComponent(ComponentTypes::AUDIO_SOURCE);
	addComponent(ComponentTypes::MESH);
}


SpareChange::~SpareChange() {

}


void SpareChange::updatePhysics(double deltaTime) {

}