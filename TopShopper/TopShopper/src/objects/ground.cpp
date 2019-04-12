#include "ground.h"
#include "PxRigidStatic.h"

using namespace physx;


Ground::Ground(PxRigidStatic *actor)
	: Entity(actor, EntityTypes::GROUND)
{
	_actor->userData = this;
}


Ground::~Ground() {

}
