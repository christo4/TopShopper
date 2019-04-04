//TODO: can maybe include a callback of some kind which detects the connecting and disconnecting of controllers in real time.
#include "inputmanager.h"
#include "core/broker.h"

InputManager::InputManager(Broker *broker)
	: _broker(broker)
{

}

//detects the amount of gamepads currently connected to the machine (can maybe take the number of players and init that way instead, but you would have to validate the input)
void InputManager::init(){

	while (glfwJoystickPresent(_joySticks[_numGamepads])) {	//tallies the amount of connected gamepads and initializes a struct for each of them.
		_numGamepads++;
		_gamePads.push_back(new Gamepad());
	}
}


//updates the states of each of the gamepad structs in the gamepads array when called
void InputManager::updateSeconds(double variableDeltaTime) {

	glfwPollEvents(); // NOTE: I MOVED THIS HERE FROM THE RENDERING CODE SINCE IT DIDNT WORK OTHERWISE

	int numAxes;
	int numButtons;
	const float * axesArray = NULL;
	const unsigned char * buttonArray = NULL;

	for (unsigned int i = 0; i < _numGamepads; i++) {
		
		axesArray = glfwGetJoystickAxes(_joySticks[i], &numAxes);			//get the state of the axes for curr gamepad
		buttonArray = glfwGetJoystickButtons(_joySticks[i], &numButtons);	//get the state of the buttons for curr gamepad

		//update the state of the current gamepad's axes

	

		_gamePads[i]->leftStickX = axesArray[0];
		_gamePads[i]->leftStickY = axesArray[1];
		_gamePads[i]->rightStickX = axesArray[2];
		_gamePads[i]->rightStickY = axesArray[3];
		_gamePads[i]->leftTrigger = axesArray[4];
		_gamePads[i]->rightTrigger = axesArray[5];
		
		//update the state of the current gamepad's buttons
		_gamePads[i]->aButton = (bool) buttonArray[0];
		_gamePads[i]->bButton = (bool) buttonArray[1];
		_gamePads[i]->xButton = (bool) buttonArray[2];
		_gamePads[i]->yButton = (bool) buttonArray[3];
		_gamePads[i]->leftBump = (bool) buttonArray[4];
		_gamePads[i]->rightBump = (bool) buttonArray[5];
		_gamePads[i]->backButton = (bool) buttonArray[6];

		bool newStartButtonState = (bool) buttonArray[7];
		_gamePads[i]->startButtonJustPressed = (!_gamePads[i]->startButton && newStartButtonState);
		_gamePads[i]->startButton = newStartButtonState;

		_gamePads[i]->leftStick = (bool) buttonArray[8];
		_gamePads[i]->rightStick = (bool) buttonArray[9];
		_gamePads[i]->upButton = (bool) buttonArray[10];
		_gamePads[i]->rightButton = (bool) buttonArray[11];
		_gamePads[i]->downButton = (bool) buttonArray[12];
		_gamePads[i]->leftButton = (bool) buttonArray[13];
	}

	// KEYBOARD AND MOUSE INPUT...
	GLFWwindow *window = _broker->getRenderingManager()->getWindow();
	_keyboardAndMouse->wKey = GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W);
	_keyboardAndMouse->aKey = GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A);
	_keyboardAndMouse->sKey = GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S);
	_keyboardAndMouse->dKey = GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D);
	_keyboardAndMouse->leftShiftKey = GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
	_keyboardAndMouse->spaceKey = GLFW_PRESS == glfwGetKey(window, GLFW_KEY_SPACE);

	bool newPKeyState = GLFW_PRESS == glfwGetKey(window, GLFW_KEY_P);
	_keyboardAndMouse->pKeyJustPressed = (!_keyboardAndMouse->pKey && newPKeyState);
	_keyboardAndMouse->pKey = newPKeyState;

}

//returns a pointer to the structure representing the gamepad number given as a parameter (1,2,3,4)
Gamepad* InputManager::getGamePad(int gamePadNumber) {
	if (gamePadNumber <= _numGamepads && gamePadNumber > 0) {
		return _gamePads[gamePadNumber-1];
	}
	return nullptr;
}


InputManager::~InputManager() {

}