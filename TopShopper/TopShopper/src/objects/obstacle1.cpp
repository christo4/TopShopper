#include "obstacle1.h"
#include "PxRigidStatic.h"

using namespace physx;


Obstacle1::Obstacle1(PxRigidStatic *actor)
	: Entity(actor, EntityTypes::OBSTACLE1)
{
	_actor->userData = this;
}


Obstacle1::~Obstacle1() {

}
