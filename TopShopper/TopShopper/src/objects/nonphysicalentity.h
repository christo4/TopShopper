
#ifndef NONPHYSICALENTITY_H_
#define NONPHYSICALENTITY_H_

#include "entity.h"

// DEFINITION:
// EACH NONPHYSICALENTITY WILL "EXIST" IN OUR "SCENE", BUT NOT PART OF A PHYSX SCENE AND THUS NOT PART OF THE PHYSICAL WORLD SIMULATION
class NonPhysicalEntity : public Entity {
public:
	NonPhysicalEntity();
	virtual ~NonPhysicalEntity();
private:

};





#endif // NONPHYSICALENTITY_H_
