#ifndef SHOPPINGCARTPLAYER_H_
#define SHOPPINGCARTPLAYER_H_

#include "entity.h"

class VehicleShoppingCart;

class ShoppingCartPlayer : public Entity {
	public:
		ShoppingCartPlayer(VehicleShoppingCart *shoppingCartBase, int inputID);
		virtual ~ShoppingCartPlayer();

		VehicleShoppingCart *_shoppingCartBase = nullptr;

		void updatePhysics(double deltaTime) override;

		int getInputID() { return _inputID; }
		void setInputID(int id) { _inputID = id; }

		// TODO: functions to control what audio clip plays on certain events, reaction functions based on collision pairs (e.g. increment points by value passed in by collision)

	private:
		int _inputID; // either 1, 2, 3 or 4 (what human player this cart belongs to)

		// TODO: bunch of game state stuff like shopping list, points,

};





#endif // SHOPPINGCARTPLAYER_H_
