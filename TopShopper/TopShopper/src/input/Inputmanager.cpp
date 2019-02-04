//TODO: can maybe include a callback of some kind which detects the connecting and disconnecting of controllers in real time.
#include "inputmanager.h"
#include "core/broker.h"

InputManager::InputManager(Broker * broker): _broker(broker) {

}

//detects the amount of gamepads currently connected to the machine (can maybe take the number of players and init that way instead, but you would have to validate the input)
void InputManager::init(){

	while (glfwJoystickPresent(_joySticks[_numGamepads])) {	//tallies the amount of connected gamepads and initializes a struct for each of them.
		_numGamepads++;
		_gamePads.push_back(new Gamepad());
	}
}


//updates the states of each of the gamepad structs in the gamepads array when called
void InputManager::updateMilliseconds(double deltaTime) {

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
		_gamePads[i]->startButton = (bool) buttonArray[7];
		_gamePads[i]->leftStick = (bool) buttonArray[8];
		_gamePads[i]->rightStick = (bool) buttonArray[9];
		_gamePads[i]->upButton = (bool) buttonArray[10];
		_gamePads[i]->rightButton = (bool) buttonArray[11];
		_gamePads[i]->downButton = (bool) buttonArray[12];
		_gamePads[i]->leftButton = (bool) buttonArray[13];
	}


	// now that our gamepads have been updated, we pass the input along to update the control state of every player cart...

	passAlongInputsToCarts();



}

//returns a pointer to the structure representing the gamepad number given as a parameter (1,2,3,4)
Gamepad * InputManager::getGamePad(int gamePadNumber) {
	if (gamePadNumber <= _numGamepads && gamePadNumber > 0) {
		return _gamePads[gamePadNumber-1];
	}
	return NULL;
}


InputManager::~InputManager()
{
}


void InputManager::passAlongInputsToCarts() {

	if (_numGamepads < 1) {
		return;
	}

	std::vector<std::shared_ptr<ShoppingCartPlayer>> playerCarts = _broker->get_PhysicsManager_ActiveScene_AllShoppingCartPlayers();

	for (std::shared_ptr<ShoppingCartPlayer> &playerCart : playerCarts) {
		int playerID = playerCart->getInputID();
		// ~~~~~~~~~I can either do no error checking here or leave it be and a wrongly set ID will cause an indexoutofbounds error
		Gamepad *pad = getGamePad(playerID);
		// now read inputs from this gamepad and pass it along to cart's processInput methods that will then change movement behaviour for next physics step
		
		physx::PxReal accel = glm::clamp(((pad->rightTrigger + 1) / 2), 0.0f, 1.0f);
		physx::PxReal reverse = glm::clamp(((pad->leftTrigger + 1) / 2), 0.0f, 1.0f);
		physx::PxReal handbrake = pad->xButton ? 1.0f : 0.0f;
		physx::PxReal steer = glm::clamp(pad->leftStickX, 0.0f, 1.0f);
		bool turboButtonPressed = pad->bButton;

		playerCart->_shoppingCartBase->processRawInputDataController(accel, reverse, handbrake, steer, turboButtonPressed);

	}


}
