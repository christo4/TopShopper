#include "ground.h"
#include "PxRigidStatic.h"

using namespace physx;


Ground::Ground(PxRigidStatic *actor)
	: Entity(actor, EntityTypes::GROUND)
{
	_actor->userData = this;

	addComponent(ComponentTypes::MESH);
	addComponent(ComponentTypes::NAV_MESH);
}


Ground::~Ground() {

}


void Ground::updatePhysics(double deltaTime) {

}
