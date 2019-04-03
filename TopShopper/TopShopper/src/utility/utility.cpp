#include "utility.h"


physx::PxVec3 castGLMVec4ToPxVec3(glm::vec4 glmVec) {
	return physx::PxVec3(glmVec.x, glmVec.y, glmVec.z);
}


std::vector<physx::PxVec3> castVectorOfGLMVec4ToVectorOfPxVec3(std::vector<glm::vec4> v_glmVec) {
	std::vector<physx::PxVec3> v_pxVec;
	for (glm::vec4 glmVec : v_glmVec) {
		v_pxVec.push_back(castGLMVec4ToPxVec3(glmVec));
	}
	return v_pxVec;
}


bool isApproxEqual(physx::PxVec3 v1, physx::PxVec3 v2) {
	float myEpsilon = 0.0001f;
	if (fabs(v1.x - v2.x) >= myEpsilon) return false;
	if (fabs(v1.y - v2.y) >= myEpsilon) return false;
	if (fabs(v1.z - v2.z) >= myEpsilon) return false;

	return true;
}


//SOURCE: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

// EULER ANGLES -> QUATERNION...
physx::PxQuat toQuaternion(double yaw, double pitch, double roll) // yaw (Z), pitch (Y), roll (X)
{
	// Abbreviations for the various angular functions
	double cy = cos(yaw * 0.5);
	double sy = sin(yaw * 0.5);
	double cp = cos(pitch * 0.5);
	double sp = sin(pitch * 0.5);
	double cr = cos(roll * 0.5);
	double sr = sin(roll * 0.5);

	physx::PxQuat q;
	q.w = cy * cp * cr + sy * sp * sr;
	q.x = cy * cp * sr - sy * sp * cr;
	q.y = sy * cp * sr + cy * sp * cr;
	q.z = sy * cp * cr - cy * sp * sr;
	return q;
}


// QUATERNION -> EULER ANGLES...
void toEulerAngle(const physx::PxQuat& q, double& roll, double& pitch, double& yaw)
{
	// roll (x-axis rotation)
	double sinr_cosp = +2.0 * (q.w * q.x + q.y * q.z);
	double cosr_cosp = +1.0 - 2.0 * (q.x * q.x + q.y * q.y);
	roll = atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = +2.0 * (q.w * q.y - q.z * q.x);
	if (fabs(sinp) >= 1)
		pitch = copysign(physx::PxPi / 2, sinp); // use 90 degrees if out of range
	else
		pitch = asin(sinp);

	// yaw (z-axis rotation)
	double siny_cosp = +2.0 * (q.w * q.z + q.x * q.y);
	double cosy_cosp = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);
	yaw = atan2(siny_cosp, cosy_cosp);
}