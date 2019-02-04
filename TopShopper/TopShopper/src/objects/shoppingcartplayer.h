
#ifndef SHOPPINGCARTPLAYER_H_
#define SHOPPINGCARTPLAYER_H_

#include "entity.h"
//#include "vehicle/PxVehicleDrive4W.h"
#include "vehicle/VehicleShoppingCart.h"



class ShoppingCartPlayer : public Entity {
	public:
		//ShoppingCartPlayer(std::string name, physx::PxVehicleDrive4W *vehicle4W);
		ShoppingCartPlayer(std::string name, VehicleShoppingCart *shoppingCartBase, int inputID);
		virtual ~ShoppingCartPlayer();
		// functions to control what audio clip plays on certain events, reaction functions based on collision pairs (e.g. increment points by value passed in by collision)

		VehicleShoppingCart *_shoppingCartBase;


		void updatePhysics(double deltaTime) override;

		//physx::PxVehicleDrive4W *_vehicle4W = nullptr;
		//physx::PxRigidDynamic *_actor = nullptr;



		int getInputID() { return _inputID; }
		void setInputID(int id) { _inputID = id; }
	private:
		// bunch of game state stuff like shopping list, points,


		int _inputID;

};






#endif // SHOPPINGCARTPLAYER_H_
