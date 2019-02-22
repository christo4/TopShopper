#include "shoppingcartplayer.h"
#include "vehicle/VehicleShoppingCart.h"


ShoppingCartPlayer::ShoppingCartPlayer(VehicleShoppingCart *shoppingCartBase)
	: Entity(shoppingCartBase->_vehicle4W->getRigidDynamicActor(), EntityTypes::SHOPPING_CART_PLAYER), _shoppingCartBase(shoppingCartBase)
{
	_actor->userData = this;

	//addComponent(ComponentTypes::AUDIO_LISTENER); // adds an AL with default stats
	// TODO: config the AL fields here

	//addComponent(ComponentTypes::AUDIO_SOURCE); // adds an AS with default stats
	// TODO: config the AS fields here

	//addComponent(ComponentTypes::CAMERA); // adds a camera with default stats
	// TODO: config the camera fields here

	//addComponent(ComponentTypes::MESH); // adds a mesh with default stats
	// TODO: config the mesh fields here

	addComponent(ComponentTypes::PLAYER_SCRIPT);
}


ShoppingCartPlayer::~ShoppingCartPlayer() {

}