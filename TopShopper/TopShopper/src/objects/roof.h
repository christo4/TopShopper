#ifndef ROOF_H_
#define ROOF_H_

#include "entity.h"

namespace physx {
	class PxRigidStatic;
};

class Roof : public Entity {
public:
	Roof(physx::PxRigidStatic *actor);
	virtual ~Roof();

private:

};





#endif // ROOF_H_
