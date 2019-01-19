


#ifndef COMPONENT_H_
#define COMPONENT_H_


// ~~~~~NOTE: each child class will implement very specialized behaviour if attached to an entity

class Component {
public:
	Component();
	virtual ~Component();
private:
	// 1. have a reference to the entity this component is attached to (so we can get high-lvl info about it (e.g. tag/name) or access sibling components)
};




















#endif // COMPONENT_H_
