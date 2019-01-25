
#ifndef STATICPHYSICALENTITY_H_
#define STATICPHYSICALENTITY_H_

#include "physicalentity.h"

// DEFINTION:
// EACH STATIC PHYSICAL ENTITY IS A PHYSICAL ENTITY WHOSE TRANSFORM WILL NEVER BE ALTERED AFTER CREATION
class StaticPhysicalEntity : public PhysicalEntity {
public:
	StaticPhysicalEntity();
	virtual ~StaticPhysicalEntity();

	physx::PxRigidStatic* _actor = nullptr;
private:

};





#endif // STATICPHYSICALENTITY_H_
