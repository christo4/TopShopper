#ifndef COLA_H_
#define COLA_H_

#include "entity.h"

namespace physx {
	class PxRigidDynamic;
};

class Cola : public Entity {
public:
	Cola(physx::PxRigidDynamic *actor);
	virtual ~Cola();

private:

};




#endif // COLA_H_