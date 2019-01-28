
#ifndef SHOPPINGCART_H_
#define SHOPPINGCART_H_

#include "entity.h"
class PhysicsManager;


class ShoppingCart : public Entity {
	public:
		ShoppingCart(PhysicsManager* physicsManager);
		virtual ~ShoppingCart();

	private:
		PhysicsManager* _physicsManager = nullptr;
};










#endif // SHOPPINGCART_H_
