#ifndef OBSTACLE7_H_
#define OBSTACLE7_H_

#include "entity.h"

namespace physx {
	class PxRigidStatic;
};

class Obstacle7 : public Entity {
public:
	Obstacle7(physx::PxRigidStatic *actor);
	virtual ~Obstacle7();

private:

};





#endif // OBSTACLE7_H_
