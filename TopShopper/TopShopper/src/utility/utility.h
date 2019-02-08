#ifndef UTILITY_H_
#define UTILITY_H_

// NOTE: always implement global funcions inside a .cpp, otherwise if multiple files includes this header, you get a linker error (same symbol multi defined)


#include <vector>
#include <glm/glm.hpp>
#include <foundation/PxVec3.h>


physx::PxVec3 castGLMVec4ToPxVec3(glm::vec4 glmVec);

std::vector<physx::PxVec3> castVectorOfGLMVec4ToVectorOfPxVec3(std::vector<glm::vec4> v_glmVec);






#endif // UTILITY_H_