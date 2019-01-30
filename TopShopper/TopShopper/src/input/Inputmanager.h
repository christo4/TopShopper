#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_


class Broker;



struct gamePad {
	float leftAnalogX;
	float leftAnalogY;
	float rightAnalogX;
	float rightAnalogY;
	float leftTrigger;
	float rightTrigger;
	bool aButton;
	bool xButton;
	bool bButton;
	bool yButton;
	bool leftBump;
	bool rightBump;
	bool leftStick;
	bool rightStick;
};






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
