#include "aimanager.h"
#include "core/broker.h"
#include "PxPhysicsAPI.h"
#include "objects/sparechange.h"


using namespace physx;


AIManager::AIManager(Broker *broker) 
	: _broker(broker)
{

}

AIManager::~AIManager() {

}

void AIManager::init() {

}

void AIManager::updateMilliseconds(double deltaTime) {
	// call UPDATE() for all behaviour scripts...
	for (std::shared_ptr<Entity> &entity : _broker->getPhysicsManager()->getActiveScene()->_entities) {
		std::shared_ptr<Component> comp = entity->getComponent(ComponentTypes::BEHAVIOUR_SCRIPT);
		if (comp != nullptr) {
			std::shared_ptr<BehaviourScript> script = std::static_pointer_cast<BehaviourScript>(comp);
			script->update(deltaTime);
		}
	}
}