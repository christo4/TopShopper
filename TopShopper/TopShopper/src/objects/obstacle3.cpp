#include "obstacle3.h"
#include "PxRigidStatic.h"

using namespace physx;


Obstacle3::Obstacle3(PxRigidStatic *actor)
	: Entity(actor, EntityTypes::OBSTACLE3)
{
	_actor->userData = this;

	//addComponent(ComponentTypes::MESH);
}


Obstacle3::~Obstacle3() {

}