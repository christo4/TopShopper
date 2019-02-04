
#include "shoppingcartplayer.h"



/*
// INITS A SHOPPINGCARTPLAYER PREFAB WITH DEFAULT VALUES
ShoppingCartPlayer::ShoppingCartPlayer(std::string name, physx::PxVehicleDrive4W *vehicle4W)
	: _vehicle4W(vehicle4W)
{
	// INIT BASIC INFO AND PHYSICS BASE ACTOR

	//_name = name;
	_tag = SHOPPING_CART_PLAYER;
	_actor = _vehicle4W->getRigidDynamicActor();

	_actor->userData = this; // link actor 1-1 with this instance

	_actor->setName(name.c_str());

	// NOW ADD/CONFIG ANY COMPONENTS

	addComponent(ComponentTypes::AUDIO_LISTENER); // adds an AL with default stats
	// TODO: config the AL fields here

	addComponent(ComponentTypes::AUDIO_SOURCE); // adds an AS with default stats
	// TODO: config the AS fields here

	addComponent(ComponentTypes::CAMERA); // adds a camera with default stats
	// TODO: config the camera fields here

	addComponent(ComponentTypes::INPUT_CONTROLLER); // adds an IC with default stats
	// TODO: config the IC fields here

	addComponent(ComponentTypes::MESH); // adds a mesh with default stats
	// TODO: config the mesh fields here

}
*/

// INITS A SHOPPINGCARTPLAYER PREFAB WITH DEFAULT VALUES
ShoppingCartPlayer::ShoppingCartPlayer(std::string name, VehicleShoppingCart *shoppingCartBase) 
	: _shoppingCartBase(shoppingCartBase)
{
	// INIT BASIC INFO AND PHYSICS BASE ACTOR

	_tag = SHOPPING_CART_PLAYER; // set fixed tag
	_shoppingCartBase->_vehicle4W->getRigidDynamicActor()->setName(name.c_str()); // set name
	_shoppingCartBase->_vehicle4W->getRigidDynamicActor()->userData = this; // link actor 1-1 with this instance

	// NOW ADD/CONFIG ANY COMPONENTS

	addComponent(ComponentTypes::AUDIO_LISTENER); // adds an AL with default stats
	// TODO: config the AL fields here

	addComponent(ComponentTypes::AUDIO_SOURCE); // adds an AS with default stats
	// TODO: config the AS fields here

	addComponent(ComponentTypes::CAMERA); // adds a camera with default stats
	// TODO: config the camera fields here

	//addComponent(ComponentTypes::INPUT_CONTROLLER); // adds an IC with default stats
	// TODO: config the IC fields here

	addComponent(ComponentTypes::MESH); // adds a mesh with default stats
	// TODO: config the mesh fields here
}



ShoppingCartPlayer::~ShoppingCartPlayer() {

}





void ShoppingCartPlayer::updatePhysics(double deltaTime) {
	_shoppingCartBase->updatePhysics(deltaTime);
}
