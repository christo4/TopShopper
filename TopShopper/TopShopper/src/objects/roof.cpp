#include "roof.h"
#include "PxRigidStatic.h"

using namespace physx;


Roof::Roof(PxRigidStatic *actor)
	: Entity(actor, EntityTypes::ROOF)
{
	_actor->userData = this;

	//addComponent(ComponentTypes::MESH);
}


Roof::~Roof() {

}
