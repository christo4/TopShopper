


#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_


class Broker;

class InputManager {
public:
	InputManager(Broker* broker);
	virtual ~InputManager();
	void init();
	void updateMilliseconds(double deltaTime);
private:
	Broker* _broker = nullptr;
};








#endif // INPUTMANAGER_H_
