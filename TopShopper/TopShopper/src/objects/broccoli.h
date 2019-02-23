#ifndef BROCCOLI_H_
#define BROCCOLI_H_

#include "entity.h"

namespace physx {
	class PxRigidDynamic;
};

class Broccoli : public Entity {
public:
	Broccoli(physx::PxRigidDynamic *actor);
	virtual ~Broccoli();

private:

};




#endif // BROCCOLI_H_
