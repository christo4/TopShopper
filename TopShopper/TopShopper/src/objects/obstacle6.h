#ifndef OBSTACLE6_H_
#define OBSTACLE6_H_

#include "entity.h"

namespace physx {
	class PxRigidStatic;
};

class Obstacle6 : public Entity {
public:
	Obstacle6(physx::PxRigidStatic *actor);
	virtual ~Obstacle6();

private:

};





#endif // OBSTACLE6_H_
