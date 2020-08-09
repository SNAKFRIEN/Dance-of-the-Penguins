#pragma once

#include "glm/glm.hpp"

namespace EliMath
{
	//Calculate where a ray hits the floor (y = 0)
	//direction does not need to be normalized
	glm::vec3 IntersectFloor(glm::vec3 origin, glm::vec3 direction);
}