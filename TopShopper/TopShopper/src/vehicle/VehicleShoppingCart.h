
#ifndef VEHICLE_SHOPPING_CART_H_
#define VEHICLE_SHOPPING_CART_H_

#include "PxPhysicsAPI.h"
#include "snippetvehiclecommon/SnippetVehicleCreate.h"



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


		void updatePhysics(double deltaTime);

	private:
		physx::PxVehicleDrive4WRawInputData _rawInputData;
		bool _isAirborne;




};









#endif // VEHICLE_SHOPPING_CART_H_
