#ifndef OBSTACLE3_H_
#define OBSTACLE3_H_

#include "entity.h"

namespace physx {
	class PxRigidStatic;
};

class Obstacle3 : public Entity {
public:
	Obstacle3(physx::PxRigidStatic *actor);
	virtual ~Obstacle3();

private:

};





#endif // OBSTACLE3_H_
