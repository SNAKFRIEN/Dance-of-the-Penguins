#pragma once

#include "glm/glm.hpp"

#include <vector>

/*This class calculates off screen positions to spawn penguins.
The vertical fov is always the same, horizontal fov isn't.
This, in combination with the fact that the gameplay should be
the same regardless of your screen width, means that all objects
will spawn at the top or bottom edge of the screen.*/

class Spawner
{
public:
	//zOffset is used to place the spawn further away from the edge of the screen
	//xOffset is used to move the spawn along the edge of the screen relative to its center
	glm::vec3 FindOffScreenSpawnPoint(glm::vec3 cameraPos, glm::vec3 cameraLookAtPoint, float fov, float zOffset = 0.0f, float xOffset = 0.0f) const;
};