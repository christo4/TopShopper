#include "shoppingcartplayer.h"
#include "vehicle/VehicleShoppingCart.h"


ShoppingCartPlayer::ShoppingCartPlayer(VehicleShoppingCart *shoppingCartBase)
	: Entity(shoppingCartBase->_vehicle4W->getRigidDynamicActor(), EntityTypes::SHOPPING_CART_PLAYER), _shoppingCartBase(shoppingCartBase)
{
	_actor->userData = this;

	addComponent(ComponentTypes::PLAYER_SCRIPT);
}


ShoppingCartPlayer::~ShoppingCartPlayer() {

}