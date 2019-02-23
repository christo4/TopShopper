#ifndef WATERMELON_H_
#define WATERMELON_H_

#include "entity.h"

namespace physx {
	class PxRigidDynamic;
};

class Watermelon : public Entity {
public:
	Watermelon(physx::PxRigidDynamic *actor);
	virtual ~Watermelon();

private:

};




#endif // WATERMELON_H_
