#include "obstacle4.h"
#include "PxRigidStatic.h"

using namespace physx;


Obstacle4::Obstacle4(PxRigidStatic *actor)
	: Entity(actor, EntityTypes::OBSTACLE4)
{
	_actor->userData = this;

	//addComponent(ComponentTypes::MESH);
}


Obstacle4::~Obstacle4() {

}