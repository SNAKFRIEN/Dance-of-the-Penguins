#include "Spawner.h"

#include "EliMath.h"

#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

glm::vec3 Spawner::FindOffScreenSpawnPoint(glm::vec3 cameraPos, glm::vec3 cameraLookAtPoint, float fovInRadians, float zOffset) const
{
	//Calculate right vector of camera
	glm::vec3 cameraForward = glm::normalize(cameraLookAtPoint - cameraPos);
	glm::vec3 cameraRightVector = glm::cross(cameraForward, glm::vec3(0.0f, 1.0f, 0.0f));
	
	//Create ray towards the top and bottom of the screen directly above and below where the camera is looking
	glm::vec3 toTop = glm::rotate(cameraForward, fovInRadians / 2.0f, cameraRightVector);
	glm::vec3 toBottom = glm::rotate(cameraForward, -fovInRadians / 2.0f, cameraRightVector);

	glm::vec3 topOfScreen = EliMath::IntersectFloor(cameraPos, toTop);
	glm::vec3 bottomOfScreen = EliMath::IntersectFloor(cameraPos, toBottom);

	if (abs(topOfScreen.z) > abs(bottomOfScreen.z))
	{
		//Looking towards top of screen, spawn near bottom
		return bottomOfScreen + glm::vec3(0.0f, 0.0f, zOffset);
	}
	else
	{
		//Looking towards bottom of screen, spawn near top
		return topOfScreen - glm::vec3(0.0f, 0.0f, zOffset);
	}
}
