#include "obstacle5.h"
#include "PxRigidStatic.h"

using namespace physx;


Obstacle5::Obstacle5(PxRigidStatic *actor)
	: Entity(actor, EntityTypes::OBSTACLE5)
{
	_actor->userData = this;
}


Obstacle5::~Obstacle5() {

}
