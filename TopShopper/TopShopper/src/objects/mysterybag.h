#ifndef MYSTERYBAG_H_
#define MYSTERYBAG_H_

#include "entity.h"

namespace physx {
	class PxRigidDynamic;
};

class MysteryBag : public Entity {
public:
	MysteryBag(physx::PxRigidDynamic *actor);
	virtual ~MysteryBag();

private:

};




#endif // MYSTERYBAG_H_
