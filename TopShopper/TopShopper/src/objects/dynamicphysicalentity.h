
#ifndef DYNAMICPHYSICALENTITY_H_
#define DYNAMICPHYSICALENTITY_H_

#include "physicalentity.h"

// EACH DYNAMIC PHYSICAL ENTITY IS A PHYSICAL ENTITY WHOSE TRANSFORM CAN CHANGE THROUGHOUT THE SIMULATION
// CAN EITHER BE NON-KINEMATIC (AFFECTED BY PHYSICS FORCES) OR KINEMATIC (NOT AFFECTED BY PHYSICS FORCES) 
class DynamicPhysicalEntity : public PhysicalEntity {
public:
	DynamicPhysicalEntity(bool isKinematic);
	virtual ~DynamicPhysicalEntity();

	physx::PxRigidDynamic* _actor = nullptr;
private:
	bool _isKinematic;
};





#endif // DYNAMICPHYSICALENTITY_H_