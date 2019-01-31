


#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_
#include <vector>
#include <GLFW/glfw3.h>
#include <iostream>


//Struct to represent the state of a gamepad at any given time.
struct Gamepad {
	float leftStickX;
	float leftStickY;
	float rightStickX;
	float rightStickY;
	float leftTrigger;
	float rightTrigger;
	bool aButton;
	bool bButton;
	bool xButton;
	bool yButton;
	bool leftBump;
	bool rightBump;
	bool backButton;
	bool startButton;
	bool leftStick;
	bool rightStick;
	bool upButton;
	bool rightButton;
	bool downButton;
	bool leftButton;
};


class Broker;

class InputManager {
public:
	InputManager(Broker* broker);
	virtual ~InputManager();
	void init();
	void updateMilliseconds(double deltaTime);
	Gamepad * getGamePad(int gamePadNumber);
private:
	int _numGamepads;
	int _joySticks[4] = { GLFW_JOYSTICK_1 ,GLFW_JOYSTICK_2 ,GLFW_JOYSTICK_3 ,GLFW_JOYSTICK_4 };
	std::vector<Gamepad*> _gamePads;
	Broker* _broker = nullptr;
};








#endif // INPUTMANAGER_H_
