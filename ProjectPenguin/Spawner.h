#pragma once

#include "glm/glm.hpp"

#include <vector>

/*This class calculates off screen positions to spawn penguins.
The vertical fov is always the same, horizontal fov isn't.
This, in combination with the fact that the gameplay should be
the same regardless of your screen width, means that all objects
will either spawn above or below the screen.*/

class Spawner
{
public:
	glm::vec3 GetAvailablePosition(glm::vec3 cameraPos, glm::vec3 cameraLookAtPoint, float fov) const;
};