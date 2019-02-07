#ifndef GROUND_H_
#define GROUND_H_

#include "entity.h"

namespace physx {
	class PxRigidStatic;
};

class Ground : public Entity {
	public:
		Ground(physx::PxRigidStatic *actor);
		virtual ~Ground();

		void updatePhysics(double deltaTime) override;


	private:


};





#endif // GROUND_H_
