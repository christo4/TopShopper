#ifndef APPLE_H_
#define APPLE_H_

#include "entity.h"

namespace physx {
	class PxRigidDynamic;
};

class Apple : public Entity {
public:
	Apple(physx::PxRigidDynamic *actor);
	virtual ~Apple();

private:

};




#endif // APPLE_H_
