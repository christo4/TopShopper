#ifndef OBSTACLE4_H_
#define OBSTACLE4_H_

#include "entity.h"

namespace physx {
	class PxRigidStatic;
};

class Obstacle4 : public Entity {
public:
	Obstacle4(physx::PxRigidStatic *actor);
	virtual ~Obstacle4();

private:

};





#endif // OBSTACLE4_H_
