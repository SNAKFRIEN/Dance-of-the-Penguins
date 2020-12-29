#pragma once

#include "glm/glm.hpp"

#include <vector>

class Spawner
{
public:
	//Find spawn near top of bottom of the screen
	//zOffset is used to place the spawn further away from the edge of the screen
	glm::vec3 FindOffScreenSpawnPoint(glm::vec3 cameraPos, glm::vec3 cameraLookAtPoint, float fovInRadians, float zOffset = 0.0f) const;
	//Find spawn at certain distance from the player. halfXRange and halfZRange are used to make sure its within the ice rink
	glm::vec3 FindDistancedSpawnPoint(glm::vec3 playerPos, float minPlayerDistance, float halfXRange, float halfZRange) const;
	//Generate random target (direction) near the player within given radius
	glm::vec3 FindCloseTarget(glm::vec3 playerPos, float maxDistance);
};