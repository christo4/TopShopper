#ifndef OBSTACLE5_H_
#define OBSTACLE5_H_

#include "entity.h"

namespace physx {
	class PxRigidStatic;
};

class Obstacle5 : public Entity {
public:
	Obstacle5(physx::PxRigidStatic *actor);
	virtual ~Obstacle5();

private:

};





#endif // OBSTACLE5_H_
