#ifndef OBSTACLE2_H_
#define OBSTACLE2_H_

#include "entity.h"

namespace physx {
	class PxRigidStatic;
};

class Obstacle2 : public Entity {
public:
	Obstacle2(physx::PxRigidStatic *actor);
	virtual ~Obstacle2();

private:

};





#endif // OBSTACLE2_H_
