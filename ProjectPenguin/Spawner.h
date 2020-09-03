#pragma once

#include "glm/glm.hpp"

#include <vector>

//REMOVE or REPLACE this comment now that we have fishing penguins:
/*This class calculates off screen positions to spawn penguins.
The vertical fov is always the same, horizontal fov isn't.
This, in combination with the fact that the gameplay should be
the same regardless of your screen width, means that all objects
will spawn at the top or bottom edge of the screen.*/

class Spawner
{
public:
	//Find spawn near top of bottom of the screen
	//zOffset is used to place the spawn further away from the edge of the screen
	glm::vec3 FindOffScreenSpawnPoint(glm::vec3 cameraPos, glm::vec3 cameraLookAtPoint, float fovInRadians, float zOffset = 0.0f) const;
	//Find spawn at certain distance from the player
	glm::vec3 FindDistancedSpawnPoint(glm::vec3 playerPos, float minPlayerDistance, float halfXRange, float halfZRange) const;
};