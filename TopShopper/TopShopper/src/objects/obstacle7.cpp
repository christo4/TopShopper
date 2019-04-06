#include "obstacle7.h"
#include "PxRigidStatic.h"

using namespace physx;


Obstacle7::Obstacle7(PxRigidStatic *actor)
	: Entity(actor, EntityTypes::OBSTACLE7)
{
	_actor->userData = this;

	//addComponent(ComponentTypes::MESH);
}


Obstacle7::~Obstacle7() {

}
