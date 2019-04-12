#include "obstacle2.h"
#include "PxRigidStatic.h"

using namespace physx;


Obstacle2::Obstacle2(PxRigidStatic *actor)
	: Entity(actor, EntityTypes::OBSTACLE2)
{
	_actor->userData = this;
}


Obstacle2::~Obstacle2() {

}