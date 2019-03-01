#ifndef VEHICLE_SHOPPING_CART_H_
#define VEHICLE_SHOPPING_CART_H_

#include "PxPhysicsAPI.h"
#include "snippetvehiclecommon/SnippetVehicleCreate.h"



//enum VehicleStates {
	//NORMAL,
	//TURBO,
	//TURBO_STUCK,

//};



class VehicleShoppingCart {
	public:
		VehicleShoppingCart(physx::PxPhysics *physics, physx::PxCooking *cooking);
		virtual ~VehicleShoppingCart();

		snippetvehicle::VehicleDesc _vehicleDesc;
		physx::PxVehicleDrive4W *_vehicle4W = nullptr;


		bool getIsAirborne() { return _isAirborne; }
		void setIsAirborne(bool isAirborne) { _isAirborne = isAirborne; }

	
		physx::PxVehicleDrive4WRawInputData getRawInputData() { return _rawInputData; }
		void processRawInputDataKeyboard(const bool accelKeyPressed, const bool reverseKeyPressed, const bool handbrakeKeyPressed, const bool steerLeftKeyPressed, const bool steerRightKeyPressed, const bool turboKeyPressed);
		void processRawInputDataController(const physx::PxReal accel, const physx::PxReal reverse, const physx::PxReal handbrake, const physx::PxReal steer, const bool turboButtonPressed);
		void clearRawInputDataKeyboard();
		void clearRawInputDataController();

		void smoothAndFeedInputs(double fixedDeltaTime);

		bool IsTurboing() { return _isTurboing; }
		bool IsBashProtected() { return _isBashProtected; }

		void setBashProtected();
		void tickBashProtectionTimer(double fixedDeltaTime);


		std::vector<physx::PxShape*> _wheelShapes;


	private:
		physx::PxVehicleDrive4WRawInputData _rawInputData;
		bool _isAirborne;

		// ***NOTE: only 1 of the process methods should be called in a frame (exclusively key/M or gamepad controlled)
		// This flag will then be used to select which smoothing data / setting method to use
		bool _isKeyAndMouseControlled = true; // if false then isGamePadControlled 


		bool _isTurboing = false;
		bool _isBashProtected = false;
		double _bashProtectionTimer = -1.0;

		//double _stateTimer = DBL_MAX; // use DBL_MAX for practically infinite timer

};









#endif // VEHICLE_SHOPPING_CART_H_
