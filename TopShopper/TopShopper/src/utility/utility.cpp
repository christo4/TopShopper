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
