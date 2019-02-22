#ifndef SHOPPINGCARTPLAYER_H_
#define SHOPPINGCARTPLAYER_H_

#include "entity.h"

class VehicleShoppingCart;

class ShoppingCartPlayer : public Entity {
	public:
		ShoppingCartPlayer(VehicleShoppingCart *shoppingCartBase);
		virtual ~ShoppingCartPlayer();

		VehicleShoppingCart *_shoppingCartBase = nullptr;

	private:

};





#endif // SHOPPINGCARTPLAYER_H_
