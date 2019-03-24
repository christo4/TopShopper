#ifndef COOKIE_H_
#define COOKIE_H_

#include "entity.h"

namespace physx {
	class PxRigidDynamic;
};

class Cookie : public Entity {
public:
	Cookie(physx::PxRigidDynamic *actor);
	virtual ~Cookie();

private:

};




#endif // COOKIE_H_
