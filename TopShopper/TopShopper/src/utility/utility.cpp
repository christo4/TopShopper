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