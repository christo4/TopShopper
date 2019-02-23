#ifndef MILK_H_
#define MILK_H_

#include "entity.h"

namespace physx {
	class PxRigidDynamic;
};

class Milk : public Entity {
public:
	Milk(physx::PxRigidDynamic *actor);
	virtual ~Milk();

private:

};




#endif // MILK_H_
