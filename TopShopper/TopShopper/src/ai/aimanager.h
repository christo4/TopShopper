


#ifndef AIMANAGER_H_
#define AIMANAGER_H_

class Broker;

class AIManager {
public:
	AIManager(Broker* broker);
	virtual ~AIManager();
	void init();
	void updateMilliseconds(double deltaTime);
private:
	Broker* _broker = nullptr;
};




















#endif // AIMANAGER_H_

