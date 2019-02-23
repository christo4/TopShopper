#ifndef WATER_H_
#define WATER_H_

#include "entity.h"

namespace physx {
	class PxRigidDynamic;
};

class Water : public Entity {
public:
	Water(physx::PxRigidDynamic *actor);
	virtual ~Water();

private:

};




#endif // WATER_H_
