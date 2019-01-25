
#ifndef PHYSICALENTITY_H_
#define PHYSICALENTITY_H_

#include "entity.h"


// DEFINITION:
// EACH PHYSICAL ENTITY WILL EXIST IN A PHYSX SCENE AND THUS BE PART OF THE PHYSICAL WORLD SIMULATION
class PhysicalEntity : public Entity {
	public:
		PhysicalEntity();
		virtual ~PhysicalEntity();
	private:

};





#endif // PHYSICALENTITY_H_