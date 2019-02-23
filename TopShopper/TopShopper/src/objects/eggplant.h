#ifndef EGGPLANT_H_
#define EGGPLANT_H_

#include "entity.h"

namespace physx {
	class PxRigidDynamic;
};

class Eggplant : public Entity {
public:
	Eggplant(physx::PxRigidDynamic *actor);
	virtual ~Eggplant();

private:

};




#endif // EGGPLANT_H_
