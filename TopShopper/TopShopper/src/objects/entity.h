


#ifndef ENTITY_H_
#define ENTITY_H_



class Entity {
public:
	Entity();
	virtual ~Entity();
	// 1. have addComponent()
	// 2. have removeComponent()
	// 3. have getComponent()
private:
	// 1. have a 'name' (str)
	// 2. have a 'tag' (str)
	// 3. have a vector of components (or maybe fixed size where each index coresponds to null or the component of a certain type, but in the latter case, that forces only 0/1 of each component attached to an entity)
};




















#endif // ENTITY_H_
