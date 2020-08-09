#include "EliMath.h"

glm::vec3 EliMath::IntersectFloor(glm::vec3 origin, glm::vec3 direction)
{
	assert(direction.z != 0 || direction.x != 0);
	
	float y = 0.0f;
	float x;
	float z;
	
	//Calculate x
	if (direction.x != 0)
	{
		float riseOverRun = direction.y / direction.x;
		float offset = origin.y - riseOverRun * origin.x;
		assert(riseOverRun != 0);
		x = -offset / riseOverRun;
	}
	else
	{
		x = origin.x;
	}

	//Calculate z
	if (direction.z != 0)
	{
		float riseOverRun = direction.y / direction.z;
		float offset = origin.y - riseOverRun * origin.z;
		assert(riseOverRun != 0);
		z = -offset / riseOverRun;
	}
	else
	{
		z = origin.z;
	}

	return glm::vec3(x, y, z);
}
