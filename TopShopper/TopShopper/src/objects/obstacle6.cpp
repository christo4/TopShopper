#include "obstacle6.h"
#include "PxRigidStatic.h"

using namespace physx;


Obstacle6::Obstacle6(PxRigidStatic *actor)
	: Entity(actor, EntityTypes::OBSTACLE6)
{
	_actor->userData = this;

	//addComponent(ComponentTypes::MESH);
}


Obstacle6::~Obstacle6() {

}
