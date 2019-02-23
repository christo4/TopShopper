#ifndef CARROT_H_
#define CARROT_H_

#include "entity.h"

namespace physx {
	class PxRigidDynamic;
};

class Carrot : public Entity {
public:
	Carrot(physx::PxRigidDynamic *actor);
	virtual ~Carrot();

private:

};




#endif // CARROT_H_