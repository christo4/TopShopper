#ifndef OBSTACLE1_H_
#define OBSTACLE1_H_

#include "entity.h"

namespace physx {
	class PxRigidStatic;
};

class Obstacle1 : public Entity {
public:
	Obstacle1(physx::PxRigidStatic *actor);
	virtual ~Obstacle1();

private:

};





#endif // OBSTACLE1_H_