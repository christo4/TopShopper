#ifndef BANANA_H_
#define BANANA_H_

#include "entity.h"

namespace physx {
	class PxRigidDynamic;
};

class Banana : public Entity {
public:
	Banana(physx::PxRigidDynamic *actor);
	virtual ~Banana();

private:

};




#endif // BANANA_H_
